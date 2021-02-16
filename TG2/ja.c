#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define MAXIMO_NOME_JOGADOR 50
#define ENERGIA_INICIAL_JOGADOR 500
#define DANO_INICIAL_JOGADOR 25
#define CELULA_INICIAL_JOGADOR 10
#define ANDAR_INICIAL_JOGADOR 1
#define MAXIMO_NOME_MONSTRO 50
#define ENERGIA_INICIAL_MONSTRO 500
#define DANO_INICIAL_MONSTRO 75
#define CELULA_INICIAL_MONSTRO 14
#define ANDAR_INICIAL_MONSTRO 2
#define NENHUM -1
#define MAXIMO_NOME_CELULA 50
#define MAXIMO_TEXTO_DESCRICAO 500
#define MAX_CELULAS 100
#define MAX_OBJETOS 100
#define MAXIMO_NOME_OBJETO 50
#define MOVIMENTO 50
#define MAXLIN 100
#define COR_JOGADOR 1
#define COR_MONSTRO 2
#define COR_PERIGO 3
#define COR_AVISO 4
#define COR_BOM 5
#define COR_ADMIN 6


struct Jogador{
	char nome[MAXIMO_NOME_JOGADOR];
	int energia;
	int dano;
	int celula;
	int objeto;
	int tesouro;
};

struct Monstro{
	char nome[MAXIMO_NOME_MONSTRO];
	int energia;
	int dano;
	int celula;
};

struct Celula{
	int norte;
	int sul;
	int oeste;
	int este;
	int cima;
	int baixo;
	//struct Objeto *objeto;
	int andar;
	int objeto;
	int tesouro;
	//char nome[MAXIMO_NOME_CELULA];
	char descricao[MAXIMO_TEXTO_DESCRICAO];	
};

struct Objeto{
	char nome[MAXIMO_NOME_OBJETO];
	int eficiencia;
};

WINDOW *CriarJanelas(int altura, int comprimento, int yinicial, int xinicial);
void IniciarVisualizacao(WINDOW *janelas []);
void CarregarJogo(struct Jogador *jogador, struct Monstro *monstro);
void GuardarJogoAtual(struct Jogador *jogador, struct Monstro *monstro);
void InicializarJogador(struct Jogador *jogador);
void InicializarMonstro(struct Monstro *pMonstro);
void DarBoasVindas(struct Jogador *jogador, struct Monstro *monstro);
void ListarJogador(struct Jogador *jogador);
int CriarObjetos(struct Objeto objetos[]);
void ListarObjetos(struct Objeto objetos[], int nObjetos);
int InicializarMapa(struct Celula mapa[]);
void ListarObjetos(struct Objeto objetos[], int nObjetos);
void ListarMapa(struct Celula mapa[], struct Objeto objetos[], int nCelulasMapa);
void MostrarJogadorSalaAtual(struct Jogador *jogador, struct Celula mapa[]);
void VerificarObjeto(struct Jogador *jogador, struct Objeto objeto [], struct Celula mapa []);
void VerificarTesouro(struct Jogador *jogador, struct Celula mapa []);
void MostrarSituacaoAtual(struct Jogador *jogador, struct Monstro *monstro, struct Objeto objeto[]);
void MostrarSituacaoAtualSU(struct Jogador *jogador, struct Monstro *monstro, struct Celula mapa[]);
void MovimentarJogador(struct Jogador *jogador, struct Monstro *monstro, struct Celula mapa[], int *fimDeJogo);
void MenuSair(struct Jogador *jogador, struct Monstro *monstro, int *fimDeJogo);
void MovimentarMonstro(struct Jogador *jogador, struct Monstro *monstro, struct Celula mapa[]);
void VerificarCombate(struct Jogador *jogador, struct Monstro *monstro, struct Objeto objeto[], int *combate);
void Combater(struct Jogador *jogador, struct Monstro *monstro, struct Objeto objeto[]);
void VerificarSeFoge(int *combate);
void VerificarModoDeJogo(struct Jogador *jogador, int argc, char* argv[], int *su);
void VerificarfimDeJogo(struct Jogador *jogador, struct Monstro *monstro, int *fimDeJogo);


