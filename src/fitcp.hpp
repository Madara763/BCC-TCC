/*
Essa biblioteca contem funcoes para executar o calculo de interseccao de muller e preparata(1978)
Funciona em cima da implementacao propria da DCEL
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
poligono_2d_t extrai_poligono_projecao(const dcel_t& d);


// Testa de que lado o ponto esta em relacao a uma linha
// Produto vetorial 2D para determinar o lado da linha
// Aqui consideramos que a navegacao eh sempre sem sentido anti-horario
// Se o prod vetorial > 0 p esta dentro
// Se o prod vetorial = 0 p esta na borda, em cima da aresta
// Se o prod vetorial < 0 p esta fora
bool dentro_do_plano_de_corte(ponto_2d p, ponto_2d p1, ponto_2d p2);

//Calcula a intersecao 2D dos poligonos projetados usando a biblioteca Clipper2.
//Retorna um pair, indicando qual o poligono com o vertice na interseccao e qual o indice do vertice 
//Ex: Vertice em A "return {1, n}", Vertice em B "return {2, n}". Sendo N o indice do vertice do poligono_2d_t
//Se a interseccao for nula, o pair retornado eh (0, 0)
//CASO ESPECIAL: Quando houver interseccao, mas nenhum vertice de A* ou B* estiver na interseccao
//O pair devolvido tera o pirmeiro valor igual a 3, indicando que caiu nesse caso especial, o segundo campo sera 0,
// e as coordenadas do ponto de cruzamento de aresta estara no terceiro parametro
std::pair<uint8_t, std::size_t> encontrar_ponto_p_estrela(const poligono_2d_t& poligonoA, const poligono_2d_t& poligonoB, ponto_2d& p_estrela);

//Calculo da PRE IMAGEM do ponto de interseccao encontrado na projecao 2D
//Dispara um raio vertical a partir do ponto p (x,y) e encontra os furos na casca do poliedro
//Retorna uma tupla, onde o primeiro booleano diz se sobreposicao das pre imagens e valida
//O segundo pair contem respectivamente o limite superior e inferior do segmento de 
// reta vertical em p_estrela que esta dentro da interseccao dos dois poliedros
std::pair<bool,std::pair<double, double>> intervalo_pre_img(dcel_t* d1, dcel_t* d2, ponto_2d p_estrela);

//Encontra uma das arestas que geraram o vertice virtual par enconntrar o p*
//Triangula as duas faces dessa aresta
//Divide a aresta no meio, dividindo cada face em duas
//Adiciona o novo vertice na dcel, e retorna qual dcel e qual o indice do novo vertice
//std::pair<uint8_t, size_t> adiciona_vertice_virtual(ponto_2d p_estrela, const poligono_2d_t& polA, const poligono_2d_t& polB, dcel_t* d1, dcel_t* d2);

#endif
