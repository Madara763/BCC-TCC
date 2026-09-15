/*
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

descritor_dcel* processa_arq_entrada(std::string arq); 


#endif