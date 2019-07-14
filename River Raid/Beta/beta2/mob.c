#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

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

char tabuleiro[10][135];
int altura, largura;
int probX, probF, probO, probT;
int velocidade;

/*Variaveis Globadas definidas por mim*/
int posicaoJogador = 0;
int combustivel = 400;
int pontos = 0;

typedef struct {
    char nick[11];
    int score;    
}Player;

typedef struct {
    int ammo;
    char gnr;
}enemyT;

typedef struct {
    int hp; 
}enemyO;

void jogoMapa() {

    int i, j;
    int objeto, posicao, spawn;

    usleep(70000);
    system(CLEAR);

    /* CHECA MATRIZ */
    for (i = 0; i< 10; i++) {

        for (j=134; j>=0; j--) {

            if (tabuleiro[i][j] == '>'  ) {

                 /*Tem tiro? Necessita mover o tiro*/

                moveTiro(i, j);
            }
        }
    }

    /*CHECA POSICAO INIMIGOS*/
    for (i=0; i<10; i++) {

        for (j=0; j<135; j++) {

            if ( (tabuleiro[i][j] == 'X') || (tabuleiro[i][j] == 'O') || (tabuleiro[i][j] == 'T') || (tabuleiro[i][j] == '<') ) {
            
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
    for (i=0; i<10; i++) {

        for (j=0; j<135; j++) {

            if (i==0 || i==9) {
                tabuleiro[i][j] = '#';
            }
            else if (i==5 - posicaoJogador && j==0) {
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
            else if (i == 2 && j== 134) {

                objeto = rand()%100 + 1; /*Determina se objeto é X (taxa de 55%) ou F(45%)*/
                posicao = rand()%9 + 1; /*Determina linha em que vai gerar o objeto (numero pseudo-aleatorio de 1 a 9)*/
                spawn = rand()%100 + 1; /*Checa se ira spawnar objeto ou nao (taxa de spawn de 40%)*/

                if (spawn >= 60) {

                    if (objeto <= pro ) {
                        
                        tabuleiro[ posicao ][j] = 'X';
                    }
                    else{   
                        tabuleiro[posicao][j] = 'F';
                    }
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
    for (i = 0; i<10; i++) {
        for (j=0; j<135; j++) {

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

    if ( (tabuleiro[5 - posicaoJogador][0] == '+') && (tabuleiro[5 - posicaoJogador][1] == 'F') ) {
        /*Adiciona 40 de combustivel caso o jogador se choque com combustivel*/
        combustivel +=40;
    }
}