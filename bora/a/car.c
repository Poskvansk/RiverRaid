/*
Universidade de Brasilia
Instituto de Ciencias Exatas
Departamento de Ciencia da Computacao

Algoritmos e Programação de Computadores – 1/2018

Aluno(a): Luis Filipe Siqueira Ribeiro
Matricula: 180053906
Turma: A
Versão do compilador: gcc (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609

Descricao: Jogo no qual o jogador (representado por um '+') deve desviar dos inimigos (representados por um 'X')*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
    #define CLEAR "cls"

#else
    #define CLEAR "clear"
    #include <termios.h>
#endif

#ifndef _WIN32
    int kbhit(){
        struct termios oldt, newt;
        int ch, oldf;
        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        if(ch != EOF){
        ungetc(ch,stdin);
        return 1;
    }
        return 0;
}
    int getch(void) {
        int ch;
        struct termios oldt;
        struct termios newt;
        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

#else
    #include <conio.h>
#endif

#define RAND() ((rand()%100 + 1))

/*Variaveis Globais definidas pelas especificacoes*/
char tabuleiro[10][135];
int altura, largura;
int probX, probF, probT, probF,probO;
int velocidade, ranked;

/*Variaveis Globadas definidas por mim*/
int posicaoJogador = 0;
int combustivel = 400;
int pontos = 0;
int enemySpawn, Ohp, muniT, contO=0;

typedef struct {
    char nick[11];
    int score;    
}Player;

typedef struct {
    int ammoT;
}enemyT;

typedef struct {
    int hp; 
    char img;
}enemyO;

enemyO O[50];

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
int gameOver() {

    if ( (tabuleiro[(altura/2) - posicaoJogador][0] == '+') && (tabuleiro[(altura/2) - posicaoJogador][1]=='X') ) {
        /*Retorna 1 para caso de colisao*/
        return 1;
    }
    if ( (tabuleiro[(altura/2) - posicaoJogador][0] == '+') &&  (tabuleiro[(altura/2) - posicaoJogador][1]=='T') ) {
        /*Caso de colisao*/
        return 1;
    }
    if ( (tabuleiro[(altura/2) - posicaoJogador][0] == '+') &&  ( (tabuleiro[(altura/2) - posicaoJogador][2]=='<') || (tabuleiro[(altura/2) - posicaoJogador][1]=='<') ) ) {
        /*Caso de colisao*/
        return 1;
    }
    if (combustivel <=0) {
        /*Retorna 2 para caso de zero combustivel*/
        return 2;
    }
    else {
        /*Retorna 0 para indicar que jogo continua*/
        return 0;
    }
}

void destroiTodosX() {

    int i, j;

    for (i=0; i<altura; i++) {

        for (j=0; j<largura; j++) {
            if (tabuleiro[i][j] == 'X') {
                tabuleiro[i][j] = ' ';
                pontos+=40;
            }
        }
    }
}

void destroiAlvo() {

    int i, j;

    for (i=0; i<altura; i++) {

        for (j=largura; j>=0; j--) {

            if ( (tabuleiro[i][j] == '>') && (tabuleiro[i][j+1] == 'X') ){
                tabuleiro[i][j] = ' ';
                tabuleiro[i][j+1] = ' ';
                pontos += 50;
            }
            if ( (tabuleiro[i][j] == '>') && tabuleiro[i][j + 1] == 'F') {
                tabuleiro[i][j] = ' ';
                tabuleiro[i][j+1] = ' ';
            }

            if ( (tabuleiro[i][j] == '>') && tabuleiro[i][j + 1] == 'T') {
                tabuleiro[i][j] = ' ';
                tabuleiro[i][j+1] = ' ';
                pontos += 50;
            }

            if ( (tabuleiro[i][j] == '>') && (tabuleiro[i][j+1] == O[contO].img) ) {
                O[contO].hp--;

                if (O[contO].hp<=0) {
                    destroiTodosX();
                }
            }
        }
    }
}