int main(int argc, char* argv[]){
	/*WINDOW *janelaInicial;
	WINDOW *janelaJogador;
	WINDOW *janelaMonstro;
	WINDOW *janelaMapa;
	WINDOW *janelaAcao;
	WINDOW *janelaAdmin;

	int alturaInicial, comprimentoInicial = 50;
	int yInicial, xInicial = 0;

	int alturaJogador = 40;
	int comprimentoJogador = 20;
	int yJogador = 0;
	int xJogador = comprimentoInicial - 22;

	int alturaMonstro = 40;
	int comprimentoMonstro = 20;
	int yMonstro = comprimentoJogador + 2;
	int xMonstro = xJogador;

	int alturaMapa = 20;
	int comprimentoMapa = 50;
	int yMapa = 0;
	int xMapa = 0;

	int alturaAcao = 20;
	int comprimentoAcao = 50;
	int yAcao = comprimentoMapa + 2;
	int xAcao = xMapa;

	int alturaAdmin = 20;
	int comprimentoAdmin = 50;
	int yAdmin = comprimentoAcao + 2;
	int xIAdmin = xAcao;*/

	// Criar semáforos
	//semáforo que indica que o jogador já pode movimentar
  	sem_t *semMovimentarJogador = mmap(NULL, sizeof (sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	//semáforo que indica que o monstro já pode movimentar
	sem_t *semMovimentarMonstro = mmap(NULL, sizeof (sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	// Criação de variáveis de memória partilhada
	struct Jogador *jogador = mmap(NULL, sizeof (struct Jogador), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	struct Monstro *monstro = mmap(NULL, sizeof (struct Monstro), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	struct Celula *mapa = mmap(NULL, MAX_CELULAS * sizeof (struct Celula), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	struct Objeto *objetos = mmap(NULL, MAX_OBJETOS * sizeof (struct Objeto), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int *su = mmap(NULL, sizeof (int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int *combate = mmap(NULL, sizeof (int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int *fimDeJogo = mmap(NULL, sizeof (int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	int nCelulasMapa, nObjetos, pid, childPid;
	char decisao[1];
	*su = 0;
	*combate = 0;
	*fimDeJogo = 0;

	// Iniciar os semáforos
	sem_init(semMovimentarJogador, 1, 0);
  	sem_init(semMovimentarMonstro, 1, 0);
	
	//IniciarVisualizacao(janelas);

	/*int linhas = 10, colunas = 40, y = 2, x = 4, i;

	initscr();
	cbreak();
	noecho();

	// Creria janelas
	janelas[0] = newwin(100, 100, 0, 0);
	janelas[1] = newwin(60, 40, 2, 2);
	janelas[2] = newwin(30, 20, 2, 44);
	janelas[4] = newwin(30, 20, 34, 44);

	// Adiciona caixas as janelas
	for(i = 0; i < 4; ++i)
		box(janelas[i], 0, 0);

	endwin();*/

	initscr(); //Start curses mode
	if (has_colors() == FALSE) {
		endwin();
		printw("O terminal não suporta cores!\n");
		refresh();
	}

	// Criação das cores
	start_color();

	init_pair(COR_JOGADOR, COLOR_CYAN, COLOR_BLACK);
	init_pair(COR_MONSTRO, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COR_PERIGO, COLOR_RED, COLOR_BLACK);
	init_pair(COR_AVISO, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COR_BOM, COLOR_GREEN, COLOR_BLACK);
	init_pair(COR_ADMIN, COLOR_BLUE, COLOR_BLACK);

	//janelaInicial = CriarJanelas(alturaInicial, comprimentoInicial, yInicial, xInicial);

	// Processo que trata de criar o jogo
	pid=fork();
  	if(pid == 0){

		// Iniciar mapa atual
		nCelulasMapa = InicializarMapa(mapa);

		// Criar objetos
		nObjetos = CriarObjetos(objetos);
		
		//Verificar super user
		VerificarModoDeJogo(jogador, argc, argv, su);

		// Só mostra se for super user
		if (*su == 1){
			//Inicializar monstro
			InicializarMonstro(monstro);

			//Listar Objetos
			ListarObjetos(objetos, nObjetos);
			sleep(5);
			clear();

			//Listar mapa
			ListarMapa(mapa, objetos, nCelulasMapa);
		}else{
			printw("Seja bem vindo!\nGostaria de carregar um jogo guardado? (s -> sim ou n -> não)\n");
			refresh();
			scanf("%s", decisao);
			if (strcmp(decisao, "s") == 0)
				CarregarJogo(jogador, monstro);
			else{
				//Inicializar jogador
				InicializarJogador(jogador);

				//Inicializar monstro
				InicializarMonstro(monstro);
			}
			
		}
		clear();
		// Dar as boas vindas ao jogador
		DarBoasVindas(jogador, monstro);
		sleep(3);
		clear();
	}else{

		wait(&childPid);

		// Menu iniciar
		//Menu(jogador, monstro, mapa, objetos, nObjetos, nCelulasMapa, argc, argv);

		//sem_post(semMovimentarMonstro);
		//sem_post(semMovimentarJogador);
		
		// Processo que trata de gerir o jogo
		pid=fork();
		if(pid == 0){
			while (*fimDeJogo == 0){
				MostrarSituacaoAtual(jogador, monstro, objetos);
				// Se for super utilizador mostra onde o monstro se encontra
				if (*su == 1)
					MostrarSituacaoAtualSU(jogador, monstro, mapa);

				if(*combate == 1){
					pid=fork();
					// Processo que trata do combate entre o jogador e o monstro
					if(pid == 0){
						Combater(jogador, monstro, objetos);
						sleep(1);
						VerificarSeFoge(combate); //Verifica se foge
						//sem_post(semMovimentarMonstro);
						//sem_post(semMovimentarJogador);
					}
					wait(&childPid);
					clear();
				}else{
					// Diz ao semáforos que já podem correr os processos
					sem_post(semMovimentarMonstro);
					sem_post(semMovimentarJogador);

					// Processo que trata da movimentação do monstro
					pid=fork();
					if(pid == 0){
						int t = (rand() % 10);
						sleep(t);
						sem_wait(semMovimentarMonstro);
						MovimentarMonstro(jogador, monstro, mapa);//Movimentar monstro
						//Post sobre o semáforo que controla a obtenção de novos números
						//sem_post(semMovimentarMonstro);
					}

					// Diz ao semáforos que já podem correr os processos
					//sem_post(semMovimentarJogador);

					// Processo que trata da movimentação do jogador
					pid=fork();
					if(pid == 0){
						sem_wait(semMovimentarJogador);
						//ListarJogador(jogador);

						//Mostrar sala atual
						MostrarJogadorSalaAtual(jogador, mapa);

						// Verifica se encontrou algum objeto
						VerificarObjeto(jogador, objetos, mapa);

						// Verifica se encontrou o tesouro
						VerificarTesouro(jogador, mapa);

						//Movimenta jogador
						MovimentarJogador(jogador, monstro, mapa, fimDeJogo);

					}
					wait(&childPid);

					clear();

					//Verificar se o jogador e o monstro se encontram
					VerificarCombate(jogador, monstro, objetos, combate);
					
				}

				VerificarfimDeJogo(jogador, monstro, fimDeJogo);
				//printw("Fim de jogo: %d\n\n", *fimDeJogo);
			}
			sleep(5);
		}else{
			wait(&childPid);
			//printw("\nPAI!!!\n");

			//Os semáforos são destruidos
			sem_destroy(semMovimentarJogador);
			sem_destroy(semMovimentarMonstro);

			//As zonas de memória partilhadas são destruídas
			munmap(jogador, sizeof(struct Jogador));
			munmap(monstro, sizeof(struct Monstro));
			munmap(mapa, sizeof(struct Celula));
			munmap(objetos, sizeof(struct Objeto));
			munmap(su, sizeof(int));
			munmap(combate, sizeof(int));
			munmap(fimDeJogo, sizeof(int));
			munmap(semMovimentarJogador, sizeof(sem_t));
			munmap(semMovimentarMonstro, sizeof(sem_t));

			printw("Obrigado por jogares!\n\n");
			refresh();
			sleep(5);
			clear();
		}
	}
	endwin();
	
	//wait(&childPid);
	/*while (*fimDeJogo == 0){
		ListarJogador(jogador);

		//Mostrar sala atual
		MostrarJogadorSalaAtual(jogador, mapa);

		// Se for super utilizador mostra onde o monstro se encontra
		if (su == 1)
			MostrarSituacaoAtual(jogador, monstro, mapa);

		if (combate == 0){
			//Movimenta jogador
			MovimentarJogador(jogador, monstro, mapa, fimDeJogo);
				
			// Verifica se encontrou algum objeto
			VerificarObjeto(jogador, objetos, mapa);

			// Verifica se encontrou o tesouro
			VerificarTesouro(jogador, mapa);

		}else
			VerificarSeFoge(combate); //Verifica se foge

		//Verificar se o jogador e o monstro se encontram
		VerificarCombate(jogador, monstro, objetos, combate);
			
		if(*combate == 0)
			MovimentarMonstro(jogador, monstro, mapa);//Movimentar monstro

		printw("Fim do Jogo: %d\n", *fimDeJogo);
		VerificarfimDeJogo(jogador, monstro, fimDeJogo);
	}*/

	
	return 0;
}

WINDOW *CriarJanelas(int altura, int comprimento, int yinicial, int xinicial){
    WINDOW *local_win;

	local_win = newwin(altura, comprimento, yinicial, xinicial);
	box(local_win, 0 , 0); // 0, 0 dão os caracteres default para as linhas verticais e horizontais
	wrefresh(local_win); // Mostra a caixa

	return local_win;
}

void MostrarNaJanela(WINDOW *janela, char frases [], int quantidadeFrases, int cor){
	// Calcula o x e o y inicial da janela
	int y, x;
	getbegyx(janela, y, x);

	// Começa a aplicar a cor selecionada
	attron(COLOR_PAIR(cor));

	// Percorre o vetor com as frases e adiciona à janela
	for (int i = 0; i < quantidadeFrases; i++)
		mvwprintw(janela, y+i+1, x+1, "%s", frases[i]);

	// Atualiza o ecrÂ
	refresh();

	// Para de aplicar a cor selecionada
	attroff(COLOR_PAIR(cor));
}

void IniciarVisualizacao(WINDOW *janelas[]){
	/*int linhas = 10, colunas = 40, y = 2, x = 4, i;

	initscr();
	cbreak();
	noecho();

	// Creria janelas
	janelas[0] = newwin(100, 100, 0, 0);
	janelas[1] = newwin(60, 40, 2, 2);
	janelas[2] = newwin(30, 20, 2, 44);
	janelas[4] = newwin(30, 20, 34, 44);

	// Adiciona caixas as janelas
	for(i = 0; i < 4; ++i)
		box(janelas[i], 0, 0);*/
	
	/*// Iniciar NCurses
	initscr();

	int alturaPrincipal, comprimentoPrincipal = 100;
	int yPrincipal, xPrincipal = 0;

	janelaPrincipal = newwin(alturaPrincipal, comprimentoPrincipal, yPrincipal, xPrincipal);
	
	refresh();

	box(janelaPrincipal, 0, 0);

	mvwprintw(janelaPrincipal, 1, 1, "Hello World!");

	wrefresh(janelaPrincipal);*/

	// Fechar NCurses
	//endwin();

}

/*void Menu(struct Jogador jogador, struct Monstro monstro, struct Celula mapa [], struct Objeto objetos [], int nObjetos, int nCelulasMapa, int argc, char* argv[]){
	char decisao[3];
	int su;

	//Verificar super user
	su = VerificarModoDeJogo(jogador, argc, argv);

	// Só mostra se for super user
	if (su == 1){
		//Inicializar jogador
		InicializarJogador(&jogador);

		//Inicializar monstro
		InicializarMonstro(&monstro);

		//Listar Objetos
		ListarObjetos(objetos, nObjetos);

		//Listar mapa
		ListarMapa(mapa, objetos, nCelulasMapa);
	}else{
		printw("Seja bem vindo!\nGostaria de carregar um jogo guardado? (sim ou não)\n");
		scanf("%s", decisao);
		if (strcmp(decisao, "sim") == 0)
			CarregarJogo(&jogador, &monstro);
		else{
			//Inicializar jogador
			InicializarJogador(&jogador);

			//Inicializar monstro
			InicializarMonstro(&monstro);
		}
		
	}
	DarBoasVindas(jogador, monstro);
	ListarJogador(jogador);
}*/

/**
 * Carrega o jogo guardado anteriormente se o utilizador desejar.
 * @param jogador -> ponteiro para a variável partilhada jogador
 * @param monstro -> ponteiro para a variável partilhada monstro
*/
void CarregarJogo(struct Jogador *jogador, struct Monstro *monstro){
	FILE *f;
	char l[MAXLIN];
	int linha = 0;
	char *palavra;
	int info = 0;
	char delim [] = " ";

	f = fopen("jaSave.txt", "r");
	while(fgets(l, MAXLIN, f) != NULL){
		//fputs(l, stdout);
		if (linha == 0){
			palavra = strtok(l, delim);
			info = 0;
			while(palavra != NULL){
				//printw("'%d'\n", atoi(palavra));
				switch (info){
					case 0:
						strcpy(jogador->nome, palavra);
						break;
					case 1:
						jogador->energia = atoi(palavra);
						break;
					case 2:
						jogador->dano = atoi(palavra);
						break;
					case 3:
						jogador->celula = atoi(palavra);
						break;
					case 4:
						jogador->objeto = atoi(palavra);
						break;
					case 5:
						jogador->tesouro = atoi(palavra);
						break;
					default:
						break;
				}
				palavra = strtok(NULL, delim);
				info++;
			}
			linha++;
		}else{
			palavra = strtok(l, delim);
			info = 0;
			while(palavra != NULL){
				//printw("'%d'\n", atoi(palavra));
				switch (info){
					case 0:
						strcpy(monstro->nome, palavra);
						break;
					case 1:
						monstro->energia = atoi(palavra);
						break;
					case 2:
						monstro->dano = atoi(palavra);
						break;
					case 3:
						monstro->celula = atoi(palavra);
						break;
					/*case 4:
						monstro->andar = atoi(palavra);
						break;*/
					default:
						break;
				}
				palavra = strtok(NULL, delim);
				info++;
			}
		}
	}
	fclose(f);
	printw("Jogo Carregado!\n\n");
	refresh();
}

/**
 * Criara o jogador com os dados iniciais
 * @param jogador -> ponteiro para a variável partilhada jogador
*/
void InicializarJogador(struct Jogador *jogador){
	// Pede o nome do jogador ao utilizador
	printw("Qual o seu nome aventureiro?\n");
	refresh();
	scanf("%s", jogador->nome);

	//Inicia com os dados iniciais
	jogador->energia = ENERGIA_INICIAL_JOGADOR;
	jogador->dano = DANO_INICIAL_JOGADOR;
	jogador->celula = CELULA_INICIAL_JOGADOR;
	//jogador->andar = ANDAR_INICIAL_JOGADOR;
	jogador->objeto = NENHUM;
	jogador->tesouro = NENHUM;
}

/**
 * Criara o monstro com os dados iniciais
 * @param monstro -> ponteiro para a variável partilhada monstro
*/
void InicializarMonstro(struct Monstro *pMonstro){
	// Pede o nome do monstro ao utilizador
	printw("Qual o nome do monstro?\n");
	refresh();
	scanf("%s", pMonstro->nome);

	//Inicia com os dados iniciais
	pMonstro->energia = ENERGIA_INICIAL_MONSTRO;
	pMonstro->dano = DANO_INICIAL_MONSTRO;
	pMonstro->celula = CELULA_INICIAL_MONSTRO;
	//pMonstro->andar = ANDAR_INICIAL_MONSTRO;
}

/**
 * Mensagem de boas vindas para o jogador
 * @param jogador -> ponteiro para a variável partilhada jogador
 * @param monstro -> ponteiro para a variável partilhada monstro
*/
void DarBoasVindas(struct Jogador *jogador, struct Monstro *monstro){
	printw("\nBoa sorte %s para fugir do %s! Vai precisar...\n\n", jogador->nome, monstro->nome);
	refresh();
}

/**
 * Lista as informações do jogador
 * @param jogador -> ponteiro para a variável partilhada jogador
*/
void ListarJogador(struct Jogador *jogador){
	printw("*** Jogador ***\n");
	printw("nome: %s\n", jogador->nome);
	printw("energia: %d\n", jogador->energia);
	//printw("celula: %d\n", jogador->celula);
	//printw("andar: %d\n", jogador->andar);
	printw("objeto: %d\n", jogador->objeto);
	printw("tesouro: %d\n\n", jogador->tesouro);
}

/**
 * Trás as informações dos objetos que estão guardados no ficheiro jaObjetos.txt
 * @param objetos -> Vetor onde guarda os objetos
*/
int CriarObjetos(struct Objeto objetos[]){
	FILE *f;
	char l[MAXLIN];
	int linha = 0;
	int nObjeto = 0;

	// Abre o ficheiro dos objetos
	f = fopen("jaObjetos.txt", "r");

	// Enquanto houver linhas no ficheiro
	while(fgets(l, MAXLIN, f) != NULL){
		//fputs(l, stdout);

		// Enquanto não houver quebra de linha pertence ao mesmo objeto
		if (strcmp(l, "\n") == 0){
			nObjeto++;
			linha = 0;
		}else{
			// Na primeira linha encontra o nome do objeto
			if (linha == 0){
				strcat(objetos[nObjeto].nome, l);
				linha++;
			}else
				objetos[nObjeto].eficiencia = atoi(l); // Na segunda linha encontra a eficiência do objeto
		}
	}

	// Fecha o ficheiro
	fclose(f);

	// Retorna o número total de objetos
	return nObjeto+1;
}

/**
 * Lista as informações dos objetos
 * @param objetos -> Vetor onde guarda os objetos
 * @param nObjetos -> Quantidade de objetos
*/
void ListarObjetos(struct Objeto objetos[], int nObjetos){
	for(int i = 0; i < nObjetos; i++){
		printw("***Objeto %d***\n", i);
		printw("Objeto: %s\n", objetos[i].nome);
		printw("Eficiência: %d\n", objetos[i].eficiencia);
		refresh();
	}
}

/**
 * Vai ao ficheiro jaSalas.txt e trás toda a informação sobre o mapa do jogo
 * @param mapa -> Vetor onde será guardado o mapa
*/
int InicializarMapa(struct Celula mapa[]){
	FILE *f;
	char l[MAXLIN];
	//char descricao [] = "";
	int linha = 0;
	int nSala = 0;
	char delim [] = " ";
	char *palavra;
	int info = 0;

	// Abre o ficheiro
	f = fopen("jaSalas.txt", "r");

	// Enquanto houver linhas no ficheiro
	while(fgets(l, MAXLIN, f) != NULL){
		//fputs(l, stdout);

		// Verifica se é uma nova sala
		if (strcmp(l, "\n") == 0){
			nSala++;
			linha = 0;
			//strcpy(mapa[nSala].descricao, "");
		}else{
			if (linha == 0){
				// Separa por um espaço a palavra da frase
				palavra = strtok(l, delim);
				info = 0;

				//enquanto houver palavras
				while(palavra != NULL){
					//printw("'%d'\n", atoi(palavra));

					// Verifica qual parte se refere essa palavra
					switch (info){
						case 0:
							mapa[nSala].norte = atoi(palavra);
							break;
						case 1:
							mapa[nSala].sul = atoi(palavra);
							break;
						case 2:
							mapa[nSala].este = atoi(palavra);
							break;
						case 3:
							mapa[nSala].oeste = atoi(palavra);
							break;
						case 4:
							mapa[nSala].cima = atoi(palavra);
							break;
						case 5:
							mapa[nSala].baixo = atoi(palavra);
							break;
						case 6:
							mapa[nSala].andar = atoi(palavra);
							break;
						case 7:
							mapa[nSala].objeto = atoi(palavra);
							break;
						case 8:
							mapa[nSala].tesouro = atoi(palavra);
							break;
						default:
							break;
					}
					// Continua a separar por espaços para obter a próxima palavra
					palavra = strtok(NULL, delim);
					info++;
				}
				linha++;
			}else
				strcat(mapa[nSala].descricao, l); // Guarda a descrição da sala
		}
	}

	// Fecha o ficheiro
	fclose(f);

	// Retorna a quantidade de salas
	return nSala+1;
}

/**
 * Mostra a informação do mapa e dos objetos em cada sala
 * @param mapa -> Vetor onde está guardado o mapa 
 * @param objetos -> Vetor onde está guardado os objetos
 * @param nCelulasMapa -> a quantidade de salas no mapa
*/
void ListarMapa(struct Celula mapa[], struct Objeto objetos[], int nCelulasMapa){
	for(int i = 0; i < nCelulasMapa; i++){
		printw("***Sala %d***\n", i);
		printw("norte: %d\n", mapa[i].norte);
		printw("sul: %d\n", mapa[i].sul);
		printw("este: %d\n", mapa[i].este);
		printw("oeste: %d\n", mapa[i].oeste);
		printw("Cima: %d\n", mapa[i].cima);
		printw("Baixo: %d\n", mapa[i].baixo);
		printw("Andar: %d\n", mapa[i].andar);

		// Verifica se existe um objeto no mapa
		if(mapa[i].objeto != NENHUM){
			printw("Nome Objeto: %s\n", objetos[mapa[i].objeto].nome);
			printw("Força Objeto: %d\n", objetos[mapa[i].objeto].eficiencia);
		}

		printw("Tesouro: %d\n", mapa[i].tesouro);
		printw("descricao: %s\n", mapa[i].descricao);
		refresh();
		sleep(1);
		clear();
	}
}

/**
 * Mostra a descrição da sala onde o jogador se encontra
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param mapa -> Vetor onde está guardado o mapa
*/
void MostrarJogadorSalaAtual(struct Jogador *jogador, struct Celula mapa[]){
	printw("%s\n", mapa[jogador->celula].descricao);
}

/**
 * Mostra como está o jogador e o monstro no momento atual
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro
 * 
*/
void MostrarSituacaoAtual(struct Jogador *jogador, struct Monstro *monstro, struct Objeto objeto[]){
	attron(COLOR_PAIR(COR_JOGADOR));
	
	/* Informações do jogador */
	printw("\n\n*** Jogador ***\n");
	printw("%s estás com %d de vida e tens %d de dano.\n", jogador->nome, jogador->energia, jogador->dano);
	
	// Se o jogador tiver um objeto
	if(jogador->objeto != -1){
		printw("Tens em tua posse um(a) %s\nQue tem %d de eficiência extra.\n", objeto[jogador->objeto].nome, objeto[jogador->objeto].eficiencia);
		printw("Assim ficas com um total de %d de dano.\n", jogador->dano + objeto[jogador->objeto].eficiencia);
	}

	attroff(COLOR_PAIR(COR_JOGADOR));

	attron(COLOR_PAIR(COR_MONSTRO));
	
	/* Informações do monstro */
	printw("\n*** Monstro ***\n");
	printw("O %s está com %d de vida e tem %d de dano.\n\n\n", monstro->nome, monstro->energia, monstro->dano);
	
	attroff(COLOR_PAIR(COR_MONSTRO));

	refresh();
}

/**
 * Mostra onde o jogador e o monstro se encontra para fins de teste se for super user
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro
 * @param mapa -> Vetor que guarda o mapa
*/
void MostrarSituacaoAtualSU(struct Jogador *jogador, struct Monstro *monstro, struct Celula mapa[]){
	attron(COLOR_PAIR(COR_ADMIN));
	
	printw("*** Admin ***\n");
	printw("Estás na sala %d que fica no andar %d.\n", jogador->celula, mapa[jogador->celula].andar);
	printw("O monstro está na sala %d que fica no andar %d.\n\n", monstro->celula, mapa[monstro->celula].andar);

	refresh();

	attroff(COLOR_PAIR(COR_ADMIN));
}

/**
 * Movimenta o jogador consoante o que o utilizador quer
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro para ser guardado no save se for para sair
 * @param mapa -> Vetor que guarda o mapa
 * @param fimDeJogo -> variável de saída de jogo
*/
void MovimentarJogador(struct Jogador *jogador, struct Monstro *monstro, struct Celula mapa[], int *fimDeJogo){
	char comando[MOVIMENTO];
	int movimentou = 0;

	// Pergunta ao utilizador o que ele quer fazer até o comando ser aceite
	do{
		// Pergunta ao utilizador por um comando
		printw("Digite um comando.\n");
		printw("Para se movimentar digite: n -> norte, s -> sul, e -> este, o -> oeste, c -> cima, ou b -> baixo\n");
		printw("Para sair do jogo digite sair.\n");
		refresh();

		scanf("%s", comando);

		// Verifica o comando inserido pelo utilizador e se ele é válido
		if (strcmp(comando, "n") == 0 && mapa[jogador->celula].norte != NENHUM){
			jogador->celula = mapa[jogador->celula].norte;
			movimentou = 1;
		}else if ( strcmp(comando, "s") == 0 && mapa[jogador->celula].sul != NENHUM){
			jogador->celula = mapa[jogador->celula].sul;
			movimentou = 1;
		}else if (strcmp(comando, "e") == 0 && mapa[jogador->celula].este != NENHUM){
			jogador->celula = mapa[jogador->celula].este;
			movimentou = 1;
		}else if ( strcmp(comando, "o") == 0 && mapa[jogador->celula].oeste != NENHUM){
			jogador->celula = mapa[jogador->celula].oeste;
			movimentou = 1;
		}else if ( strcmp(comando, "c") == 0 && mapa[jogador->celula].cima != NENHUM){
			jogador->celula = mapa[jogador->celula].cima;
			//jogador->andar++;
			movimentou = 1;
		}else if ( strcmp(comando, "b") == 0 && mapa[jogador->celula].baixo != NENHUM){
			jogador->celula = mapa[jogador->celula].baixo;
			//jogador->andar--;
			movimentou = 1;
		}else if (strcmp(comando, "sair") == 0){
			MenuSair(jogador, monstro, fimDeJogo);
			movimentou = 1;
		}else
			printw("Não podes ir nessa direção!\n");
	} while (movimentou == 0);
}

/**
 * Verifica se o jogador encontrou um objeto no mapa
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param objeto -> Vetor que guarda os objetos
 * @param mapa -> Vetor que guarda o mapa
*/
void VerificarObjeto(struct Jogador *jogador, struct Objeto objeto [], struct Celula mapa []){
	if (mapa[jogador->celula].objeto != NENHUM){
		attron(COLOR_PAIR(COR_BOM));
		char decisao [1];
		printw("Encontraste um(a) %sQue tem %d de eficiência.\n", objeto[mapa[jogador->celula].objeto].nome, objeto[mapa[jogador->celula].objeto].eficiencia);
		if (jogador->objeto == NENHUM)
			printw("Não tens nenhum objeto.\n");
		else
			printw("Atualmente tens um(a) %sQue tem %d de eficiência.\n", objeto[jogador->objeto].nome, objeto[jogador->objeto].eficiencia);

		printw("Gostarias de trocar? (s -> sim, n -> não).\n");
		refresh();
		scanf("%s", decisao);
		if (strcmp(decisao, "s") == 0){
			jogador->objeto = mapa[jogador->celula].objeto;
			mapa[jogador->celula].objeto = NENHUM;
		}
		attroff(COLOR_PAIR(COR_BOM));
	}
}

/**
 * Verifica se o jogador encontrou o tesouro
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param mapa -> Vetor que guarda o mapa
*/
void VerificarTesouro(struct Jogador *jogador, struct Celula mapa []){
	// Verifica se o tesouro está naquela sala
	if (mapa[jogador->celula].tesouro != NENHUM){
		attron(COLOR_PAIR(COR_BOM));
		
		printw("Encontraste o tesouro!.\nCorre para a saída!\n\n");
		refresh();

		jogador->tesouro = 1; // Diz que o jogador tem o tesouro
		mapa[jogador->celula].tesouro = NENHUM; // Remove o tesouro do mapa
		
		attroff(COLOR_PAIR(COR_BOM));
	}
}

/**
 * Mostra o menu de sair onde verifica se o jogador quer guardar o jogo atual
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro
 * @param fimDeJogo -> variável de saída de jogos
*/
void MenuSair(struct Jogador *jogador, struct Monstro *monstro, int *fimDeJogo){
	char decisao [3];

	// Pergunta ao utilizador se quer guardar o jogo atual
	printw("Gostaria de guardar o jogo atual? (sim ou nao)\n");
	refresh();
	scanf("%s", decisao);
	//printw("Obrigado por jogar!\nAté a próxima!\n");

	// Se for sim guarda o jogo atual
	if (strcmp(decisao, "sim") == 0){
		GuardarJogoAtual(jogador, monstro);
		printw("Jogo salvo!\n");
		refresh();
	}
	*fimDeJogo = 1;
	//exit(0);
}

/**
 * Guarda o jogo atual
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro
*/
void GuardarJogoAtual(struct Jogador *jogador, struct Monstro *monstro){
	FILE *f;

	// Abre o ficheiro onde vai ser guardado o jogo
	f = fopen ("jaSave.txt","w");

	// Guarda os dados do jogador no momento atual
	fprintf (f, "%s %d %d %d %d %d\n", jogador->nome, jogador->energia, jogador->dano, jogador->celula, jogador->objeto, jogador->tesouro);
	
	// Guarda os dados do monstro no momento atual
	fprintf (f, "%s %d %d %d", monstro->nome, monstro->energia, monstro->dano, monstro->celula);

	// Fecha o ficheiro
	fclose (f);
}

/**
 * Movimenta o monstro, se ele tiver num andar diferente do jogador percorre o algorítmo
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro
 * @param mapa -> Vetor onde guarda o mapa
*/
void MovimentarMonstro(struct Jogador *jogador, struct Monstro *monstro, struct Celula mapa[]){
	// Verifica se o jogador está num andar diferente
	if (mapa[jogador->celula].andar != mapa[monstro->celula].andar){
		int movimentar = 0;
		//printw("Cima: %d, Baixo: %d\n", mapa[monstro->celula].cima, mapa[monstro->celula].baixo);

		// Verifica se o monstro está perto das escadas
		if (mapa[monstro->celula].cima == NENHUM && mapa[monstro->celula].baixo == NENHUM){
			// Percorre o algorímo para o monstro seguir em direção às escadas
			//printw("Não encontrou as escadas.\n");
			if (mapa[monstro->celula].norte != NENHUM)
				movimentar = mapa[monstro->celula].norte;
			if (mapa[monstro->celula].sul != NENHUM && mapa[monstro->celula].sul > movimentar)
				movimentar = mapa[monstro->celula].sul;
			if (mapa[monstro->celula].este != NENHUM && mapa[monstro->celula].este > movimentar)
				movimentar = mapa[monstro->celula].este;
			if (mapa[monstro->celula].oeste != NENHUM && mapa[monstro->celula].oeste > movimentar)
				movimentar = mapa[monstro->celula].oeste;
		}else if (mapa[jogador->celula].andar < mapa[monstro->celula].andar)
			movimentar = mapa[monstro->celula].baixo; // se o monstro estiver no andar de baixo, desce
		else
			movimentar = mapa[monstro->celula].cima; // senão sobe
		//printw("Movimentar: %d\n", movimentar);
		monstro->celula = movimentar;
	}else{ // se estiverem no mesmo andar o movimento do monstro irá ser aleatório
		int direcoes = 0;
		int norte = NENHUM;
		int sul = NENHUM;
		int este = NENHUM;
		int oeste = NENHUM;

		// Conta para quais direções o monstro pode ir
		if (mapa[monstro->celula].norte != NENHUM){
			norte = mapa[monstro->celula].norte;
			direcoes++;
		}
		if (mapa[monstro->celula].sul != NENHUM){
			sul = mapa[monstro->celula].sul;
			direcoes++;
		}
		if (mapa[monstro->celula].este != NENHUM){
			este = mapa[monstro->celula].este;
			direcoes++;
		}
		if (mapa[monstro->celula].oeste != NENHUM){
			oeste = mapa[monstro->celula].oeste;
			direcoes++;
		}

		// Cria um vetor com as possiveis movimentações do monstro
		int movimentar [direcoes];
		int i = 0;

		//printw("Norte: %d\nSul: %d\nEste: %d\nOeste: %d\n", norte, sul, este, oeste);

		// Vê para quais direções o monstro pode ir
		if (norte != NENHUM){
			movimentar [i] = norte;
			i++;
		}
		if (sul != NENHUM){
			movimentar [i] = sul;
			i++;
		}
		if (este != NENHUM){
			movimentar [i] = este;
			i++;
		}
		if (oeste != NENHUM)
			movimentar [i] = oeste;

		// Escolha uma direção aleatória
		int movimento = rand() % direcoes;

		// Movimenta o monstro
		monstro->celula = movimentar[movimento];

		//printw("Movimentar: %d\n\n", monstro->celula);
	}
	

	//char* movimentos[MOVIMENTO] = {"norte", "este", "sul", "oeste", "cima", "baixo"};
	/*int nMovimentos = 6;
	int movimento;
	int movimentou = 0;
	do{
		movimento = rand() % nMovimentos;
		if (strcmp(movimento, "norte") == 0 && mapa[monstro->celula].norte != NENHUM){
			monstro->celula = mapa[monstro->celula].norte;
			movimentou = 1;
		}else if (strcmp(movimento, "este") == 0 && mapa[monstro->celula].este != NENHUM){
			monstro->celula = mapa[monstro->celula].este;
			movimentou = 1;
		}else if (strcmp(movimento, "sul") == 0 && mapa[monstro->celula].sul != NENHUM){
			monstro->celula = mapa[monstro->celula].sul;
			movimentou = 1;
		}else if (strcmp(movimento, "oeste") == 0 && mapa[monstro->celula].oeste != NENHUM){
			monstro->celula = mapa[monstro->celula].oeste;
			movimentou = 1;
		}else if (strcmp(movimento, "cima") == 0 && mapa[monstro->celula].cima != NENHUM){
			monstro->celula = mapa[monstro->celula].cima;
			monstro->andar++;
			movimentou = 1;
		}else if (strcmp(movimento, "baixo") == 0 && mapa[monstro->celula].baixo != NENHUM){
			monstro->celula = mapa[monstro->celula].baixo;
			monstro->andar--;
			movimentou = 1;
		}else{
			//movimentos.erase(movimento);
			nMovimentos--;
		}
	} while (movimentou == 0);*/
}

/**
 * Verifica se o jogador e o monstro entram em combate
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro
 * @param mapa -> Vetor onde guarda o mapa
 * @param combate -> variável para verificar se estão em combate
*/
void VerificarCombate(struct Jogador *jogador, struct Monstro *monstro, struct Objeto objeto[], int *combate){
	attron(COLOR_PAIR(COR_PERIGO));
	// Verifica se estam na mesma sala
	if(jogador->celula == monstro->celula){
		printw("%s encontraste-te com o %s!\n", jogador->nome, monstro->nome);
		
		if (jogador->objeto != NENHUM)
			printw("Tens um(a) %s", objeto[jogador->objeto].nome);

		printw("Vais entrar em combate.\nBoa Sorte!\n\n");

		*combate = 1;

		refresh();
	}
	attroff(COLOR_PAIR(COR_PERIGO));
}

/**
 * Retira vida do jogador e do monstro consoante a sua força atual
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro
 * @param objetos -> Vetor que guarda os objetos
*/
void Combater(struct Jogador *jogador, struct Monstro *monstro, struct Objeto objeto[]){
	// Retira a vida do jogador, consoante o dano do monstro
	jogador->energia -= monstro->dano;

	// Verifica se o jogador tem algum objeto
	if (jogador->objeto != NENHUM)
		monstro->energia -= jogador->dano + objeto[jogador->objeto].eficiencia; // Se tiver é retirado o dano do jogador e do objeto da vida do monstro
	else
		monstro->energia -= jogador->dano; // Se não tiver objeto tira apenas o dano do jogador
}

/**
 * Verifica se o jogador foge da luta
 * @param combate -> variável que indica se o jogador e o monstro estão a lutar
*/
void VerificarSeFoge(int *combate){
	int chances = 5;
	char resposta [1];

	attron(COLOR_PAIR(COR_AVISO));
	// Pergunta ao utilizador se quer tentar fugir do combate
	printw("\n\nQueres tentar fugir da luta?\n s -> sim ou n -> nao?\n");
	refresh();
	scanf("%s", resposta);

	// Se a resposta for sim
	if (strcmp(resposta, "s") == 0){
		int tentativa = rand() % 10;

		// Tenta escapar segundo as chances
		if (chances <= tentativa){
			printw("Escapaste do monstro!\n\n");
			*combate = 0;
		}else
			printw("Não conseguiste fugir do monstro! Melhor sorte para a próxima!\n\n"); // Se não conseguir fugir
	}else
		printw("Boa sorte!\n\n"); // Se não quiser fugir continua em combate
	
	refresh();

	attroff(COLOR_PAIR(COR_AVISO));
}

/**
 * Verifica se o utilizador entrou como super user ou não
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param argc -> quantidade de argumentos
 * @param argv -> vetor com os argumentos
 * @param su -> ponteira que diz se é um super user ou um user normal
*/
void VerificarModoDeJogo(struct Jogador *jogador, int argc, char* argv[], int *su){
	// Verifica se na chamada tem mais que 1 argumento
	if(argc == 5){
		// Se no 2º argumento for o código correto inicia como super user
		if (strcmp(argv[1], "1234") == 0){

			jogador->energia = atoi(argv[2]);
			jogador->dano = atoi(argv[3]);
			jogador->celula = atoi(argv[4]);
			
			printw("Não sei se descobriste o código ou és um dos desenvolvedores, mas divertete!\n");
			*su = 1;
		}else{ // Se errou co código, torna a vida do jogador mais difícil
			jogador->energia = 100;
			jogador->dano = 40;
			jogador->celula = 10;
			printw("Tentaste entrar no modo super utilizador sem permissão!\nAgora vou te tornar a vida difícil!\n");
		}
		refresh();
		// Pede o nome do jogador ao utilizador
		printw("Qual o seu nome aventureiro?\n");
		refresh();
		scanf("%s", jogador->nome);
		printw("%s vais ter %d de energia e com %d de dano.\nVais começar no quarto %d.\n", jogador->nome, jogador->energia, jogador->dano, jogador->celula);
	}
}

/**
 * Verifica se o jogador conseguiu chegar a um do finais
 * @param jogador -> Ponteiro onde está guardado a informação do jogador
 * @param monstro -> Ponteiro onde está guardado a informação do monstro
 * @param fimDeJogo -> variável de saída de jogos
*/
void VerificarfimDeJogo(struct Jogador *jogador, struct Monstro *monstro, int *fimDeJogo){
	attron(COLOR_PAIR(COR_BOM));
	
	// Verifica se obteve um dos seguintes finais
	if (jogador->celula == CELULA_INICIAL_JOGADOR && jogador->tesouro != NENHUM){ // Se o jogador conseguiu chegar à sída com o tesouro
		printw("Parabéns\nConseguiste roubar o tesouro e escapar com vida!\nMas na realidade tu é que és o monstro, por roubares o que não te pertence!\n\n");
		*fimDeJogo = 1;
		refresh();
	}/*else if(jogador->celula == CELULA_INICIAL_JOGADOR && jogador->tesouro == NENHUM){ // Se o jogador fugiu
		printw("Fugiste dos teus problemas!\nVolta quando tiveres mais coragem!\n\n");
		*fimDeJogo = 1;
		refresh();
	}*/else if(monstro->energia <= 0){ // Se o jogador consegui matar o monstro
		printw("Parabéns!\nMataste o monstro!\nO castelo é teu!\nPorém ativaste a maldição e agora tornaste no monstro!\n\n");
		*fimDeJogo = 1;
		refresh();
	}else if(jogador->energia <= 0){ // Se o jogador morreu
		printw("Esta aventura não é para fracos.\nTenta superar as tuas falhas e torna-te no aventureiro que sei que és!\n\n");
		*fimDeJogo = 1;
		refresh();
	}

	attroff(COLOR_PAIR(COR_BOM));
}