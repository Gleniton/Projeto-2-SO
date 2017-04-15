#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#define VAZIO 0
#define PRONTO 1
#define EXECUTANDO 2
#define BLOQUEADO 3
#define TERMINADO 4
#define SUSPENSO 5
#define TAMQUADROS 40
#define ALPHA 100

/*
	.id
	.status
	.priority
	.submissionTime
	.executionTime
	.blockTime
	.turnAroundTime
	.responseTime
	.waitingTime
	.tempoExecutando
	.tempoBloqueado
	.tempoSubmetido
*/



struct listaPaginas{
	tipoPagina pagina;
	struct listaPaginas *proximo;
};typedef struct listaPaginas tipoListaPaginas;

struct processo{
	tipoListaPaginas *cabecaPg;
	unsigned int id;
	unsigned int status;
	unsigned int priority;
	unsigned int submissionTime;
	unsigned int executionTime;
	unsigned int blockTime;
	unsigned int turnAroundTime; //tempo entre submeter o processo e completá-lo
	unsigned int responseTime; //tempo entre submeter o processo e ele começar a executar
	unsigned int waitingTime; //tempo esperando na fila de pronto
	unsigned int tempoExecutando;  //tempo gasto executando
	unsigned int tempoBloqueado; //tempo gasto bloqueado
	unsigned int tempoSubmetido; //tempo em que o processo foi submetido
	//throughput: numero de processos concluídos divido pelo tempo de simulação total
};typedef struct processo tipoProcesso;

struct noh{
	tipoProcesso processo;
	struct noh *proximo;
};typedef struct noh tipoNoh;

struct lista{
	tipoNoh *cabeca;
	tipoNoh *cauda;
	char nome;
	unsigned int nElementos;
	unsigned int quantum;
	unsigned int contador;
	unsigned int atingiuQuantumMaximo;
};typedef struct lista tipoLista;

struct stats{
    unsigned int nProcessos;
    float retornoMedio;
    float servicoMedio;
    float respostaMedia;
    float throughput;
    float esperaMedia;
    float usoProcessador;
    unsigned int duracaoDaSimulacao;
};typedef struct stats tipoStats;

unsigned int temChave(tipoLista **l, unsigned int chave){
    unsigned int achou = 0;
    tipoNoh *pAtual;
    pAtual = (*l)->cabeca;
    while(pAtual != NULL){
        if(pAtual->processo.status == chave){
            achou = 1;
            break;
        }
        pAtual = pAtual->proximo;
    }
    return achou;
}

unsigned int temProcessoNovo(tipoLista **l, unsigned int t){
    unsigned int sucesso = 0;
    tipoNoh *pAtual;
    pAtual = (*l)->cabeca;
    while(pAtual != NULL){
        if(pAtual->processo.submissionTime <= t){
            sucesso = 1;
            break;
        }
        pAtual = pAtual->proximo;
    }
    return sucesso;
}

tipoLista* inicializaLista(unsigned int quantum, char nomeRecebido){
	tipoLista *novaLista;
	novaLista = (tipoLista*)malloc(sizeof(tipoLista));
	novaLista->nElementos = 0;
	novaLista->nome = nomeRecebido;
	novaLista->quantum = quantum;
	novaLista->atingiuQuantumMaximo = 0;
	novaLista->contador = 0;
	novaLista->cabeca = NULL;
	novaLista->cauda = NULL;
	return novaLista;
}

void insereElementoListaNoFinal(tipoLista **l, tipoNoh *novoNoh){
    //Insere elementos no final da lista
	tipoNoh *pAtual;
	novoNoh->proximo = NULL;
	if((*l)->nElementos == 0){
		(*l)->cabeca = novoNoh;
		(*l)->cauda = novoNoh;
		(*l)->nElementos++;
	}
	else{
		pAtual = (*l)->cauda;
		pAtual->proximo = novoNoh;
		(*l)->cauda = novoNoh;
		(*l)->nElementos++;
	}
}

