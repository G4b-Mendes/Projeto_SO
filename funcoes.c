#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "funcoes.h"

#define posicao(i, j, n) ((i) * (n) + (j))

double calcula_tempo(struct timespec tempo_Inicial, struct timespec tempo_Final)
{
	return	((double) tempo_Final.tv_sec + 1.0e-9 * tempo_Final.tv_nsec) - 
			((double) tempo_Inicial.tv_sec + 1.0e-9 * tempo_Inicial.tv_nsec);
}

int *aloca_Matriz(int n)
{
	int *matriz = (int *)malloc(n * n * sizeof(int));
	
	if(matriz == NULL)
	{
		printf("Erro na alocação");

		exit(1);
	} 

	return matriz;
}

void *thread_Leitura(void *args)
{	
	int n = ((Parametros_Thread*)args)->ordem; 
	int *matriz = ((Parametros_Thread*)args)->matriz1;
	FILE *arquivo = ((Parametros_Thread*)args)->arquivo;

	for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++){
			fscanf(arquivo, "%d ", &matriz[posicao(i, j, n)]);
        }       
    }

	return NULL;
}

void *thread_Gravar(void *args)
{
	int n = ((Parametros_Thread*)args)->ordem; 
	int *matriz = ((Parametros_Thread*)args)->matriz1;
	FILE *arquivo = ((Parametros_Thread*)args)->arquivo;

	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			fprintf(arquivo, "%d ", matriz[(posicao(i, j, n))]);
		}

		fseek(arquivo, -1, SEEK_CUR); 	// Retrocede um caractere para remover o último espaço
		fputc('\n', arquivo);			// Adiciona a quebra de linha após cada linha da matriz
	
	}

	return NULL;
}

void *thread_Soma(void *args)
{
	int _inicio = ((Parametros_Thread*)args)->inicio; 
	int _fim = ((Parametros_Thread*)args)->fim;
	int n = ((Parametros_Thread*)args)->ordem;
	int _num_Thread = ((Parametros_Thread*)args)->numero_thread;
	int *_matriz1 = ((Parametros_Thread*)args)->matriz1;
	int *_matriz2 = ((Parametros_Thread*)args)->matriz2;
	int *_matriz3 = ((Parametros_Thread*)args)->matriz3;

	for(int i = _inicio; i <= _fim; i += _num_Thread)
	{
   	    for(int j = 0; j < n; j++)
		{
  		    _matriz3[posicao(i, j, n)] = _matriz1[(posicao(i, j, n))] + _matriz2[(posicao(i, j, n))];
   	    }
    }

	return NULL;
}

void *thread_Multiplica(void *args)
{
	int _inicio = ((Parametros_Thread*)args)->inicio;
	int _fim = ((Parametros_Thread*)args)->fim;
	int n = ((Parametros_Thread*)args)->ordem;
	int _num_Thread = ((Parametros_Thread*)args)->numero_thread;
	int *_matriz1 = ((Parametros_Thread*)args)->matriz1;
	int *_matriz2 = ((Parametros_Thread*)args)->matriz2;
	int *_matriz3 = ((Parametros_Thread*)args)->matriz3;	
		
	for(int i = _inicio; i < _fim + 1; i += _num_Thread)
	{
   		for(int j = 0; j < n; j++)
		{
			_matriz3[posicao(i, j, n)] = 0;
        	for(int k = 0; k < n; k++)
			{
   	     		_matriz3[(posicao(i, j, n))] += _matriz1[posicao(i, k, n)] * _matriz2[posicao(k, j, n)];
        	}
   	    }
	}

	return NULL;
}

void *thread_reduzir(void *args)
{

	int _inicio = ((Parametros_Thread*)args)->inicio;
	int _fim = ((Parametros_Thread*)args)->fim;
	int *matriz = ((Parametros_Thread*)args)->matriz1;
	int soma = 0;
	
	for(int i = _inicio; i < _fim; i++)
	{
   	 	soma += matriz[i];
	}

	int* reducao = malloc(sizeof(int));
	*reducao = soma;
	return reducao;
}

