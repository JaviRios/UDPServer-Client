#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

//Declaration
void sigintHandler(int sig_num);
void initResumes();
void procesarDatos(int received[]);
void pintarDatos();
void parsearAcelerometro(int received[], int iteracion, int position);
void parsearColor(int received[], int i, int position);
int minimo(int data[]);
int maximo(int data[]);
float media(int data[]);
//float desviacion(int data[]);

const int TAMANO_MEDICION = 15;
const int NUMERO_MUESTRAS = 10;

int valoresEjeX[10];
int valoresEjeY[10];
int valoresEjeZ[10];
int valoresCData[10];
int valoresColorRed[10];
int valoresColorGreen[10];
int valoresColorBlue[10];

int IS_ACTIVE = 1;

int  fd_socket = -1;
struct sockaddr_in server, from;
socklen_t address_size;


int main(int argc, char **argv){

	//char buffer[1024];//this is where the info received from client will be save
	int  rec = -1, send = -1;
	int port = atoi(argv[1]);//setting port. converting to int

	const char HOLA_SERVER[] = "HELLO SERVER";
	const char HOLA_RASPI[] = "HELLO RASPI";
	const char ACK[] = "ACK";
	int firstMessageReceived = 0;

	if(argc != 2){
		printf("USAGE: %s <port>\n", argv[0]);
		exit(0);
	}

	signal(SIGINT, sigintHandler);

	fd_socket = socket(AF_INET, SOCK_DGRAM, 0);
	//Create a new socket of type SOCK_DGRAM (udp socket) in domain AF_INET (i'm giving the ip address for the computer to understand it), using protocol 0 (chosen automatically)

	if(fd_socket < 0){
			printf("ERROR: Opening socket\n");
			exit(0);
		}

	memset(&server, '\0', sizeof(server));//initialyzing structure

	//setting up our server
	server.sin_family = AF_INET;//type of address
	server.sin_port = htons(port);//setting the port. high so it is not used
	server.sin_addr.s_addr = INADDR_ANY;// ADDRESS
	printf("\033[1;33m <<< SE HA CREADO EL SOCKET EXITOSAMENTE >>> \e[0m \n");

	int connection_status = bind(fd_socket, (struct sockaddr *) &server, sizeof(server));//assign address to server
	if(connection_status < 0){
		printf("\033[1;91m ERROR: no se ha podido crear el socket :( \e[0m \n");
		exit(0);
	}
	//printf("\033[1;31m Hola mundo!\e[0m \n");
	printf("\033[1;33m ¡¡EL SERVIDOR ESTÁ FUNCIONANDO \e[0m \n");
	printf("\033[1;33m  Esperando para recibir mensajes..\e[0m \n");
    address_size=sizeof(from);

	while(IS_ACTIVE){
		char buffer[256];
	    //data received WAIT FOR MSG
		rec =  recvfrom(fd_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &address_size);
		//int bufferReceived[rec];

		if(rec >= 0){
			printf("TAMAÑO RECIBIDO -> %i\n", rec);
			//strncpy(bufferReceived, buffer, rec);
		}
		if(firstMessageReceived == 0){
			if(strcmp(buffer, HOLA_SERVER) == 0){
				printf("Primer mensaje recibido 'HOLA_SERVER'... OK\n");
				send = sendto(fd_socket, HOLA_RASPI, sizeof(HOLA_RASPI), 0, (struct sockaddr *)&from, address_size);
				firstMessageReceived = 1;
			}else{
				printf("MENSAJE ERRONEO, CIERRO EL CHIRINGUITO\n");
				send = sendto(fd_socket, "WRONG MESSAGE",20, 0, (struct sockaddr *)&from, address_size);
				close(fd_socket);
				return 0;
			}
		}else{
			printf("Datos recibidos...\n");
			int received[rec];
			printf("[");
			for(int i=0; i<rec; i++){
				received[i] = (int)buffer[i] & (0x000000ff);
				// Descomentar para ver array que envia RASPI
				printf("%x,", received[i]);
			}
			printf("]\n");
			// Se procesan los datos y se envia ACK

			initResumes();
			procesarDatos(received);
			pintarDatos();

			// FIN Se procesan los datos y se envia ACK
			send = sendto(fd_socket, ACK, sizeof(ACK), 0, (struct sockaddr *)&from, address_size);
			printf("------------------\n");
		}
	}
	printf("SERVIDOR ACABADO \n");

	close(fd_socket);
	return 0;
}

/*
medidas[0]= ejex1
medidas[1]= ejex2
medidas[2]= ejey1
medidas[3]= ejey2
medidas[4]= ejez1
medidas[5]= ejez2
medidas[6]= cdata1
medidas[7]= cdata2
medidas[8]= redcolor1
medidas[9]= redcolor2
medidas[10]= greencolor1
medidas[11]= greencolor2
medidas[12]= bluecolor1
medidas[13]= bluecolor2
medidas[14]= RELLENAR CON 0
 */

void initResumes(){
	memset(valoresEjeX, '\0', NUMERO_MUESTRAS);
	memset(valoresEjeY, '\0', NUMERO_MUESTRAS);
	memset(valoresEjeZ, '\0', NUMERO_MUESTRAS);
	memset(valoresCData, '\0', NUMERO_MUESTRAS);
	memset(valoresColorRed, '\0', NUMERO_MUESTRAS);
	memset(valoresColorGreen, '\0', NUMERO_MUESTRAS);
	memset(valoresColorBlue, '\0', NUMERO_MUESTRAS);
}

