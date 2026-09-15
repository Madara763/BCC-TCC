/*
Essa biblioteca contem funcoes para imprimir informacoes na tela
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/
#include "mensagens.hpp"

#include <iostream>
#include <string>

using namespace std; //Para simplificar o uso da stl

#define NOME_PROGRAMA "cip"

//Aviso para quando os parametros tiverem errados
void msg_erro_parametros(){
  cerr << "Erro: Parametros Incorretos. Use -h para mais informacoes.\n";
};

//Mensagems de como usar os parametros corretamente
void msg_print_uso_parametros(){
  cout<<"Usos "<<NOME_PROGRAMA<<";\n";
  cout<<"\t\t -i: Indica o nome dos arquivos de entrada. Obrigatoriamente deve ser dois arquivos.\n";
  cout<<"\t\t -o: Indica o nome do arquivo de saida.\n";
  cout<<"\t\t -h: Para exibir essa mensagem.\n";
  cout<<"\t\t Exemplo de uso: ./"<<NOME_PROGRAMA<<" -i poligono1 poligono2 -o interseccao\n";
  cout<<"\t\t Exemplo de uso: ./"<<NOME_PROGRAMA<<" -h\n";
};

void msg_quantidade_parametros_errado(){
  cerr << "Erro: quantidade de parametros invalida.\n\n";
}

void msg_erro_uso_entrada(){
  cerr << "Erro: o parametro -i precisa do nome de dois arquivos.\n";
};

void msg_erro_uso_saida(){
  cerr << "Erro: o parametro -o precisa de um nome de arquivo.\n";
};

void msg_erro_parametro_desconhecido(string parametro){
  cerr << "Erro: parametro desconhecido: " << parametro << "\n";
};

void msg_falta_arq_entrada(){
  cerr << "Erro: arquivos de entrada nao informado.\n";
};

void msg_falta_arq_saida(){
  cerr << "Erro: arquivo de saida nao informado.\n";
};

void msg_erro_abrir_arquivo(){
  cout << "Erro ao abrir o arquivo.\n";
};

void msg_erro_ler_arquivo(string arq){
  cout << "Erro ao ler o arquivo: "<<arq<<".\n";
};