void leitura_Matriz(FILE *nome_ArquivoA, FILE *nome_ArquivoB, int *matriz_A, int *matriz_B, int n, int num_Thread)
{
	if(num_Thread == 1)
	{
		Parametros_Thread *parametro_Leitura = (Parametros_Thread *)malloc(sizeof(Parametros_Thread) * 2);
		
		parametro_Leitura[0].ordem = n;
		parametro_Leitura[0].matriz1 = matriz_A;
		parametro_Leitura[0].arquivo = nome_ArquivoA;

		parametro_Leitura[1].ordem = n;
		parametro_Leitura[1].matriz1 = matriz_B;
		parametro_Leitura[1].arquivo = nome_ArquivoB;

		thread_Leitura((void *)&parametro_Leitura[0]);
		thread_Leitura((void *)&parametro_Leitura[1]);
	}

	else
	{
		pthread_t *threads_Id = (pthread_t *)malloc(sizeof(pthread_t) * 2);
		
		Parametros_Thread *parametro_Leitura = (Parametros_Thread *)malloc(sizeof(Parametros_Thread) * 2);

		parametro_Leitura[0].ordem = n;
		parametro_Leitura[0].matriz1 = matriz_A;
		parametro_Leitura[0].arquivo = nome_ArquivoA;

		parametro_Leitura[1].ordem = n;
		parametro_Leitura[1].matriz1 = matriz_B;
		parametro_Leitura[1].arquivo = nome_ArquivoB;
		
		for(int i = 0; i < 2; i++)
		{
			pthread_create(&threads_Id[i], NULL, thread_Leitura, (void *) &parametro_Leitura[i]);
		}

		for (int i = 0; i < 2; i++) 
		{
            pthread_join(threads_Id[i], NULL);
        }
	}
}

double soma_Matriz(int *matriz_A, int *matriz_B, int *matriz_D, int n, int num_thread)
{
	struct timespec tempo_Inicio = {0, 0};
	struct timespec tempo_Fim = {0, 0};

	if (num_thread == 1)
	{
		Parametros_Thread *parametro_Soma = (Parametros_Thread *)malloc(sizeof(Parametros_Thread));

		parametro_Soma[0].inicio = 0;
		parametro_Soma[0].fim = n - 1;
		parametro_Soma[0].ordem = n;
		parametro_Soma[0].numero_thread = num_thread;
		parametro_Soma[0].matriz1 = matriz_A;
		parametro_Soma[0].matriz2 = matriz_B;
		parametro_Soma[0].matriz3 = matriz_D;
		
		clock_gettime(CLOCK_MONOTONIC, &tempo_Inicio);
		
		thread_Soma((void *)&parametro_Soma[0]);
		
		clock_gettime(CLOCK_MONOTONIC, &tempo_Fim);

		double tempo_soma = calcula_tempo(tempo_Inicio, tempo_Fim);

		return tempo_soma;
	}

	else
	{
		pthread_t *threads_Id = (pthread_t *)malloc(sizeof(pthread_t) * num_thread);

	
		Parametros_Thread *parametro_Soma = (Parametros_Thread *)malloc(sizeof(Parametros_Thread) * num_thread);

		clock_gettime(CLOCK_MONOTONIC, &tempo_Inicio);

		for (int i = 0; i < num_thread; i++)
		{
			parametro_Soma[i].inicio = i;
			parametro_Soma[i].fim = (n + 1) - (num_thread - i);
			parametro_Soma[i].ordem = n;
			parametro_Soma[i].numero_thread = num_thread;
			parametro_Soma[i].matriz1 = matriz_A;
			parametro_Soma[i].matriz2 = matriz_B;
			parametro_Soma[i].matriz3 = matriz_D;
		
			pthread_create(&threads_Id[i], NULL, thread_Soma, (void *) &parametro_Soma[i]);
		}

		for(int i = 0; i < num_thread; i++)
		{
			pthread_join(threads_Id[i], NULL);
		}

		clock_gettime(CLOCK_MONOTONIC, &tempo_Fim);

		double tempo_soma = calcula_tempo(tempo_Inicio, tempo_Fim);

		return tempo_soma;
	}	

}

