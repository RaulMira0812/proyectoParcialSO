#ifndef CHAT_ROOM_H
#define CHAT_ROOM_H

#include "users.h"
#include "list.h"
#include "commands.h"
#include "constants.h"
#include "misc.h"
#include "global_server_cmds.h"

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


void iniciar_canales(); //Allocate LOBBY chat room and chat_room_list (see chat_room.c)

canal* nuevo_canal(char* nombre); //Crea un canal y comienza el candado

void liberar_canal(canal* canal); //libera un canal y finaliza el candado

void add_user_to_lobby(usuario* u); //Add user to the special LOBBY room


void agrega_usuario(usuario* u, char* c); //Agrega usuario u al canal c si existe, si no, primero lo crea

//Receive-act on commands from client
void chat_room_loop(usuario* u);

void ejecuta_comando(char* msg, usuario* u); //Ejecuta un comando que recibe del chat de usuario

void broadcast(char* msg, canal* c, char* emisor); //envia un broadcast todos los usuarios en el canal

//Send list of all users in a room to user u. +LIST command
void list_users_in_room(user* u); 

//Send list of all rooms on server to user u. +ROOMS command
void list_all_rooms(user* u); 

//Send list of all uesrs on server to user u. +LIST_ALL command.
void list_server_users(user* u); 

//Send a message to a single user. Prepends +MSG command
void print_to_user(user* u, char* msg); 

//Sends string msg to user. For sending commands other than +MSG
void print_raw_to_user(user* u, char* msg); 

//Returns chat room if it's in list, otherwise returns NULL
chat_room* chat_room_exists(char* chat_room_name); 

#endif
