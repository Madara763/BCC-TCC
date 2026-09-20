/*
Oferece funcoes auxiliares a main()
Essa biblioteca contem funcoes para processar arquivos descritores das malhas da DCEL
Recebe os nomes dos arquivos e processa os dados em estruturas da STL
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/
#include "parse.hpp"

#include <fstream>
#include <sstream>
#include <iostream>


//Recebe o nome de um arquivo de texto contendo a malha da DCEL 
//Le o arquivo e gera um descritor da DCEL
//A memoria do descritor eh liberada "sozinha"
descritor_dcel* processa_arq_entrada(std::string nome_arq){

	//gera um stream com o arquivo de entrada
	std::ifstream arquivo(nome_arq);
  descritor_dcel* dcel_p1{nullptr};

	// Verifica se o arquivo foi aberto com sucesso
	if(arquivo.is_open()){

    //Variaveis para ler o arquivo de entrada
    std::uint64_t nvertices, nfaces, nv;
    dcel_p1 = new(descritor_dcel);
    std::string linha; 

		// le o numero de vertices e de faces
		arquivo>>nvertices>>nfaces;
    
    //Percorre o arquivo lendo os vertices
		ponto_3d p;
    for(std::uint64_t i=0; i<nvertices; i++){
      arquivo>>p.x>>p.y>>p.z;
      dcel_p1->pontos.push_back(p);	//Coloca os vertices no vetor
    };

		#ifdef DEBUG
		cout<<"Vertices Lidos:\n";
    for(ponto_3d x : dcel_p1->pontos){ cout << x; }
		#endif

		//descarta o fim da ultima linha, vamos ver linha inteiras daqui pra frente
  	arquivo.ignore(); 

    //Percorre o arquivo para montar as faces
    for(uint64_t i=0; i<nfaces; i++){

			//Le a linha contendo a sequancia de vertices da face
			getline(arquivo, linha);
      istringstream ss(linha);

			//Aloca o vetor da face atual
			vector<uint64_t> face_atual;

			//Insere o numero do vertice na lista da face
			while(ss >> nv){
				face_atual.push_back(nv - 1); //Ajustar a posicao com o indice
			}

			//Insere a face no vetor de faces do descritor da dcel do poliedro 
			dcel_p1->faces.push_back(face_atual);
    };

		// Fecha o arquivo 
		arquivo.close();
	}

  //Finaliza e retorna o descritor da dcel
  //Se nao entrou no if retorna o nullptr
  return dcel_p1;
};

//Recebe um nome de arquivo e um descritor da DCEL
//Imprime um descritor da DCEL no arquivo 
//Retorna 0 se deu certo, e 1 se houve algum erro
int imprime_descritor_dcel_no_arquivo(std::string nome_arq, descritor_dcel* dcel){
	
	//Descritor da DCEL nao faz sentido
	if(dcel->pontos.size() == 0 || dcel->faces.size() == 0)
		return 1;
	
	//gera um stream com o arquivo para escrever
	std::ofstream arquivo(nome_arq);
	if (arquivo.is_open()){
		
		//Escreve a primeira linha
		arquivo<<static_cast<int>(dcel->pontos.size())<<" "<<static_cast<int>(dcel->faces.size())<<"\n";
		
		//Escreve cada coordenada em ordem
		for(ponto_3d p : dcel->pontos){
			arquivo<<p<<"\n";
		}
		
		#ifdef DEBUG
		//Escreve a ordem dos vertices de cada face NA TELA
		for(size_t i = 0; i< dcel->faces.size(); i++){
			std::cout<<"I vale -> "<<i<<"\n";
			for(uint64_t j=0; j< dcel->faces[i].size(); j++ ){
				std::cout<<dcel->faces[i][j]<<" ";
			}
			std::cout<<"\n";
		}
		#endif


		//Escreve a ordem dos vertices de cada face
		for(vector<uint64_t> v : dcel->faces){
			for(uint64_t i : v){
				arquivo<<i<<" ";
			}
			arquivo<<"\n";
		}

		// Fecha o arquivo 
		arquivo.close();
	}

	return 0;
}

// Recebe o nome de um arquivo de saida e um ponteiro para uma dcel_t
// Imprime a DCEL no arquivo no mesmo formato do arquivo de entrada
// Retorna 0 se deu certo, e 1 se houve erro ao abrir/escrever
int imprime_dcel_no_arquivo(std::string nome_arq, const dcel_t* d) {
	if(d == nullptr || d->mapa_vertices.empty() || d->mapa_faces.empty()) {
		return 1;
	}

	std::ofstream arquivo(nome_arq);
	if(!arquivo.is_open()) {
		return 1;
	}

	//Escreve a primeira linha: <num_vertices> <num_faces>
	arquivo << d->mapa_vertices.size() << " " << d->mapa_faces.size() << "\n";

	//escreve cada vertice (x, y, z)
	for(const auto& v : d->mapa_vertices) {
		arquivo << v.pos.x << " " << v.pos.y << " " << v.pos.z << "\n";
	}

	//Escreve os indices dos vertices de cada face 
	for(size_t i = 0; i < d->mapa_faces.size(); ++i) {
		std::vector<size_t> verts_face = get_vertices_face(*d, i);
		for(size_t j = 0; j < verts_face.size(); ++j) {
			arquivo << (verts_face[j] + 1) << (j + 1 == verts_face.size() ? "" : " ");
		}
		arquivo << "\n";
	}

	arquivo.close();
	return 0;
}

// Sobrecarga para imprimir a dcel_t diretamente em std::ostream 
void imprime_dcel_formatada(std::ostream& os, const dcel_t* d) {
	if(d == nullptr || d->mapa_vertices.empty() || d->mapa_faces.empty()) {
		return;
	}

	//Escreve a primeira linha: <num_vertices> <num_faces>
	os << d->mapa_vertices.size() << " " << d->mapa_faces.size() << "\n";

	//Escreve cada vertice (x, y, z)
	for(const auto& v : d->mapa_vertices) {
		os << v.pos.x << " " << v.pos.y << " " << v.pos.z << "\n";
	}

	//Escreve os indices dos vertices de cada face 
	for(size_t i = 0; i < d->mapa_faces.size(); ++i) {
		std::vector<size_t> verts_face = get_vertices_face(*d, i);
		for(size_t j = 0; j < verts_face.size(); ++j) {
			os << (verts_face[j] + 1) << (j + 1 == verts_face.size() ? "" : " ");
		}
		os << "\n";
	}
}