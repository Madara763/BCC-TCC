/*
Oferece funcoes auxiliares a main()
Essa biblioteca contem funcoes para imprimir informacoes na tela
Criado por: Davi Lazzarin 
Data: 15/09/2026
*/

#ifndef __MSGLIB__
#define __MSGLIB__

#include <string>

using namespace std; //Para simplificar o uso da stl

//O nome das funcoes sao bem descritivos...
void msg_erro_parametros();
void msg_print_uso_parametros();
void msg_quantidade_parametros_errado();
void msg_erro_uso_entrada();
void msg_erro_uso_saida();
void msg_erro_parametro_desconhecido(string parametro);
void msg_falta_arq_entrada();
void msg_falta_arq_saida();
void msg_erro_abrir_arquivo();
void msg_erro_ler_arquivo(string arq);
void msg_erro_escrever_arquivo(string arq);
#endif