#ifndef SERVER_MUTEX
#define SERVER_MUTEX
#include <pthread.h>

pthread_mutex_t socket_mutex; //Valida que dos hilos no sobreescriban simultaneamente el socket del cliente
pthread_mutex_t usuarios_mutex; //Valida que dos hilos de usuarios no modifiquen simultaneamente la lista de usuarios
pthread_mutex_t canal_mutex; //Valida que dos hilos no mocdifiquen simultaneamente la lista de canales

void inicializar_mutexes();
#endif
