/*
Essa biblioteca contem funcoes para implementar por completo a estrutura da Doubly Connected Edge List
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/
#include "dcel.hpp"

/*
Lembrete das estruturas em dcel.hpp

struct descritor_dcel{
  std::vector<ponto_3d> vertices;
  std::vector<vector<uint64_t>> faces;
};
struct dcel_t{
  std::vector<vertice_3d_t> mapa_vertices;
  std::vector<face_t> mapa_faces;
  std::vector<semi_aresta_t> mapa_sa;
};
*/

//Cria uma dcel a partir do descritor
dcel_t* cria_dcel(descritor_dcel* descritor){

  //Se o ponteiro para o descritor tiver vazio
  if(descritor == nullptr) return nullptr;

  //Variaveis auxiliares na montagem da DCEL
  std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, hash_pair> mapa_twin;
  vertice_3d_t ini, fim;
  semi_aresta_t sa_inicial;
  uint64_t ind_vertice_inicial, ind_sa_anterior, ind_sa_atual;

  //Aloca a DCEL
  dcel_t* d=new(dcel_t);

  //Todos os pontos do descritor viram vertices incompletos no mapa_vertices
  //Necessario para manter a mesma ordem dos pontos nos vertices
  copia_pontos_para_vertices(descritor, d);

  //==========Cria as faces==========
  for(std::vector<uint64_t> descritor_face : descritor->faces){

    face_t face_nova; //Cria uma nova face
    uint64_t indice_face_nova{d->mapa_faces.size()}; //Local onde essa face sera inserida

    //Vamos iterar sobre o descritor da face
    std::vector<uint64_t>::iterator iti=descritor_face.begin(); //Apontara para o vertice de inicio da semi-aresta
    std::vector<uint64_t>::iterator itf=descritor_face.begin(); //Apontara para o vertice final da semi-aresta

    //==========Cria as semi-arestas==========

    bool primeira_sa_da_face{true};
    
    //Salva os indices iniciais
    ind_vertice_inicial= *iti; //Usado para fechar a face

    //O for inicia no segundo vertice da face
    ++itf; // Aponta para o segundo indice (que e o final da primeira semi-aresta)

    std::cout<<"INICIANDO UMA FACE\n";

    for(; itf != descritor_face.end(); ++itf){

      std::cout<<"iti=> "<<(*iti)<<" itf=> "<<(*itf)<<"\n";

      semi_aresta_t sa_nova;
      ind_sa_atual = d->mapa_sa.size();

      //==========Faz isso para a primeira semi-aresta  da face==========
      if(primeira_sa_da_face){
        
        //Indice futuro da semi-aresta no mapa, nesse momento ainda nao tem nada nesse indice
        face_nova.ind_sa_inicial = ind_sa_atual; 

        ind_sa_anterior = ind_sa_atual; //Salva esse indice para usar na config da proxima

        //Verifica se o vertice inicial da semi-aresta ja ponta para alguma semi-aresta
        //Caso nao, aponta para a que esta sendo criada
        if(d->mapa_vertices[ind_vertice_inicial].sa_incidente == INVALID_INDEX)
          d->mapa_vertices[ind_vertice_inicial].sa_incidente = ind_sa_atual;
        
        //Preenche a semi-aresta
        sa_inicial.ind_vertice = ind_vertice_inicial;
        sa_inicial.ind_face = indice_face_nova;

        //Busca se existe uma semi-aresta par a essa
        std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, hash_pair>::iterator twin = mapa_twin.find({(*itf),(*iti)});
        if(twin != mapa_twin.end()){ //Encontrou uma semi-aresta par
          sa_inicial.par = twin->second;

          //Se existe uma sa twin dessa, essa eh a twin da outra
          //Aponta a twin da outra para essa
          d->mapa_sa[twin->second].par = ind_sa_atual;
        }

        //Como eh a primiera semi-aresta nao tem os ponteiros prox, anterior

        //Copia a primeira semi-aresta da face para o mapa de sa da DCEL
        d->mapa_sa.push_back(sa_inicial);

        primeira_sa_da_face = false;
      }//Fim da primeira semi-aresta
      else{  

        //Verifica se o vertice inicial da semi-aresta ja ponta para alguma semi-aresta
        //Caso nao, aponta para a que esta sendo criada
        if(d->mapa_vertices[(*iti)].sa_incidente == INVALID_INDEX)
          d->mapa_vertices[(*iti)].sa_incidente = ind_sa_atual;

        //Preenche a semi-aresta
        sa_nova.ind_vertice = (*iti);
        sa_nova.ind_face = indice_face_nova;
        sa_nova.ante = ind_sa_anterior;

        //Ajusta o proximo da anterior
        d->mapa_sa[ind_sa_anterior].prox = ind_sa_atual;

        //Busca se existe uma semi-aresta par a essa
        std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, hash_pair>::iterator twin = mapa_twin.find({(*itf),(*iti)});
        if(twin != mapa_twin.end()){ //Encontrou uma semi-aresta par
          sa_nova.par = twin->second;

          //Se existe uma sa twin dessa, essa eh a twin da outra
          //Aponta a twin da outra para essa
          d->mapa_sa[twin->second].par = ind_sa_atual;
        }
        
        //Quando nao eh a primeira da face, apenas o campo prox fica vazio
        //Sera preenchido na criacao na proxima    

        //Copia essa semi-aresta para o mapa de sa da DCEL
        d->mapa_sa.push_back(sa_nova);
      }//Fim das demais semi-arestas


      //Registra essa semi-aresta no mapa de semi-arestas
      mapa_twin[{(*iti), (*itf)}] = ind_sa_atual;

      ind_sa_anterior = ind_sa_atual;

      iti=itf; //Para iniciar a proxima semi-aresta
    }//For cria as semiarestas

    // std::cout<<"FECHANDO A FACE\n";

    // std::cout<<"iti=> "<<(*iti)<<" itf=> "<<(*itf)<<"\n";


    //Fecha a face e adiciona no mapa de faces da DCEL
    //Cria a semi-aresta do ultimo vertice para o primeiro
    semi_aresta_t sa_nova;
    ind_sa_atual = d->mapa_sa.size();

    sa_nova.ind_vertice = (*iti);
    sa_nova.ind_face = indice_face_nova;
    sa_nova.prox = face_nova.ind_sa_inicial; //Aponta para a primeira sa da face
    sa_nova.ante=ind_sa_anterior;

    d->mapa_sa[face_nova.ind_sa_inicial].ante = ind_sa_atual; 
    d->mapa_sa[ind_sa_anterior].prox = ind_sa_atual;

    //Busca se existe uma semi-aresta par a essa
    std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, hash_pair>::iterator twin = mapa_twin.find({(ind_vertice_inicial),(*iti)});
    if(twin != mapa_twin.end()){ //Encontrou uma semi-aresta par
      sa_nova.par = twin->second;
      //Se existe uma sa twin dessa, essa eh a twin da outra
      //Aponta a twin da outra para essa
      d->mapa_sa[twin->second].par = ind_sa_atual;
    }

    //Registra essa semi-aresta no mapa de semi-arestas
    mapa_twin[{(*iti), ind_vertice_inicial}] = ind_sa_atual;

    //Copia essa semi-aresta para o mapa de sa da DCEL
    d->mapa_sa.push_back(sa_nova);

    //Criou todas as semi-arestas da face atual
    //Salva a face atual
    d->mapa_faces.push_back(face_nova);
    
  }//For cria faces

  return d;

}

//Copia o vetor de pontos do descritor para o vetor de vertices da DCEL
//Os vertices nao possuem indice da semi-aresta incidente
void copia_pontos_para_vertices(descritor_dcel* descritor, dcel_t* d){

  for(ponto_3d p : descritor->pontos){
    vertice_3d_t v{p.x, p.y, p.z, INVALID_INDEX};
    d->mapa_vertices.push_back(v);
  }
}