void moveCima() {

    posicaoJogador += 1;
    if (  ( (altura/2) - posicaoJogador) <=0 ) {

        /*Limita posicao do jogador nas bordas do mapa*/
        posicaoJogador--;
    }
}

void moveBaixo() {

    posicaoJogador -= 1;
    if ( (altura/2) - (posicaoJogador-1) >= altura) {

        /*Limita posicao do jogador nas bordas do mapa*/
        posicaoJogador++;
    }
}

void combustivelParaEsquerda(int i, int j) {

    /*Move todos os F uma unidade para esquerda*/
    if(tabuleiro[i][j] == 'F') {
        tabuleiro[i][j - 1] = 'F';
        tabuleiro[i][j] = ' ';
    }
}

void inimigoParaEsquerda(int i, int j) {

    /*Move todos os X uma unidade para esquerda*/
    if (tabuleiro[i][j] == 'X') {
        tabuleiro[i][j - 1] = 'X';
        tabuleiro[i][j] = ' ';
    }
    else if (tabuleiro[i][j] ==  O[contO].img) {
        tabuleiro[i][j - 1] = O[contO].img;
        tabuleiro[i][j] = ' ';
    }
    else if (tabuleiro[i][j] == 'T') {

        int probTiro = rand()%100 +1;
        if (probTiro <= 2) {
            tabuleiro[i][j-2] = '<';
        }

        tabuleiro[i][j - 1] = 'T';
        tabuleiro[i][j] = ' ';
    }

    else if (tabuleiro[i][j] == '<') {

        tabuleiro[i][j - 2] = '<';
        tabuleiro[i][j] = ' ';
    }
}

void moveTiro(int i, int j) {

    if (tabuleiro[i][j] == '>') {

        /*Move tiro uma unidade para direita*/
        tabuleiro[i][j + 1] = '>';
        tabuleiro[i][j] = ' ';

        if (j+1 >= largura-1) {
            /*Desfaz o tiro caso ele ultrapasse a borda da matriz*/
            tabuleiro[i][j] = ' ';
            tabuleiro[i][j +1] = ' ';            
        }
    }
}

