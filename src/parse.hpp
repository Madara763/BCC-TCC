/*
Oferece funcoes auxiliares a main()
Essa biblioteca contem funcoes para processar arquivos descritores das malhas da DCEL
Recebe os nomes dos arquivos e processa os dados em estruturas da STL
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/
#ifndef __PARSELIB__
#define __PARSELIB__

#include "dcel.hpp"
#include <string>


struct parametros {
	std::string arq_entrada1;
  std::string arq_entrada2;
	std::string arq_saida;
};

//Recebe o nome de um arquivo de texto contendo a malha da DCEL 
//Le o arquivo e gera um descritor da DCEL
//A memoria do descritor eh liberada "sozinha"
descritor_dcel* processa_arq_entrada(std::string nome_arq); 

//Recebe um nome de arquivo e um descritor da DCEL
//Imprime um descritor da DCEL no arquivo 
//Retorna 0 se deu certo, e 1 se houve algum erro
int imprime_dcel_no_arquivo(std::string nome_arq, descritor_dcel* dcel);


#endif