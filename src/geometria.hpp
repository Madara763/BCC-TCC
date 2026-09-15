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

using namespace std; //Para simplificar o uso da stl

//========================================
// Defines dos tipos
//========================================

//Alterar o tipo confome implementacao
#define t_ponto double 
const double EPS = 1E-9;


typedef struct{
  t_ponto x{0.0}, y{0.0};
} ponto_2d;

typedef struct{
  t_ponto x{0.0}, y{0.0}, z{0.0};
} ponto_3d;

typedef struct {
  ponto_2d ini;
  ponto_2d fim;
} aresta_2d;

typedef struct {
  ponto_3d ini;
  ponto_3d fim;
} aresta_3d;

typedef struct {
  vector<aresta_3d> borda; //lista de arestas que delimitam o semiespaco da face
  t_ponto n1{0.0}, n2{0.0}, n3{0.0}; //coord do vetor normal da face
} face;

struct semi_aresta{

  static uint64_t proximo_id; //contador global para o id 

  uint64_t id; //identificador unico da semiaresta

  ponto_3d ini; //vertice onde inicia a semi aresta

  semi_aresta *prox{nullptr}; //next ptr
  semi_aresta *ante{nullptr}; //previous prt
  semi_aresta *par{nullptr};  //twin
  face f;
  
  // Construtor
  semi_aresta() : id(proximo_id++) {} // cada instância recebe um ID unico

};

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
// Defines das Funcoes
//========================================


#endif