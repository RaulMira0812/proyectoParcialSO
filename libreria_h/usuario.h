#ifndef USUARIO_H
#define USUARIO_H

#include "canal.h"
#include "lista.h"
#include "server_comandos.h"
#include "server_mutexes.h"
#include "constantes.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>


typedef struct chat_room_struct chat_room;

typedef struct canal canal; //El nodo de cada lista

typedef struct usuario {
	char* nickname;
	char* nombre_real;
	char* hostname;
	char* server_name
	pthread_t hilo_usuario; //cada usuario tiene asignado un hilo unico
	int socket_usuario;
	canal* canal_actual;
	pthread_mutex_t usuario_sock_mutex;
}usuario;

//typedef struct usuario_struct usuario;

lista* usuarios_todos; //lista de todos los usuarios.

void inicializar_lista_usuarios(); //Crear una lista vacia de usuarios 

void inicializar_usuario(char* nickname, int sock, pthread_t hilo_usuario); //Crear un nuevo usuario, lo agrego a la lista de todos los usuarios y al canal principal

void set_prompt_usuario(usuario* u, char* nuevo_prompt); //+PROMPT comando

void liberar_usuario(usuario* u); //Libero los datos pertenecientes a un usuario

void remover_usuario(usuario* u); //remover el usuario de la lista de todos los usuarios y del canal actual.

bool existe_usuario(char* nickname); //Verificar si el usuario se encuentra en la lista de todos los usuarios

#endif