void jogoMapa() {

    int i, j;
    int objeto, objeto2, objeto3, posicao, posicaoO, posicaoT, spawn, spawnF;

    usleep(velocidade);
    system(CLEAR);

    /* CHECA MATRIZ */
    for (i = 0; i< altura; i++) {

        for (j=largura; j>=0; j--) {

            if (tabuleiro[i][j] == '>'  ) {

                 /*Tem tiro? Necessita mover o tiro*/
                moveTiro(i, j);
            }
        }
    }

    /*CHECA POSICAO INIMIGOS*/
    for (i=0; i<altura; i++) {

        for (j=0; j<largura; j++) {

            if ( (tabuleiro[i][j] == 'X') || (tabuleiro[i][j] == O[contO].img) || (tabuleiro[i][j] == 'T') || (tabuleiro[i][j] == '<') ) {
            
                /*Tem inimigo? Move pra esquerda*/
                destroiAlvo();
                inimigoParaEsquerda(i, j);
            }
            else if (tabuleiro[i][j] == 'F') {

                /*Tem combustivel? Move para esquerda*/
                destroiAlvo();
                combustivelParaEsquerda(i, j);
            }
        }
    }

    /*Salva a matriz (mapa) do jogo*/
    for (i=0; i<altura; i++) {

        for (j=0; j<largura; j++) {

            if (i==0 || i==altura-1) {
                tabuleiro[i][j] = '#';
            }
            else if (i==(altura/2) - posicaoJogador && j==0) {
                tabuleiro[i][j] = '+';
            }
            else if (tabuleiro[i][j] == 'X') {
                tabuleiro[i][j] = 'X';
            }
            else if (tabuleiro[i][j] == 'O') {
                tabuleiro[i][j] = 'O';
            }
            else if (tabuleiro[i][j] == 'T') {
                tabuleiro[i][j] = 'T';
            }
            else if (tabuleiro[i][j] == '<') {
                tabuleiro[i][j] = '<';
            }
            else if (tabuleiro[i][j] == 'F') {
                tabuleiro[i][j] = 'F';
            }
            else if (i == 2 && (j==largura-1) ) {

                objeto = rand()%100 +1;             /*Determina se objeto e X ou F*/
                posicao = rand()%(altura-1) +1;     /*Determina linha em que vai gerar o objeto (numero pseudo-aleatorio de entre 1 e altura do mapa)*/
                spawn = rand()%100 +1;              /*Checa se ira spawnar objeto ou nao (taxa padrao 60%)*/
                spawnF = rand()%100 +1;             /*Checa se ira spawnar F*/

                if (spawn <= enemySpawn ) {                        

                    objeto2 = rand()%100 +1;            /*Determina spawns de O*/
                    posicaoO = rand()%(altura-1) +1;
                    objeto3 = rand()%100 +1;            /*Determina spawns de T*/
                    posicaoT = rand()%(altura-1) +1;

                    if (objeto <= probX ){

                            /*Spawn X*/
                            tabuleiro[posicao][j] = 'X';
                    }

                    else if ( (objeto2 <= probO) && (contO<30) ) {
                        
                        /*Spawn O*/


                        O[contO].img = 'O';
                        O[contO].hp = Ohp;

                        tabuleiro[ posicaoO][j] = O[contO].img;
                    }

                    else if ( objeto3 <= probT ) {

                        /*Spawn T*/
                        tabuleiro[ posicaoT][j] = 'T';
                    }                        
                }

                if (spawnF <= probF) {   
                    /*Spawn F*/
                    tabuleiro[posicao][j] = 'F';
                }
               
            }    

            else if (tabuleiro[i][j] == '>') {
                tabuleiro[i][j] = '>';
            }

            else {
                tabuleiro[i][j] = ' ';
            }
        }
    }

    /*Printa a matriz (mapa) do jogo*/
    printf("COMBUSTIVEL:  %d             PONTOS:  %d \n", combustivel, pontos);
    for (i = 0; i<altura; i++) {
        for (j=0; j<largura; j++) {

            if (tabuleiro[i][j] == 'X') {
                /*Colore o X de vermelho, em negrito*/
                printf("\e[1;31mX\e[0m");                
            }
            else if (tabuleiro[i][j] == 'F') {
                /*Colore o F de verde, em negrito*/
                printf("\e[1;32mF\e[0m");
            }
            else if (tabuleiro[i][j] == '#') {
                /*Deixa o '#' em negrito, cor branca*/
                printf("\e[1;37m#\e[0m");
            }
            else if (tabuleiro[i][j] == '+') {
                /*Colore o '+' de amarelo, em negrito*/
                printf("\e[1;33m+\e[0m");
            }
            else {
                printf("%c", tabuleiro[i][j]);
            }            
        }
        printf("\n");
    }

    if ( (tabuleiro[(altura/2) - posicaoJogador][0] == '+') && (tabuleiro[(altura/2) - posicaoJogador][1] == 'F') ) {
        /*Adiciona 40 de combustivel caso o jogador se choque com combustivel*/
        combustivel +=40;
    }
} 

int jogoInicio() {

    char movimento;
    int fimJogo = 0;
    int i, j;

    do {
        /*Loop execucao jogo enquanto nao houver Game Over*/
        if (kbhit() ) {

            /*Le acao do jogador caso pressione alguma tecla*/
            movimento = getch();

            if (movimento == 'w' || movimento == 'W') {

                /*Move para cima, decresce combustivel, atualiza mapa, checa game over*/                
                moveCima();
                fimJogo = gameOver();
                combustivel-=2;
                jogoMapa(O);                
            }

            else if (movimento == 's' || movimento == 'S') {

                /*Move para baixo, decresce combustivel, atualiza mapa, checa game over*/
                moveBaixo();
                fimJogo = gameOver();
                combustivel-=2;
                jogoMapa(O);                
            }

            else if (movimento == 0x20) {

                /*Posiciona o tiro a frente do jogador, decresce combustivel, checa game over, checa colisao com objetos, atualiza o mapa*/
                i = (altura/2) - posicaoJogador;
                j = 1;
                tabuleiro[i][j] = '>';
                combustivel -=3;

                fimJogo = gameOver();
                jogoMapa(O);                
                destroiAlvo(O);
            }
        }   
        else {
            /*Continua a rodar o jogo caso nenhuma tecla pressionada*/            
            fimJogo = gameOver();
            combustivel--;
            jogoMapa(O);            
        }
    } while (fimJogo == 0);

    /*Retorna o que causou o Game Over*/
    if (fimJogo == 1) {
        /*Caso de colisao*/
        getch();
        return 1;        
    }
    else {
        /*Caso de falta de combustivel*/
        getch();
        return 2;        
    }
}

