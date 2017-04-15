struct pagina{
	unsigned int id;
	unsigned int nPagina;
	unsigned int tempo;
};typedef struct pagina tipoPagina;

struct quadro{
	tipoPagina p[TAMQUADROS];
	unsigned int temQuadroLivre;
	unsigned int nFaltas;
	unsigned int ativaFaltas;
};typedef struct quadro tipoQuadro;

void inicializaQuadros(tipoQuadro *q){
	int i;
	q->temQuadroLivre = 0;
	q->nFaltas = 0;
	q->ativaFaltas = 0;
	for(i = 0;i < TAMQUADROS;i++){
		q->p[i].id = 0;
		q->p[i].nPagina = 0;
		q->p[i].tempo = 0;
	}
}

void removePaginas(tipoQuadro *q, unsigned int idRecebido){
	int i;
	for(i = 0;i < TAMQUADROS;i++){
		if(q->p[i].id == idRecebido){
			q->p[i].id = 0;
			q->p[i].nPagina = 0;
			q->p[i].tempo = 0;
		}
	}
}

struct processoLeve{
	unsigned int status;
	unsigned int executionTime;
	unsigned int blockTime;
	unsigned int waitingTime; //tempo esperando na fila de pronto
	unsigned int tempoExecutando;  //tempo gasto executando
	unsigned int tempoBloqueado; //tempo gasto bloqueado
	unsigned int contador;
	unsigned int quantum;
	unsigned int atingiuQuantumMaximo;
};typedef struct processoLeve tipoProcessoLeve;

void simulaEstatisticasFuturo(tipoProcessoLeve *processoRecebido){
	if(processoRecebido->status == PRONTO){
        processoRecebido->waitingTime++;
    }
    if(processoRecebido->status == BLOQUEADO){
        processoRecebido->tempoBloqueado++;
    }
    if(processoRecebido->status == EXECUTANDO){
        processoRecebido->tempoExecutando++;
        processoRecebido->contador++;
    }
}

void simulaEstadoFuturo(tipoProcessoLeve *processoRecebido){
    if(processoRecebido->status == PRONTO){
	
    }
    if(processoRecebido->status == BLOQUEADO){
        if(processoRecebido->tempoBloqueado == processoRecebido->blockTime){
            processoRecebido->status = PRONTO;
        }
    }
    if(processoRecebido->status == EXECUTANDO){
        if(processoRecebido->tempoBloqueado + processoRecebido->tempoExecutando == processoRecebido->executionTime){
            processoRecebido->status = TERMINADO;
            processoRecebido->contador = 0;
        }
        if(processoRecebido->tempoBloqueado != processoRecebido->blockTime){
			processoRecebido->status = BLOQUEADO;
			processoRecebido->contador = 0;
        }
        if(processoRecebido->contador == processoRecebido->quantum){
            processoRecebido->status = PRONTO;
            processoRecebido->contador = 0;
            processoRecebido->atingiuQuantumMaximo = 1;
        }
    }
}

