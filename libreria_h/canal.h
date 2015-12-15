#ifndef CHAT_ROOM_H
#define CHAT_ROOM_H

#include "usuario.h"
#include "lista.h"
#include "server_comandos.h"
#include "server_mutexes.h"
#include "constantes.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/socket.h>

//esta estructura:
//Administra la lista de canales.
//escucha y responde las peticiones del cliente

typedef struct usuario usuario;

typedef struct canal {
	list* usuarios; //lista de usuarios en este canal
	char* nombre_canal; //nombre del canal
	char* mensaje_actual; //último mensaje enviado al usuario
	pthread_mutex_t candado; //Para asegurar exclusion mutua
}canal;


void iniciar_canales(); //inicia la cola de espera y la lista de canales

canal* nuevo_canal(char* nombre); //Crea un canal y comienza el candado

void liberar_canal(canal* canal); //libera un canal y finaliza el candado

void agrega_usuario_a_espera(usuario* u); //Agrega un usuario a la cola de espera

void agrega_usuario_canal(usuario* u, char* c); //Agrega usuario u al canal c si existe, si no, primero lo crea

void canal_loop(usuario* u);//Recibe/ejecuta los comandos del cliente 

void ejecuta_comando(char* cmd, usuario* u); //Ejecuta un comando que recibe del chat de usuario

void broadcast(char* msg, canal* c, char* emisor); //envia un broadcast todos los usuarios en el canal

void lista_usuarios(user* u); //envia al usuario u la lista de todos los usuarios en el canal 

void lista_canales(user* u); //envia al usuario u la lista de todos los canales en el server 

void lista_usuarios_server(user* u); //envia al usuario u la lista de todos los usuarios conectados al server 

void mensaje_a_usuario(user* u, char* msg); //envia un mensaje a un usuario. 

void comando_a_usuario(user* u, char* msg); //envia otro tipo de comando al usuario.

canal* canal_existe(char* nombre_canal); //retorna el canal si existe, caso contrario retorna nulo

#endif
