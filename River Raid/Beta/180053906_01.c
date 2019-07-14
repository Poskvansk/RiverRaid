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
int probX, probF;
int velocidade;

/*Variaveis Globadas definidas por mim*/
int posicaoJogador = 0;
int combustivel = 400;
int pontos = 0;

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

int gameOver() {

    if ( (tabuleiro[5 - posicaoJogador][0] == '+') && (tabuleiro[5 - posicaoJogador][1]=='X') ) {

        /*Retorna 1 para caso de colisao*/
        return 1;
    }

    if (combustivel <= 0) {
        /*Retorna 2 para caso de zero combustivel*/
        return 2;
    }

    else {
        /*Retorna 0 para indicar que jogo continua*/
        return 0;
    }
}

void destroiAlvo() {

    int i, j;

    for (i=0; i<10; i++) {

        for (j=134; j>=0; j--) {

            if ( (tabuleiro[i][j] == '>') && (tabuleiro[i][j+1] == 'X') ){
                tabuleiro[i][j] = ' ';
                tabuleiro[i][j+1] = ' ';
                pontos += 50;
            }
            if ( (tabuleiro[i][j] == '>') && tabuleiro[i][j + 1] == 'F') {
                tabuleiro[i][j] = ' ';
                tabuleiro[i][j+1] = ' ';
            }  
        }
    }

}

void moveCima() {

    posicaoJogador += 1;
    if (posicaoJogador > 4) {
        posicaoJogador = 4;
    }
}

void moveBaixo() {
    
    posicaoJogador -= 1;
    if (posicaoJogador < -3) {
        posicaoJogador = -3;
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
}

void moveTiro(int i, int j) {

    if (tabuleiro[i][j] == '>') {

        /*Move tiro uma unidade para direita*/

        tabuleiro[i][j + 1] = '>';
        tabuleiro[i][j] = ' ';

        if (j+1 >= 135) {
            /*Desfaz o tiro caso ele ultrapasse a borda da matriz*/
            tabuleiro[i][j] = ' ';
            tabuleiro[i][j +1] = ' ';            
        }
    }
   return;
}

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

            if (tabuleiro[i][j] == 'X') {
            
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
            else if (tabuleiro[i][j] == 'F') {
                tabuleiro[i][j] = 'F';
            }
            else if (i == 2 && j== 134) {

                objeto = rand()%100 + 1; /*Determina se objeto é X (taxa de 55%) ou F(45%)*/
                posicao = rand()%9 + 1; /*Determina linha em que vai gerar o objeto (numero pseudo-aleatorio de 1 a 9)*/
                spawn = rand()%100 + 1; /*Checa se ira spawnar objeto ou nao (taxa de spawn de 40%)*/

                if (spawn >= 60) {

                    if (objeto <= 55 ) {
                        
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
                jogoMapa();
                
            }

            else if (movimento == 's' || movimento == 'S') {

                /*Move para baixo, decresce combustivel, atualiza mapa, checa game over*/
                moveBaixo();
                fimJogo = gameOver();
                combustivel-=2;
                jogoMapa();
                
            }

            else if (movimento == 0x20) {

                /*Posiciona o tiro a frente do jogador, decresce combustivel, checa game over, checa colisao com objetos, atualiza o mapa*/
                i = 5 - posicaoJogador;
                j = 1;
                tabuleiro[i][j] = '>';
                combustivel -=3;

                fimJogo = gameOver();
                destroiAlvo();
                jogoMapa();
                
            }
        }   
        else {
            /*Continua a rodar o jogo caso nenhuma tecla pressionada*/
            
            fimJogo = gameOver();
            combustivel--;
            jogoMapa();
            
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

void configuracoes() {

    /*Menu de configuracoes do jogo, sera adicionado futuramente*/

    system(CLEAR);
    printf("Construcao em andamento\n");
    getchar();
    return;
}

void ranking () {

    /*Ranking dos jogadores, sera adicionado futuramente*/

    system(CLEAR);
    printf("Construcao em andamento\n");
    getchar();
    return;
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

    printf("Colete o combustivel (indicado por 'F') para aumentar sua quantidade de combustivel. Cada unidade de combustivel coletada reabastece 40 pontos de combustivel.\n");
    printf("O combustivel decresce automaticamente. Cada movimento vertical do jogador decresce o combustivel em 2 pontos.\n");
    printf("Tiros decrescem o combustivel em 3 pontos.\n");
    printf("Se seu combustivel chegar a 0 (ZERO), voce perde o jogo!\n\n");

    printf("Aperte a barra de espaco para atirar.\n"); 
    printf("Os tiros sao usados para destruir os inimigos em seu caminho, mas cuidado, eles tambem destroem o combustivel!\n\n");

    printf("Para cada inimigo destruido sua pontuacao sobe em 50 pontos.\n");

    getch();
    return;
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

    printf("\033[34m                ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄               ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄       ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄  \n");
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

    srand( time(0) );

    boasVindas();

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
                ranking();
            break;

            case 4:
                instrucoes();
            break;

            case 5:
                exit(EXIT_SUCCESS);
            break;

           default:
                exit(EXIT_FAILURE);
            break;
        }
    } while (flag == 0);

    do{
        /*Loop do jogo enquanto nao houver Game Over*/
        fimJogo = jogoInicio();

    } while (fimJogo == 0);

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