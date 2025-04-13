#include <stdio.h>
#include <stdlib.h>
#include "funcoes.c"

int main() {
    int continua;

    do {
        printf("*****************************************************************\n");
        printf("*                                                               *\n");
        printf("*          Seja bem-vindo(a) ao conversor de imagem             *\n");
        printf("*                                                               *\n");
        printf("*****************************************************************\n");
        executa(input_file);
        printf("Deseja executar o programa novamente? 1 para SIM - 0 para NAO: ");
        getchar(); // consome enter
        scanf("%d", &continua);
    } while (continua);
    printf("*****************************************************************\n");
    printf("*                                                               *\n");
    printf("*          Programa encerrado! Obrigado por utilizar!           *\n");
    printf("*                                                               *\n");
    printf("*****************************************************************\n");
    return 0;
}
