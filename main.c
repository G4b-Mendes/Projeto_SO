#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "funcoes.h"

int main(int argc, char *argv[])
{   
    if (argc != 8)
    {
        printf("numero de parametros invalido");
        exit(1);
    }

    int num_thread = atoi(argv[1]);
    int n = atoi(argv[2]);

    double tempo_Soma = 0;
    double tempo_Multiplicacao = 0;
    double tempo_Reducao = 0;
    double tempo_Total = 0;

    int *matrizA = aloca_Matriz(n);
    int *matrizB = aloca_Matriz(n);
    int *matrizC = aloca_Matriz(n);
    int *matrizD = aloca_Matriz(n);
    int *matrizE = aloca_Matriz(n);
    
    FILE *arquivo1 = fopen(argv[3], "r+");
    FILE *arquivo2 = fopen(argv[4], "r+");
    FILE *arquivo3 = fopen(argv[5], "r+");
    FILE *arquivo4 = fopen(argv[6], "w+");
    FILE *arquivo5 = fopen(argv[7], "w+");

    leitura_Matriz(arquivo1, arquivo2, matrizA, matrizB, n, num_thread);

    tempo_Soma = soma_Matriz(matrizA, matrizB, matrizD, n, num_thread); 

    gravar_Ler(arquivo4, arquivo3, matrizD, matrizC, n, num_thread);

    tempo_Multiplicacao = multiplica_Matriz(matrizC, matrizD, matrizE, n, num_thread);

    tempo_Reducao = reduzir_Gravar(arquivo5, matrizE, n, num_thread); 

    tempo_Total = tempo_Soma + tempo_Multiplicacao + tempo_Reducao;

    printf("Tempo soma: %lf \n", tempo_Soma);
    printf("Tempo multiplicação: %lf \n", tempo_Multiplicacao);
    printf("Tempo redução: %lf \n", tempo_Reducao);
    printf("Tempo total: %lf \n", tempo_Total);

    fclose(arquivo1);
    fclose(arquivo2);
    fclose(arquivo3);
    fclose(arquivo4);
    fclose(arquivo5);

    return 0;
}
