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
#include "Clipper2Lib/include/clipper2/clipper.h"

#include <cmath>
#include <limits>
#include <utility> 
#include <cstdint>

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

poligono_2d_t extrai_poligono_projecao(const dcel_t& d) {
	poligono_2d_t contorno;
	size_t aresta_inicial = INVALID_INDEX;

  //Aqui olhamos para o poliedro e nao para a dcel em si
  //Vamos procurar as arestas do poliedro que estao na "borda"
  //A partir de uma aresta (uma semi-aresta e seu par) vamos comparar as duas faces 
  //E verificar a inclinacao de uma face em relacao a outra
	//Regra do Artigo: n3(f') * n3(f'') <= 0
	for(size_t i = 0; i < d.mapa_sa.size(); ++i) {
		const semi_aresta_t& sa = d.mapa_sa[i];
		if(sa.par == INVALID_INDEX) continue;

		t_coord n3_esq = d.mapa_faces[sa.ind_face].n3;
		t_coord n3_dir = d.mapa_faces[d.mapa_sa[sa.par].ind_face].n3;

		if(n3_esq * n3_dir <= EPS) {
			//Garante uma orientacao de caminhada com o  topo(ordenadno pelo eixo z) sempre a esquerda
			if(n3_esq >= 0.0) {
				aresta_inicial = i;
				break;
			}
		}
	}

  //Ao encontrar uma aresta no contorno (o equador) do poliedro
  //Podemos caminhar a partir dela para montar o poligono do contorno

	if(aresta_inicial == INVALID_INDEX) return contorno;

	//Caminha pelo ciclo do contorno do poliedro convexo
	size_t atual = aresta_inicial;
	do {
		//Salva o vertice projetado 2D
		size_t ind_vertice = d.mapa_sa[atual].ind_vertice;
		ponto_3d pos3d = d.mapa_vertices[ind_vertice].pos;
		contorno.vertices.push_back({pos3d.x, pos3d.y});
		contorno.indices_originais.push_back(ind_vertice);

		//Pega todos os vizinhos do proximo vertice para achar a proxima aresta de suporte
		size_t vertice_destino = get_destino(d, atual);
		std::vector<size_t> incidentes = vertex(d, vertice_destino);

		size_t proxima_aresta = INVALID_INDEX;
		for(size_t sa_incidente : incidentes) {
			
      //Nao volta pela aresta que acabou de chegar
			if(sa_incidente == d.mapa_sa[atual].par) continue;

			t_coord n3_esq = d.mapa_faces[d.mapa_sa[sa_incidente].ind_face].n3;
			t_coord n3_dir = d.mapa_faces[d.mapa_sa[d.mapa_sa[sa_incidente].par].ind_face].n3;

			if(n3_esq * n3_dir <= EPS && n3_esq >= 0.0) {
				proxima_aresta = sa_incidente;
				break;
			}
		}

		atual = proxima_aresta;

	} while(atual != aresta_inicial && atual != INVALID_INDEX);

	return contorno;
}


//SERIA USADO NO SHAMOSHOEY, ATUALEMENTE SEM USO
// Testa de que lado o ponto esta em relacao a uma linha
// Produto vetorial 2D para determinar o lado da linha
// Aqui consideramos que a navegacao eh sempre sem sentido anti-horario
// Se o prod vetorial > 0 p esta dentro
// Se o prod vetorial = 0 p esta na borda, em cima da aresta
// Se o prod vetorial < 0 p esta fora
// bool dentro_do_plano_de_corte(ponto_2d p, ponto_2d p1, ponto_2d p2) {
// 	return ( (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x) >= -EPS);
// }

//==================================================
// CALCULO DO PONTO P* VIA CLIPPER2
//==================================================

// Fator de multiplicacao para conversao de ponto flutuante para int64_t na Clipper2
constexpr double CLIPPER_SCALE = 1e6;

