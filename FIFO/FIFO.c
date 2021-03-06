struct pagina{
	unsigned int id;
	unsigned int nPagina;
	unsigned int ordemInsercao; //comecar com 1
	unsigned int tempo;
};typedef struct pagina tipoPagina;

struct quadro{
	tipoPagina p[TAMQUADROS];
	unsigned int temQuadroLivre;
	unsigned int nFaltas;
	unsigned int ativaFaltas;
	unsigned int proximaInsercao; //começar com 1
};typedef struct quadro tipoQuadro;

void inicializaQuadros(tipoQuadro *q){
	int i;
	q->temQuadroLivre = 0;
	q->nFaltas = 0;
	q->ativaFaltas = 0;
	q->proximaInsercao = 1;
	for(i = 0;i < TAMQUADROS;i++){
		q->p[i].id = 0;
		q->p[i].nPagina = 0;
		q->p[i].ordemInsercao = 0;
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
			q->p[i].ordemInsercao = 0;
		}
	}
}

void gerenciaPaginas(tipoQuadro *q, unsigned int idRecebida, unsigned int pagRecebida, unsigned int tempoRecebido){
	//miss = 0
	//hit = 1
	unsigned int hit = 0;
	unsigned int i;
	unsigned int nQuadroLivre = 0;
	unsigned int primeiroInserido = 0;
	q.temQuadroLivre = 0;
	for(i = 0;i < TAMQUADROS;i++){
		//verifica se deu HIT
		if(q->p[i].id == idRecebida && q->p[i].nPagina == pagRecebida && q->p[i].tempo == tempoRecebido){
			hit = 1;
			break;
		}
		//verifica se tem algum quadro vazio
		if(q->p[i].ordemInsercao == 0 && q->temQuadroLivre == 0){
			q->temQuadroLivre = 1;
			nQuadroLivre = i;
		}
	}
	if(!hit){
		if(q->temQuadroLivre){
			q->p[nQuadroLivre].id = idRecebida;
			q->p[nQuadroLivre].nPagina = pagRecebida;
			q->p[nQuadroLivre].tempo = tempoRecebido;
			q->p[nQuadroLivre].ordemInsercao = q->proximaInsercao;
			q->proximaInsercao++;
		}
		else{
			for(i = 1;i < TAMQUADROS;i++){
				if(q->p[i].ordemInsercao < q->p[primeiroInserido].ordemInsercao)
					primeiroInserido = i; 
			}
			q->p[primeiroInserido].id = idRecebida;
			q->p[primeiroInserido].nPagina = pagRecebida;
			q->p[primeiroInserido].tempo = tempoRecebido;
			q->p[primeiroInserido].ordemInsercao = q->proximaInsercao;
			q->proximaInsercao++;
		}
		if(q->ativaFaltas) q->nFaltas++;
	}
	//ativar faltas
	if(!q->ativaFaltas){
		q->ativaFaltas = 1;
		for(i = 0;i < TAMQUADROS;i++){
			if(q->p[i].ordemInsercao == 0){
				q->ativaFaltas = 0;
				break;
			}
		}
	}
}