void procesarDatos(int received[]){
	int position = 0;
	for(int i=0; i<sizeof(received); i=i+TAMANO_MEDICION){
		parsearAcelerometro(received, i, position);
		parsearColor(received, i, position);
		position++;
	}
	//lumninance = (0.299) * (resumeRedColor) + (0.587) * (resumeGreenColor) + (0.114) * (resumeBlueColor);///Formula de audiovisuales
}

void pintarDatos(){
	float mediaColorRed = media(valoresColorRed);
	float mediaColorGreen = media(valoresColorGreen);
	float mediaColorBlue = media(valoresColorBlue);
	float mediaColorCData = media(valoresCData);
	float mediaEjeX = media(valoresEjeX);
	float mediaEjeY = media(valoresEjeY);
	float mediaEjeZ = media(valoresEjeZ);
	printf("Media color: R: %2f, G:%2f, B:%2f, IR:%2f\n",mediaColorRed, mediaColorGreen, mediaColorBlue, mediaColorCData);
	printf("Media acele: X: %2f, Y:%2f, Z:%2f\n", mediaEjeX, mediaEjeY, mediaEjeZ);

	int maxColorRed = maximo(valoresColorRed);
	int maxColorGreen = maximo(valoresColorGreen);
	int maxColorBlue = maximo(valoresColorBlue);
	int maxColorCData = maximo(valoresCData);
	int maxEjeX = maximo(valoresEjeX);
	int maxEjeY = maximo(valoresEjeY);
	int maxEjeZ = maximo(valoresEjeZ);
	printf("Maximo color: R: %i, G:%i, B:%i, IR:%i\n",maxColorRed, maxColorGreen, maxColorBlue, maxColorCData);
	printf("Maximo acele: X: %i, Y:%i, Z:%i\n", maxEjeX, maxEjeY, maxEjeZ);

	int minColorRed = minimo(valoresColorRed);
	int minColorGreen = minimo(valoresColorGreen);
	int minColorBlue = minimo(valoresColorBlue);
	int minColorCData = minimo(valoresCData);
	int minEjeX = minimo(valoresEjeX);
	int minEjeY = minimo(valoresEjeY);
	int minEjeZ = minimo(valoresEjeZ);
	printf("Minimo color: R: %i, G:%i, B:%i, IR:%i\n",minColorRed, minColorGreen, minColorBlue, minColorCData);
	printf("Minimo acele: X: %i, Y:%i, Z:%i\n", minEjeX, minEjeY, minEjeZ);

	/*
	int desvColorRed = desviacion(valoresColorRed);
	int desvColorGreen = desviacion(valoresColorGreen);
	int desvColorBlue = desviacion(valoresColorBlue);
	int desvColorCData = desviacion(valoresCData);
	int desvEjeX = desviacion(valoresEjeX);
	int desvEjeY = desviacion(valoresEjeY);
	int desvEjeZ = desviacion(valoresEjeZ);
	printf("Desviacion color: R: %2f, G:%2f, B:%2f, IR:%2f\n",desvColorRed, desvColorGreen, desvColorBlue, desvColorCData);
	printf("Desviacion acele: X: %2f, Y:%2f, Z:%2f\n", desvEjeX, desvEjeY, desvEjeZ);
	*/
}

float media(int data[]){
	float suma = 0;
	for(int i=0; i<sizeof(data); i++){
		suma = suma + data[i];
	}
	return suma / sizeof(data);
}

int maximo(int data[]){
	float maximo = 0;
	for(int i=0; i<sizeof(data); i++){
		if(data[i] > maximo){
			maximo = data[i];
		}
	}
	return maximo;
}

int minimo(int data[]){
	float minimo = 0;
	for(int i=0; i<sizeof(data); i++){
		if(data[i] < minimo){
			minimo = data[i];
		}
	}
	return minimo;
}
/*
float desviacion(int data[]){
	float med = media(data);
	float valor = 0;
	for(int i=0; i<sizeof(data); i++){
		valor += pow(data[i]-med, 2);
	}
	return sqrt(valor/sizeof(data));
}
*/

void parsearAcelerometro(int received[], int i, int position){
	int16_t ejeX = received[1+i];
	ejeX = ejeX << 8;
	ejeX |= received[0+i];
	ejeX = ejeX >>2;
	valoresEjeX[position] = ejeX;


	int16_t ejeY = received[3+i];
	ejeY = ejeY << 8;
	ejeY |= received[2+i];
	ejeY = ejeY >>2;
	valoresEjeY[position] = ejeY;

	int16_t ejeZ = received[5+i];
	ejeZ = ejeZ << 8;
	ejeZ |= received[4+i];
	ejeZ = ejeZ >>2;
	valoresEjeZ[position] = ejeZ;
}

void parsearColor(int received[], int i, int position){
	int cdata = ((received[7+i] * 256) + received[6+i]);
	int redColor = ((received[9+i] * 256) + received[8+i]);
	int greenColor = ((received[11+i] * 256) + received[10+i]);
	int blueColor = ((received[13+i] * 256) + received[12+i]);

	valoresColorRed[position] = redColor;
	valoresColorGreen[position] = greenColor;
	valoresColorBlue[position] = blueColor;
	valoresCData[position] = cdata;
}

/* Signal Handler for SIGINT */
void sigintHandler(int sig_num)
{
    // Reset handler to catch SIGINT next time.
    printf("\n SigINT captured... Closing Server\n");
    sendto(fd_socket, "END", 3, 0, (struct sockaddr *)&from, address_size);
    IS_ACTIVE = 0;
    fflush(stdout);

    exit(1);
}
