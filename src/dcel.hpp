/*
Essa biblioteca contem funcoes para implementar por completo a estrutura da Doubly Connected Edge List
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/
#ifndef __DCELLIB__
#define __DCELLIB__

#include "geometria.hpp"

#include <unordered_map>

//para debug
#include <iostream>


//========================================
// Defines dos tipos
//========================================

//Estrutura gerada ao ler os arquivos com as malhas
//Usado para gerar a DCEL
//Nao precisa de destrutor explicito
struct descritor_dcel{
  std::vector<ponto_3d> pontos;
  std::vector<vector<uint64_t>> faces;
};

struct dcel_t{

  //Armazena todos os vertices do poliedro
  //Mantem a mesma relacao ponto/indice do descritor
  std::vector<vertice_3d_t> mapa_vertices;

  //Armazena todas as faces do poliedro
  //Mantem a mesma relacao ponto/indice do descritor
  std::vector<face_t> mapa_faces;

  //Armazena todas as semi arestas
  //A relacao de semi-aresta/indice eh dada pela ordem de criacao, baseada na ondem das faces
  std::vector<semi_aresta_t> mapa_sa;

};

//========================================
// Sobrecarga impressao da DCEL - PARA DEBUG
//========================================


inline std::ostream& operator<<(std::ostream& os, const dcel_t& dcel){

  os << "\n";
  os << "========== DCEL ==========\n\n";
  os << "VERTICES\n";

  for (size_t i = 0; i < dcel.mapa_vertices.size(); ++i)
  {
    const vertice_3d_t& v = dcel.mapa_vertices[i];
    os << "[" << i << "] ";
    os << "ponto=("<< v.pos.x << ", "<< v.pos.y << ", "<< v.pos.z << ")";
    os << "\n";
  }

  os << "\n";
  os << "FACES\n";

  for (size_t i = 0; i < dcel.mapa_faces.size(); ++i)
  {
    const face_t& f = dcel.mapa_faces[i];

    os << "[" << i << "] ";
    os << "SA inicial=";
    if (f.ind_sa_inicial == INVALID_INDEX)
      os << "INVALID";
    else
      os << f.ind_sa_inicial;

    os << "\n";

    os << "    Normal=("<< f.n1 << ", "<< f.n2 << ", "<< f.n3 << ")\n";
    os << "    Plano: "<< f.n1 << "x + "<< f.n2 << "y + "<< f.n3 << "z + "<< f.d<< " = 0\n";
  }

  os << "\n";
  os << "SEMI-ARESTAS\n";

  for (size_t i = 0; i < dcel.mapa_sa.size(); ++i)
  {
    const semi_aresta_t& sa = dcel.mapa_sa[i];

    os << "[" << i << "] ";
    os << "vertice=";
    if (sa.ind_vertice == INVALID_INDEX)
      os << "INVALID";
    else
      os << sa.ind_vertice;

    os << " face=";

    if (sa.ind_face == INVALID_INDEX)
      os << "INVALID";
    else
      os << sa.ind_face;

    os << " prox=";

    if (sa.prox == INVALID_INDEX)
      os << "INVALID";
    else
      os << sa.prox;

    os << " ante=";

    if (sa.ante == INVALID_INDEX)
      os << "INVALID";
    else
      os << sa.ante;

    os << " par=";

    if (sa.par == INVALID_INDEX)
      os << "INVALID";
    else
      os << sa.par;

    os << "\n";
  }

  os << "\n";
  os << "==========================\n";

  return os;
}


//========================================
// Defines das Funcoes
//========================================

//Cria uma dcel a partir do descritor
dcel_t* cria_dcel(descritor_dcel* descritor);

//Recebe uma face com seus vertices e semi-arestas corretos
//Calcula a equacao do plano preenchendo a propria face
void calcula_eq_plano_face(dcel_t* d, face_t& face_nova);

//Metodos do artigo

//Retorna as semi-arestas que saem do vertice, em anti-horario
std::vector<size_t> vertex(const dcel_t& d, size_t ind_vertice);

//Retorna as meia arestas que limitam a face
std::vector<size_t> face(const dcel_t& d, size_t ind_face);

//Obtem o vertice onde a semi-aresta acaba
//O inicio da proxima, ou o inicio da twin
inline size_t get_destino(const dcel_t& d, size_t ind_sa);

//Obtem o indice da face a direita
//Como cada semi-aresta guarda a face a esquerda
//Retorna a face a esquerda do twin da semi-aresta
inline size_t get_face_direita(const dcel_t& d, size_t ind_sa);

//Retorna os vertices do contorno da face
std::vector<size_t> get_vertices_face(const dcel_t& d, size_t ind_fa);

//Retorna true se a face eh triangular
bool eh_face_triangular(const dcel_t& d, size_t ind_face);


//Copia o vetor de pontos do descritor para o vetor de vertices da DCEL
//Os vertices nao possuem indice da semi-aresta incidente
void copia_pontos_para_vertices(descritor_dcel* descritor, dcel_t* d);


//========================================
//Define funcoes de debug
//========================================

//Funcao para testar e imprimir o contorno de todas as faces
void debug_dcel_metodo_face(const dcel_t* d);

//Funcao para testar e imprimir as arestas incidentes de cada vertice
void debug_dcel_metodo_vertex(const dcel_t* d);

#endif