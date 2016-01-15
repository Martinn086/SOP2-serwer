#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


int main(void) {
	puts("Serwer uruchomiony...");

  char * fifo = "/tmp/demo6_fifo";


	//Tworzenie lacza serera
	int status = mkfifo(fifo, 0666);
	if (status == -1) {
		perror("FIFO creating error");
    exit(1);
	}


	//Otworz lacze serwera i nasluchuj odpowiedzi
	int fdServer = open(fifo, O_RDONLY);
	if (fdServer == -1) {
		perror("FIFO opening error");
		exit(1);
	}
	puts("Otwarto lacze serwera...");


	//Wczytaj komende z lacza
	char stringBuffer[256];			//Bufor przechowujacy odpowiedz z lacza
	memset(stringBuffer, 0, 256);   //Wypelnij bufor zerami, zeby wykryc koniec stringa
	int res; //Status odpowiedzi
	char Len; //Dlugosc odpowiedzi

	res = read(fdServer, &Len, 1);
	if (res == 0) {
		//Jezeli dlugosc odpowiedzi jest rowna zero, opusc program
		exit(1);
	}
	read(fdServer, stringBuffer, Len);					//Przeczytaj string z lacza
	printf("\nSerwer otrzymal komende: %s\n\n", stringBuffer);


	//Wczytaj nazwe FIFO klienta z lacza
	char clientFifo[256];			//Bufor przechowujacy nazwe FIFO z lacza
	memset(clientFifo, 0, 256);   //Wypelnij bufor zerami, zeby wykryc koniec stringa
	int resId; //Status odpowiedzi
	char LenId; //Dlugosc odpowiedz

	resId = read(fdServer, &LenId, 1);
	if (resId == 0) {
		//Jezeli dlugosc kolejki jest rowna zero, opusc program
		exit(1);
	}
	printf("Hej, tyle odstalem... %d\n", LenId);
	read(fdServer, clientFifo, LenId+7);					//Przeczytaj string z lacza
	printf("\nSerwer otrzymal status kolejki: %s\n\n", clientFifo);
	//TODO przekazywanie id FIFO klienta nie działa



	sleep(1); //Daj czas klientowi na utworzenie prywatnego lacza


	//Otworz lacze klienta w trybie do zapisu
	int fdClient = open("/tmp/client_fifo", O_WRONLY);
	if (fdClient ==  -1) {
		perror("FIFO opening error");
		exit(1);
	}


	// close(fdClient[0]);
	dup2(fdClient,1);


	//Wykonaj polecenie przeslane laczem w podprocesie
	int process = fork();
	switch (process) {

		case 0:
			execlp(stringBuffer,stringBuffer,NULL);
			printf("Nie wykonano stringa\n");
			exit(1);
			break;

		default:
			wait(&process);

	};


	//Zamknij lacze serwera
	close(fdServer);


	//Zamknij polaczenie z laczem klienta
	close(fdClient);


	return EXIT_SUCCESS;
}
