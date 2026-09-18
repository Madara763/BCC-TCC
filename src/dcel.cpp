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

    for(; itf != descritor_face.end(); ++itf){

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
      else{  //==========Faz isso para as demais semi-aresta da face==========

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

    //==========Faz isso para a ultima semi-aresta e fecha a face==========

    //Fecha a face e adiciona no mapa de faces da DCEL
    //Cria a semi-aresta do ultimo vertice para o primeiro
    semi_aresta_t sa_nova;
    ind_sa_atual = d->mapa_sa.size();

    sa_nova.ind_vertice = (*iti);

	  // Verifica se o vertice inicial da semi-aresta ja ponta para alguma semi-aresta
	  // Caso nao, aponta para a que esta sendo criada
	  if(d->mapa_vertices[(*iti)].sa_incidente == INVALID_INDEX)
		  d->mapa_vertices[(*iti)].sa_incidente = ind_sa_atual;

    sa_nova.ind_face = indice_face_nova; 
    sa_nova.prox = face_nova.ind_sa_inicial; //Aponta para a primeira sa da face
    sa_nova.ante=ind_sa_anterior; 

    d->mapa_sa[face_nova.ind_sa_inicial].ante = ind_sa_atual; //Apont a anterior da primeira
    d->mapa_sa[ind_sa_anterior].prox = ind_sa_atual;  //Proxima da anterior

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
    //Agora preenche a equacao do plano da face
    calcula_eq_plano_face(d, face_nova);

    //Salva a face atual
    d->mapa_faces.push_back(face_nova);

    
  }//For cria faces

  return d;

}

// Recebe uma face com seus vertices e semi-arestas corretos
// Calcula a equacao do plano preenchendo a propria face
void calcula_eq_plano_face(dcel_t* d, face_t& face_nova) {
	
  //Para esse calculo precisamos de 3 ponto nao colineares(assumimos que os vertices ja sao colineares)
  //Pega o indice dos 3 primeiros
	size_t id_sa1 = face_nova.ind_sa_inicial;
	size_t id_sa2 = d->mapa_sa[id_sa1].prox;
	size_t id_sa3 = d->mapa_sa[id_sa2].prox;

	//Salvamos as coordenadas dos vertices em tipos ponto_3d para facilitar
	ponto_3d p1 = d->mapa_vertices[d->mapa_sa[id_sa1].ind_vertice].pos;
	ponto_3d p2 = d->mapa_vertices[d->mapa_sa[id_sa2].ind_vertice].pos;
	ponto_3d p3 = d->mapa_vertices[d->mapa_sa[id_sa3].ind_vertice].pos;

	//Criar os vetores a partir dos pontos 
	ponto_3d u = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z}; //p2 - p1
	ponto_3d v = {p3.x - p2.x, p3.y - p2.y, p3.z - p2.z}; //p3 - p2

	//Calcula o produto vetorial para achar a normal
	//Produto vetorial padrao (u x v):
	//nx = (u.y * v.z - u.z * v.y)
	//ny = (u.z * v.x - u.x * v.z)
	//nz = (u.x * v.y - u.y * v.x)
	//
	//IMPORTANTE: COMO O ARTIGO DE MULLER E PREPARATA CONSIDERA QUE A NORMAL APONTE
	//PARA DENTRO DO POLIEDRO, INVERTEMOS O DO VETOR AQUI
  //ISSO E NECESSARIO POIS COM A ORIENTACAO DAS SEMI-ARESTAS EM ANTI-HORARIO ESSE VETOR APONTARIA PARA FORA
	t_coord nx = -(u.y * v.z - u.z * v.y);
	t_coord ny = -(u.z * v.x - u.x * v.z);
	t_coord nz = -(u.x * v.y - u.y * v.x);

	//Normaliza o vetor normal (para evitar problemas com FP)
	t_coord norma = std::sqrt(nx * nx + ny * ny + nz * nz);
	if(norma > EPS) {  // EPS esta definido no seu geometria.hpp
		nx /= norma;
		ny /= norma;
		nz /= norma;
	}

	//Preenche na face
	face_nova.n1 = nx;
	face_nova.n2 = ny;
	face_nova.n3 = nz;

	//Calcula a constante d da equacao do plano
	//Como n.P + d = 0 para um ponto P no plano, entao d = - (n . p1)
	face_nova.d = -(nx * p1.x + ny * p1.y + nz * p1.z);
}

