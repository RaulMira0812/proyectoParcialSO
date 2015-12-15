void ejecuta_comando(char* cmd, usuario* u) {
	char* cmd_nombre = NULL;
	char* cmd_argumento = NULL;
	char* msg = NULL;
	
	cmd_nombre = strtok(cmd, " ");//Separo el comando enviado por el usuario, obteniendo solo el nombre del comando
	
<<<<<<< HEAD
	if (strcmp(cmd_nombre, MSG_CMD) == 0) { 
		msg = (char*)(&cmd[strlen(MSG_CMD)+1]); //Obtengo el contenido del mensaje
		trim_str(msg); 
		msg_all_users(msg, u->current_room, u->nick);
	}
=======
	if (strcmp(cmd_nombre, "MSG") == 0) { 
		msg = (char*)(&cmd[strlen("MSG")+1]); //Obtengo el contenido del mensaje
		trim_str(msg); 
		msg_all_users(msg, u->current_room, u->nick);
	}
	else if (strcmp(cmd_nombre, "PRIVMSG")==0) { //+JOIN
		receptor = (char*)(&cmd[strlen("PRIVMSG")+1]);
		msg = (char*)(&cmd[strlen("PRIVMSG")+1]);
		cmd_argumento = strtok(NULL, " "); //Room name is the second token
		add_user_to_room(u, cmd_argumento);
	}
>>>>>>> ae737454fb38d97f61e011659daa2c1f1fc3e8a1
	else if (strcmp(cmd_nombre, JOIN_CMD)==0) { //+JOIN
		cmd_argumento = strtok(NULL, " "); //Room name is the second token
		add_user_to_room(u, cmd_argumento);
	}
	else if (strcmp(cmd_nombre, LIST_CMD) == 0) {
		list_users_in_room(u);
	}
	else if (strcmp(cmd_nombre, LIST_ALL_CMD) == 0) {
		list_server_users(u);
	}
	else if (strcmp(cmd_nombre, ROOMS_CMD) == 0) {
		list_all_rooms(u);
	}
	else {
		printf("Received bad command from user '%s', sending fail msg\n", u->nick);
		print_raw_to_user(u, (char*)FAIL_CMD);
	}
}
void add_user_to_room(user* u, char* room_name) { //Adds user to room if it exists, otherwise creates new room and adds user
	if (room_name == NULL || strlen(room_name) <= 0) {
		printf("Error: Attempt to add user to NULL or empty room name.\n");
		return;
	}
	if (strcasecmp(room_name, u->current_room->room_name) == 0) {
		print_to_user(u, "[Server]: You are already in that chat room!");
		return;
	}
	if (!valid_charset(room_name)) {
		print_to_user(u, "[Server]: Invalid room name!\n"
		"[Server]: Alphanumeric or '_', '-', '*', '&' required!\n ");
		return;
	}
	
	chat_room* room_to_enter;
	char* new_room_name; //Will dynamically allocate to pass to new_room(char*)
	char msg_buffer[BUFFER_SIZE];
	char new_prompt[MAX_PROMPT_SIZE];
	
	sprintf(new_prompt, "%s @ %s>", u->nick, room_name);
	set_user_prompt(u, new_prompt); //Set user's prompt
	
	if ( (room_to_enter = chat_room_exists(room_name))) { //Check if room exists.
		remove_node(u->current_room->users_in_room, u); //Remove user from old chat room
		u->current_room = room_to_enter; //Set user's current room to the new room
		add_to_list(room_to_enter->users_in_room, (void*)u); //Add user to new chat room's list of users
		//If we successfully added user to room, send success command
		print_raw_to_user(u, (char*)JOIN_SUCCESS_CMD);
		//Build string to send user. Different if joining room vs. creating new room
		sprintf(msg_buffer, "[Server]: Joining room '%s'\n \n", room_to_enter->room_name);
		print_to_user(u, msg_buffer); //Send message to user
		printf("User %s joined existing room %s.\n", u->nick, room_to_enter->room_name);
	}
	else {
	//Need to dynamically allocate name to store in chat_room object if a new room is created
		new_room_name = (char*)malloc(strlen(room_name) + 1);
		strcpy(new_room_name, room_name);
		room_to_enter = new_chat_room(new_room_name); //Create new chat room
		add_to_list(all_chat_rooms, room_to_enter); //Add new room to universal list of chat rooms
		add_to_list(room_to_enter->users_in_room, (void*)u); //Add user to new chat room's list of users
		remove_node(u->current_room->users_in_room, u); //Remove user from old chat room
		u->current_room = room_to_enter; //Set user's current room to the new room
		//If we successfully added user to room, send success command
		print_raw_to_user(u, (char*)JOIN_SUCCESS_CMD);
		//Build string to send user. Different if joining room vs. creating new room
		sprintf(msg_buffer, "[Server]: Creating new room '%s'\n \n", new_room_name);
		print_to_user(u, msg_buffer);
		printf("User %s joined new room %s.\n", u->nick, new_room_name);
	}
}
<<<<<<< HEAD
=======

