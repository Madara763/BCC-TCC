/*
Essa lib contem funcoes para testar a DCEL e a FITCP
Tem o objetivo de explanar o funcionamento do artigo "Finding the Intersection of Two Convex Polyhedra" e da DCEL
Criado por: Davi Lazzarin
Data: 29/09/2026
*/
#ifndef __DEBUGLIB__
#define __DEBUGLIB__

#include <iostream>

#include "geometria.hpp"
#include "dcel.hpp"
#include "fitcp.hpp"
#include "Clipper2Lib/include/clipper2/clipper.h"

//========================================
//Define funcoes de debug para a DCEL
//========================================

//Funcao para testar e imprimir o contorno de todas as faces
void debug_dcel_metodo_face(const dcel_t* d);

//Funcao para testar e imprimir as arestas incidentes de cada vertice
void debug_dcel_metodo_vertex(const dcel_t* d);

//Triangula a DCEL inteira, exibe na TELA informacoes sobre a DCEL
void debug_triangula_e_valida_dcel(dcel_t* d);

//Recebe as projecoes e imprime na tela se ha ou nao a interseccao e qual vertice corresponde ao p*
void debug_teste_interseccao_2d(const poligono_2d_t& pol1, const poligono_2d_t& pol2);


#endif