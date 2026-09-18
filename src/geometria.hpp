/*
Essa biblioteca contem funcoes e declaracoes auxiliares para uso na DCEL
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/

#ifndef __GEOMETRIALIB__
#define __GEOMETRIALIB__

#include <vector>
#include <cstdint>
#include <ostream>
#include <limits>
#include <cmath>

using namespace std; //Para simplificar o uso da stl

//========================================
// Defines dos tipos
//========================================

//Alterar o tipo confome implementacao
#define t_coord double 
const double EPS = 1E-9;
constexpr size_t INVALID_INDEX = numeric_limits<size_t>::max();

typedef struct{
  t_coord x{0.0}, y{0.0};
} ponto_2d;

typedef struct{
  t_coord x{0.0}, y{0.0}, z{0.0};
} ponto_3d;

typedef struct{
  ponto_3d pos;
  size_t sa_incidente{INVALID_INDEX}; //Indice de uma semiaresta que inicia nesse vertice
} vertice_3d_t;

typedef struct{
  ponto_2d ini;
  ponto_2d fim;
} aresta_2d;

typedef struct{
  vertice_3d_t ini; 
  vertice_3d_t fim;
} aresta_3d;

typedef struct{
  //Guarda o indice da semiaresta inicial
  size_t ind_sa_inicial{INVALID_INDEX};
  t_coord n1{0.0}, n2{0.0}, n3{0.0}; //coord do vetor normal da face
  t_coord d{0.0}; //Constante da eq do plano
} face_t;

typedef struct{

  //Para identificar vertices e faces
  size_t ind_vertice{INVALID_INDEX};   // indice do mapa_vert da DCEL
  size_t ind_face{INVALID_INDEX};      // indice da mapa_face da DCEL
 
  //Indice das semiarestas no mapa_sa da DCEL
  size_t prox{INVALID_INDEX}; //next ptr
  size_t ante{INVALID_INDEX}; //previous prt
  size_t par{INVALID_INDEX};  //twin
}semi_aresta_t;

//========================================
// Sobrecargas para Impressao
//========================================


// Sobrecarga para impressao do ponto 
inline std::ostream& operator<<(std::ostream& os, const ponto_2d& p) {
  #ifdef DEBUG
  os << "(" << p.x << ", " << p.y << ") ";
  #else
  os << p.x <<" "<< p.y ;
  #endif
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const ponto_3d& p) {
  #ifdef DEBUG
  os << "(" << p.x << ", " << p.y << ", " << p.z << ") ";
  #else
  os << p.x <<" "<< p.y <<" "<< p.z ;
  #endif
  return os;
}



//========================================
// Funcao hash para o unordered_map
//========================================

struct hash_pair {
  std::size_t operator()( const std::pair<uint64_t, uint64_t>& p ) const { 
    std::size_t h1 = std::hash<uint64_t>{}(p.first);
    std::size_t h2 = std::hash<uint64_t>{}(p.second);
    return h1 ^ (h2 << 1); 
  } 
};

#endif