void configPadrao() {  

    /*Configuracoes iniciais do jogo*/
    FILE* fd;
    fd = fopen("configuracoes.txt", "w");
    fprintf(fd, " 10\n");       /*altura*/
    fprintf(fd, " 135\n");      /*largura*/
    fprintf(fd, " 75000\n");    /*velocidade (usleep)*/
    fprintf(fd, " 60\n");       /*Taxa de spawn inimigo (enemySpawn)*/
    fprintf(fd, " 50\n");       /*probabilidade X (probX)*/
    fprintf(fd, " 20\n");       /*probabilidade F (probF))*/
    fprintf(fd, " 5\n");        /*probabilidade O (probO)*/
    fprintf(fd, " 10\n");       /*HP inimigo O  (Ohp)*/
    fprintf(fd, " 20\n");       /*probabilidade T (probT)*/
    fprintf(fd, " 5\n");        /*municao T (muniT)*/
    fprintf(fd, " 0\n");        /*ranked ativado? sim = 1 , nao = !1 (ranked)*/
    fclose(fd);
}

void chngConfig() {
            
    /*Funcao que altera as configuracoes com os valores digitados pelo usuario*/        
    FILE* fd;
    fd = fopen("configuracoes.txt", "w");
    fprintf(fd, " %d\n", altura);
    fprintf(fd, " %d\n", largura);
    fprintf(fd, " %d\n", velocidade);
    fprintf(fd, " %d\n", enemySpawn);
    fprintf(fd, " %d\n", probX);
    fprintf(fd, " %d\n", probF);
    fprintf(fd, " %d\n", probO);
    fprintf(fd, " %d\n", Ohp);
    fprintf(fd, " %d\n", probT);
    fprintf(fd, " %d\n", muniT);
    fprintf(fd, " %d\n", ranked);
    fclose(fd);
}

void applyConfig() {

    /*Funcao que le o arquivo texto com as configuracoes e aplica no jogo*/    
    FILE* fd;
    fd = fopen("configuracoes.txt", "r");
    fscanf(fd, " %d", &altura);
    fscanf(fd, " %d", &largura);
    fscanf(fd, " %d", &velocidade);
    fscanf(fd, " %d", &enemySpawn);
    fscanf(fd, " %d", &probX);
    fscanf(fd, " %d", &probF);
    fscanf(fd, " %d", &probO);
    fscanf(fd, " %d", &Ohp);
    fscanf(fd, " %d", &probT);
    fscanf(fd, " %d", &muniT);
    fscanf(fd, " %d", &ranked);
    fclose(fd);
}

