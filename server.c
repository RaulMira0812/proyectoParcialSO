/*
    PROYECTO PARCIAL SISTEMAS OPERATIVOS
    Integrantes: Lozano Estefania, Lasso Henry & Mira Raul
    Fecha Entrega: 16 de Diciembre del 2015
    Descripción: Servidor IRC (Internet Relay Chat)
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>


//#include "libreria_h/server_commands.h"
#define DELIMS " \t\r\n"
#define MAX_LENGTH 1024
#define SIZE_STR 100
#define PUERTO 2016
#define CONEXIONES_MAXIMAS 256

#include "libreria_h/server_mutexes.h"
#include "libreria_h/usuario.h"
#include "libreria_h/canal.h"
#include "libreria_h/server_comandos.h"
#include "libreria_h/constantes.h"



//Al aceptar un cliente, debemos comenzar un nuevo hilo para continuar aceptando clientes. 
//Necesitamos esta estructura para la informacion principal del cliente
typedef struct usuario_info_s {
	pthread_t hilo; 
	int socket; 
} usuario_info; 

int aceptar_conexion(int socket); 
void* leer_comandos(usuario_info* ui);
int crear_conexion(int puerto);
//char* parse_connect_cmd(char* cmd, bool* quit); //Returns the nick for connect cmd
//void wait_for_connect_cmd(usuario_info* client_socket); //Un nuevo hilo es creado para la conexion del socket
//void catch_broken_pipe(int signum); 

int main(int argc, char** argv) {

	int error;  
	int socket_server;
	usuario_info *usuario;	
	int cliente;

	char msje[SIZE_STR];

	socket_server=crear_conexion(PUERTO);
	if(socket_server == -1){
		fprintf(stderr, "No se puede abrir el socket en el servidor\n");
		exit (-1);
	}else{
		fprintf(stderr, "\nServidor Iniciado");
		fprintf(stderr, "-------------------\n");
	}

	//register function to deal with a broken pipe, e.g. when client exits program and server tries to write to stream
	//signal(SIGPIPE, catch_broken_pipe); 

	//Init mutexes used 
	inicializar_mutexes(); 

	//Init lista of usuarios to empty lista
	inicializar_lista_usuarios(); 

	//Init lista of chat rooms to empty lista, instantiate lobby chat room
	iniciar_canales(); 
	
	while((cliente=aceptar_conexion(socket_server))){
		if(cliente==-1){
			printf("Error: No se puede abrir el socket del cliente\n");
			exit(-1);
		}else{
			usuario = (usuario_info*)malloc(sizeof(usuario_info));//memoria
			usuario->socket=cliente;//socket
			int a=pthread_create(&(usuario->hilo),NULL, (void*)&leer_comandos, (usuario_info*)(usuario));//hilo
			printf("que paso?:%d\n",a);
		}
		printf("d");		
	}

	return EXIT_SUCCESS;
}

int aceptar_conexion(int descriptor){
	struct sockaddr cliente;
	socklen_t len_cliente;
	int hijo;

	len_cliente=sizeof(cliente);
	memset(&cliente,0,sizeof(struct sockaddr));//inicializar en cero la variable

	hijo=accept(descriptor,&cliente,&len_cliente);
	if(hijo==-1)
		return -1;

	return hijo;
}

int crear_conexion(int puerto){
	struct sockaddr_in direccion;
	struct sockaddr cliente;
	socklen_t len_cliente;
	//struct servent *puert;
	int descriptor;

	//Se abre el socket
	descriptor=socket(AF_INET,SOCK_STREAM,0);
	if(descriptor == -1)
		return -1;

	direccion.sin_family=AF_INET;
	direccion.sin_port=htons(puerto);
	direccion.sin_addr.s_addr=INADDR_ANY;
	if(bind(descriptor,(struct sockaddr *)&direccion,sizeof(direccion))	== -1){
		close(descriptor);
		return -1;
	}

	if(listen(descriptor,CONEXIONES_MAXIMAS)==-1){
		close(descriptor);
		return -1;
	}

	return descriptor;
}

void* leer_comandos(usuario_info* ui){
	char *cmd;
	char line[MAX_LENGTH];
	char msje[SIZE_STR];

	int usuario_thread=ui->hilo;
	int new_socket=ui->socket;

	char nick[20];
	sprintf(nick,"usuario_%d",new_socket);
	if(!existe_usuario(nick))
		inicializar_usuario(nick,new_socket,usuario_thread);

	memset(msje,'\0',SIZE_STR);
	strcpy(msje,"Bienvenido a APOLO-IRC ");
	strcpy(msje,nick);
	strcpy(msje,"\n");
	send(ui->socket,msje,strlen(nick),0);

	

	while(1){

		//memset(msje,'\0',SIZE_STR);
		//recv(new_socket,msje,30,0);
		//printf("%s\n",msje);

/*
		if(!fgets(line,MAX_LENGTH,stdin))
			printf("1");
			break;
		if((cmd= strtok(line,DELIMS))){
			printf("2");
			errno=0;
			if(strcmp(cmd,"iniciar")==0){
				memset(msje,'\0',SIZE_STR);
				strcpy(msje,"SERVER INICIA");
				send(ui->socket,msje,strlen(msje),0);
			}else if(strcmp(cmd,"salir")==0){
				raise(SIGINT);
			}
			printf("3");
			if(errno) perror("Error: el comando ingresado tuvo un error");
		}*/
	}
	pthread_exit(NULL);
}
