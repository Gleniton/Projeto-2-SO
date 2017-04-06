struct pagina{
	unsigned int id;
	unsigned int nPagina;
}typedef struct pagina tipoPagina;

struct quadro{
	tipoPagina p[40];
	unsigned int temQuadroLivre;
	unsigned int nFaltas;
	unsigned int ativaFaltas;
}typedef struct quadro tipoQuadro;

tipoQuadro quadros[40];

void gerenciaPaginas(tipoQuadro *q, unsigned int idRecebida, unsigned int pagRecebida){
	//miss = 0
	//hit = 1
	unsigned int distanciaExecucao = 0;
	unsigned int quadroSubstituido = 0;
	unsigned int achouSubstituto = 0;
	unsigned int hit = 0;
	unsigned int i;
	unsigned int j;
	unsigned int nQuadroLivre = 0;
	q.temQuadroLivre = 0;
	for(i = 0;i < 40;i++){
		if(q.p[i].id == idRecebida && q.p[i].nPagina == pagRecebida){
			hit = 1;
			break;
		}
		if(q.p[i].id == 0 && q.p[i].nPagina == 0 && q.temQuadroLivre == 0){
			q.temQuadroLivre = 1;
			nQuadroLivre = i;
		}
	}
	if(!hit){
		if(q.temQuadroLivre){
			q.p[nQuadroLivre].id = idRecebida;
			q.p[nQuadroLivre].nPagina = pagRecebida;
			if(q.ativaFaltas) q.nFaltas++;
		}
		else{
			for(i = 0;i < 40;i++){
				j = 0;
				pAtual = *l;
				while(pAtual != NULL){
					if(pAtual->id == q.p[i].id && pAtual->nPagina == q.p[i].nPagina){
						if(distanciaExecucao < j){
							quadroSubstituido = i;
							distanciaExecucao = j;
						}
						break;
					}
					if(pAtual->proximo == NULL && achouSubstituto == 0){
						achouSubstituto = 1;
						quadroSubstituido = i;
						break;
					}
					j++;
					pAtual = pAtual->proximo;
				}
				if(achouSubstituto) break;
			}
			q.p[quadroSubstituido].id = idRecebida;
			q.p[quadroSubstituido].nPagina = pagRecebida;
			if(q.ativaFaltas) q.nFaltas++;
		}
	}
	//ativar faltas
	if(!q.ativaFaltas){
		q.ativaFaltas = 1;
		for(i = 0;i < 40;i++){
			if(q.p[i].id == 0 && q.p[i].nPagina == 0){
				q.ativaFaltas = 0;
				break;
			}
		}
	}
}