void configuracoes() {

    /*Menu de configuracoes do jogo, sera adicionado futuramente*/
    system(CLEAR);
    printf("Digite o valor para a altura (o valor padrao eh 10):  ");
    scanf("%d", &altura);
    printf("\n");

    printf("Digite o valor para a largura (o valor padrao eh 135): ");
    scanf("%d", &largura);
    printf("\n");

    printf("Digite o valor para a velocidade (o valor padrao eh 75000, quanto menor o valor, maior a velocidade): ");
    scanf("%d", &velocidade);
    printf("\n");

    printf("Digite a taxa de spawn de inimigos (o valor padrao eh 60): ");
    scanf("%d", &enemySpawn);
    printf("\n");

    printf("Digite o valor para a probabilidade de aparicao do inimigo 'X' (o valor padrao eh 50): ");
    scanf("%d", &probX);
    printf("\n");

    printf("Digite o valor para a probabilidade de aparicao do combustivel 'F' (o valor padrao eh 20): ");
    scanf("%d", &probF);
    printf("\n");

    printf("Digite o valor para a probabilidade de aparicao do inimigo 'O' (o valor padrao eh 5): ");
    scanf("%d", &probO);
    printf("\n");

    printf("Digite o valor da quantidade de pontos de vida do inimigo 'O' (o valor padrao eh 10): ");
    scanf("%d", &Ohp);
    printf("\n");

    printf("Digite o valor para a probabilidade de aparicao do inimigo 'T' (o valor padrao eh 20): ");
    scanf("%d", &probT);
    printf("\n");

    printf("Digite o valor para a quantidade de municao nos inimigos 'T' (o valor padrao eh 5): ");
    scanf("%d", &muniT);
    printf("\n");

    printf("Digite 1 para jogar rankeado, 2 para não: ");
    scanf("%d", &ranked);
    printf("\n");

    chngConfig();
    applyConfig();
}

void bubbleSort(Player jogador[]) {

    int i, j, aux;

    /*Ordenar o ranking dos jogadores por score*/

    for (i=0;i<11; i++) {
        for (j=0; j<10; j++){
            if(jogador[j].score < jogador[j+1].score) {
                aux = jogador[j+1].score;
                jogador[j+1].score = jogador[j].score;
                jogador[j].score = aux;
            }
        }
    }
}

void entraRanking(Player jogador[]) {

    /*Ordena o ranking e salva*/

    FILE* fd;
    fd = fopen("ranking.bin", "wb");

    bubbleSort(jogador);

    fwrite(jogador, sizeof(Player), 10, fd);

    fclose(fd);
}

void checkRanking (Player jogador[11]) {

    int flag=0;
    int nickLen;

    /*Checa se o jogador pode entrar no ranking, avaliando a pontuacao e o modo ranked*/

    if ( (pontos > jogador[10].score) && (ranked == 1) ) {
        printf("        Voce pode entrar no ranking!\n");

        printf("\n        Digite um nick entre 1 e 10 letras: \n");

        while (flag==0) {
                
            /*Verifica o nome valido do jogador (1 a 10 caracteres)*/

            scanf("%s", jogador[10].nick);
            nickLen = strlen(jogador[10].nick);

            if ( (nickLen <1) || (nickLen > 10) ) {
                printf("Tamanho Invalido!\n");
            }
            else {
                flag = 1;
            }
        }

        jogador[10].score = pontos;
    }

    entraRanking(jogador);
}

void ranking(Player jogador[]) {

    int i;

    system(CLEAR);
    FILE* fd;
    fd = fopen("ranking.bin", "rb");

    /*Mostra o ranking. Se nao houver ranking ainda, cria um*/

    if (fd == NULL) {

        /*Cria ranking zerado caso nao haja ranking anterior*/

        fd = fopen("ranking.bin", "wb");
        for (i=0; i<11; i++) {
            strcpy(jogador[i].nick, "\0");
            jogador[i].score = 0;
        }

        fwrite(jogador, sizeof(Player), 11 , fd);
    }

    /*Printa Ranking*/

    printf("    ESSA AREA EH DESTINADA A EXALTAR OS MELHORES JOGADORES DE RIVER RAID! CONTEMPLE: \n\n");

    fread(jogador, sizeof(Player), 11, fd);

    for (i=0; i<10; i++) {
        printf("                                        %d - %s    %d\n", i+1, jogador[i].nick, jogador[i].score);
    }

    fclose(fd);
    getch();
}

