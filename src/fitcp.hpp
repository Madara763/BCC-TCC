/*
Essa biblioteca contem funcoes para executar o calculo de interseccao de muller e preparata(1978)
Funciona em cima da implementacao propria da DCEL
Define alguns tipos para empacotar os retornos de cada passo do algortimo
fitcp vem do nome do artigo "Finding the Intersection of Two Convex Polyhedra"
Criado por: Davi Lazzarin 
Data: 19/09/2026
*/
#ifndef __FITCPLIB__
#define __FITCPLIB__

#include "geometria.hpp"
#include "dcel.hpp"

#include <utility> 
#include <cstdint>

//========================================
// Tipos para retorno das etapas do fitcp
//========================================

//Tipo de retorno do passo 1, projecao 2d em (X,Y)
typedef struct{
  std::vector<ponto_2d> vertices;
  std::vector<size_t> indices_originais; //Rastreia o vertice original da DCEL
} projecao_poligono_2d_t;

//Tipo de retorno do passo 2, busca da interseccao nos poligonos projetados
typedef struct{
  //Indica de onde veio o ponto p*, se eh um vertice real ou se veio de cruzamento de arestas
  //1 se p* eh coincide com um vertice da dcel A, 2 da B
  //3 se o popnto p* veiod e cruzamento de arestas e nao pode ser mapeado em um vertice real
  //E 0 se nao ha ponto p* na interseccao da projecao 2D
  uint8_t cod_origem{0};
  size_t ind_original{INVALID_INDEX}; //Se cod_origem for 1 ou 2, contem o indice do vertice na DCEL
  ponto_2d p_estrela{0.0, 0.0}; //Cordenadas (X,Y) do ponto, quando existir
} interseccao_2d_t;

//Tipo de retorno do passo 3
//Calula a pre imagem do ponto p*
typedef struct{
  bool existe_sobreposicao{false}; //indica se existe uma sobreposicao nos segmentos verticais em A e B
  double limite_sup{0.0}; //Altura no eixo Z 
  double limite_inf{0.0}; //Altura no eixo Z
  //Quando true, indica que o poliedro A, esta acima do B quando ordenado no eixo Z
  //Usaremos para construir o near-side no proximo passo quando a sobreposicao for invalida em p*
  bool pol_a_esta_acima{true}; 

} analise_interseccao_vertical_t;


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
projecao_poligono_2d_t extrai_poligono_projecao(const dcel_t& d);


// Testa de que lado o ponto esta em relacao a uma linha
// Produto vetorial 2D para determinar o lado da linha
// Aqui consideramos que a navegacao eh sempre sem sentido anti-horario
// Se o prod vetorial > 0 p esta dentro
// Se o prod vetorial = 0 p esta na borda, em cima da aresta
// Se o prod vetorial < 0 p esta fora
bool dentro_do_plano_de_corte(ponto_2d p, ponto_2d p1, ponto_2d p2);

//Calcula a intersecao 2D dos poligonos projetados usando a biblioteca Clipper2.
//Retorna informacoes sobre o calculo da interseccao em 2D
interseccao_2d_t encontrar_ponto_p_estrela(const projecao_poligono_2d_t& poligonoA, const projecao_poligono_2d_t& poligonoB);

//Calculo da PRE IMAGEM do ponto de interseccao encontrado na projecao 2D
//Dispara um raio vertical a partir do ponto p (x,y) e encontra os furos na casca do poliedro
//Retorna uma tupla, onde o primeiro booleano diz se sobreposicao das pre imagens e valida
//O segundo pair contem respectivamente o limite superior e inferior do segmento de 
// reta vertical em p_estrela que esta dentro da interseccao dos dois poliedros
analise_interseccao_vertical_t intervalo_pre_img(dcel_t* d1, dcel_t* d2, ponto_2d p_estrela);

//Encontra uma das arestas que geraram o vertice virtual par enconntrar o p*
//Triangula as duas faces dessa aresta
//Divide a aresta no meio, dividindo cada face em duas
//Adiciona o novo vertice na dcel, e retorna qual dcel e qual o indice do novo vertice
//std::pair<uint8_t, size_t> adiciona_vertice_virtual(ponto_2d p_estrela, const poligono_2d_t& polA, const poligono_2d_t& polB, dcel_t* d1, dcel_t* d2);

#endif
