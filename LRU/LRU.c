struct pagina{
	unsigned int id;
	unsigned int nPagina;
	unsigned int idade;
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
		q->p[i].idade = 0;
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
			q->p[i].idade = 0;
		}
	}
}

void gerenciaPaginas(tipoQuadro *q, unsigned int idRecebida, unsigned int pagRecebida, unsigned int tempoRecebido){
	//miss = 0
	//hit = 1
	unsigned int hit = 0;
	unsigned int i;
	unsigned int nQuadroLivre = 0;
	unsigned int maisVelho = 0;
	q.temQuadroLivre = 0;
	//verifica hit e aumenta idade
	for(i = 0;i < TAMQUADROS;i++){
		//verifica se deu HIT
		if(q->p[i].id == idRecebida && q->p[i].nPagina == pagRecebida && q->p[i].tempo == tempoRecebido){
			hit = 1;
			q->p[i].idade = 0;
		}
		else{
			q->p[i].idade++;
		}
		//verifica se existem quadros livres
		if(q->p[i].id == 0 && q->p[i].nPagina == 0 && q->p[i].idade == 0 && q->p[i].tempo == 0 && q->temQuadroLivre == 0){
			q->temQuadroLivre = 1;
			nQuadroLivre = i;
		}
	}
	//miss
	if(!hit){
		if(q->temQuadroLivre){
			q->p[nQuadroLivre].id = idRecebida;
			q->p[nQuadroLivre].nPagina = pagRecebida;
			q->p[nQuadroLivre].tempo = tempoRecebido;
			q->p[nQuadroLivre].idade = 0;
		}
		else{
			for(i = 1;i < TAMQUADROS;i++){
				if(q->p[i].idade > q->p[maisVelho].idade) maisVelho = i;
			}
			q->p[maisVelho].id = idRecebida;
			q->p[maisVelho].nPagina = pagRecebida;
			q->p[maisVelho].tempo = tempoRecebido;
			q->p[i].idade = 0;
		}
		if(q->ativaFaltas) q->nFaltas++;
	}
	//ativar faltas
	if(!q->ativaFaltas){
		q->ativaFaltas = 1;
		for(i = 0;i < TAMQUADROS;i++){
			if(q->p[i].id == 0 && q->p[i].nPagina == 0 && q->p[i].idade == 0 && q->p[i].tempo == 0){
				q->ativaFaltas = 0;
				break;
			}
		}
	}
}