>>>>>>> ae737454fb38d97f61e011659daa2c1f1fc3e8a1
void broadcast(char* msg, canal* c, char* emisor) {
	char buffer[BUFFER_SIZE];
	int tmp_socket = -1;
	nodo* tmp_nodo = c -> usuarios -> primer_nodo;
	usuario* tmp_usuario = NULL;
	int err = -1;
	while (tmp_nodo != NULL) {
<<<<<<< HEAD
		tmp_usuario = (user*)(tmp_nodo -> data);
=======
		tmp_usuario = (usuario*)(tmp_nodo -> valor);
>>>>>>> ae737454fb38d97f61e011659daa2c1f1fc3e8a1
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
<<<<<<< HEAD
=======

>>>>>>> ae737454fb38d97f61e011659daa2c1f1fc3e8a1
void list_users_in_room(user* u) { //Print out all the users in a user's room to the user
	node* tmp = u->current_room->users_in_room->head;
	user* tmp_user = NULL;
	int user_no = 0;
	char strbuffer[BUFFER_SIZE];
	print_to_user(u, "All users in room:");
	while (tmp != NULL) {
		tmp_user = (user*)(tmp->data);
		sprintf(strbuffer, ">User %d: %s", ++user_no, tmp_user->nick);
		print_to_user(u, strbuffer);
		tmp = tmp->next;
	}
	print_to_user(u, " \n"); //Endline after list of users for pretty formatting.
}
void list_server_users(user* u) {
	node* tmp = all_users->head;
	user* tmp_user = NULL;
	int user_no = 0;
	char strbuffer[BUFFER_SIZE];
	print_to_user(u, "All users on server:");
	while (tmp != NULL) {
		tmp_user = (user*)(tmp->data);
		sprintf(strbuffer, ">User %d: %s in room %s", ++user_no, tmp_user->nick, tmp_user->current_room->room_name);
		print_to_user(u, strbuffer);
		tmp = tmp->next;
	}
	print_to_user(u, " \n"); //Endline after list of users for pretty formatting.
}
void list_all_rooms(user* u) {
	node* tmp = all_chat_rooms->head;
	chat_room* tmp_room = NULL;
	int room_no = 0;
	char strbuffer[BUFFER_SIZE];
	print_to_user(u, "All chat rooms on server:");
	while (tmp != NULL) {
		tmp_room = (chat_room*)tmp->data;
		sprintf(strbuffer, ">Room %d: %s", ++room_no, tmp_room->room_name);
		print_to_user(u, strbuffer);
		tmp = tmp->next;
	}
	print_to_user(u, " \n");
}
void print_to_user(user* u, char* msg) {
	char buffer[BUFFER_SIZE];
	sprintf(buffer, "%s %s", PRINT_CMD, msg);
	int err = -1;
	pthread_mutex_lock(&(u->user_sock_mutex));
	err = send(u->user_socket, buffer, strlen(buffer) + 1, 0);
	pthread_mutex_unlock(&(u->user_sock_mutex));
	if (err < 0)
		printf("Error in 'print_to_user' function.\nError: %s\n", strerror(errno));
	else if (err == 0) {
		printf("Error in 'print_to_user': 0 bytes written.\nError: %s\n", strerror(errno));
	}
}
void print_raw_to_user(user* u, char* msg) {
	if (msg == NULL || strlen(msg) <=0) {
		printf("Error: NULL or 0-length message in print_raw_to_user()\n");
		return;
	}
	int err = -1;
	pthread_mutex_lock(&(u->user_sock_mutex));
	err = send(u->user_socket, msg, strlen(msg) + 1, 0);
	pthread_mutex_unlock(&(u->user_sock_mutex));
	
	if (err < 0)
		printf("Error sending in 'print_raw_to_user' function.\nError: %s\n", strerror(errno));
	else if (err == 0)
		printf("Error sending in 'print_raw_to_user': 0 bytes written.\nError: %s\n", strerror(errno));
}
chat_room* chat_room_exists(char* chat_room_name) {
	node* tmp = all_chat_rooms->head;
	while (tmp != NULL) {
		if (strcasecmp(((chat_room*)(tmp->data))->room_name, chat_room_name)==0)
			return (chat_room*)(tmp->data);
		tmp = tmp->next ;
	}
	return NULL;
<<<<<<< HEAD
}
=======
}
>>>>>>> ae737454fb38d97f61e011659daa2c1f1fc3e8a1
