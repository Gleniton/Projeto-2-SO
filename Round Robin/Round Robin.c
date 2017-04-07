#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#define VAZIO 0
#define PRONTO 1
#define EXECUTANDO 2
#define BLOQUEADO 3
#define TERMINADO 4
#define SUSPENSO 5

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

struct processo{
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


tipoLista* inicializaLista(unsigned int quantum){
	tipoLista *novaLista;
	novaLista = (tipoLista*)malloc(sizeof(tipoLista));
	novaLista->nElementos = 0;
	novaLista->quantum = quantum;
	novaLista->atingiuQuantumMaximo = 0;
	novaLista->contador = 0;
	novaLista->cabeca = NULL;
	novaLista->cauda = NULL;
	return novaLista;
}

void insereElementoListaNoFinal(tipoLista **l, tipoProcesso processoRecebido){
    //Insere elementos no final da lista
	tipoNoh *novoElemento;
	tipoNoh *pAtual;
	novoElemento = (tipoNoh*)malloc(sizeof(tipoNoh));
	novoElemento->processo = processoRecebido;
	novoElemento->proximo = NULL;
	if((*l)->nElementos == 0){
		(*l)->cabeca = novoElemento;
		(*l)->cauda = novoElemento;
		(*l)->nElementos++;
	}
	else{
		pAtual = (*l)->cauda;
		pAtual->proximo = novoElemento;
		(*l)->cauda = novoElemento;
		(*l)->nElementos++;
	}
}

void carregaLote(FILE* file, tipoLista** l){
    //carrega processos do arquivo para uma lista LOTE
    tipoProcesso processoNovo;
    while((fscanf(file, "%u,%u,%u,%u,%u\n", &processoNovo.id, &processoNovo.submissionTime, &processoNovo.priority, &processoNovo.executionTime, &processoNovo.blockTime)) != EOF){
        processoNovo.status = PRONTO;
		processoNovo.turnAroundTime = 0;
		processoNovo.responseTime = 0;
		processoNovo.waitingTime = 0;
		processoNovo.tempoExecutando = 0;
		processoNovo.tempoBloqueado = 0;
		processoNovo.tempoSubmetido = 0;
		insereElementoListaNoFinal(l, processoNovo);
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
                if((*l1)->cabeca == NULL){
                    (*l1)->cauda = NULL;
                }
                insereElementoListaNoFinal(l2, pAtual->processo);
                (*l1)->nElementos--;
                free(pAtual);
                sucesso = 1;
                break;
            }
            else if(pAtual == (*l1)->cauda){
                pAnt->proximo = pAtual->proximo;
                (*l1)->cauda = pAnt;
                insereElementoListaNoFinal(l2, pAtual->processo);
                (*l1)->nElementos--;
                free(pAtual);
                sucesso = 1;
                break;
            }
            else{
                pAnt->proximo = pAtual->proximo;
                insereElementoListaNoFinal(l2, pAtual->processo);
                (*l1)->nElementos--;
                free(pAtual);
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
            insereElementoListaNoFinal(l2, pAtual->processo);
            (*l1)->cabeca = pAtual->proximo;
			if((*l1)->cabeca == NULL){
				(*l1)->cauda = NULL;
            }
            free(pAtual);
            pAtual = (*l1)->cabeca;
            (*l1)->nElementos--;
            break;
        }
        else if(pAnt != NULL && pAtual->processo.status == chave){
            insereElementoListaNoFinal(l2, pAtual->processo);
            if(pAtual->proximo == NULL){
                (*l1)->cauda = pAnt;
            }
            pAnt->proximo = pAtual->proximo;
            free(pAtual);
            pAtual = pAnt->proximo;
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
            insereElementoListaNoFinal(l2, pAtual->processo);
            (*l1)->cabeca = pAtual->proximo;
			if((*l1)->cabeca == NULL){
                (*l1)->cauda = NULL;
            }
            free(pAtual);
            pAtual = (*l1)->cabeca;
            (*l1)->nElementos--;
        }
        else if(pAnt != NULL && pAtual->processo.status == chave){
            insereElementoListaNoFinal(l2, pAtual->processo);
            if(pAtual->proximo == NULL){
                (*l1)->cauda = pAnt;
            }
            pAnt->proximo = pAtual->proximo;
            free(pAtual);
            pAtual = pAnt->proximo;
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

void executaProcesso(tipoLista **l){
    if((*l)->cabeca != NULL){
        (*l)->cabeca->processo.status = EXECUTANDO;
    }
}

void calculaEstatisticas(tipoLista **l, unsigned int t){
    tipoNoh *pAtual;
    pAtual = (*l)->cabeca;
    while(pAtual != NULL){
        if(pAtual->processo.status == PRONTO){
            pAtual->processo.waitingTime++;
        }
        if(pAtual->processo.status == BLOQUEADO){
            pAtual->processo.tempoBloqueado++;
        }
        if(pAtual->processo.status == EXECUTANDO){
            pAtual->processo.tempoExecutando++;
            (*l)->contador++;
            if(pAtual->processo.tempoExecutando == 1){
                pAtual->processo.responseTime = t;
            }
        }
        pAtual = pAtual->proximo;
    }
}


void mudaEstado(tipoLista **l){
    tipoNoh *pAtual;
    pAtual = (*l)->cabeca;
    while(pAtual != NULL){
        if(pAtual->processo.status == PRONTO){

        }
        if(pAtual->processo.status == BLOQUEADO){
            if(pAtual->processo.tempoBloqueado == pAtual->processo.blockTime){
                pAtual->processo.status = PRONTO;
            }
        }
        if(pAtual->processo.status == EXECUTANDO){
            if(pAtual->processo.tempoBloqueado + pAtual->processo.tempoExecutando == pAtual->processo.executionTime){
                pAtual->processo.status = TERMINADO;
                (*l)->contador = 0;
            }
            if(pAtual->processo.tempoBloqueado != pAtual->processo.blockTime){
                pAtual->processo.status = BLOQUEADO;
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
    unsigned int alpha = 200;
    unsigned int t = 0;
    tipoStats estatisticas = {0,0,0,0,0,0,0,0};
    FILE *file;
    FILE *file2;
    tipoLista *lote;
    tipoLista *listaProcessos;
    tipoLista *listaBloqueados;
    lote = inicializaLista(0);
    listaProcessos = inicializaLista(4);
    listaBloqueados = inicializaLista(0);
    file = fopen("cenario5.txt","r");
    file2 = fopen("./Round_Robin_Saida/saidacenario5.2.txt","w");
    if(file == NULL || file2 == NULL){
        printf("Erro ao abrir arquivos.\n");
    }
    else{
        carregaLote(file, &lote);
        while(lote->nElementos + listaBloqueados->nElementos + listaProcessos->nElementos != 0){
            //printf("t = %d\n", t);
            while(((listaProcessos->nElementos + listaBloqueados->nElementos) < alpha) && lote->nElementos != 0){
                if(!(criaProcessos(&lote, &listaProcessos, t))){
                    break;
                }
            }
            enviaTodosChaveL1ParaL2(&listaBloqueados, &listaProcessos, PRONTO);
            //printf("t(%d) %d %d %d\n", t, lote->nElementos, listaProcessos->nElementos, listaBloqueados->nElementos);
            executaProcesso(&listaProcessos);
            calculaEstatisticas(&listaProcessos, t);
            calculaEstatisticas(&listaBloqueados, t);
            mudaEstado(&listaProcessos);
            mudaEstado(&listaBloqueados);
            removeChaveL1(&listaProcessos, file2, t, TERMINADO, &estatisticas);
            enviaPrimeiraChaveL1ParaL2(&listaProcessos, &listaBloqueados, BLOQUEADO);
            if(listaProcessos->atingiuQuantumMaximo == 1){
                enviaElementoParaFinalDaLista(&listaProcessos);
                listaProcessos->atingiuQuantumMaximo = 0;
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
        fprintf(file2, "Round Robin\n");
        fprintf(file2, "Alpha: %d\n", alpha);
        fprintf(file2, "Número de Processos: %d\n", estatisticas.nProcessos);
        fprintf(file2, "Tempo de retorno médio: %f\n", estatisticas.retornoMedio);
        fprintf(file2, "Tempo de serviço médio: %f\n", estatisticas.servicoMedio);
        fprintf(file2, "Utilização do processador: %f\n", estatisticas.usoProcessador);
        fprintf(file2, "Tempo de resposta médio: %f\n", estatisticas.respostaMedia);
        fprintf(file2, "Throughput: %f\n", estatisticas.throughput);
        fprintf(file2, "Tempo de espera médio: %f\n", estatisticas.esperaMedia);
        fprintf(file2, "Duração da simulação: %d\n", estatisticas.duracaoDaSimulacao);
    }
    fclose(file);
    fclose(file2);
    return 0;
}
