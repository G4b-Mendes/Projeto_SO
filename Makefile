compilar:
	gcc -std=c11 -Wall -Wextra -pthread funcoes.c main.c -o programa.exe

execucao_1:
	./programa.exe 1 100 arqA100.dat arqB100.dat arqC100.dat arqD100.dat arqE100.dat
	
execucao_2:
	./programa.exe 2 100 arqA100.dat arqB100.dat arqC100.dat arqD100.dat arqE100.dat

execucao_3:
	./programa.exe 4 100 arqA100.dat arqB100.dat arqC100.dat arqD100.dat arqE100.dat

execucao_4:
	./programa.exe 1 1000  arqA1000.dat arqB1000.dat arqC1000.dat arqD1000.dat arqE1000.dat

execucao_5:
	./programa.exe 2 1000  arqA1000.dat arqB1000.dat arqC1000.dat arqD1000.dat arqE1000.dat

execucao_6:
	./programa.exe 4 1000  arqA1000.dat arqB1000.dat arqC1000.dat arqD1000.dat arqE1000.dat

clean:
	rm programa.exe
