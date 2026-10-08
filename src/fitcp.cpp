/*
Essa biblioteca contem funcoes para executar o calculo de interseccao de muller e preparata(1978)
Funciona em cima da implementacao propria da DCEL
fitcp vem do nome do artigo "Finding the Intersection of Two Convex Polyhedra"
Criado por: Davi Lazzarin
Data: 19/09/2026
*/
#include "fitcp.hpp"

/*
    Biblioteca externa disponivel em https://github.com/AngusJohnson/Clipper2
    Usamos essa lib devido sua implementacao baseada no algoritmo de Vatti no lugar do algoritmo de Shamos-Hoey
    Escolha feita devido a complexidade de implententacao e erros de ponto flutuante envolvidos no Shamos-Hoey
    Assintoticamente mais caro.
    Vatti -> O(NlogN)
    Shamos-Hoey -> O(N)
*/
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>

#include "Clipper2Lib/include/clipper2/clipper.h"

//==================================================
// FUNCOES PARA A PRIMEIRA VERIFICACAO DO ARTIGO
//==================================================
/*
  O objetivo dessa parte eh projetar os dois poliedros em 2D
  E verificar se as projecoes se sobrepoem, se nao houver uma sobreposicao em 2D
  significa que os poliedro estao separados, ou seja, a interseccao eh nula (ja podemos encerrar a busca)
  E se os poligonos (projecao dos poliedros em 2D) se sobrepuserem, ai vamos verificar
  se a sobreposisao tambem existem no eixo z (n3).
  Usaremos uma implementacao pronta do metodo de Shamos_Hoey
  Usaremos a Clipper2 de Angus Johnson
*/

//Encontra o contorno de suporte vertical de um poliedro
//Retorna o poligono 2D da projeção, e um vetor com os indices das semi arestas que foram projetadas
poligono_2d_t extrai_poligono_projecao(const dcel_t &d)
{
  poligono_2d_t contorno;
  size_t aresta_inicial = INVALID_INDEX;

  // Aqui olhamos para o poliedro e nao para a dcel em si
  // Vamos procurar as arestas do poliedro que estao na "borda"
  // A partir de uma aresta (uma semi-aresta e seu par) vamos comparar as duas faces
  // E verificar a inclinacao de uma face em relacao a outra
  // Regra do Artigo: n3(f') * n3(f'') <= 0
  for (size_t i = 0; i < d.mapa_sa.size(); ++i)
  {
    const semi_aresta_t &sa = d.mapa_sa[i];
    if (sa.par == INVALID_INDEX)
      continue;

    t_coord n3_esq = d.mapa_faces[sa.ind_face].n3;
    t_coord n3_dir = d.mapa_faces[d.mapa_sa[sa.par].ind_face].n3;

    if (n3_esq * n3_dir <= EPS)
    {
      // Garante uma orientacao de caminhada com o  topo(ordenadno pelo eixo z) sempre a esquerda
      if (n3_esq >= 0.0)
      {
        aresta_inicial = i;
        break;
      }
    }
  }

  // Ao encontrar uma aresta no contorno (o equador) do poliedro
  // Podemos caminhar a partir dela para montar o poligono do contorno

  if (aresta_inicial == INVALID_INDEX)
    return contorno;

  // Caminha pelo ciclo do contorno do poliedro convexo
  size_t atual = aresta_inicial;
  do
  {
    // Salva o vertice projetado 2D
    size_t ind_vertice = d.mapa_sa[atual].ind_vertice;
    ponto_3d pos3d = d.mapa_vertices[ind_vertice].pos;
    contorno.vertices.push_back({pos3d.x, pos3d.y});
    contorno.indices_originais.push_back(ind_vertice);

    // Pega todos os vizinhos do proximo vertice para achar a proxima aresta de suporte
    size_t vertice_destino = get_destino(d, atual);
    std::vector<size_t> incidentes = vertex(d, vertice_destino);

    size_t proxima_aresta = INVALID_INDEX;
    for (size_t sa_incidente : incidentes)
    {
      // Nao volta pela aresta que acabou de chegar
      if (sa_incidente == d.mapa_sa[atual].par)
        continue;

      t_coord n3_esq = d.mapa_faces[d.mapa_sa[sa_incidente].ind_face].n3;
      t_coord n3_dir = d.mapa_faces[d.mapa_sa[d.mapa_sa[sa_incidente].par].ind_face].n3;

      if (n3_esq * n3_dir <= EPS && n3_esq >= 0.0)
      {
        proxima_aresta = sa_incidente;
        break;
      }
    }

    atual = proxima_aresta;

  } while (atual != aresta_inicial && atual != INVALID_INDEX);

  return contorno;
}

