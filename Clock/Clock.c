struct pagina{
	unsigned int id;
	unsigned int nPagina;
	unsigned int sinal;
}typedef struct pagina tipoPagina;

struct quadro{
	tipoPagina p[40];
	unsigned int temQuadroLivre;
	unsigned int nFaltas;
	unsigned int ativaFaltas;
	unsigned int ponteiro; //começar com 0
}typedef struct quadro tipoQuadro;

tipoQuadro quadros[40];

void gerenciaPaginas(tipoQuadro *q, unsigned int idRecebida, unsigned int pagRecebida){
	//miss = 0
	//hit = 1
	unsigned int hit = 0;
	unsigned int i;
	unsigned int nQuadroLivre = 0;
	q.temQuadroLivre = 0;
	for(i = 0;i < 40;i++){
		if(q.p[i].id == idRecebida && q.p[i].nPagina == pagRecebida){
			hit = 1;
			q.p[i].sinal = 1;
			break;
		}
		if(q.p[i].id == 0 && q.p[i].nPagina == 0 && q.p[i].sinal == 0 && q.temQuadroLivre == 0){
			q.temQuadroLivre = 1;
			nQuadroLivre = i;
		}
	}
	if(!hit){
		if(q.temQuadroLivre == 1){
			q.ponteiro = nQuadroLivre;
		}
		while(q.p[q.ponteiro].sinal == 1){
			q.p[q.ponteiro].sinal = 0;
			q.ponteiro++;
			if(q.ponteiro == 40) q.ponteiro = 0;
		}
		q.p[q.ponteiro].id = idRecebida;
		q.p[q.ponteiro].nPagina = pagRecebida;
		q.p[q.ponteiro].sinal = 1;
		q.ponteiro++;
		if(q.ativaFaltas) q.nFaltas++;
	}
	//ativar faltas
	if(!q.ativaFaltas){
		q.ativaFaltas = 1;
		for(i = 0;i < 40;i++){
			if(q.p[i].id == 0 && q.p[i].nPagina == 0 && q.p[i].sinal == 0){
				q.ativaFaltas = 0;
				break;
			}
		}
	}
}