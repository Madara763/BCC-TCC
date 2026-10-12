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
  if (!poliedro1){
    msg_erro_ler_arquivo(param.arq_entrada1);
    return 1;
  }

  // Le a entrada e gera os vetores de vertices e faces para gerar a dcel do segundo arquivo
  descritor_dcel *poliedro2 = processa_arq_entrada(param.arq_entrada2);

  // Verifica se o arquivo foi aberto com sucesso
  if (!poliedro2){
    msg_erro_ler_arquivo(param.arq_entrada2);
    return 1;
  }

  //===============Gera a DCEl a partir dos descritores===============

  dcel_t *d1 = cria_dcel(poliedro1);
  dcel_t *d2 = cria_dcel(poliedro2);

  //===============PASSO 1: PROJECAO 2D===============
  projecao_poligono_2d_t polA = extrai_poligono_projecao(*d1);
  projecao_poligono_2d_t polB = extrai_poligono_projecao(*d2);

  //===============PASSO 2: PONTO COMUM NO DOMINIO DA INTERSECCAO===============
  interseccao_2d_t saida_p_estrela = encontrar_ponto_p_estrela(polA, polB);
  
  if (saida_p_estrela.tem_interseccao){ // Existe um ponto na interseccao 2D de A ou B (vertice ou sobreposicao de arestas)
    
    //===============PASSO 3: PRE IMAGEM===============
    analise_interseccao_vertical_t pre_img = intervalo_pre_img(d1, d2, saida_p_estrela.p_estrela_deslocado);
    if(pre_img.existe_sobreposicao){
      //===============PASSO FINAL: PODE IR PARA A DUALIZACAO===============  
      ponto_3d origem_para_dualizacao = calcula_ponto_interno(pre_img ,saida_p_estrela.p_estrela_deslocado); 
      dcel_t* d3 = calcula_interseccao_via_dualizacao(d1, d2, origem_para_dualizacao);
      imprime_dcel_no_arquivo(param.arq_saida, d3); //Gera a saida
      delete (d3);
    }
    else{ //===============PASSO 4: ENCONTRAR AS CASCAS NEAR-SIDE===============      
      
    }
  }
  else{
    //===============PASSO FINAL: INTERSECCAO VAZIA===============
    std::cout<<"Poligonos projetados não se sobrepoẽm.\n";
	}

  // Libera memoria
  delete (d1);
  delete (d2);
  delete (poliedro1);
  delete (poliedro2);

  return 0;
}