void gravar_Ler(FILE *nome_ArquivoA, FILE *nome_ArquivoB, int *matriz_D, int *matriz_C, int n, int num_Thread)
{
	if(num_Thread == 1)
	{
		Parametros_Thread *parametro_Gravar = (Parametros_Thread *)malloc(sizeof(Parametros_Thread) * 2);
		Parametros_Thread *parametro_Ler = (Parametros_Thread *)malloc(sizeof(Parametros_Thread) * 2);

		parametro_Gravar[0].ordem = n;
		parametro_Gravar[0].matriz1 = matriz_D;
		parametro_Gravar[0].arquivo = nome_ArquivoA;

		parametro_Ler[0].ordem = n;
		parametro_Ler[0].matriz1 = matriz_C;
		parametro_Ler[0].arquivo = nome_ArquivoB;

		thread_Gravar((void *)&parametro_Gravar[0]);
		thread_Leitura((void *)&parametro_Ler[0]);
	}

	else
	{
		pthread_t *threads_Id = (pthread_t *)malloc(sizeof(pthread_t) * 2);

		Parametros_Thread *parametro_Gravar = (Parametros_Thread *)malloc(sizeof(Parametros_Thread));
		Parametros_Thread *parametro_Ler = (Parametros_Thread *)malloc(sizeof(Parametros_Thread));

		parametro_Gravar[0].ordem = n;
		parametro_Gravar[0].matriz1 = matriz_D;
		parametro_Gravar[0].arquivo = nome_ArquivoA;

		parametro_Ler[0].ordem = n;
		parametro_Ler[0].matriz1 = matriz_C;
		parametro_Ler[0].arquivo = nome_ArquivoB;
		
		pthread_create(&threads_Id[0], NULL, thread_Gravar, (void *) &parametro_Gravar[0]);
		pthread_create(&threads_Id[1], NULL, thread_Leitura, (void *) &parametro_Ler[0]);

		for(int i = 0; i < 2; i++)
		{
			pthread_join(threads_Id[i], NULL);
		}
	}
}


double multiplica_Matriz(int *matriz_C, int *matriz_D, int *matriz_E, int n, int num_Thread)
{
	struct timespec tempo_Inicio = {0, 0};
	struct timespec tempo_Fim = {0, 0};

	if (num_Thread == 1)
	{
		Parametros_Thread *parametro_Multiplica = (Parametros_Thread *)malloc(sizeof(Parametros_Thread));

		parametro_Multiplica[0].inicio = 0;
		parametro_Multiplica[0].fim = n * n;
		parametro_Multiplica[0].ordem = n;
		parametro_Multiplica[0].numero_thread = num_Thread;
		parametro_Multiplica[0].matriz1 = matriz_C;
		parametro_Multiplica[0].matriz2 = matriz_D;
		parametro_Multiplica[0].matriz3 = matriz_E;

		clock_gettime(CLOCK_MONOTONIC, &tempo_Inicio);
		
		thread_Multiplica((void *)&parametro_Multiplica[0]);

		clock_gettime(CLOCK_MONOTONIC, &tempo_Fim);

		double tempo_soma = calcula_tempo(tempo_Inicio, tempo_Fim);

		return tempo_soma;
	}

	else
	{
		pthread_t *threads_Id = (pthread_t *)malloc(sizeof(pthread_t) * num_Thread);

		Parametros_Thread *parametro_Multiplica = (Parametros_Thread *)malloc(sizeof(Parametros_Thread) * num_Thread);

		clock_gettime(CLOCK_MONOTONIC, &tempo_Inicio);

		for (int i = 0; i < num_Thread; i++)
		{
			parametro_Multiplica[i].inicio = i;
			parametro_Multiplica[i].fim = (n + 1) - (num_Thread - i);
			parametro_Multiplica[i].ordem = n;
			parametro_Multiplica[i].numero_thread = num_Thread;
			parametro_Multiplica[i].matriz1 = matriz_C;
			parametro_Multiplica[i].matriz2 = matriz_D;
			parametro_Multiplica[i].matriz3 = matriz_E;
		
			pthread_create(&threads_Id[i], NULL, thread_Multiplica, (void *) &parametro_Multiplica[i]);
		}

		for(int i = 0; i < num_Thread; i++)
		{
			pthread_join(threads_Id[i], NULL);
		}
		
		clock_gettime(CLOCK_MONOTONIC, &tempo_Fim);

		double tempo_soma = calcula_tempo(tempo_Inicio, tempo_Fim);

		return tempo_soma;
	}
}

