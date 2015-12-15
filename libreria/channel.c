#include "../include/channel.h"

static const char* nombre_canal_espera = "LOBBY"; //Nombre del canal de espera
static list* canales = NULL; //lista de todos los canales
static canal* canal_espera = NULL; //canal de espera (lobby)

void iniciar_canales() {

	if (canales == NULL) {
		canales = new_list((void(*)(void*))liberar_canal); 
	}
	if (canal_espera == NULL) {
		canal_espera = nuevo_canal((char*)nombre_canal_espera); 
		agregar_nodos(canales, (void*)canal_espera); 
	}
}

canal* nuevo_canal(char* nombre) { //Crea un nuevo canal.

	canal* nuevo_canal = (canal*)malloc(sizeof(canal)); 
	nuevo_canal -> usuarios = nueva_lista((void(*)(void*))liberar_usuario); 
	nuevo_canal -> nombre_canal = nombre; 
	nuevo_canal -> mensaje_actual = NULL; 
	pthread_mutex_init(&(nuevo_canal->candado), NULL); 
	return nuevo_canal; 
}	

void liberar_canal(canal* c) {
	pthread_mutex_destroy(&(c->candado));
	free(c->nombre_canal); 
	free(c); 
}

void agrega_usuario_a_espera(user* u) { 

	char nuevo_prompt[MAX_PROMPT_SIZE]; 
	char command_buffer[BUFFER_SIZE]; 

	sprintf(nuevo_prompt, "%s $ %s>", u->nick, nombre_canal_espera); 
	set_prompt_usuario(u, nuevo_prompt); // envia el comando prompt al usuario para forzar cambiar el prompt

	agregar_nodo(canal_espera -> usuarios, (void*)u); 
	u->canal_actual = canal_espera;  // el usuario actualmente esta en el lobby

	comando_a_usuario(u, (char*)JOIN_SUCCESS_CMD); 
	sprintf(command_buffer, "+PRINT [Server]: Conectado como usuario '%s'.\n[Server]: Ingresando al lobby.\n \n", u->nick); 
	comando_a_usuario(u, command_buffer); 
	canal_loop(u); 
}

void canal_loop(user* u) {
	char fraccion_de_comando[BUFFER_SIZE];
	char* siguiente_comando = NULL; 	
	bool err = false;	

	fraccion_de_comando[0] = '\0'; 
	while (true) {
		if (siguiente_comando) {
			free(siguiente_comando); 
			siguiente_comando = NULL;}
		siguiente_comando = get_siguiente_comando(u->socket_usuario, fraccion_de_comando, &err); 

		if (err) {
printf("Error al leer el mensaje del cliente.\nCerrando el socket #%d al cliente '%s'\nError: %s\n", u->socket_usuario, u->nick, strerror(errno)); 
			remover_usuario(u); 
			if (siguiente_comando) {
				free(siguiente_comando); 
			}
			pthread_exit(NULL); 
		}	
		else if (siguiente_comando == NULL) { //Todavía no ha recibido el mensaje completo
			continue; 	
		}
		else {
			trim_str(siguiente_comando); 
			printf("Mensaje del cliente %s: %s\n", u->nick, siguiente_comando); 
			ejecuta_comando(siguiente_comando, u); 
		}
	}
}

void ejecuta_comando(char* cmd, usuario* u) {
	char* cmd_nombre = NULL;
	char* cmd_argumento = NULL;
	char* msg = NULL;
	
	cmd_nombre = strtok(cmd, " ");//Separo el comando enviado por el usuario, obteniendo solo el nombre del comando

	if (strcmp(cmd_nombre, "MSG") == 0) { 
		msg = (char*)(&cmd[strlen("MSG")+1]); //Obtengo el contenido del mensaje
		trim_str(msg); 
		broadcast(msg, u->canal_actual, u->nick);
	}
	else if (strcmp(cmd_nombre, "PRIVMSG")==0) { //+JOIN
		receptor = (char*)(&cmd[strlen("PRIVMSG")+1]);
		msg = (char*)(&cmd[strlen("PRIVMSG")+1]);
		cmd_argumento = strtok(NULL, " "); //Room name is the second token
		agrega_usuario_canal(u, cmd_argumento);
	}

	else if (strcmp(cmd_nombre, JOIN_CMD)==0) { //+JOIN
		cmd_argumento = strtok(NULL, " "); //Room name is the second token
		agrega_usuario_canal(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, LIST_CMD) == 0) {
		lista_usuarios(u);
	}
	else if (strcmp(cmd_nombre, LIST_ALL_CMD) == 0) {
		lista_usuarios_server(u);
	}
	else if (strcmp(cmd_nombre, ROOMS_CMD) == 0) {
		lista_canales(u);
	}
	else {
		printf("Received bad command from user '%s', sending fail msg\n", u->nick);
		comando_a_usuario(u, (char*)FAIL_CMD);
	}
}

