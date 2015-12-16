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
#define PUERTO 2020
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
			//printf("que paso?:%d\n",a);
		}
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

	//char *cmd;
	//char line[MAX_LENGTH];
	//char msje[SIZE_STR];
	
	char partial_cmd[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];

	int usuario_thread=ui->hilo;
	int new_socket=ui->socket;
	bool *err=false;

	bool quit=false;

	char nick[20];
	sprintf(nick,"usuario_%d",new_socket);
	if(!existe_usuario(nick))
		inicializar_usuario(nick,new_socket,usuario_thread);

	partial_cmd[0]='\0';

	while(1){
		char* next_cmd=NULL;
		char* tmp_cmd=NULL;

		printf("1)%s\n",next_cmd);

		quit=false;

		if((recv(new_socket,buffer,BUFFER_SIZE,0))<0){
			return NULL;
		}
		
		printf("4)%s\n",partial_cmd);
		int len = strlen(buffer);
		printf("1.01)%s\n",buffer);
		//strncat(partial_cmd,buffer,len);

		next_cmd = (char*)malloc(sizeof(char)*(strlen(partial_cmd)+1));
		tmp_cmd = (char*)malloc(sizeof(char)*(strlen(partial_cmd)+1));

		strcpy(next_cmd,buffer);
		printf("1.1)%s\n",next_cmd);

		strcpy(tmp_cmd,buffer);
		printf("1.2)%s\n",tmp_cmd);

		partial_cmd[0]='\0';
		
		usuario* usuario_actual;
		nodo* tmp = usuarios_todos->primer_nodo; 
		while (tmp != NULL) {
			usuario* tmp_u = (usuario*)tmp->valor;
			if(tmp_u->nickname == nick)
				usuario_actual=tmp_u;
			tmp = tmp->nodo_siguiente; 
		}
		if (strcmp(strtok(tmp_cmd, " "), "QUIT")==0) { 
			printf("Comando +QUIT. Terminando la conexion\n");
			close(new_socket);
			free(ui);
			free(next_cmd);
			free(tmp_cmd);
			free(buffer);
			int j;
			for(j=0;j<BUFFER_SIZE;j++){
				buffer[j]= '\0';
			} 
			pthread_exit(NULL);
		}else{
			ejecuta_comando(next_cmd,usuario_actual);
			free(tmp_cmd);
			int j;
			for(j=0;j<BUFFER_SIZE;j++){
				buffer[j]= '\0';
			} 
		}
		
		
	}
	pthread_exit(NULL);
}