void carregaLote(FILE* file, tipoLista **l){
    //carrega processos do arquivo para uma lista LOTE
    tipoProcesso processoNovo;
	tipoNoh *novoNoh;
    while((fscanf(file, "%u,%u,%u,%u,%u\n", &processoNovo.id, &processoNovo.submissionTime, &processoNovo.priority, &processoNovo.executionTime, &processoNovo.blockTime)) != EOF){
		processoNovo.status = PRONTO;
		processoNovo.turnAroundTime = 0;
		processoNovo.responseTime = 0;
		processoNovo.waitingTime = 0;
		processoNovo.tempoExecutando = 0;
		processoNovo.tempoBloqueado = 0;
		processoNovo.tempoSubmetido = 0;
		processoNovo.cabecaPg = NULL;
		novoNoh = (tipoNoh*)malloc(sizeof(tipoNoh));
		novoNoh->processo = processoNovo;
		insereElementoListaNoFinal(l, novoNoh);
    }
}

void carregaListaDePaginas(FILE* file3, tipoLista **l){
	printf("carregaListaDePaginas\n");
	tipoNoh *pAtual;
	pAtual = (*l)->cabeca;
	long posicaoAtual = 0;
	tipoListaPaginas *lp;
	tipoListaPaginas *pgAtual;
	char aux;
	char aux2;
	unsigned int idCorrigida;
	while(pAtual != NULL){
		//lembrar de inicializar todos atributos de lp
		fscanf(file3, "%u,", &idCorrigida);
		idCorrigida++;
		do{
			lp = (tipoListaPaginas*)malloc(sizeof(tipoListaPaginas));
			lp->pagina.id = idCorrigida;
			lp->proximo = NULL;
			fscanf(file3, "%u:%u,", &(lp->pagina.tempo), &(lp->pagina.nPagina));
			posicaoAtual = ftell(file3);
			fscanf(file3, "%c", &aux);
			fscanf(file3, "%c", &aux2);
			if(aux2 != '\n'){
                fseek(file3, posicaoAtual, SEEK_SET);
			}
			if(pAtual->processo.cabecaPg == NULL){
				pAtual->processo.cabecaPg = lp;
			}
			else{
				pgAtual = pAtual->processo.cabecaPg;
				while(pgAtual->proximo != NULL){
					pgAtual = pgAtual->proximo;
				}
				pgAtual->proximo = lp;
			}
		}while(aux2 != '\n');
		pAtual = pAtual->proximo;
	}
}

unsigned int criaProcessos(tipoLista **l1, tipoLista **l2, unsigned int t){
    //pega lista de LOTE e envia para lista de PRONTOS
    unsigned int sucesso = 0;
    tipoNoh *pAtual;
    tipoNoh *pAnt;
    pAtual = (*l1)->cabeca;
    pAnt = NULL;
    while(pAtual != NULL){
        if(pAtual->processo.submissionTime <= t){
            pAtual->processo.tempoSubmetido = t;
            if(pAtual == (*l1)->cabeca){
                (*l1)->cabeca = pAtual->proximo;
				insereElementoListaNoFinal(l2, pAtual);
                if((*l1)->cabeca == NULL){
                    (*l1)->cauda = NULL;
                }
                (*l1)->nElementos--;
                sucesso = 1;
                break;
            }
            else if(pAtual == (*l1)->cauda){
                pAnt->proximo = NULL;
                (*l1)->cauda = pAnt;
                insereElementoListaNoFinal(l2, pAtual);
                (*l1)->nElementos--;
                sucesso = 1;
                break;
            }
            else{
                pAnt->proximo = pAtual->proximo;
                insereElementoListaNoFinal(l2, pAtual);
                (*l1)->nElementos--;
                sucesso = 1;
                break;
            }
        }
        else{
            pAnt = pAtual;
            pAtual = pAtual->proximo;
        }
    }
    return sucesso;
}