void agrega_usuario_canal(user* u, char* nombre_canal) { //Agrega el usuario u al canal si existe, si no primero crea el canal
	if (nombre_canal == NULL || strlen(nombre_canal) <= 0) {
		printf("Error: Se intento conectar a un canal inexistente.\n");
		return;
	}
	if (strcasecmp(nombre_canal, u->canal_actual->nombre_canal) == 0) {
		mensaje_a_usuario(u, "[Server]: Usted ya se encuentra conectado a este canal!");
		return;
	}
	if (!valid_charset(nombre_canal)) {
		mensaje_a_usuario(u, "[Server]: Nombre del canal invalido!\n"
		"[Server]: debe ser alfanumerico o  '_', '-', '*', '&'!\n ");
		return;
	}
	
	canal* canal_a_entrar;
	char* nombre_nuevo_canal; 
	char msg_buffer[BUFFER_SIZE];
	char new_prompt[MAX_PROMPT_SIZE];
	
	sprintf(new_prompt, "%s @ %s>", u->nick, nombre_canal);
	set_prompt_usuario(u, new_prompt); //Setea el prompt del usuario
	
	if ( (canal_a_entrar = canal_existe(nombre_canal))) { //Verifica si el canal ya existe
		remover_nodo(u->canal_actual->usuarios, u); //Quita al usuario del canal en que estaba anteriormente
		u->canal_actual = canal_a_entrar; //agrega al usuario al canal actual
		agregar_nodo(canal_a_entrar->usuarios, (void*)u); //agrega al usuario a la lista de usuarios de dicho canal
		//Si se pudo agregar al usuario al canal le enviamos un mensaje de exito
		comando_a_usuario(u, (char*)JOIN_SUCCESS_CMD);
		sprintf(msg_buffer, "[Server]: Uniendose al canal '%s'\n \n", canal_a_entrar->nombre_canal);
		mensaje_a_usuario(u, msg_buffer); //Envia el mensaje al usuario
		printf("Usuario %s se unio al canal %s.\n", u->nick, canal_a_entrar->nombre_canal);
	}
	else {
		nombre_nuevo_canal = (char*)malloc(strlen(nombre_canal) + 1);
		strcpy(nombre_nuevo_canal, nombre_canal);
		canal_a_entrar = nuevo_canal(nombre_nuevo_canal); //Crea un nuevo canal
		agregar_nodo(canales, canal_a_entrar); //Agrega el nuevo canal a la lista de canales
		agregar_nodo(canal_a_entrar->usuarios, (void*)u); //Agrega el usuario al nuevo canal
		remover_nodo(u->canal_actual->usuarios, u); //elimina al usuario del antiguo canal que estaba
		u->canal_actual = canal_a_entrar; //define el canal del usuario al que se acabo de unir
		//If we successfully added user to room, send success command
		comando_a_usuario(u, (char*)JOIN_SUCCESS_CMD);
		sprintf(msg_buffer, "[Server]: Creando canal '%s'\n \n", nombre_nuevo_canal);
		mensaje_a_usuario(u, msg_buffer);
		printf("Usuario %s se unio al nuevo canal %s.\n", u->nick, nombre_nuevo_canal);
	}
}


