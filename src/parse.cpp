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
int imprime_dcel_no_arquivo(std::string nome_arq, descritor_dcel* dcel){
	
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