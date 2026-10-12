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
void debug_teste_interseccao_2d(const projecao_poligono_2d_t& pol1, const projecao_poligono_2d_t& pol2) {
	std::cout << "========== TESTE INTERSECCAO 2D (p*) ==========\n";

	//executa a interseccao 2d e recebe o par de resultados
	interseccao_2d_t resultado = encontrar_ponto_p_estrela(pol1, pol2);

	if(!resultado.tem_interseccao) {
		std::cout << "Resultado: Poliedros separados. Interseccao nula.\n";
	}
  else{
		std::cout << "Resultado: Sobreposicao detectada! Coordenadas de p*: ("<< resultado.p_estrela.x << ", " << resultado.p_estrela.y << ")\n";
		std::cout << "Ponto interno: Coordenadas do baricentro: ("<< resultado.p_estrela_deslocado.x << ", " << resultado.p_estrela_deslocado.y << ")\n";
    if(resultado.origem == 1 || resultado.origem == 2)
		  std::cout << "Status: Ponto p* coincide com o vertice [ "<<resultado.ind_original<<" ] do Poliedro " << static_cast<int>(resultado.origem) << ".\n";
    else
      std::cout << "Status: Vertice \"Virtual\"\n";
	}

	std::cout << "===============================================\n";
}

//Recebe o ponto p* e calcula a pre imagem, imprime na tela detalhes da etapa
void debug_teste_pre_imagem(dcel_t* d1, dcel_t* d2, ponto_2d p_estrela){
	analise_interseccao_vertical_t pre_img = intervalo_pre_img(d1, d2, p_estrela);
	std::cout << "========== TESTE SOBREPOSICAO VERTICAL EM (X,Y) = "<<p_estrela<<" ==========\n";
	if(pre_img.existe_sobreposicao) {
		std::cout << "Sobreposição da pré imagem válida.\n";
		std::cout << "Ponto p* -> " << p_estrela << "\n";
		std::cout << "Segmento da pré imagem na intersecção de -> " << p_estrela << " " << pre_img.limite_inf << " até " << p_estrela << " " << pre_img.limite_sup << "\n";
	}
	else {
		std::cout << "Sem sobreposição vertical no eixo Z (pré imagem) nos poliedros no ponto "<<p_estrela<<".\n";
	}
	std::cout << "===============================================\n";
}

// Recebe as dcel e o ponto interno a ambas, imprime passo a passo a dualizacao e os fechos convexos
void debug_teste_dualizacao(dcel_t* d1, dcel_t* d2, ponto_3d origem) {
	std::cout << "\n========== DEBUG: CALCULO DE INTERSECCAO VIA DUALIZACAO ==========\n";
	std::cout << "Ponto Interno (Origem Deslocada): " << origem << "\n";

	if(!d1 || !d2) {
		std::cout << "[Erro] Um ou ambos os poliedros sao nulos.\n";
		return;
	}

	std::vector<quickhull::Vector3<double>> qh_pontos;

	// ----- PASSO 1: Dualização do Poliedro A -----
	std::cout << "\n--- PASSO 1: Dualizacao do Poliedro 1 ---\n";
	for(size_t i = 0; i < d1->mapa_faces.size(); ++i) {
		const face_t& f = d1->mapa_faces[i];

		// Ajusta a constante 'd' baseada na nova origem (translacao espacial)
		double d_novo = (f.n1 * origem.x + f.n2 * origem.y + f.n3 * origem.z) + f.d;

		if(std::abs(d_novo) > EPS) {  // Evita divisão por zero
			ponto_3d p_dual{(f.n1 / d_novo), (f.n2 / d_novo), (f.n3 / d_novo)};
			std::cout << "  Face [" << i << "]: d_novo=" << d_novo
			          << " \t-> Ponto Dual: " << p_dual << "\n";
			qh_pontos.push_back(quickhull::Vector3<double>(p_dual.x, p_dual.y, p_dual.z));
		} else {
			std::cout << "  Face [" << i << "]: [Ignorada] O plano passa muito perto da origem (d_novo <= EPS).\n";
		}
	}

	// ----- PASSO 2: Dualização do Poliedro B -----
	std::cout << "\n--- PASSO 2: Dualizacao do Poliedro 2 ---\n";
	for(size_t i = 0; i < d2->mapa_faces.size(); ++i) {
		const face_t& f = d2->mapa_faces[i];

		double d_novo = (f.n1 * origem.x + f.n2 * origem.y + f.n3 * origem.z) + f.d;

		if(std::abs(d_novo) > EPS) {
			ponto_3d p_dual{(f.n1 / d_novo), (f.n2 / d_novo), (f.n3 / d_novo)};
			std::cout << "  Face [" << i << "]: d_novo=" << d_novo
			          << " \t-> Ponto Dual: " << p_dual << "\n";
			qh_pontos.push_back(quickhull::Vector3<double>(p_dual.x, p_dual.y, p_dual.z));
		} else {
			std::cout << "  Face [" << i << "]: [Ignorada] O plano passa muito perto da origem (d_novo <= EPS).\n";
		}
	}

	// ----- PASSO 3: Cálculo do Fecho Convexo Dual -----
	std::cout << "\n--- PASSO 3: Fecho Convexo no Espaco Dual (QuickHull) ---\n";
	std::cout << "Total de pontos duais gerados (Faces A + Faces B): " << qh_pontos.size() << "\n";

	if(qh_pontos.size() < 4) {
		std::cout << "[Erro] Quantidade de pontos duais (" << qh_pontos.size()
		          << ") insuficiente para formar um poliedro 3D.\n";
		return;
	}

	// Executa a QuickHull nos pontos duais
	quickhull::QuickHull<double> qh;
	quickhull::ConvexHull<double> hull = qh.getConvexHull(qh_pontos, true, false);

	// Obtém os buffers resultantes
	const auto& qh_vertices = hull.getVertexBuffer();
	const auto& qh_indices = hull.getIndexBuffer();

	std::cout << "Fecho Convexo Dual calculado com sucesso!\n";
	std::cout << "  - Vértices no fecho dual (Correspondem às FACES da Intersecao Primal): " << qh_vertices.size() << "\n";
	std::cout << "  - Faces triangulares no fecho dual (Correspondem aos VÉRTICES da Intersecao Primal): " << (qh_indices.size() / 3) << "\n";

	// ----- PASSO 4: Restauração para o Espaço Primal -----
	std::cout << "\n--- PASSO 4: Retorno ao Espaco Primal ---\n";
	std::cout << "No codigo principal, o fecho convexo gera a DCEL dual e depois reconstruimos as coordenadas primais aplicando a formula:\n";
	std::cout << "  P_primal_x = (Face_dual_Nx / d_novo) + Origem_x\n";
	std::cout << "  P_primal_y = (Face_dual_Ny / d_novo) + Origem_y\n";
	std::cout << "  P_primal_z = (Face_dual_Nz / d_novo) + Origem_z\n";

	std::cout << "\nA topologia e mantida convertendo vizinhancas:\n";
	std::cout << "  Vertice Dual -> Face Primal\n";
	std::cout << "  Face Dual -> Vertice Primal\n";
	std::cout << "==================================================================\n\n";
}