//Calcula a intersecao 2D dos poligonos projetados usando a biblioteca Clipper2.
//Retorna um pair, indicando qual o poligono com o vertice na interseccao e qual o indice do vertice 
//Ex: Vertice em A "return {1, n}", Vertice em B "return {2, n}". Sendo N o indice do vertice do poligono_2d_t
//Se a interseccao for nula, o pair retornado eh (0, 0)
//CASO ESPECIAL: Quando houver interseccao, mas nenhum vertice de A* ou B* estiver na interseccao, um novo vertice tem que ser adicionado a DCEL
//O pair devolvido tera o pirmeiro valor igual a 3, indicando que caiu nesse caso especial, o segundo campo sera 0,
// e as coordenadas do ponto de cruzamento de aresta estara no terceiro parametro
std::pair<uint8_t, std::size_t> encontrar_ponto_p_estrela(const poligono_2d_t& poligonoA, const poligono_2d_t& poligonoB, ponto_2d& p_estrela) {
	Clipper2Lib::Path64 pathA, pathB;

	//Converte Poligono A para o formato int64 da Clipper2
	for(const ponto_2d& p : poligonoA.vertices){
		pathA.push_back(Clipper2Lib::Point64(std::round(p.x * CLIPPER_SCALE), std::round(p.y * CLIPPER_SCALE)));
	}

	//Converte Poligono B para o formato int64 da Clipper2
	for(const ponto_2d& p : poligonoB.vertices){
		pathB.push_back(Clipper2Lib::Point64( std::round(p.x * CLIPPER_SCALE), std::round(p.y * CLIPPER_SCALE)));
	}


	Clipper2Lib::Paths64 subject, clip, solution;
	subject.push_back(pathA);
	clip.push_back(pathB);

	//Executa a Intersecao -> regra NonZero
	solution = Clipper2Lib::Intersect(subject, clip, Clipper2Lib::FillRule::NonZero);

	//Se nao ha colisao, os poliedros estao separados
	if(solution.empty() || solution[0].empty()) {
		return {0,0}; //Esse par indica que nao ha interseccao
	}

	//Extrai o primeiro vertice da area de sobreposicao
	Clipper2Lib::Point64 p_clipper = solution[0][0];

	// Restaura a precisao de ponto flutuante
	p_estrela.x = static_cast<t_coord>(p_clipper.x) / CLIPPER_SCALE;
	p_estrela.y = static_cast<t_coord>(p_clipper.y) / CLIPPER_SCALE;

	//Busca qual indice original da DCEL corresponde a p*
	//Para que retorne o indice do vertice na dcel
	double menor_distancia = std::numeric_limits<double>::max();
	size_t id_dcel_encontrado = INVALID_INDEX;
	uint8_t id_poligono = 0;

	//Tolerancia baseada na escala de arredondamento
	double epsilon_clipper = 1.0 / CLIPPER_SCALE;

	//Verifica proximidade nos vertices originais de A*
	for(size_t i = 0; i < poligonoA.vertices.size(); ++i) {
		double dist = std::hypot(poligonoA.vertices[i].x - p_estrela.x, poligonoA.vertices[i].y - p_estrela.y);
		if(dist < menor_distancia) {
			menor_distancia = dist;
			id_dcel_encontrado = poligonoA.indices_originais[i];
			id_poligono = 1;
		}
	}

	//Verifica proximidade nos vertices originais de B*
	for(size_t i = 0; i < poligonoB.vertices.size(); ++i) {
		double dist = std::hypot(poligonoB.vertices[i].x - p_estrela.x, poligonoB.vertices[i].y - p_estrela.y);
		if(dist < menor_distancia) {
			menor_distancia = dist;
			id_dcel_encontrado = poligonoB.indices_originais[i];
			id_poligono = 2;
		}
	}

	// Se a distancia para o vertice real mais proximo for maior que o erro de conversao,
  // significa que o ponto gerado pela Clipper2 eh o cruzamento exato entre duas arestas.
  if(menor_distancia > epsilon_clipper) {
    // Como p_estrela eh apenas uma projecao 2D, nao podemos inseri-lo na DCEL agora,
    // pois a coordenada Z exata ainda sera descoberta pelo "ray casting" vertical.
    // Usamos o id_poligono = 3 um ponto virtual.
    id_poligono = 3; 
    id_dcel_encontrado = INVALID_INDEX;
  }

	return {id_poligono, id_dcel_encontrado };
}