void instrucoes() {

    /*Explicacoes sobre o jogo*/
    system(CLEAR);
    printf("        INSTRUCOES    \n\n");
    printf("O Jogador e indicado pelo '+' no canto esquerdo da tela\n");
    printf("A quantidade de combustivel e indicada no canto superior esquerdo.\n");
    printf("Sua pontuacao e indicada tambem no canto superior esquerdo.\n");
    printf("\n");
    printf("Desvie dos 'X' que vem em sua direcao.\n");
    printf("Se um deles lhe tocar, voce sera destruido e perdera o jogo!\n\n");
    printf("O inimgo 'O' eh mais resistente do que os inimigos comuns, ele necessita de mais de um tiro pra ser destruido!\n");
    printf("Porem ao ser destruido, ele desroi ele destroi juntamente TODOS os 'X' do mapa!\n");
    printf("E fique tranquilo, o 'O' nao causa dano ao jogador.\n");

    printf("\nO inimigo 'T' eh perigoso! Ele atira na direcao do jogador! Desvie dos tiros e do inimigo!\n\n");

    printf("Colete o combustivel (indicado por 'F') para aumentar sua quantidade de combustivel. Cada unidade de combustivel coletada reabastece 40 pontos de combustivel.\n");
    printf("O combustivel decresce automaticamente. Cada movimento vertical do jogador decresce o combustivel em 2 pontos.\n");
    printf("Tiros decrescem o combustivel em 3 pontos.\n");
    printf("Se seu combustivel chegar a 0 (ZERO), voce perde o jogo!\n\n");

    printf("Aperte a barra de espaco para atirar.\n"); 
    printf("Os tiros sao usados para destruir os inimigos em seu caminho, mas cuidado, eles tambem destroem o combustivel!\n\n");

    printf("Para cada inimigo destruido sua pontuacao sobe em 50 pontos.\n");

    printf("Para jogar rankeado va em opcoes e complete-as ate o final\n");
    getch();
}

int menuOpcoes() {

    /*Menu de selecao*/
    int opcao;

    system(CLEAR);  
    printf("\n");
    printf("                                                    1 - Jogar\n");
    printf("                                                    2 - Opcoes\n");
    printf("                                                    3 - Ranking\n");
    printf("                                                    4 - Instrucoes\n");
    printf("                                                    5 - Sair\n");
    printf("\n");
    printf("                                                Escolha uma opcao: ");
    
    scanf("%d", &opcao);
    getchar();
    return opcao;
}

void boasVindas() {

    /*Tela inicial*/
    system(CLEAR);
    printf("\033[34m                 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄               ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄       ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄  \n");
    printf("                ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌             ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌\n");
    printf("                ▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀  ▐░▌           ▐░▌ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌     ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌ \n");
    printf("                ▐░▌       ▐░▌     ▐░▌       ▐░▌         ▐░▌  ▐░▌          ▐░▌       ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     ▐░▌       ▐░▌\n");
    printf("                ▐░█▄▄▄▄▄▄▄█░▌     ▐░▌        ▐░▌       ▐░▌   ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌     ▐░▌       ▐░▌\n");
    printf("                ▐░░░░░░░░░░░▌     ▐░▌         ▐░▌     ▐░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░▌       ▐░▌\n");
    printf("                ▐░█▀▀▀▀█░█▀▀      ▐░▌          ▐░▌   ▐░▌     ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀█░█▀▀      ▐░█▀▀▀▀█░█▀▀ ▐░█▀▀▀▀▀▀▀█░▌     ▐░▌     ▐░▌       ▐░▌\n");
    printf("                ▐░▌     ▐░▌       ▐░▌           ▐░▌ ▐░▌      ▐░▌          ▐░▌     ▐░▌       ▐░▌     ▐░▌  ▐░▌       ▐░▌     ▐░▌     ▐░▌       ▐░▌\n");
    printf("                ▐░▌      ▐░▌  ▄▄▄▄█░█▄▄▄▄        ▐░▐░▌       ▐░█▄▄▄▄▄▄▄▄▄ ▐░▌      ▐░▌      ▐░▌      ▐░▌ ▐░▌       ▐░▌ ▄▄▄▄█░█▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌\n");
    printf("                ▐░▌       ▐░▌▐░░░░░░░░░░░▌        ▐░▌        ▐░░░░░░░░░░░▌▐░▌       ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ \n");
    printf("                 ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀          ▀          ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀       ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀  \n");
    printf("\033[0m                                                                                                                                \n");
    printf("                                                                BEM VINDO\n\n");
    
    printf("                                                       Aperte ENTER para continuar\n");

    if (getchar()) {
        return;
    }
    else {
        boasVindas();
    }
}

