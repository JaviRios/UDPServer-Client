#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "gpio_blink.h"
#include "tcs34725_ColorSensor.h"
#include "i2c_bus.h"
#include "acelerometro.h"

extern char data[8];
extern char buf[6];

void sigintHandler(int sig_num);

int main(int argc, char **argv){
	int sock, n;
	socklen_t length;
	struct sockaddr_in server, from;
	struct hostent *hp;

	const int TAMANO_MEDICION = 15;
	const int TIEMPO_ESPERA_MEDICION = 1;
	const int TIEMPO_ENVIO_DATOS = 10;
	const char HOLA_SERVER[] = "HELLO SERVER";
	const char HOLA_RASPI[] = "HELLO RASPI";
	const char DATO_EQUIVOCADO[]= "WRONG MESSAGE";
	const char ACK[] = "ACK";
	const char END[] = "END";
	char buffer[256];
	int iteracion = 1;
	char bufferDatos[(TAMANO_MEDICION * TIEMPO_ENVIO_DATOS)];
	colorsensor sensor = config();
	colorsensor aceler = MMA8451_Init();

	if(argc != 3){
		printf("Usage: server port\n");
		exit(1);
	}
	sock = socket(AF_INET,SOCK_DGRAM,0);//create socket(server/client)
	if(sock < 0){
		printf("ERROR: socket error\n");
		exit(0);
	}

	server.sin_family = AF_INET;
	hp=gethostbyname(argv[1]);

	if(hp == 0){
		printf("ERROR: unknown host\n");
		exit(0);
	}

	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2]));
	length=sizeof(struct sockaddr_in);

	////Comunicacion establecimiento de conexion/////
	n = sendto(sock, HOLA_SERVER, sizeof(HOLA_SERVER), 0, (struct sockaddr *)&server, length); //envía HELLO_SERVER
	if(n < 0){
		printf("ERROR: NO SE HA PODIDO ENVIAR EL MENSAJE\n");
		exit(0);
	}else{
		n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &length);

		 if(strcmp(buffer, HOLA_RASPI) == 0){
			 printf("Primer mensaje recibido 'HOLA_RASPI'... OK\n");
		 }else{
			 printf("Mensaje equivocado, cierro conexion.\n");
			 close(sock);
			 exit(0);
		 }
	}


	printf("Entrando en modo envio de datos\n");
	while (1){ // Modo datos
			 if(iteracion == TIEMPO_ENVIO_DATOS){ // Si han pasado 10 sleep(1), enviamos datos
				 iteracion = 1;
				 /*
				 for(int i=0; i<sizeof(bufferDatos); i++){
					printf("----> Dato %i: %x\n", i, bufferDatos[i]);
				 } */

				 printf("Enviamos las muestras completas\n");
				 n = sendto(sock, bufferDatos, sizeof(bufferDatos), 0, (struct sockaddr *)&server, length);
				 if(n>0){
					 printf(" - Datos enviados\n");
				 }
				 n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &length);
				 if(strcmp(buffer, ACK) == 0){
					 printf(" - ACK Recibido\n\n\n\n\n");
				 }else if(strcmp(buffer, END) == 0){
					 // FIN DEL PROGRAMA
					 printf("SE HA RECIBIDO: ");
					 printf("%s \n", buffer);
					 printf("----PROGRAMA FINALIZADO-----");
					 close(sock);
					 exit(0);
				 }
			 }else{ // Guardamos los datos capturados para enviarlos luego
				 printf("Hacemos la medicion numero: %i\n\n\n", iteracion);
				 reading_data(&sensor);
				 MMA8451_Read_All(&aceler);
				 for(int i=0; i<strlen(buf); i++){
					 bufferDatos[((iteracion-1)*TAMANO_MEDICION)+i] = buf[i];
				 }
				 for(int i=0; i<strlen(data); i++){
					 bufferDatos[((iteracion-1)*TAMANO_MEDICION)+strlen(buf)+i] = data[i];
				 }
				 bufferDatos[((iteracion-1)*TAMANO_MEDICION)+(TAMANO_MEDICION-1)] = '0';
			 }
			 iteracion++;

			 sleep(TIEMPO_ESPERA_MEDICION);
	}
	close(sock);
	return 0;
}

void sigintHandler(int sig_num)
{
    // Reset handler to catch SIGINT next time.
    printf("\n SigINT captured... Closing Server\n");
    fflush(stdout);
    exit(1);
}
