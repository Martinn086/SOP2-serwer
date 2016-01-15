#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


int main(void) {

	char komenda[30];

	char * clientFifo = "/tmp/client_fifo";


	//Pobierz komende od uzytkownika
	printf("Podaj komende do przekazania do serwerowi: ");
	fgets(komenda,30,stdin);


	//Otworz lacze serwera w trybie do zapisu
	int fdServer = open("/tmp/demo6_fifo", O_WRONLY);
	if (fdServer ==  -1) {
		perror("FIFO opening error");
		exit(1);
	}


	//Poslij komende na lacze serwera

	//Z jakiegos powodu musze zmniejszac L o 1, nie wiem dlaczego....
	char L = (char)(strlen(komenda)-1);
	write(fdServer, &L, 1); //Wyslij rozmiar stringa
	write(fdServer, komenda, strlen(komenda) ); 	//A pozniej wyslij stringa


	char LId = (char)(strlen(clientFifo)-1);
	write(fdServer, &LId, 1); //Wyslij rozmiar stringa
	write(fdServer, clientFifo, strlen(clientFifo) ); 	//A pozniej wyslij stringa
	//


	//Zamknij polaczenie z laczem serwera
	close(fdServer);


	//Utworz prywatne lacze klienta
	int status = mkfifo(clientFifo, 0666);
	if (status == -1) {
		perror("FIFO creating error");
    exit(1);
	}
	puts("Utworzono fifo");


	//Oworz prywatne lacze klienta i nasluchuj odpowiedzi
	int fdClient = open(clientFifo, O_RDONLY);
	if (fdClient == -1) {
		perror("FIFO opening error");
		exit(1);
	}
	puts("Otwarto lacze klienta...\n");


	// close(fdClient[1]);


	//Wczytaj odpowiedz z lacza
	char stringBuffer[256];			//Bufor przechowujacy odpowiedz z lacza
	memset(stringBuffer, 0, 256);   //Wypelnij bufor zerami, zeby wykryc koniec stringa

	read(fdServer, stringBuffer, 255);
	printf("%s\n",stringBuffer);


	close(fdClient);
	printf("\nZamknieto lacze klienta...\n");

	return EXIT_SUCCESS;
}
