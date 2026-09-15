/*
Essa biblioteca contem funcoes para implementar por completo a estrutura da Doubly Connected Edge List
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/
#ifndef __DCELLIB__
#define __DCELLIB__

#include "geometria.hpp"

#include <unordered_map>
#include <vector>

using namespace std; //Para simplificar o uso da stl

//Estrutura gerada ao ler os arquivos com as malhas
//Usado para gerar a DCEL
struct descritor_dcel{
  vector<ponto_3d> vertices;
  vector<vector<uint64_t>> faces;
};






#endif