void enviaPrimeiraChaveL1ParaL2(tipoLista **l1, tipoLista **l2, unsigned int chave){
    //envia de l1 para l2
    tipoNoh *pAtual;
    tipoNoh *pAnt;
    pAtual = (*l1)->cabeca;
    pAnt = NULL;
    while(pAtual != NULL){
        if(pAnt == NULL && pAtual->processo.status == chave){
            (*l1)->cabeca = pAtual->proximo;
			insereElementoListaNoFinal(l2, pAtual);
			if((*l1)->cabeca == NULL){
				(*l1)->cauda = NULL;
            }
            pAtual = (*l1)->cabeca;
            (*l1)->nElementos--;
            break;
        }
        else if(pAnt != NULL && pAtual->processo.status == chave){
			pAnt->proximo = pAtual->proximo;
            insereElementoListaNoFinal(l2, pAtual);
			pAtual = pAnt->proximo;
            if(pAtual == NULL){
                (*l1)->cauda = pAnt;
            }
            (*l1)->nElementos--;
            break;
        }
        else{
            pAnt = pAtual;
            pAtual = pAtual->proximo;
        }
    }
}

void enviaTodosChaveL1ParaL2(tipoLista **l1, tipoLista **l2, unsigned int chave){
    //envia de l1 para l2
    tipoNoh *pAtual;
    tipoNoh *pAnt;
    pAtual = (*l1)->cabeca;
    pAnt = NULL;
    while(pAtual != NULL){
        if(pAnt == NULL && pAtual->processo.status == chave){
			(*l1)->cabeca = pAtual->proximo;
            insereElementoListaNoFinal(l2, pAtual);
			if((*l1)->cabeca == NULL){
                (*l1)->cauda = NULL;
            }
            pAtual = (*l1)->cabeca;
            (*l1)->nElementos--;
        }
        else if(pAnt != NULL && pAtual->processo.status == chave){
			pAnt->proximo = pAtual->proximo;
            insereElementoListaNoFinal(l2, pAtual);
			pAtual = pAnt->proximo;
            if(pAtual == NULL){
                (*l1)->cauda = pAnt;
            }
            (*l1)->nElementos--;
        }
        else{
            pAnt = pAtual;
            pAtual = pAtual->proximo;
        }
    }
}

void removeChaveL1(tipoLista **l1, FILE* file2, unsigned int t,unsigned int chave, tipoStats *st){
    tipoNoh *pAtual;
    tipoNoh *pAnt;
    pAtual = (*l1)->cabeca;
    pAnt = NULL;
    while(pAtual != NULL){
        if(pAtual->processo.status == chave){
            if(chave == TERMINADO){
                pAtual->processo.turnAroundTime = t - pAtual->processo.tempoSubmetido;
                fprintf(file2, "Id: %d\nTurnAround Time: %d\nResponse Time: %d\nWaiting Time:%d\n", pAtual->processo.id, pAtual->processo.turnAroundTime,pAtual->processo.responseTime, pAtual->processo.waitingTime);
                st->nProcessos++;
                st->esperaMedia += pAtual->processo.waitingTime;
                st->respostaMedia += pAtual->processo.responseTime;
                st->retornoMedio += pAtual->processo.turnAroundTime;
                st->servicoMedio += pAtual->processo.executionTime - pAtual->processo.blockTime;
            }
            if(pAtual == (*l1)->cabeca){
                (*l1)->cabeca = pAtual->proximo;
                if((*l1)->cabeca == NULL){
                    (*l1)->cauda = NULL;
                }
                free(pAtual);
                (*l1)->nElementos--;
                break;
            }
            else if(pAtual == (*l1)->cauda){
                pAnt->proximo = pAtual->proximo;
                (*l1)->cauda = pAnt;
                free(pAtual);
                (*l1)->nElementos--;
                break;
            }
            else{
                pAnt->proximo = pAtual->proximo;
                free(pAtual);
                (*l1)->nElementos--;
                break;
            }
        }
        else{
            pAnt = pAtual;
            pAtual = pAtual->proximo;
        }
    }
}