//Copia o vetor de pontos do descritor para o vetor de vertices da DCEL
//Os vertices nao possuem indice da semi-aresta incidente
void copia_pontos_para_vertices(descritor_dcel* descritor, dcel_t* d){

  for(ponto_3d p : descritor->pontos){
    vertice_3d_t v{p.x, p.y, p.z, INVALID_INDEX};
    d->mapa_vertices.push_back(v);
  }
}

//========================================
//Metodos do artigo
//========================================

//Retorna as semi-arestas (os indices) que saem do vertice, em anti-horario
std::vector<size_t> vertex(const dcel_t& d, size_t ind_vertice){

	std::vector<size_t> incidentes;
	size_t inicio = d.mapa_vertices[ind_vertice].sa_incidente;
	size_t atual = inicio;

	if(inicio == INVALID_INDEX) return incidentes;

	do{
		incidentes.push_back(atual);

		//Pula para a semi-aresta twin da atual
		size_t id_par = d.mapa_sa[atual].par;
    //id_par eh uma sa que chega no vertice
		if(id_par == INVALID_INDEX) break;  // nao deve entrar aqui

    //O proximo do id_par eh outra sa que sai do vertice
		atual = d.mapa_sa[id_par].prox;

	} while(atual != inicio && atual != INVALID_INDEX); //Valida se ja deu a volta

	return incidentes; //retorna uma copia do vetor com as sa incidentes no vertice
}

//Retorna as meia arestas (os indices) que limitam a face
std::vector<size_t> face(const dcel_t& d, size_t ind_face){

	std::vector<size_t> contorno;
	size_t inicio = d.mapa_faces[ind_face].ind_sa_inicial; //pega a primeira sa da face
	size_t atual = inicio;

	if(inicio == INVALID_INDEX) return contorno;

  //percorre o contorno usando o ->prox
	do{
		contorno.push_back(atual); //salva no vector
		atual = d.mapa_sa[atual].prox; //vai para a proxima
	} while(atual != inicio && atual != INVALID_INDEX); //verifica se deu a volta na face

  //retorna uma copia do vector com as sa da face
	return contorno;
}



//========================================
//Define funcoes de debug
//========================================


// Funcao para testar e imprimir o contorno de todas as faces
void debug_dcel_metodo_face(const dcel_t* d) {
	std::cout << "\n========== TESTE DO METODO FACE() ==========\n";

	// Itera por todas as faces da malha
	for(size_t i = 0; i < d->mapa_faces.size(); ++i) {
		std::cout << "Face [" << i << "]: ";

		// Chama a funcao que percorre o contorno
		std::vector<size_t> contorno = face(*d, i);

		// Imprime a sequencia de semi-arestas
		std::cout << "Caminho SA -> { ";
		for(size_t sa_idx : contorno) {
			std::cout << sa_idx << " ";
		}
		std::cout << "} | ";

		// Imprime a sequencia dos vertices para vermos a geometria
		std::cout << "Vertices -> [ ";
		for(size_t sa_idx : contorno) {
			std::cout << d->mapa_sa[sa_idx].ind_vertice << " ";
		}
		std::cout << "]\n";
	}
	std::cout << "============================================\n";
}

// Funcao para testar e imprimir as arestas incidentes de cada vertice
void debug_dcel_metodo_vertex(const dcel_t* d) {
	std::cout << "\n========= TESTE DO METODO VERTEX() =========\n";

	// Itera por todos os vertices da malha
	for(size_t i = 0; i < d->mapa_vertices.size(); ++i) {
		std::cout << "Vertice [" << i << "] (";
		std::cout << d->mapa_vertices[i].pos.x << ", "
		          << d->mapa_vertices[i].pos.y << ", "
		          << d->mapa_vertices[i].pos.z << "): ";

		// Chama a funcao que gira em torno do vertice
		std::vector<size_t> incidentes = vertex(*d, i);

		// Imprime a sequencia de semi-arestas que saem dele
		std::cout << "SA Incidentes (Saem) -> { ";
		for(size_t sa_idx : incidentes) {
			std::cout << sa_idx << " ";
		}
		std::cout << "} | ";

		// Imprime para quais vertices essas arestas estao apontando
		std::cout << "Destinos -> [ ";
		for(size_t sa_idx : incidentes) {
			// O destino e o vertice de inicio da PROXIMA semi-aresta
			size_t prox_sa = d->mapa_sa[sa_idx].prox;
			std::cout << d->mapa_sa[prox_sa].ind_vertice << " ";
		}
		std::cout << "]\n";
	}
	std::cout << "============================================\n";
}
