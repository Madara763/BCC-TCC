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
  //lista de arestas que delimitam o semiespaco da face
  //Guarda os indices das semiarestas da face no mapa_
  vector<uint64_t> borda; 
  t_ponto n1{0.0}, n2{0.0}, n3{0.0}; //coord do vetor normal da face
} face;

struct semi_aresta{

  static uint64_t proximo_id; //contador global para o id 

  uint64_t id; //identificador unico da semiaresta

  //Para identificar vertices e faces
  uint64_t ind_vertice;   // indice/chave do mapa_vert da DCEL
  uint64_t ind_face;      // indice/chave da mapa_face da DCEL

  //Indice/Chave das semiarestas no mapa_sa da DCEL
  uint64_t prox{0}; //next ptr
  uint64_t ante{0}; //previous prt
  uint64_t par{0};  //twin
  
  // Construtor
  semi_aresta() : id(proximo_id++) {} // cada instância recebe um ID unico

};

struct dcel{

  //Armazena todos os vertices do poliedro
  //Mantem a mesma relacao ponto/indice do descritor
  std::vector<ponto_3d> mapa_vertices;

  //Armazena todas as faces do poliedro
  //Mantem a mesma relacao ponto/indice do descritor
  std::vector<face> mapa_faces;

  //Armazena todas as semi arestas
  //A relacao de semi-aresta/indice eh dada pela ordem de criacao, baseada na ondem das faces
  std::vector<semi_aresta> mapa_sa;

  //Criar metodos para acesso direto 

}


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