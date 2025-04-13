#include <stdio.h>
#include <stdlib.h>
#include "libMichael.h"

// Declaração dos ponteiros para os arquivos
FILE *fpin, *fpout_PB_MA, *fpout_PB_MP, *fpoutN;

// Matrizes para os canais R, G e B da imagem
int **imagemR, **imagemG, **imagemB;

// Dimensões da imagem e valor de quantização
int ncol, nlin, quantizacao;

// Nome do arquivo de entrada
char input_file[256];

// Função que compara duas strings (substitui strcmp)
int compara_strings(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Função para copiar uma string (substitui strcpy)
void copia_string(char *dest, const char *src) {
    int i = 0;
    while ((dest[i] = src[i]) != '\0') i++;
}

// Encontra o último ponto '.' em uma string (para identificar a extensão do arquivo)
char *encontra_ultimo_ponto(char *str) {
    char *ultimo = NULL;
    while (*str) {
        if (*str == '.') ultimo = str;
        str++;
    }
    return ultimo;
}

// Verifica se o nome do arquivo termina com ".ppm"
int termina_com_ppm(char *str) {
    int i = 0;
    while (str[i] != '\0') i++;

    if (i < 4) return 0;

    return (str[i - 4] == '.' && str[i - 3] == 'p' && str[i - 2] == 'p' && str[i - 1] == 'm');
}

// Concatena uma extensão a uma string (nome de arquivo)
void concatena_extensao(char *dest, const char *extensao) {
    int i = 0;
    while (dest[i] != '\0') i++;
    int j = 0;
    while (extensao[j] != '\0') {
        dest[i++] = extensao[j++];
    }
    dest[i] = '\0';
}

// Pula comentários no cabeçalho do arquivo .ppm
void pular_comentarios() {
    char c;
    do {
        c = fgetc(fpin);
        if (c == '#') {
            while ((c = fgetc(fpin)) != '\n' && c != EOF);
        }
    } while (c == '#' || c == '\n');
    ungetc(c, fpin); // Devolve o caractere não-comentário ao fluxo
}

// Remove a extensão do nome de um arquivo
void remover_extensao(char *nome_arquivo, char *nome_sem_extensao) {
    copia_string(nome_sem_extensao, nome_arquivo);
    char *ponto = encontra_ultimo_ponto(nome_sem_extensao);
    if (ponto != NULL && compara_strings(ponto, ".ppm") == 0) {
        *ponto = '\0'; // Substitui o ponto por '\0' para cortar a string
    }
}

// Abre os arquivos de entrada e saída
void abrir_arquivos(char *input_file) {
    char base_name[256];
    remover_extensao(input_file, base_name);

    char output_file[256];

    // Abre arquivo para imagem em tons de cinza (média aritmética)
    sprintf(output_file, "%s_1.pgm", base_name);
    if ((fpout_PB_MA = fopen(output_file, "w")) == NULL) {
        printf("Não foi possível abrir o arquivo de saída PB_MA\n");
        exit(1);
    }

    // Abre arquivo para imagem em tons de cinza (média ponderada)
    sprintf(output_file, "%s_2.pgm", base_name);
    if ((fpout_PB_MP = fopen(output_file, "w")) == NULL) {
        printf("Não foi possível abrir o arquivo de saída PB_MP\n");
        exit(1);
    }

    // Abre arquivo para imagem negativa
    sprintf(output_file, "%s_3.ppm", base_name);
    if ((fpoutN = fopen(output_file, "w")) == NULL) {
        printf("Não foi possível abrir o arquivo de saída ImagemN\n");
        exit(1);
    }
}

// Lê os dados de imagem RGB do arquivo
void ler_imagem(void) {
    imagemR = (int **)malloc(nlin * sizeof(int *));
    imagemG = (int **)malloc(nlin * sizeof(int *));
    imagemB = (int **)malloc(nlin * sizeof(int *));
    for (int cont = 0; cont < nlin; cont++) {
        imagemR[cont] = (int *)malloc(ncol * sizeof(int));
        imagemG[cont] = (int *)malloc(ncol * sizeof(int));
        imagemB[cont] = (int *)malloc(ncol * sizeof(int));
    }
    for (int lin = 0; lin < nlin; lin++) {
        for (int col = 0; col < ncol; col++) {
            fscanf(fpin, "%d", &imagemR[lin][col]);
            fscanf(fpin, "%d", &imagemG[lin][col]);
            fscanf(fpin, "%d", &imagemB[lin][col]);
        }
    }
}

// Lê o cabeçalho da imagem (formato, tamanho, quantização)
void ler_cabecalho(void) {
    char controle[4];

    pular_comentarios();
    fscanf(fpin, "%s", controle); // P3 (imagem colorida)

    pular_comentarios();
    fscanf(fpin, "%d %d", &ncol, &nlin); // Número de colunas e linhas

    pular_comentarios();
    fscanf(fpin, "%d", &quantizacao); // Valor máximo (geralmente 255)
}

// Fecha todos os arquivos abertos
void fechar_arquivos(void) {
    fclose(fpin);
    fclose(fpout_PB_MA);
    fclose(fpout_PB_MP);
    fclose(fpoutN);
}

// Grava imagem em tons de cinza usando média aritmética
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

// Grava imagem em tons de cinza usando média ponderada
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

// Grava imagem negativa (inverte os valores de RGB)
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

// Solicita o nome do arquivo ao usuário
void ler_nome(void) {

    while(1)
    {
        printf("Digite o nome do arquivo da imagem de entrada: ");
        scanf("%s", input_file);

        if (!termina_com_ppm(input_file)) {
            concatena_extensao(input_file, ".ppm");
        }

        if ((fpin = fopen(input_file, "r")) == NULL) {
            printf("Não foi possível abrir o arquivo de imagem %s\n", input_file);
        }
        else {
            break;
        }
    }   
}

// Função principal de execução de todo o processo
void executa(char *input_file) {
    ler_nome();                        // Lê nome do arquivo
    abrir_arquivos(input_file);       // Abre arquivos
    ler_cabecalho();                  // Lê cabeçalho da imagem
    ler_imagem();                     // Lê dados RGB
    gravar_imagem_PB_MA(input_file);  // Grava imagem PB (média aritmética)
    gravar_imagem_PB_MP(input_file);  // Grava imagem PB (média ponderada)
    gravar_imagemN(input_file);       // Grava imagem negativa
    fechar_arquivos();                // Fecha todos os arquivos
}
