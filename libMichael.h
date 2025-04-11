#ifndef LIBMICHAEL_H
#define LIBMICHAEL_H

void pular_comentarios();
void abrir_arquivos(char *input_file);
void ler_cabecalho(void);
void ler_imagem(void);
void fechar_arquivos(void);
void gravar_imagem_PB_MA(char *output_file);
void gravar_imagem_PB_MP(char *output_file);
void gravar_imagemN(char *output_file);
void remover_extensao(char *nome_arquivo, char *nome_sem_extensao);
void ler_nome(void);
void executa(char *input_file);

#endif // LIBMICHAEL_H
