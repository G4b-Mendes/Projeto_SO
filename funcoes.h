#ifndef FUNCOES_H
#define FUNCOES_H

typedef struct Parametros_Thread
{
    int inicio;
    int fim;
    int ordem;
    int numero_thread;
    int *matriz1;
    int *matriz2;
    int *matriz3;
    int *reducao;
    FILE *arquivo;  
}Parametros_Thread;

double calcula_tempo(struct timespec tempo_Inicial, struct timespec tempo_Final);

int *aloca_Matriz(int n);

void *thread_Leitura(void *args);

void *thread_Gravar(void *args);

void *thread_Soma(void *args);

void *thread_Multiplica(void *args);

void *thread_reduzir(void *args);

void leitura_Matriz(FILE *nome_ArquivoA, FILE *nome_ArquivoB, int *matriz_A, int *matriz_B, int n, int num_Thread);

double soma_Matriz(int *matriz_a, int *matriz_b, int *matriz_c, int ordem, int num_thread);

void gravar_Ler(FILE *nome_ArquivoA, FILE *nome_ArquivoB, int *matriz_A, int *matriz_B, int n, int num_Thread);

double multiplica_Matriz(int *matriz_C, int *matriz_D, int *matriz_E, int n, int num_Thread);

double reduzir_Gravar(FILE *nome_Arquivo, int *matriz_E, int n, int num_Thread);

#endif