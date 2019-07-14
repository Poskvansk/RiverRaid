#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
	char nick[11];
	int score;
}Player;

char tabuleiro[10][135];
int altura=99, largura=99;
int probX=99, probF=99, probO=99, probT=99;
int ammoT=99;
int velocidade=99;

int pontos=0;
int ranked=0;



int compare(const void* a, const void* b) {

    /*Funcao de comparacao usada no Qsort, compara a pontuacao dos jogadores para posiciona-los no ranking*/
    /*Os ponteiros 'a' e 'b' recebem o score, e ordena decrescentemente*/
    if ( ((Player*)a)-> score > ((Player*)b)->score) {
        return -1;
    }
    if ( ((Player*)a)-> score < ((Player*)b)->score) {
        return 1;
    }
    else{
        return 0;
    }
}

void entraRanking (Player jogador[11]) {

    FILE* fd;
    fd = fopen("ranking.bin", "wb");

    qsort(jogador,sizeof(Player), 11, compare);
    fwrite(jogador, sizeof(Player), 11, fd);
    fclose(fd);
}

void checkRanking (Player jogador[11]) {

    int flag=0;
    int nickLen;

    printf("CHECK\n");
    scanf("%d%d", &ranked, &pontos);

    if ( (pontos > jogador[10].score) && (ranked == 1) ) {
        printf("        Voce pode entrar no ranking!\n");

        printf("\n        Digite um nick entre 1 e 10 letras: \n");

        while (flag==0) {
            
            scanf("%s", jogador[9].nick);
            nickLen = strlen(jogador[9].nick);

            if ( (nickLen <1) || (nickLen > 10) ) {
                printf("Tamanho Invalido!\n");
            }
            else {
                flag = 1;
            }
        }

        jogador[9].score = pontos;
    }

    entraRanking(jogador);
}

void clearRanking(Player jogador[]) {

    int i;

    FILE* fd;
    fd = fopen("ranking.bin", "rb");

    if( fd == NULL ) {

        fd = fopen("ranking.bin", "wb");

        for (i=0; i<10; i++) {
            
            jogador[i].nick[0] = '\0';
            jogador[i].score = 0;
        }
        fwrite(jogador, sizeof(Player), 10, fd);
    }
    fclose(fd);
}

void ranking (Player jogador[11]) {

    /*Ranking dos jogadores*/
    int i;

    FILE* fd;
    fd = fopen("ranking.bin", "rb");

    if( fd == NULL ) {

       clearRanking(jogador);
    }

    printf("ESSE ESPAÇO FOI FEITO PARA EXALTAR O ADMIRÁVEL FEITO DOS MELHORES JOGADORES DE RIVER RAID\n\n");

    fread(jogador, sizeof(Player), 11, fd);
    qsort(jogador, 11, sizeof(Player), compare);

    for (i=0; i<10; i++) {
        printf("%d - %s     %d\n", i+1, jogador[i].nick, jogador[i].score);
    }

    printf("\n");

    fclose(fd);

    getchar();
    return;
}

int main() {

	Player jogador[11];

	clearRanking(jogador);
	ranking(jogador);
	checkRanking(jogador);
	ranking(jogador);

	return 0;
}