double reduzir_Gravar(FILE *nome_Arquivo, int *matriz_E, int n, int num_Thread)
{
	int soma = 0;
	struct timespec tempo_Inicio = {0, 0};
	struct timespec tempo_Fim = {0, 0};

	if (num_Thread == 1)
	{
		Parametros_Thread *parametro_Gravar = (Parametros_Thread *)malloc(sizeof(Parametros_Thread));
		
		parametro_Gravar[0].ordem = n;
		parametro_Gravar[0].matriz1 = matriz_E;
		parametro_Gravar[0].arquivo = nome_Arquivo;
		
		Parametros_Thread *parametro_Reduzir = (Parametros_Thread *)malloc(sizeof(Parametros_Thread));
		parametro_Reduzir[0].inicio = 0;
		parametro_Reduzir[0].fim = n;
		parametro_Reduzir[0].ordem = n;
		parametro_Reduzir[0].numero_thread = num_Thread;
		parametro_Reduzir[0].matriz1 = matriz_E;

		thread_Gravar((void *)&parametro_Gravar[0]);
		
		clock_gettime(CLOCK_MONOTONIC, &tempo_Inicio);
		
		soma = *(int*) thread_reduzir((void *)&parametro_Reduzir[0]);
		
		clock_gettime(CLOCK_MONOTONIC, &tempo_Fim);
		
		printf("Redução: %d \n", soma);

		double tempo_soma = calcula_tempo(tempo_Inicio, tempo_Fim);

		return tempo_soma;
	}

	else 
	{
		pthread_t *threads_Id = (pthread_t *)malloc(sizeof(pthread_t) * (num_Thread + 1));
		int* somaParcial = NULL;
		int num_elementos = (n * n) / num_Thread;
		if(threads_Id == NULL)
		{
			printf("Erro na alocação de threads");
			exit(1);
		}
		
		Parametros_Thread *parametro_Gravar_Reduzir = (Parametros_Thread *)malloc(sizeof(Parametros_Thread) * (num_Thread + 1));

		parametro_Gravar_Reduzir[num_Thread].ordem = n;
		parametro_Gravar_Reduzir[num_Thread].matriz1 = matriz_E;
		parametro_Gravar_Reduzir[num_Thread].arquivo = nome_Arquivo;

		pthread_create(&threads_Id[num_Thread], NULL, thread_Gravar, (void *)&parametro_Gravar_Reduzir[num_Thread]);
		
		clock_gettime(CLOCK_MONOTONIC, &tempo_Inicio);

		for(int i = 0; i < (num_Thread); i++)
		{	
			parametro_Gravar_Reduzir[i].inicio = num_elementos * i;
			parametro_Gravar_Reduzir[i].fim = num_elementos * (i + 1);
			parametro_Gravar_Reduzir[i].ordem = n;
			parametro_Gravar_Reduzir[i].numero_thread = num_Thread;
			parametro_Gravar_Reduzir[i].matriz1 = matriz_E;
			parametro_Gravar_Reduzir[i].arquivo = nome_Arquivo;
			parametro_Gravar_Reduzir[i].numero_thread = num_Thread;

			pthread_create(&threads_Id[i], NULL, thread_reduzir, (void *)&parametro_Gravar_Reduzir[i]);
		}
		// join reducao
		for(int i = 0; i < (num_Thread); i++)
		{
			pthread_join(threads_Id[i], (void**) &somaParcial);
			soma += *(int*) somaParcial;
		}
		// join thread gravacao
		pthread_join(threads_Id[num_Thread], NULL);
		
		clock_gettime(CLOCK_MONOTONIC, &tempo_Fim);

		printf("Redução: %d \n", soma);

		double tempo_soma = calcula_tempo(tempo_Inicio, tempo_Fim);
		return tempo_soma;
	}
}