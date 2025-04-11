#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libMichael.h"

FILE *fpin, *fpout_PB_MA, *fpout_PB_MP, *fpoutN;
int **imagemR, **imagemG, **imagemB, ncol, nlin, quantizacao;
char input_file[256];

// Função que ignora linhas de comentários começando com '#'
void pular_comentarios() {
    char c;
    do {
        c = fgetc(fpin);
        if (c == '#') {
            while ((c = fgetc(fpin)) != '\n' && c != EOF); // pula a linha
        }
    } while (c == '#' || c == '\n'); // continua se for mais comentários ou quebras de linha extras
    ungetc(c, fpin); // devolve o caractere útil
}

// Função para remover a extensão
void remover_extensao(char *nome_arquivo, char *nome_sem_extensao) {
    strcpy(nome_sem_extensao, nome_arquivo);
    char *ponto = strrchr(nome_sem_extensao, '.');
    if (ponto != NULL && strcmp(ponto, ".ppm") == 0) {
        *ponto = '\0';  // remove a extensão
    }
}


// Função que abre os arquivos de entrada e saída

void abrir_arquivos(char *input_file) {

    char base_name[256];
    remover_extensao(input_file, base_name);
    if ((fpin = fopen(input_file, "r")) == NULL) {
        printf("Não foi possível abrir o arquivo de imagem %s\n", input_file);
        exit(1);
    }

    char output_file[256];

    // Gravar imagem em tons de cinza com média aritmética simples
    snprintf(output_file, sizeof(output_file), "%s_1.pgm", base_name);
    if ((fpout_PB_MA = fopen(output_file, "w")) == NULL) {
        printf("Não foi possível abrir o arquivo de saída PB_MA\n");
        exit(1);
    }

    // Gravar imagem em tons de cinza com média aritmética ponderada
    snprintf(output_file, sizeof(output_file), "%s_2.pgm", base_name);
    if ((fpout_PB_MP = fopen(output_file, "w")) == NULL) {
        printf("Não foi possível abrir o arquivo de saída PB_MP\n");
        exit(1);
    }

    // Gravar imagem negativa
    snprintf(output_file, sizeof(output_file), "%s_3.ppm", base_name);
    if ((fpoutN = fopen(output_file, "w")) == NULL) {
        printf("Não foi possível abrir o arquivo de saída ImagemN\n");
        exit(1);
    }
}

void ler_imagem(void) {
    int cont, col, lin;

    imagemR = (int **)malloc(nlin * sizeof(int *));
    imagemG = (int **)malloc(nlin * sizeof(int *));
    imagemB = (int **)malloc(nlin * sizeof(int *));
    for (cont = 0; cont < nlin; cont++) {
        imagemR[cont] = (int *)malloc(ncol * sizeof(int));
        imagemG[cont] = (int *)malloc(ncol * sizeof(int));
        imagemB[cont] = (int *)malloc(ncol * sizeof(int));
    }
    for (lin = 0; lin < nlin; lin++) {
        for (col = 0; col < ncol; col++) {
            fscanf(fpin, "%d", &imagemR[lin][col]);
            fscanf(fpin, "%d", &imagemG[lin][col]);
            fscanf(fpin, "%d", &imagemB[lin][col]);
        }
    }
}

void ler_cabecalho(void) {
    char controle[4];

    pular_comentarios();              // <- antes de ler "P3"
    fscanf(fpin, "%s", controle);     // P3

    pular_comentarios();              // <- antes de ler ncol, nlin
    fscanf(fpin, "%d %d", &ncol, &nlin);

    pular_comentarios();              // <- antes de ler quantização
    fscanf(fpin, "%d", &quantizacao);
}


void fechar_arquivos(void) {
    fclose(fpin);
    fclose(fpout_PB_MA);
    fclose(fpout_PB_MP);
    fclose(fpoutN);
}

void gravar_imagem_PB_MA(char *output_file) {
    fprintf(fpout_PB_MA, "P2\n");
    fprintf(fpout_PB_MA, "# Imagem em tons de cinza (média aritmética)\n");
    fprintf(fpout_PB_MA, "%d %d\n", ncol, nlin);
    fprintf(fpout_PB_MA, "%d\n", quantizacao);
    for (int lin = 0; lin < nlin; lin++) {
        for (int col = 0; col < ncol; col++) {
            fprintf(fpout_PB_MA, "%d ", (imagemR[lin][col] + imagemG[lin][col] + imagemB[lin][col]) / 3);
        }
        fprintf(fpout_PB_MA, "\n");
    }
}

void gravar_imagem_PB_MP(char *output_file) {
    fprintf(fpout_PB_MP, "P2\n");
    fprintf(fpout_PB_MP, "# Imagem em tons de cinza (média ponderada)\n");
    fprintf(fpout_PB_MP, "%d %d\n", ncol, nlin);
    fprintf(fpout_PB_MP, "%d\n", quantizacao);
    for (int lin = 0; lin < nlin; lin++) {
        for (int col = 0; col < ncol; col++) {
            fprintf(fpout_PB_MP, "%d ", (int)(imagemR[lin][col] * 0.299 + imagemG[lin][col] * 0.587 + imagemB[lin][col] * 0.114));
        }
        fprintf(fpout_PB_MP, "\n");
    }
}

void gravar_imagemN(char *output_file) {
    fprintf(fpoutN, "P3\n");
    fprintf(fpoutN, "# Imagem negativa gerada automaticamente\n");
    fprintf(fpoutN, "%d %d\n", ncol, nlin);
    fprintf(fpoutN, "%d\n", quantizacao);
    for (int lin = 0; lin < nlin; lin++) {
        for (int col = 0; col < ncol; col++) {
            fprintf(fpoutN, "%d ", quantizacao - imagemR[lin][col]);
            fprintf(fpoutN, "%d ", quantizacao - imagemG[lin][col]);
            fprintf(fpoutN, "%d ", quantizacao - imagemB[lin][col]);
        }
        fprintf(fpoutN, "\n");
    }
}

void ler_nome(void) {
    printf("Digite o nome do arquivo da imagem de entrada: ");
    scanf("%s", input_file);

    // Verifica se o nome do arquivo termina com ".ppm"
    if (strstr(input_file, ".ppm") == NULL) {
        strcat(input_file, ".ppm");
    }
}

void executa(char *input_file)
{
    ler_nome();
    abrir_arquivos(input_file);
    ler_cabecalho();
    ler_imagem();
    gravar_imagem_PB_MA(input_file);
    gravar_imagem_PB_MP(input_file);
    gravar_imagemN(input_file);
    fechar_arquivos();
}
int main() {

    int continua;

    do{
        executa(input_file);
        printf("Deseja executar o programa novamente? 1 para SIM - 0 para NAO: ");
        getchar();
        scanf("%d", &continua);
    }while(continua);
    printf("Programa encerrado!");
    
    return 0;
}