int main () {

    int fimJogo = 0;
    int flag = 0;
    Player jogador[11];

    srand( time(0) );

    boasVindas();
    configPadrao();
    applyConfig();

    do{
        /*Seleciona opcao*/
        switch( menuOpcoes() ) {
            case 1:
                flag = 1;
            break;

            case 2:
                configuracoes();
            break;

            case 3:
                ranking(jogador);
            break;

            case 4:
                instrucoes();
            break;

            case 5:
                exit(EXIT_SUCCESS);
            break;

           default:
                printf("Opção inválida!\n");
            break;
        }
    } while (flag == 0);


    do{
        /*Loop do jogo enquanto nao houver Game Over*/
        fimJogo = jogoInicio();
    } while (fimJogo == 0);

    /*Checa se jogador pode entrar no ranking*/
    checkRanking(jogador);  

    if (fimJogo != 0) {
        /*Tela de Game Over*/
        system(CLEAR);
        printf("                                                                                                                       \n");
        printf("\e[0;31m     ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄       ▄▄  ▄▄▄▄▄▄▄▄▄▄▄       ▄▄▄▄▄▄▄▄▄▄▄  ▄               ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ \n");
        printf("    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░▌     ▐░░▌▐░░░░░░░░░░░▌     ▐░░░░░░░░░░░▌▐░▌             ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌\n");
        printf("    ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░▌░▌   ▐░▐░▌▐░█▀▀▀▀▀▀▀▀▀      ▐░█▀▀▀▀▀▀▀█░▌ ▐░▌           ▐░▌ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌\n");
        printf("    ▐░▌          ▐░▌       ▐░▌▐░▌▐░▌ ▐░▌▐░▌▐░▌               ▐░▌       ▐░▌  ▐░▌         ▐░▌  ▐░▌          ▐░▌       ▐░▌\n");
        printf("    ▐░▌ ▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░▌ ▐░▐░▌ ▐░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░▌       ▐░▌   ▐░▌       ▐░▌   ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌\n");
        printf("    ▐░▌▐░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌  ▐░▌  ▐░▌▐░░░░░░░░░░░▌     ▐░▌       ▐░▌    ▐░▌     ▐░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌\n");
        printf("    ▐░▌ ▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌   ▀   ▐░▌▐░█▀▀▀▀▀▀▀▀▀      ▐░▌       ▐░▌     ▐░▌   ▐░▌     ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀█░█▀▀ \n");
        printf("    ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌               ▐░▌       ▐░▌      ▐░▌ ▐░▌      ▐░▌          ▐░▌     ▐░▌  \n");
        printf("    ▐░█▄▄▄▄▄▄▄█░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░█▄▄▄▄▄▄▄█░▌       ▐░▐░▌       ▐░█▄▄▄▄▄▄▄▄▄ ▐░▌      ▐░▌ \n");
        printf("    ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌     ▐░░░░░░░░░░░▌        ▐░▌        ▐░░░░░░░░░░░▌▐░▌       ▐░▌\n");
        printf("     ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀▀▀▀▀▀▀▀▀▀▀          ▀          ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀ \n");
        printf("\e[0m                                                                                                                       \n");

        if (fimJogo == 1) {
            /*Texto printado caso Game Over por colisao*/
            printf("                                    Ora bolas, voce foi atingido por um inimigo!\n");
            printf("\n");
            printf("                                    Sua pontuacao foi: %d \n", pontos);
            printf("\n                                    Pressione qualquer tecla para sair do jogo.\n");
        }

        else if (fimJogo == 2) {
            /*Texto printado caso Game Over por falta de combustivel*/
            printf("                                    Ora bolas, seu combustivel acabou!\n");
            printf("\n");
            printf("                                    Sua pontuacao foi: %d \n", pontos);
            printf("\n                                    Pressione qualquer tecla para sair do jogo.\n");  
        }
    }
    getch();
    return 0;
}