#include "../libreria_h/users.h"

pthread_mutex_t sock_mutex; //garantiza que dos hilos no se sobreescriban 

void inicializar_lista_usuarios() {	//Si la lista de usuarios no se ha inicializado, se crea una lista vacia
	usuarios_todos = nueva_lista((void(*)(void*))free_user);
}

void inicializar_usuario(char* nickname, int sock, pthread_t hilo_usuario){
	usuario* nuevo_usuario = (usuario*)malloc(sizeof(usuario));
	nuevo_usuario->socket_usuario = sock;
	nuevo_usuario->canal_actual = NULL;
	nuevo_usuario->nickname = nickname;
	nuevo_usuario->hilo_usuario = hilo_usuario;
	pthread_mutex_init(&(nuevo_usuario->usuario_sock_mutex), NULL);
	
	pthread_mutex_lock(&usuarios_mutex); 
	agregar_nodo(usuarios_todos, nuevo_usuario);//Verifico si el semaforo esta desbloqueado, y agrego el usuario nuevo a la lista de todos los usuarios
	pthread_mutex_unlock(&usuarios_mutex);
	
	agregar_usuario_a_lobby(nuevo_usuario);
}

void liberar_usuario(usuario* u) {
	close(u->socket_usuario);
	pthread_mutex_destroy(&(u->usuario_sock_mutex));
	free(u->nickname);
	free(u);
}

void remover_usuario(usuario* u) { 
	if (u->canal_actual != NULL) {
		pthread_mutex_lock(&(u->canal_actual->candado));
		remover_nodo(u->canal_actual->usuarios, (void*)u);
		pthread_mutex_unlock(&(u->canal_actual->candado));
	}
	pthread_mutex_lock(&usuarios_mutex);
	remover_nodo(usuarios_todos, (void*)u); 
	pthread_mutex_unlock(&usuarios_mutex);
	liberar_usuario(u); 
}


void set_prompt_usuario(usuario* u, char* nuevo_prompt) {
	char msg_a_cliente[BUFFER_SIZE];
	int bytes_written;
	sprintf(msg_a_client, "%s %s", SET_PROMPT_CMD, nuevo_prompt);
	pthread_mutex_lock(&(u->usuario_sock_mutex)); 
	bytes_written = send(u->usuario_socket, msg_a_cliente, strlen(msg_a_cliente) + 1, 0);
	pthread_mutex_unlock(&(u->usuario_sock_mutex));
	if (bytes_written < 0) {
		printf("Error al enviar el nuevo prompt.\nError: %s\n", strerror(errno));
	}
	else if (bytes_written == 0) {
		printf("Error al enviar el nuevo prompt: 0 bytes written.\nError: %s\n", strerror(errno));
	}
}
bool existe_usuario(char* nick) {
	usuario* tmp_u;
	bool existe = false;
	nodo* iter = usuarios_todos->primer_nodo;
	while (iter != NULL) {
		tmp_u = (usuario*)iter->data;
		if (strcasecmp(tmp_u->nickname, nick)==0) {
			existe = true;
			break;
		}
		iter=iter->nodo_siguiente;
	}
	return existe;
}