tipoListaPaginas* criaListaSimulacao(tipoLista **l){
	tipoProcessoLeve processoFuturo;
	tipoNoh *pAtual;
	tipoListaPaginas *lp;
	tipoListaPaginas *iteraLp;
	tipoListaPaginas *novaPagina;
	tipoListaPaginas *pgAtual;
	pAtual = (*l)->cabeca;
	lp = NULL;
	while(pAtual != NULL){
		processoFuturo.status = EXECUTANDO;
		processoFuturo.executionTime = pAtual->processo.executionTime;
		processoFuturo.blockTime = pAtual->processo.blockTime;
		processoFuturo.waitingTime = pAtual->processo.waitingTime;
		processoFuturo.tempoExecutando = pAtual->processo.tempoExecutando;
		processoFuturo.tempoBloqueado = pAtual->processo.tempoBloqueado;
		processoFuturo.atingiuQuantumMaximo = 0;
		processoFuturo.contador = (*l)->contador;
		processoFuturo.quantum = (*l)->quantum;
		pgAtual = pAtual->processo->cabecaPg;
		while(pgAtual != NULL){
			if(pgAtual->pagina.tempo == processoFuturo.tempoExecutando){
			novaPagina = (tipoListaPaginas*)malloc(sizeof(tipoListaPaginas));
			novaPagina->pagina = pgAtual->pagina;
			novaPagina->proximo = NULL;
				if(lp == NULL){
					lp = novaPagina;
					iteraLp = lp;
				}
				else{
					iteraLp->proximo = novaPagina;
					iteraLp = iteraLp->proximo;
				}
			}
			pgAtual = pgAtual->proximo;
		}
		simulaEstatisticas(&processoFuturo);
		simulaEstado(&processoFuturo);
		if(processoFuturo.status == EXECUTANDO){
			pgAtual = pAtual->processo->cabecaPg;
			while(pgAtual != NULL){
				if(pgAtual->pagina.tempo == processoFuturo.tempoExecutando){
				novaPagina = (tipoListaPaginas*)malloc(sizeof(tipoListaPaginas));
				novaPagina->pagina = pgAtual->pagina;
				novaPagina->tempo = pgAtual->tempo;
				novaPagina->proximo = NULL;
					if(lp == NULL){
						lp = novaPagina;
						iteraLp = lp;
					}
					else{
						iteraLp->proximo = novaPagina;
						iteraLp = iteraLp->proximo;
					}
				}
				pgAtual = pgAtual->proximo;
			}
		}
		pAtual = pAtual->proximo;
	}
	return lp;
}

void gerenciaPaginas(tipoQuadro *q, unsigned int idRecebida, unsigned int pagRecebida, unsigned int tempoRecebido ,tipoLista **l){
	//miss = 0
	//hit = 1
	tipoListaPaginas *lp;
	tipoListaPaginas *pgAtual;
	unsigned int distanciaExecucao = 0;
	unsigned int quadroSubstituido = 0;
	unsigned int achouSubstituto = 0;
	unsigned int hit = 0;
	unsigned int i;
	unsigned int j;
	unsigned int nQuadroLivre = 0;
	q.temQuadroLivre = 0;
	for(i = 0;i < TAMQUADROS;i++){
		if(q->p[i].id == idRecebida && q->p[i].nPagina == pagRecebida){
			hit = 1;
			break;
		}
		if(q->p[i].id == 0 && q->p[i].nPagina == 0 && q->temQuadroLivre == 0){
			q->temQuadroLivre = 1;
			nQuadroLivre = i;
		}
	}
	if(!hit){
		if(q->temQuadroLivre){
			q->p[nQuadroLivre].id = idRecebida;
			q->p[nQuadroLivre].nPagina = pagRecebida;
			if(q->ativaFaltas) q->nFaltas++;
		}
		else{
			lp = criaListaSimulacao(l);
			for(i = 0;i < TAMQUADROS;i++){
				j = 0;
				pgAtual = lp;
				while(pgAtual != NULL){
					if(pgAtual->pagina.id == q->p[i].id && pgAtual->pagina.nPagina == q->p[i].nPagina){
						if(distanciaExecucao < j){
							quadroSubstituido = i;
							distanciaExecucao = j;
						}
						break;
					}
					if(pgAtual->proximo == NULL && achouSubstituto == 0){
						achouSubstituto = 1;
						quadroSubstituido = i;
						break;
					}
					j++;
					pgAtual = pgAtual->proximo;
				}
				if(achouSubstituto)
					break;
			}
			q->p[quadroSubstituido].id = idRecebida;
			q->p[quadroSubstituido].nPagina = pagRecebida;
			if(q->ativaFaltas) q->nFaltas++;
		}
	}
	//ativar faltas
	if(!q->ativaFaltas){
		q->ativaFaltas = 1;
		for(i = 0;i < TAMQUADROS;i++){
			if(q->p[i].id == 0 && q->p[i].nPagina == 0){
				q->ativaFaltas = 0;
				break;
			}
		}
	}
}