void enviaElementoParaFinalDaLista(tipoLista **l){
    tipoNoh *pAtual;
    pAtual = (*l)->cabeca;
    if((*l)->nElementos > 1){
        if(pAtual->processo.status == PRONTO){
            (*l)->cauda->proximo = pAtual;
            (*l)->cabeca = pAtual->proximo;
            pAtual->proximo = NULL;
            (*l)->cauda = pAtual;
        }
    }
}

void imprimeLista2(tipoLista **l){
	tipoNoh *pAtual;
	pAtual = (*l)->cabeca;
	unsigned int i;
	i = 0;
	while(pAtual != NULL){
		printf("Elemento %d:\n", i);
		printf("\tProcesso %d, Status: %d, Priority: %d, SubmissionTime: %d\n", pAtual->processo.id, pAtual->processo.status, pAtual->processo.priority, pAtual->processo.submissionTime);
		printf("\tExecutionTime: %d, blockTime: %d, turnAroundTime: %d, responseTime: %d\n", pAtual->processo.executionTime, pAtual->processo.blockTime, pAtual->processo.turnAroundTime, pAtual->processo.responseTime);
		printf("\twaitingTime: %d, tempoExecutando: %d, tempoBloqueado: %d, tempoSubmetido: %d\n", pAtual->processo.waitingTime, pAtual->processo.tempoExecutando, pAtual->processo.tempoBloqueado, pAtual->processo.tempoSubmetido);
		pAtual = pAtual->proximo;
		i++;
	}
}

void imprimeLista(tipoLista **l){
	tipoNoh *pAtual;
	pAtual = (*l)->cabeca;
	unsigned int i;
	i = 0;
	if(pAtual == NULL){
        printf("NULL\n");
	}
	while(pAtual != NULL){
        if(pAtual->processo.status == EXECUTANDO){
            printf("(P%d)--", pAtual->processo.id);
        }
        else{
            printf("P%d--", pAtual->processo.id);
        }
		//printf("\tProcesso %d, Status: %d, Priority: %d, SubmissionTime: %d\n", pAtual->processo.id, pAtual->processo.status, pAtual->processo.priority, pAtual->processo.submissionTime);
		//printf("\tExecutionTime: %d, blockTime: %d, turnAroundTime: %d, responseTime: %d\n", pAtual->processo.executionTime, pAtual->processo.blockTime, pAtual->processo.turnAroundTime, pAtual->processo.responseTime);
		//printf("\twaitingTime: %d, tempoExecutando: %d, tempoBloqueado: %d, tempoSubmetido: %d\n", pAtual->processo.waitingTime, pAtual->processo.tempoExecutando, pAtual->processo.tempoBloqueado, pAtual->processo.tempoSubmetido);
		pAtual = pAtual->proximo;
		if(pAtual == NULL){
            printf("NULL\n");
		}
		i++;
	}
}

void executaProcesso(tipoLista **l, tipoQuadro *q){
    printf("\texecutaProcesso\n");
	tipoListaPaginas *pgAtual;
	tipoNoh *pAtual;
	pAtual = (*l)->cabeca;
    if(pAtual != NULL){
		pgAtual = pAtual->processo.cabecaPg;
		while(pgAtual != NULL){
			if(pgAtual->pagina.tempo == pAtual->processo.tempoExecutando || pgAtual->pagina.nPagina == 0){
				gerenciaPaginas(q, pgAtual->pagina.id, pgAtual->pagina.nPagina, pgAtual->pagina.tempo);
			}
			pgAtual = pgAtual->proximo;
		}
        pAtual->processo.status = EXECUTANDO;
    }
}

