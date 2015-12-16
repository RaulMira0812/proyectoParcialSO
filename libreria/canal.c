#include "../libreria_h/canal.h"

static const char* nombre_canal_espera = "LOBBY"; //Nombre del canal de espera
static lista* canales = NULL; //lista de todos los canales
static canal* canal_espera = NULL; //canal de espera (lobby)

void iniciar_canales() {

	if (canales == NULL) {
		canales = nueva_lista((void(*)(void*))liberar_canal); 
	}
	if (canal_espera == NULL) {
		canal_espera = nuevo_canal((char*)nombre_canal_espera); 
		agregar_nodo(canales, (void*)canal_espera); 
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

void agrega_usuario_a_espera(usuario* u) { 

	char nuevo_prompt[MAX_PROMPT_SIZE]; 
	char command_buffer[BUFFER_SIZE]; 

	sprintf(nuevo_prompt, "%s $ %s>", u->nickname, nombre_canal_espera); 
	//set_prompt_usuario(u, nuevo_prompt); // envia el comando prompt al usuario para forzar cambiar el prompt

	agregar_nodo(canal_espera -> usuarios, (void*)u); 
	u->canal_actual = canal_espera;  // el usuario actualmente esta en el lobby

	sprintf(command_buffer, "JOIN_SUCCESS_CMD\n+PRINT [Server]: Conectado como usuario '%s'.\n[Server]: Ingresando al lobby.\n \n", u->nickname); 
	comando_a_usuario(u,command_buffer); 
	//canal_loop(u); 
}
/*
void canal_loop(usuario* u) {
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
printf("Error al leer el mensaje del cliente.\nCerrando el socket #%d al cliente '%s'\nError: %s\n", u->socket_usuario, u->nickname, strerror(errno)); 
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
			printf("Mensaje del cliente %s: %s\n", u->nickname, siguiente_comando); 
			ejecuta_comando(siguiente_comando, u); 
		}
	}
}*/

void ejecuta_comando(char* cmd, usuario* u) {
	char* cmd_nombre = NULL;
	char* cmd_argumento = NULL;
	char* msg = NULL;
	
	cmd_nombre = strtok(cmd, " ");//Separo el comando enviado por el usuario, obteniendo solo el nombre del comando

	if (strcmp(cmd_nombre, "MSG") == 0) { 
		msg = (char*)(&cmd[strlen("MSG")+1]); //Obtengo el contenido del mensaje
		//trim_str(msg); 
		broadcast(msg, u->canal_actual, u->nickname);
	}
	else if (strcmp(cmd_nombre, "INFO")==0) { 
		cmd_argumento = strtok(NULL, " "); 
		cmd_INFO(u,cmd_argumento);
	}	
	else if (strcmp(cmd_nombre, "LIST") == 0) {
		cmd_argumento = strtok(NULL, " ");
		cmd_LIST(u, cmd_argumento);
	}	
	else if (strcmp(cmd_nombre, "NICK") == 0) {
		cmd_argumento = strtok(NULL, " ");
		cmd_NICK(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "PRIVMSG") == 0) {
		cmd_argumento = strtok(NULL, " ");
		msg = strtok(NULL, " ");
		partir_cadena(msg);
		cmd_PRIVMSG(u, cmd_argumento,msg);
	}
	else if (strcmp(cmd_nombre, "MOTD") == 0) {
		cmd_argumento = strtok(NULL, " ");
		cmd_MOTD(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "PART") == 0) {
		cmd_argumento = strtok(NULL, " ");
		cmd_PART(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "SETNAME") == 0) {
		cmd_argumento = strtok(NULL, " ");
		cmd_SETNAME(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "VERSION") == 0) {
		cmd_argumento = strtok(NULL, " ");
		cmd_VERSION(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "NAMES") == 0) {
		cmd_argumento = strtok(NULL, " ");
		cmd_NAMES(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "USER") == 0) {
		cmd_argumento = strtok(NULL, " ");
		cmd_USER(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "USERS")==0) { 
		cmd_argumento = strtok(NULL, " ");
		lista_usuarios_server(u,cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "JOIN")==0) { //+JOIN
		cmd_argumento = strtok(NULL, " "); //el canal es el segundo token
		cmd_JOIN(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, "lista_CMD") == 0) {
		lista_usuarios(u);
	}
	else {
		printf("Received bad command from usuario '%s', sending fail msg\n", u->nickname);
		comando_a_usuario(u, (char*)"FAIL_CMD");
	}
}

void cmd_JOIN(usuario* u, char* nombre_canal) { //Agrega el usuario u al canal si existe, si no primero crea el canal
	if (nombre_canal == NULL || strlen(nombre_canal) <= 0) {
		printf("Error: Se intento conectar a un canal inexistente.\n");
		return;
	}
	if (strcasecmp(nombre_canal, u->canal_actual->nombre_canal) == 0) {
		mensaje_a_usuario(u, "[Server]: Usted ya se encuentra conectado a este canal!");
		return;
	}
	/*
	if (!valid_charset(nombre_canal)) {
		mensaje_a_usuario(u, "[Server]: Nombre del canal invalido!\n"
		"[Server]: debe ser alfanumerico o  '_', '-', '*', '&'!\n ");
		return;
	}*/
	
	canal* canal_a_entrar;
	char* nombre_nuevo_canal; 
	char msg_buffer[BUFFER_SIZE];
	char new_prompt[MAX_PROMPT_SIZE];
	
	sprintf(new_prompt, "%s @ %s>", u->nickname, nombre_canal);
	//set_prompt_usuario(u, new_prompt); //Setea el prompt del usuario
	
	if ( (canal_a_entrar = canal_existe(nombre_canal))) { //Verifica si el canal ya existe
		remover_nodo(u->canal_actual->usuarios, u); //Quita al usuario del canal en que estaba anteriormente
		u->canal_actual = canal_a_entrar; //agrega al usuario al canal actual
		agregar_nodo(canal_a_entrar->usuarios, (void*)u); //agrega al usuario a la lista de usuarios de dicho canal
		//Si se pudo agregar al usuario al canal le enviamos un mensaje de exito
		comando_a_usuario(u, (char*)"JOIN_SUCCESS_CMD");
		sprintf(msg_buffer, "[Server]: Uniendose al canal '%s'\n \n", canal_a_entrar->nombre_canal);
		mensaje_a_usuario(u, msg_buffer); //Envia el mensaje al usuario
		printf("Usuario %s se unio al canal %s.\n", u->nickname, canal_a_entrar->nombre_canal);
	}
	else {
		nombre_nuevo_canal = (char*)malloc(strlen(nombre_canal) + 1);
		strcpy(nombre_nuevo_canal, nombre_canal);
		canal_a_entrar = nuevo_canal(nombre_nuevo_canal); //Crea un nuevo canal
		agregar_nodo(canales, canal_a_entrar); //Agrega el nuevo canal a la lista de canales
		agregar_nodo(canal_a_entrar->usuarios, (void*)u); //Agrega el usuario al nuevo canal
		remover_nodo(u->canal_actual->usuarios, u); //elimina al usuario del antiguo canal que estaba
		u->canal_actual = canal_a_entrar; //define el canal del usuario al que se acabo de unir
		//If we successfully added usuario to room, send success command
		comando_a_usuario(u, (char*)"JOIN_SUCCESS_CMD");
		sprintf(msg_buffer, "[Server]: Creando canal '%s'\n \n", nombre_nuevo_canal);
		mensaje_a_usuario(u, msg_buffer);
		printf("Usuario %s se unio al nuevo canal %s.\n", u->nickname, nombre_nuevo_canal);
	}
}

void cmd_MOTD(usuario* u, char* cmd_argumento){
	char buffer[BUFFER_SIZE];
	if (cmd_argumento == "ESPOLirc" || cmd_argumento==NULL){
		sprintf(buffer,"*************** Bienvenido a linux Espol-irc 1.0.0 #1 ***************\n"
						"Documentation:  https://github.com/RaulMira0812/proyectoParcialSO\n"	
						"******************************************************************\n"
						"--------- Please respect the server usage policy ---------------\n");
		comando_a_usuario(u,buffer);	
	}else{
		mensaje_a_usuario(u, "[Server]: Nombre del servidor invalido!\n");
		return;
	}
}

void cmd_PART(usuario* u, char* cmd_argumento){
	canal* canal_a_entrar=canal_espera;
	char buffer[BUFFER_SIZE];
	char msg_buffer[BUFFER_SIZE];
	if (cmd_argumento==NULL){ // si no especifica argumento se desconecta del canal en que se encuentra y envia mensaje por defecto	
		sprintf(buffer, "[Server]: el usuario '%s' ha salido del canal \n", u->nickname);
	}else{
		strcpy(buffer,cmd_argumento);
		//buffer=cmd_argumento; //envia el mensaje del usuario
	}
	broadcast(buffer, u->canal_actual, u->nickname);
	u->canal_actual= NULL; //lo desconecto del canal en que estaba
	remover_nodo(u->canal_actual->usuarios, u); //Quita al usuario del canal en que estaba anteriormente
	agrega_usuario_a_espera(u); //agrega al usuario al canal de espera o lobby
	comando_a_usuario(u, (char*)"JOIN_SUCCESS_CMD");
	sprintf(msg_buffer, "[Server]: Uniendose al canal '%s'\n \n", canal_a_entrar->nombre_canal);
	mensaje_a_usuario(u, msg_buffer); //Envia el mensaje al usuario
	printf("Usuario %s se unio al canal %s.\n", u->nickname, canal_a_entrar->nombre_canal);

}

void cmd_SETNAME(usuario* u, char* cmd_argumento){
	if(cmd_argumento==NULL){
		mensaje_a_usuario(u, "[Server]:  No ha ingresado ningun nombre!\n");
		return;
	}else{
		char* nuevo_nombre;
		nuevo_nombre = cmd_argumento;
		u->nombre_real=nuevo_nombre;
		comando_a_usuario(u,(char*)"[Server]: Comando SETNAME ejecutado exitosamente, nombre cambiado\n");
	}
}

void cmd_VERSION(usuario* u, char* cmd_argumento){
	if(cmd_argumento == "ESPOLirc" || cmd_argumento==NULL){
		mensaje_a_usuario(u,"Version: 1.0.0");
	
	}else{
		mensaje_a_usuario(u,"[Server]: Nombre del servidor invalido!\n");
		return;
	}
}

void cmd_USER(usuario* u, char* cmd_argumento){
	
	if(cmd_argumento==NULL){return ;}
	char* username=NULL;
	char* hostname=NULL;
	char* servername=NULL;
	char* realname=NULL;
	
	username = strtok(cmd_argumento, " ");
	hostname = strtok(NULL, " ");
	servername = strtok(NULL, " ");
	realname = strtok(NULL, " ");

	u->hostname=hostname;
	u->nombre_real=realname;
	u->nickname=username;
	comando_a_usuario(u,(char*)"[Server]: Comando USER ejecutado exitosamente, usuario seteado\n");
	
}

void cmd_NAMES(usuario* u, char* cmd_argumento){
	char msg_buffer[BUFFER_SIZE];	
	canal* canal_esp;
	if(cmd_argumento!=NULL){ //muestra los usuarios en el canal especificado
		canal_esp = canal_existe(cmd_argumento);	//OJO 
	}else{ 	canal_esp = u->canal_actual;	
	}
		if(canal_esp==NULL){	
			mensaje_a_usuario(u, "[Server]: El canal especificado no existe!\n");
			return ;} //el canal no existe}
		int i;		
		mensaje_a_usuario(u,"Lista de usuarios en el canal especificado:\n");

		int cnt=0;
		nodo* tmp = canal_esp->usuarios->primer_nodo; 
		while (tmp != NULL) {
			usuario* tmp_u = (usuario*)tmp->valor;
			sprintf(msg_buffer, "[Server]: Usuario '%d' : '%s' \n",cnt,tmp_u->nickname);
			mensaje_a_usuario(u, msg_buffer); //Envia el mensaje al usuario
			cnt++; 
			tmp = tmp->nodo_siguiente; 
		}

}

void cmd_INFO(usuario* u, char* cmd_argumento){

	if(cmd_argumento == "ESPOLirc" || cmd_argumento==NULL){
	
		mensaje_a_usuario(u,"Servidor:EspolIRC\nCreadores:Estefania Lozano,Raul Mira & Henry Lasso\nVersion:1.0.0");
	
	}else{
	
		mensaje_a_usuario(u, "[Server]: Nombre del servidor invalido!\n");
		return;
	}

}
void cmd_NICK(usuario* u, char* cmd_argumento){
	if (cmd_argumento == NULL || strlen(cmd_argumento) <=0) {
		printf("Error: Argumento vacio en el commando NICK\n");
		return;
	}
	if (!validar_cadena(cmd_argumento)) {
		mensaje_a_usuario(u, "[Server]: Nombre de nick invalido!\n"
		"[Server]: debe ser alfanumerico o  '_', '-', '*', '&'!\n ");
		return;
	}

	char* nuevo_nick;
	nuevo_nick = cmd_argumento;

	u->nickname= nuevo_nick;

	comando_a_usuario(u,(char*)"[Server]: Comando Nick ejecutado exitosamente\n");

}

void cmd_PRIVMSG(usuario* u, char* receptor, char* msg){
	if (existe_usuario(receptor)){
		
		usuario* tmp_u;
		
		nodo* iter = usuarios_todos->primer_nodo;
		int err = -1;
		while (iter != NULL) {
			tmp_u = (usuario*)iter->valor;
			if (strcasecmp(tmp_u->nickname, receptor)==0) {
				
				pthread_mutex_lock(&(tmp_u->usuario_sock_mutex));
				err = send(tmp_u->socket_usuario, msg, strlen(msg) + 1, 0);
				pthread_mutex_unlock(&(tmp_u->usuario_sock_mutex));
				
				if (err < 0) {
					printf("Error al enviar el msg %s al usuario %s\n", msg, tmp_u-> nickname);
				}
				break;
			}
			iter=iter->nodo_siguiente;
		}

	}else{
		mensaje_a_usuario(u, "[Server]: Nick del receptor invalido!\n");
		return;
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
			printf("Error sending msg %s to usuario %s\n", buffer, tmp_usuario -> nickname);
		}
		tmp_nodo = tmp_nodo -> nodo_siguiente;
	}
}

void cmd_LIST(usuario* u,char * cmd_argumento) {
	
	nodo* tmp = canales->primer_nodo;
	canal* canal_temporal = NULL;
	int numero_canal = 0;
	char strbuffer[BUFFER_SIZE];
	
	if(cmd_argumento == "ESPOLirc" || cmd_argumento==NULL){
		mensaje_a_usuario(u, "Todos los canales en el servidor:");
		
		while (tmp != NULL) {
			canal_temporal = (canal*)tmp->valor;
			sprintf(strbuffer, ">Canal %d: %s", ++numero_canal, canal_temporal->nombre_canal);
			mensaje_a_usuario(u, strbuffer);
			tmp = tmp->nodo_siguiente;
		}
		mensaje_a_usuario(u, " \n");
	}else{
		mensaje_a_usuario(u, "[Server]: Comando LIST invalido!\n");
		return;
	}
}

void lista_usuarios(usuario* u) { //le muestra al usuario u la lista de todos los usuarios que estan en su canal
	nodo* tmp = u->canal_actual->usuarios->primer_nodo;
	usuario* usuario_temporal = NULL;
	int numero_usuario = 0;
	char strbuffer[BUFFER_SIZE];
	mensaje_a_usuario(u, "Todos los usuarios en el canal:");
	while (tmp != NULL) {
		usuario_temporal = (usuario*)(tmp->valor);
		sprintf(strbuffer, ">Usuario %d: %s", ++numero_usuario, usuario_temporal->nickname);
		mensaje_a_usuario(u, strbuffer);
		tmp = tmp->nodo_siguiente;
	}
	mensaje_a_usuario(u, " \n");
}

void lista_usuarios_server(usuario* u, char* cmd_argumento){ //le muestra al usuario u la lista de todos los usuarios en el server
	if(cmd_argumento == "ESPOLirc" || cmd_argumento==NULL){
		nodo* tmp = usuarios_todos->primer_nodo;
		usuario* usuario_temporal = NULL;
		int numero_usuario = 0;
		char strbuffer[BUFFER_SIZE];
		mensaje_a_usuario(u, "Todos los usuarios en el servidor:");
		while (tmp != NULL) {
			usuario_temporal = (usuario*)(tmp->valor);
			sprintf(strbuffer, ">Usuario %d: %s en el canal %s", ++numero_usuario, usuario_temporal->nickname, usuario_temporal->canal_actual->nombre_canal);
			mensaje_a_usuario(u, strbuffer);
			tmp = tmp->nodo_siguiente;
		}
		mensaje_a_usuario(u, " \n");
	}else{
		mensaje_a_usuario(u, "[Server]: Nombre del servidor invalido!\n");
		return;
	}	
}


void mensaje_a_usuario(usuario* u, char* msg) {
	char buffer[BUFFER_SIZE];
	sprintf(buffer, "%s %s", "", msg);
	int err = -1;
	pthread_mutex_lock(&(u->usuario_sock_mutex));
	err = send(u->socket_usuario, buffer, strlen(buffer) + 1, 0);
	pthread_mutex_unlock(&(u->usuario_sock_mutex));
	if (err < 0)
		printf("Error en 'mensaje_a_usuario' function.\nError: %s\n", strerror(errno));
	else if (err == 0) {
		printf("Error en 'mensaje_a_usuario': 0 bytes escritos.\nError: %s\n", strerror(errno));
	}
}

void comando_a_usuario(usuario* u, char* msg) {
	if (msg == NULL || strlen(msg) <=0) {
		printf("Error: NULL o mensaje vacio en comando_a_usuario()\n");
		return;
	}
	int err = -1;
	pthread_mutex_lock(&(u->usuario_sock_mutex));
	err = send(u->socket_usuario, msg, strlen(msg) + 1, 0);
	pthread_mutex_unlock(&(u->usuario_sock_mutex));
	
	if (err < 0)
		printf("Error al enviar 'comando_a_usuario' function.\nError: %s\n", strerror(errno));
	else if (err == 0)
		printf("Error al enviar'comando_a_usuario': 0 bytes escritos.\nError: %s\n", strerror(errno));
}

canal* canal_existe(char* nombre_canal) {
	nodo* tmp = canales->primer_nodo;
	while (tmp != NULL) {
		if (strcasecmp(((canal*)(tmp->valor))->nombre_canal, nombre_canal)==0)
			return (canal*)(tmp->valor);
		tmp = tmp->nodo_siguiente ;
	}
	return NULL;
}
