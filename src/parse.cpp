/*
Essa biblioteca contem funcoes para processar arquivos descritores das malhas da DCEL
Recebe os nomes dos arquivos e processa os dados em estruturas da STL
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/
#include "parse.hpp"

#include <fstream>
#include <sstream>

descritor_dcel* processa_arq_entrada(std::string arq){

	std::ifstream arquivo(arq);
  descritor_dcel* dcel_p1{nullptr};

	// Verifica se o arquivo foi aberto com sucesso
	if(arquivo.is_open()){

    //Le o arquivo de entrada
    std::uint64_t nvertices, nfaces, nv;
    dcel_p1 = new(descritor_dcel);
    std::string linha; 

		// le o numero de vertices e de faces
		arquivo>>nvertices>>nfaces;
    
    //Percorre o arquivo lendo os vertices
		ponto_3d p;
    for(std::uint64_t i=0; i<nvertices; i++){
      arquivo>>p.x>>p.y>>p.z;
      dcel_p1->vertices.push_back(p);	//Coloca os vertices no vetor
    };

		#ifdef DEBUG
		cout<<"Vertices Lidos:\n";
    for(ponto_3d x : dcel_p1->vertices){ cout << x; }
		#endif

    //Percorre o arquivo para montar as faces
    for(uint64_t i=0; i<nfaces; i++){

			//Le a linha contendo a sequancia de vertices da face
			getline(arquivo, linha);
      istringstream ss(linha);

			//Aloca o vetor da fae atual
			vector<uint64_t> face_atual;

			//Insere o numero do vertice na lista da face
			while(ss >> nv){
				face_atual.push_back(nv);
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