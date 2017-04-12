struct pagina{
	unsigned int id;
	unsigned int nPagina;
	unsigned int ordemInsercao; //comecar com 1
};typedef struct pagina tipoPagina;

struct quadro{
	tipoPagina p[TAMQUADROS];
	unsigned int temQuadroLivre;
	unsigned int nFaltas;
	unsigned int ativaFaltas;
	unsigned int proximaInsercao; //começar com 1
};typedef struct quadro tipoQuadro;

void gerenciaPaginas(tipoQuadro *q, unsigned int idRecebida, unsigned int pagRecebida){
	//miss = 0
	//hit = 1
	unsigned int hit = 0;
	unsigned int i;
	unsigned int nQuadroLivre = 0;
	unsigned int primeiroInserido = 0;
	q.temQuadroLivre = 0;
	for(i = 0;i < TAMQUADROS;i++){
		//verifica se deu HIT
		if(q.p[i].id == idRecebida && q.p[i].nPagina == pagRecebida){
			hit = 1;
			break;
		}
		//verifica se tem algum quadro vazio
		if(q.p[i].ordemInsercao == 0 && q.temQuadroLivre == 0){
			q.temQuadroLivre = 1;
			nQuadroLivre = i;
		}
	}
	if(!hit){
		if(q.temQuadroLivre){
			q.p[nQuadroLivre].id = idRecebida;
			q.p[nQuadroLivre].nPagina = pagRecebida;
			q.p[nQuadroLivre].ordemInsercao = proximaInsercao;
			q.proximaInsercao++;
		}
		else{
			for(i = 1;i < TAMQUADROS;i++){
				if(q.p[i].ordemInsercao < q.p[primeiroInserido].ordemInsercao)
					primeiroInserido = i; 
			}
			q.p[primeiroInserido].id = idRecebida;
			q.p[primeiroInserido].nPagina = pagRecebida;
			q.p[primeiroInserido].ordemInsercao = proximaInsercao;
			q.proximaInsercao++;
		}
		if(q.ativaFaltas) q.nFaltas++;
	}
	//ativar faltas
	if(!q.ativaFaltas){
		q.ativaFaltas = 1;
		for(i = 0;i < TAMQUADROS;i++){
			if(q.p[i].ordemInsercao == 0){
				q.ativaFaltas = 0;
				break;
			}
		}
	}
}