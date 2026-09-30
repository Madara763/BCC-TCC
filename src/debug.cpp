/*
Essa lib contem funcoes para testar a DCEL e a FITCP
Tem o objetivo de explanar o funcionamento do artigo "Finding the Intersection of Two Convex Polyhedra" e da DCEL
Criado por: Davi Lazzarin
Data: 29/09/2026
*/
#include "debug.hpp"

//========================================
//Define funcoes de debug para a DCEL
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
		std::cout << d->mapa_vertices[i].pos.x << ", "<< d->mapa_vertices[i].pos.y << ", "<< d->mapa_vertices[i].pos.z << "): ";

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

//Triangula a DCEL inteira, exibe na TELA informacoes sobre a DCEL
void debug_triangula_e_valida_dcel(dcel_t* d) {
	if(d == nullptr) {
		std::cout << "\n[DEBUG] Ponteiro para DCEL nulo!\n";
		return;
	}

	std::cout << "\n============================================\n";
	std::cout << "  INICIANDO TRIANGULACAO COMPLETA DA DCEL\n";
	std::cout << "============================================\n";
	std::cout << "Estado Inicial:\n";
	std::cout << " - Qtd de Vertices: " << d->mapa_vertices.size() << "\n";
	std::cout << " - Qtd de Faces:    " << d->mapa_faces.size() << "\n";
	std::cout << " - Qtd de SA:       " << d->mapa_sa.size() << "\n\n";

	//Executa a triangulacao total
	triangula_dcel_completa(d);

	std::cout << "Estado Apos Triangulacao:\n";
	std::cout << " - Qtd de Vertices: " << d->mapa_vertices.size() << "\n";
	std::cout << " - Qtd de Faces:    " << d->mapa_faces.size() << "\n";
	std::cout << " - Qtd de SA:       " << d->mapa_sa.size() << "\n";
	std::cout << "============================================\n";

	//Executa as verificacoes topologicas de face e vertice
	std::cout << "\n>>> IMPRIMINDO ESTRUTURA APOS TRIANGULACAO <<<\n";
	debug_dcel_metodo_face(d);
	debug_dcel_metodo_vertex(d);
}

//Recebe as projecoes e imprime na tela se ha ou nao a interseccao e qual vertice corresponde ao p*
void debug_teste_interseccao_2d(const poligono_2d_t& pol1, const poligono_2d_t& pol2) {
	std::cout << "========== TESTE INTERSECCAO 2D (p*) ==========\n";

	ponto_2d p_estrela;

	//executa a interseccao 2d e recebe o par de resultados
	std::pair<uint8_t, std::size_t> resultado = encontrar_ponto_p_estrela(pol1, pol2, p_estrela);

	uint8_t status_id = resultado.first; //Se o resultado.first for 0 eh pq nao teve interseccao
	std::size_t indice_vertice = resultado.second;

	bool houve_interseccao = (status_id != 0);

	if(!houve_interseccao) {
		std::cout << "Resultado: Poliedros separados. Interseccao nula.\n";
	}
  else{
		std::cout << "Resultado: Sobreposicao detectada! Coordenadas de p*: ("<< p_estrela.x << ", " << p_estrela.y << ")\n";
    if(resultado.first == 1 || resultado.first == 2)
		  std::cout << "Status: Coincide com o vertice [ "<<indice_vertice<<" ] do Poliedro " << static_cast<int>(status_id) << ".\n";
    else
      std::cout << "Status: Vertice Virtual\n";
	}

	std::cout << "===============================================\n";
}
