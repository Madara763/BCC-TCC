/*
Esse programa recebe um arquivo com a descricao de duas malhas de poligonos.
Calcula se ha interseccao utilizando o metodo de Muller e Preparata (1978).
Retorna a malha da interseccao caso haja e se os poligonos nao estiverem sobrepostos retorna um plano que os separa.
Criado por: Davi Lazzarin
Data: 15/09/2026
*/

#include <cerrno>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>

#include "parse.hpp"
#include "dcel.hpp"
#include "fitcp.hpp"
#include "mensagens.hpp"
#include "debug.hpp"

using namespace std;

bool validar_parametros(int argc, char *argv[], parametros &param);

int main(int argc, char **argv)
{
  parametros param;

  // Verificacao de parametros
  if (!validar_parametros(argc, argv, param))
    return 1;

  //===============Le os arquivos e gera os descritores===============
  // Le a entrada e gera os vetores de vertices e faces para gerar a dcel do primeiro arquivo
  descritor_dcel *poliedro1 = processa_arq_entrada(param.arq_entrada1);

  // Verifica se o arquivo foi aberto com sucesso
  if (!poliedro1)
  {
    msg_erro_ler_arquivo(param.arq_entrada1);
    return 1;
  }

  // Le a entrada e gera os vetores de vertices e faces para gerar a dcel do segundo arquivo
  descritor_dcel *poliedro2 = processa_arq_entrada(param.arq_entrada2);

  // Verifica se o arquivo foi aberto com sucesso
  if (!poliedro2)
  {
    msg_erro_ler_arquivo(param.arq_entrada2);
    return 1;
  }

  //===============Gera a DCEl a partir dos descritores===============

  dcel_t *d1 = cria_dcel(poliedro1);
  dcel_t *d2 = cria_dcel(poliedro2);

  poligono_2d_t polA = extrai_poligono_projecao(*d1);
  poligono_2d_t polB = extrai_poligono_projecao(*d2);

  debug_teste_interseccao_2d(polA, polB);

  ponto_2d p_istrela;
  std::pair<uint8_t, std::size_t> par_saida_p_estrela = encontrar_ponto_p_estrela(polA, polB, p_istrela);
  
  if (par_saida_p_estrela.first){ // Existe um ponto na interseccao 2D de A ou B (vertice ou sobreposicao de arestas)
    
    //Calcula pré imagem de p*
    std::pair<bool, std::pair<double, double>> par_pre_img = intervalo_pre_img(d1, d2, p_istrela);
    if(par_pre_img.first){
      std::cout<<"Sobreposição da pré imagem válida.\n";
      std::cout<<"Ponto p* -> "<<p_istrela<<"\n";
      std::cout<<"Segmento da pré imagem na intersecção de -> "<<p_istrela<<" "<<par_pre_img.second.first<<" até "<<p_istrela<<" "<<par_pre_img.second.second<<"\n";
    }
    else{
      std::cout<<"Sobreposição da pré imagem inválida.\n";
    }
  }
  else{
    std::cout<<"Poligonos projetados não se sobrepoẽm.\n";
	}

  
  imprime_dcel_formatada(cout, d1);
  // Libera memoria
  delete (d1);
  delete (d2);
  delete (poliedro1);
  delete (poliedro2);

  return 0;
}

// Funcao auxiliar para validar os parametros da main
bool validar_parametros(int argc, char *argv[], parametros &params)
{
  // Nenhum parâmetro
  if (argc < 2)
  {
    msg_quantidade_parametros_errado();
    msg_erro_parametros();
    return false;
  }

  // -h
  if (string(argv[1]) == "-h")
  {
    // -h deve ser utilizado sozinho
    if (argc != 2)
    {
      msg_erro_parametros();
      return false;
    }

    msg_print_uso_parametros();
    return false;
  }

  bool encontrou_i = false;
  bool encontrou_o = false;

  // Processamento dos parâmetros
  int i = 1;

  while (i < argc)
  {
    string parametro = argv[i];

    // -i
    if (parametro == "-i")
    {
      // -i não pode aparecer duas vezes
      if (encontrou_i)
      {
        msg_erro_parametros();
        return false;
      }

      encontrou_i = true;

      // Precisamos de dois argumentos depois de -i
      if (i + 2 >= argc)
      {
        msg_erro_uso_entrada();
        return false;
      }

      string entrada1 = argv[i + 1];
      string entrada2 = argv[i + 2];

      // Os dois argumentos não podem ser opções
      if (entrada1[0] == '-' || entrada2[0] == '-')
      {
        msg_erro_uso_entrada();
        return false;
      }

      params.arq_entrada1 = entrada1;
      params.arq_entrada2 = entrada2;

      i += 3;
    }

    // -o
    else if (parametro == "-o")
    {
      // -o não pode aparecer duas vezes
      if (encontrou_o)
      {
        msg_erro_parametros();
        return false;
      }

      encontrou_o = true;

      // Precisamos de um argumento depois de -o
      if (i + 1 >= argc)
      {
        msg_erro_uso_saida();
        return false;
      }

      string saida = argv[i + 1];

      // O argumento não pode ser outra opção
      if (saida[0] == '-')
      {
        msg_erro_uso_saida();
        return false;
      }

      params.arq_saida = saida;

      i += 2;
    }

    // Parâmetro desconhecido
    else
    {
      msg_erro_parametro_desconhecido(parametro);
      return false;
    }
  }

  // Verifica se -i foi informado
  if (!encontrou_i)
  {
    msg_falta_arq_entrada();
    return false;
  }

  // Verifica se -o foi informado
  if (!encontrou_o)
  {
    msg_falta_arq_saida();
    return false;
  }

  return true;
}