// Testa de que lado o ponto esta em relacao a uma linha
// Produto vetorial 2D para determinar o lado da linha
// Aqui consideramos que a navegacao eh sempre sem sentido anti-horario
// Se o prod vetorial > 0 p esta dentro
// Se o prod vetorial = 0 p esta na borda, em cima da aresta
// Se o prod vetorial < 0 p esta fora
bool dentro_do_plano_de_corte(ponto_2d p, ponto_2d p1, ponto_2d p2)
{
  return ((p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x) >= -EPS);
}

//==================================================
// CALCULO DO PONTO P* VIA CLIPPER2
//==================================================

// Fator de multiplicacao para conversao de ponto flutuante para int64_t na Clipper2
constexpr double CLIPPER_SCALE = 1e6;

// Calcula a intersecao 2D dos poligonos projetados usando a biblioteca Clipper2.
// Retorna um pair, indicando qual o poligono com o vertice na interseccao e qual o indice do vertice
// Ex: Vertice em A "return {1, n}", Vertice em B "return {2, n}". Sendo N o indice do vertice do poligono_2d_t
// Se a interseccao for nula, o pair retornado eh (0, 0)
// CASO ESPECIAL: Quando houver interseccao, mas nenhum vertice de A* ou B* estiver na interseccao, um novo vertice tem que ser adicionado a DCEL
// O pair devolvido tera o pirmeiro valor igual a 3, indicando que caiu nesse caso especial, o segundo campo sera 0,
//  e as coordenadas do ponto de cruzamento de aresta estara no terceiro parametro
std::pair<uint8_t, std::size_t> encontrar_ponto_p_estrela(const poligono_2d_t &poligonoA, const poligono_2d_t &poligonoB, ponto_2d &p_estrela)
{
  Clipper2Lib::Path64 pathA, pathB;

  // Converte Poligono A para o formato int64 da Clipper2
  for (const ponto_2d &p : poligonoA.vertices){
    pathA.push_back(Clipper2Lib::Point64(std::round(p.x * CLIPPER_SCALE), std::round(p.y * CLIPPER_SCALE)));
  }

  // Converte Poligono B para o formato int64 da Clipper2
  for (const ponto_2d &p : poligonoB.vertices){
    pathB.push_back(Clipper2Lib::Point64(std::round(p.x * CLIPPER_SCALE), std::round(p.y * CLIPPER_SCALE)));
  }

  Clipper2Lib::Paths64 subject, clip, solution;
  subject.push_back(pathA);
  clip.push_back(pathB);

  // Executa a Intersecao -> regra NonZero
  solution = Clipper2Lib::Intersect(subject, clip, Clipper2Lib::FillRule::NonZero);

  // Se nao ha colisao, os poliedros estao separados
  if (solution.empty() || solution[0].empty()){
    return {0, 0}; // Esse par indica que nao ha interseccao
  }

  // Tolerancia baseada na escala de arredondamento
  double epsilon_clipper = 1.0 / CLIPPER_SCALE;

  // Percorre todos os pontos do poligono de intersecao gerado para tentar encontrar um vertice real
  // Fazemos isso para tentar evitar a criacao de um vertrice virtualna DCEL
  for (const Clipper2Lib::Point<int64_t> &p_clipper : solution[0])
  {
    // Restaura a precisao de ponto flutuante para o ponto atual da iteracao
    p_estrela.x = static_cast<t_coord>(p_clipper.x) / CLIPPER_SCALE;
    p_estrela.y = static_cast<t_coord>(p_clipper.y) / CLIPPER_SCALE;

    // Verifica proximidade nos vertices originais de A*
    for (size_t i = 0; i < poligonoA.vertices.size(); ++i){
      double dist = std::hypot(poligonoA.vertices[i].x - p_estrela.x, poligonoA.vertices[i].y - p_estrela.y);
      if (dist <= epsilon_clipper){
        return {1, poligonoA.indices_originais[i]}; // Encontrou em A, retorna imediatamente
      }
    }

    // Verifica proximidade nos vertices originais de B*
    for (size_t i = 0; i < poligonoB.vertices.size(); ++i){
      double dist = std::hypot(poligonoB.vertices[i].x - p_estrela.x, poligonoB.vertices[i].y - p_estrela.y);
      if (dist <= epsilon_clipper){
        return {2, poligonoB.indices_originais[i]}; // Encontrou em B, retorna imediatamente
      }
    }
  }

  // Se o loop terminar sem retornar, significa que NENHUM vertice do poligono de intersecao
  // coincide com os vertices originais. Estamos lidando com um cruzamento puro de arestas.
  // Assumimos o primeiro ponto gerado como o nosso p* virtual.
  p_estrela.x = static_cast<t_coord>(solution[0][0].x) / CLIPPER_SCALE;
  p_estrela.y = static_cast<t_coord>(solution[0][0].y) / CLIPPER_SCALE;

  return {3, 0};
}