void calculaEstatisticas(tipoLista **l, unsigned int t){
    printf("\tcalculaEstatisticas\n");
    tipoNoh *pAtual;
    pAtual = (*l)->cabeca;
    while(pAtual != NULL){
        if(pAtual->processo.status == PRONTO){
            pAtual->processo.waitingTime++;
        }
        if(pAtual->processo.status == BLOQUEADO || pAtual->processo.status == SUSPENSO){
            pAtual->processo.tempoBloqueado++;
        }
        if(pAtual->processo.status == EXECUTANDO){
            pAtual->processo.tempoExecutando++;
            (*l)->contador++;
            if(pAtual->processo.tempoExecutando == 1){
                pAtual->processo.responseTime = t - pAtual->processo.tempoSubmetido;
            }
        }
        pAtual = pAtual->proximo;
    }
}

void mudaEstado(tipoLista **l, tipoLista **lote, tipoQuadro *q, unsigned int memoriaUsada, unsigned int t){
    printf("\tmudaEstado\n");
    tipoNoh *pAtual;
    pAtual = (*l)->cabeca;
    while(pAtual != NULL){
        if(pAtual->processo.status == PRONTO){

        }
        if(pAtual->processo.status == BLOQUEADO || pAtual->processo.status == SUSPENSO){
            if(pAtual->processo.tempoBloqueado == pAtual->processo.blockTime){
                pAtual->processo.status = PRONTO;
            }
        }
        if(pAtual->processo.status == EXECUTANDO){
            if(pAtual->processo.tempoBloqueado + pAtual->processo.tempoExecutando == pAtual->processo.executionTime){
                pAtual->processo.status = TERMINADO;
				removePaginas(q, pAtual->processo.id);
                (*l)->contador = 0;
            }
            if(pAtual->processo.tempoBloqueado != pAtual->processo.blockTime){
                if(temProcessoNovo(lote, t+1) && memoriaUsada == ALPHA){
                    pAtual->processo.status = SUSPENSO;
                    removePaginas(q, pAtual->processo.id);
                }
                else{
                    pAtual->processo.status = BLOQUEADO;
                }
                (*l)->contador = 0;
            }
            if((*l)->contador == (*l)->quantum){
                pAtual->processo.status = PRONTO;
                (*l)->contador = 0;
                (*l)->atingiuQuantumMaximo = 1;
            }
        }
        pAtual = pAtual->proximo;
    }
}