void broadcast(char* msg, canal* c, char* emisor) {
	char buffer[BUFFER_SIZE];
	int tmp_socket = -1;
	nodo* tmp_nodo = c -> usuarios -> primer_nodo;
	usuario* tmp_usuario = NULL;
	int err = -1;
	while (tmp_nodo != NULL) {

		tmp_usuario = (usuario*)(tmp_nodo -> valor);
		tmp_socket = tmp_usuario -> socket_usuario;
		sprintf(buffer, "%s %s dice: %s\n", "Imprime cmd", emisor, msg);
		pthread_mutex_lock(&(tmp_usuario->usuario_sock_mutex));
		err = send(tmp_socket, buffer, strlen(buffer) + 1, 0);
		pthread_mutex_unlock(&(tmp_usuario->usuario_sock_mutex));
		if (err < 0) {
			printf("Error sending msg %s to user %s\n", buffer, tmp_usuario -> nick);
		}
		tmp_nodo = tmp_nodo -> nodo_siguiente;
	}
}

void list_users_in_room(user* u) { //Print out all the users in a user's room to the user
	node* tmp = u->canal_actual->usuarios->head;
	user* tmp_user = NULL;
	int user_no = 0;
	char strbuffer[BUFFER_SIZE];
	mensaje_a_usuario(u, "All users in room:");
	while (tmp != NULL) {
		tmp_user = (user*)(tmp->data);
		sprintf(strbuffer, ">User %d: %s", ++user_no, tmp_user->nick);
		mensaje_a_usuario(u, strbuffer);
		tmp = tmp->next;
	}
	mensaje_a_usuario(u, " \n"); //Endline after list of users for pretty formatting.
}
void list_server_users(user* u) {
	node* tmp = all_users->head;
	user* tmp_user = NULL;
	int user_no = 0;
	char strbuffer[BUFFER_SIZE];
	mensaje_a_usuario(u, "All users on server:");
	while (tmp != NULL) {
		tmp_user = (user*)(tmp->data);
		sprintf(strbuffer, ">User %d: %s in room %s", ++user_no, tmp_user->nick, tmp_user->canal_actual->room_name);
		mensaje_a_usuario(u, strbuffer);
		tmp = tmp->next;
	}
	mensaje_a_usuario(u, " \n"); //Endline after list of users for pretty formatting.
}
void list_all_rooms(user* u) {
	node* tmp = all_chat_rooms->head;
	chat_room* tmp_room = NULL;
	int room_no = 0;
	char strbuffer[BUFFER_SIZE];
	mensaje_a_usuario(u, "All chat rooms on server:");
	while (tmp != NULL) {
		tmp_room = (chat_room*)tmp->data;
		sprintf(strbuffer, ">Room %d: %s", ++room_no, tmp_room->room_name);
		mensaje_a_usuario(u, strbuffer);
		tmp = tmp->next;
	}
	mensaje_a_usuario(u, " \n");
}
void mensaje_a_usuario(user* u, char* msg) {
	char buffer[BUFFER_SIZE];
	sprintf(buffer, "%s %s", PRINT_CMD, msg);
	int err = -1;
	pthread_mutex_lock(&(u->user_sock_mutex));
	err = send(u->user_socket, buffer, strlen(buffer) + 1, 0);
	pthread_mutex_unlock(&(u->user_sock_mutex));
	if (err < 0)
		printf("Error in 'mensaje_a_usuario' function.\nError: %s\n", strerror(errno));
	else if (err == 0) {
		printf("Error in 'mensaje_a_usuario': 0 bytes written.\nError: %s\n", strerror(errno));
	}
}
void comando_a_usuario(user* u, char* msg) {
	if (msg == NULL || strlen(msg) <=0) {
		printf("Error: NULL or 0-length message in comando_a_usuario()\n");
		return;
	}
	int err = -1;
	pthread_mutex_lock(&(u->user_sock_mutex));
	err = send(u->user_socket, msg, strlen(msg) + 1, 0);
	pthread_mutex_unlock(&(u->user_sock_mutex));
	
	if (err < 0)
		printf("Error sending in 'comando_a_usuario' function.\nError: %s\n", strerror(errno));
	else if (err == 0)
		printf("Error sending in 'comando_a_usuario': 0 bytes written.\nError: %s\n", strerror(errno));
}
chat_room* canal_existe(char* chat_room_name) {
	node* tmp = all_chat_rooms->head;
	while (tmp != NULL) {
		if (strcasecmp(((chat_room*)(tmp->data))->room_name, chat_room_name)==0)
			return (chat_room*)(tmp->data);
		tmp = tmp->next ;
	}
	return NULL;
}