//==================================================
// RAY CASTING -> PRE IMAGEM
//==================================================

// Calculo da PRE IMAGEM do ponto de interseccao encontrado na projecao 2D
// Dispara um raio vertical a partir do ponto p (x,y) e encontra os furos na casca do poliedro
// Retorna uma tupla, onde o primeiro booleano diz se sobreposicao das pre imagens e valida
// O segundo pair contem respectivamente o limite superior e inferior do segmento de
//  reta vertical em p_estrela que esta dentro da interseccao dos dois poliedros
std::pair<bool, std::pair<double, double>> intervalo_pre_img(dcel_t *d1, dcel_t *d2, ponto_2d p_estrela)
{
  /*Varre a dcel d1 e verifica as duas faces que sao cortadas pela reta vertical em Z que passa pelo ponto p*
    Salva as duas faces
    Varre a dcel d2 e verifica as duas faces que sao cortadas pela reta vertical em Z que passa pelo ponto p*
    Salva as duas faces
    As 4 faces sao "furadas" pela reta no eixo Z que passa por p*, esses furos estao na mesma reta
    Entao ordenamos esses pontos no eixo Z
    Se obtivermos uma ordem como p_inf_d1 <= p_inf_d2 < p_sup_d1 <= p_sup_d2 (olha a coord Z desses 4 pontos)
    Isso significa que o segmento de reta que passa por p* e tem e eh delimitada por p_inf_d1 e p_sup_d2, esta garantidamente na intersccao dos poliedros d1 e d2
  */
  // Define a eq da reta do raycasting q passa pelo ponto p*
  ponto_3d pz_raio{p_estrela.x, p_estrela.y, 0.0}; // Ponto de origem do raio
  ponto_3d vz_raio{0.0, 0.0, 1};                   // Vetor direção do raio

  // Inicia os limites do segmento de reta da interseccao
  double amin{-(std::numeric_limits<double>::infinity())}, amax{std::numeric_limits<double>::infinity()};

  // Itera sobre as faces da dcel
  for (size_t i = 0; i < d1->mapa_faces.size(); i++)
  {
    // produto escalar entre a normal da face e o vetor direção do raio
    double denominador = prod_escalar(d1->mapa_faces[i], vz_raio);

    // produto escalar entre a normal da face e o vetor direção do raio
    // pega um vertice da face
    ponto_3d vet_aux{d1->mapa_vertices[d1->mapa_sa[d1->mapa_faces[i].ind_sa_inicial].ind_vertice].pos};
    // cria um vtor que sai da origem do raio até esse vertice na face
    vet_aux.x = vet_aux.x - pz_raio.x;
    vet_aux.y = vet_aux.y - pz_raio.y;
    vet_aux.z = vet_aux.z - pz_raio.z;

    double numerador = prod_escalar(d1->mapa_faces[i], vet_aux);

    // Se a reta for paralela ao plano da face
    if (std::abs(denominador) < EPS){
      //Se o numerador for positivo, a reta inteira está do lado de FORA deste plano
			if (numerador > EPS){ //AQUI TEM UM ERRO DE PRECISAO EM ALGUNS CASOS
        return {false, {0.0, 0.0}}; 
      }
      continue; // Paralela do lado de dentro, continua testando as outras faces
    }

    double t = numerador / denominador;

    if (denominador > EPS){
      // Reta e normal no mesmo sentido -> Ponto de ENTRADA
      if (t > amin){
        amin = t; // Estreita o limite de entrada
      }
    }
    else{
      // Reta e normal em sentidos opostos -> Ponto de SAÍDA
      if (t < amax){
        amax = t; // Estreita o limite de saída
      }
    }

    //Se o intervalo colapsar, a reta não passa por dentro do objeto
    //Passa na projeçao dos planos das faces
    if (amin > amax + EPS){
      return {false, {0.0, 0.0}};
    }
  } // iteracao dcel 1

  //amin e amax guardam os limites a' e a''

  //REPETE O PROCESSO PARA O SEGUNDO POLIGONO, EM D2
  // Inicia os limites do segmento de reta da interseccao
  double bmin{-(std::numeric_limits<double>::infinity())}, bmax{std::numeric_limits<double>::infinity()};

  // Itera sobre as faces da dcel
  for (size_t i = 0; i < d2->mapa_faces.size(); i++)
  {
    // produto escalar entre a normal da face e o vetor direção do raio
    double denominador = prod_escalar(d2->mapa_faces[i], vz_raio);

    //produto escalar entre a normal da face e o vetor direção do raio
    //pega um vertice da face
    ponto_3d vet_aux{d2->mapa_vertices[d2->mapa_sa[d2->mapa_faces[i].ind_sa_inicial].ind_vertice].pos};
    // cria um vtor que sai da origem do raio até esse vertice na face
    vet_aux.x = vet_aux.x - pz_raio.x;
    vet_aux.y = vet_aux.y - pz_raio.y;
    vet_aux.z = vet_aux.z - pz_raio.z;

    double numerador = prod_escalar(d2->mapa_faces[i], vet_aux);

    // Se a reta for paralela ao plano da face
    if (std::abs(denominador) < EPS)
    {
      //Se o numerador for positivo, a reta inteira está do lado de FORA deste plano
			if (numerador > 0.0){
        std::cout<<"GATPO";
        return {false, {0.0 , 0.0}}; 
      }
      continue; // Paralela do lado de dentro, continua testando as outras faces
    }

    double t = numerador / denominador;

    if (denominador > 0.0){
      // Reta e normal no mesmo sentido -> Ponto de ENTRADA
      if (t > bmin){
        bmin = t; // Estreita o limite de entrada
      }
    }
    else{
      // Reta e normal em sentidos opostos -> Ponto de SAÍDA
      if (t < bmax){
        bmax = t; // Estreita o limite de saída
      }
    }

    //Se o intervalo colapsar, a reta não passa por dentro do objeto
    //Passa na projeçao dos planos das faces
    if (bmin > bmax + EPS){
      std::cout<<"BOLA";
      return {false, {0.0 , 0.0}};
    }
  } // iteracao dcel 2

  //bmin e bmax guardam b' e b''

  //Verifica sobreposição dos dois segmentos de reta
  //retorna o segmento de reta que está dentro dos dois poligonos ao mesmo tempo
  double t_entrada_comum = std::max(amin, bmin);
  double t_saida_comum   = std::min(amax, bmax);

  std::cout<<"Amin: "<<amin<<"Amax: "<<amax<<"Bmin: "<<bmin<<"Bmax: "<<bmax<<"\n";

  // Se o ponto de entrada comum for maior que o de saida, não há sobreposição
  if (t_entrada_comum > t_saida_comum){
    return {false, {0.0 , 0.0}};
  }

  //Reconstroi os pontos 3D  de entrada e saida do raio usando a equação parametrica do raio (P = P0 + t * V)
  //Envia a coordenada no eixo Z 
  return {true, {(pz_raio.z + t_entrada_comum), (pz_raio.z + t_saida_comum)}};
}

//Encontra uma das arestas que geraram o vertice virtual par enconntrar o p*
//Triangula as duas faces dessa aresta
//Divide a aresta no meio, dividindo cada face em duas
//Adiciona o novo vertice na dcel, e retorna qual dcel e qual o indice do novo vertice
//std::pair<uint8_t, size_t> adiciona_vertice_virtual(ponto_2d p_estrela, const poligono_2d_t& polA, const poligono_2d_t& polB, dcel_t* d1, dcel_t* d2){}


