int main(){
    unsigned int t = 0;
    tipoStats estatisticas = {0,0,0,0,0,0,0,0};
	tipoQuadro quadros;
    FILE *file;
    FILE *file2;
	FILE *file3;
    tipoLista *lote;
    tipoLista *listaProcessos;
    tipoLista *listaBloqueados;
	tipoLista *listaSuspensos;
	inicializaQuadros(&quadros);
    lote = inicializaLista(0, 'L');
    listaProcessos = inicializaLista(2, 'P');
    listaBloqueados = inicializaLista(0, 'B');
	listaSuspensos = inicializaLista(0, 'S');
    file = fopen("cenario1.txt","r");
    file2 = fopen("saidaCenario1.1.txt","w");
	file3 = fopen("referencias1.txt", "rb");
    if(file == NULL || file2 == NULL){
        printf("Erro ao abrir arquivos.\n");
    }
    else{
        carregaLote(file, &lote);
        carregaListaDePaginas(file3, &lote);
        while(lote->nElementos + listaBloqueados->nElementos + listaProcessos->nElementos + listaSuspensos->nElementos != 0){
            //system("pause");
            printf("t = %d\n", t);
			//cria novos processos
            while(((listaProcessos->nElementos + listaBloqueados->nElementos) < ALPHA) && lote->nElementos != 0){
                if(!(criaProcessos(&lote, &listaProcessos, t))){
                    break;
                }
            }
			//Se um processo atingiu seu tempo máximo de execução, é jogado para o final da fila
			if(listaProcessos->atingiuQuantumMaximo == 1){
                enviaElementoParaFinalDaLista(&listaProcessos);
                listaProcessos->atingiuQuantumMaximo = 0;
            }
			//Traz processos que estavam bloqueados para a lista de processos prontos
            enviaTodosChaveL1ParaL2(&listaBloqueados, &listaProcessos, PRONTO);
			//Traz processos que estavam suspensos para a lista de processos prontos caso não existem mais processos para serem criados
			while((listaProcessos->nElementos + listaBloqueados->nElementos) < ALPHA && temChave(&listaSuspensos, PRONTO)){
				enviaPrimeiraChaveL1ParaL2(&listaSuspensos, &listaProcessos, PRONTO);
			}
            printf("\tTamanho:\n\t\tLote:%d\n\t\tProcessos:%d\n\t\tBloqueados:%d\n\t\tSuspensos:%d\n", lote->nElementos, listaProcessos->nElementos, listaBloqueados->nElementos, listaSuspensos->nElementos);
            executaProcesso(&listaProcessos, &quadros);
            calculaEstatisticas(&listaProcessos, t);
            calculaEstatisticas(&listaBloqueados, t);
			calculaEstatisticas(&listaSuspensos, t);
            mudaEstado(&listaProcessos, &lote, &quadros, (listaProcessos->nElementos + listaBloqueados->nElementos), t);
            mudaEstado(&listaBloqueados, &lote, &quadros, (listaProcessos->nElementos + listaBloqueados->nElementos), t);
			mudaEstado(&listaSuspensos, &lote, &quadros, (listaProcessos->nElementos + listaBloqueados->nElementos), t);
			//Remove processos que foram terminados
            if(listaProcessos->cabeca != NULL){
                if(listaProcessos->cabeca->processo.status == TERMINADO){
                    removeChaveL1(&listaProcessos, file2, t, TERMINADO, &estatisticas);
                }
                //Coleta processos suspensos ou bloqueados e os envia para suas respectivas listas
                else if(listaProcessos->cabeca->processo.status == BLOQUEADO){
                    enviaPrimeiraChaveL1ParaL2(&listaProcessos, &listaBloqueados, BLOQUEADO);
                }
                else if(listaProcessos->cabeca->processo.status == SUSPENSO){
                    enviaPrimeiraChaveL1ParaL2(&listaProcessos, &listaSuspensos, SUSPENSO);
                }
			}
        t++;
        }
        estatisticas.duracaoDaSimulacao = t;
        estatisticas.esperaMedia = estatisticas.esperaMedia/estatisticas.nProcessos;
        estatisticas.respostaMedia = estatisticas.respostaMedia/estatisticas.nProcessos;
        estatisticas.retornoMedio = estatisticas.retornoMedio/estatisticas.nProcessos;
        estatisticas.throughput = (float)estatisticas.nProcessos/estatisticas.duracaoDaSimulacao;
        estatisticas.usoProcessador = estatisticas.servicoMedio/estatisticas.duracaoDaSimulacao;
        estatisticas.servicoMedio = estatisticas.servicoMedio/estatisticas.nProcessos;
        fprintf(file2, "_______________________________\n");
        fprintf(file2, "Round and Robin\n");
        fprintf(file2, "Alpha: %d\n", ALPHA);
        fprintf(file2, "Número de Processos: %d\n", estatisticas.nProcessos);
        fprintf(file2, "Tempo de retorno médio: %f\n", estatisticas.retornoMedio);
        fprintf(file2, "Tempo de serviço médio: %f\n", estatisticas.servicoMedio);
        fprintf(file2, "Utilização do processador: %f\n", estatisticas.usoProcessador);
        fprintf(file2, "Tempo de resposta médio: %f\n", estatisticas.respostaMedia);
        fprintf(file2, "Throughput: %f\n", estatisticas.throughput);
        fprintf(file2, "Tempo de espera médio: %f\n", estatisticas.esperaMedia);
        fprintf(file2, "Duração da simulação: %d\n", estatisticas.duracaoDaSimulacao);
		fprintf(file2, "Número de faltas: %d\n", quadros.nFaltas);
    }
    fclose(file);
    fclose(file2);
	fclose(file3);
    return 0;
}
