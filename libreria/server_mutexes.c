#include "../libreria/server_mutexes.h"

void inicializar_mutexes() {
	pthread_mutex_init(&socket_mutex, NULL); //Valida que dos hilos no sobreescriban simultaneamente el socket del cliente
	pthread_mutex_init(&usuarios_mutex, NULL); //Valida que dos hilos de usuarios no modifiquen simultaneamente la lista de usuarios
	pthread_mutex_init(&canal_mutex, NULL); //Valida que dos hilos no mocdifiquen simultaneamente la lista de canales 
}