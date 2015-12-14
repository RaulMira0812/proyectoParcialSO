#ifndef SERVER_MUTEX
#define SERVER_MUTEX
#include <pthread.h>

pthread_mutex_t socket_mutex; //Guarantee 2 threads don't simultaneously write to client socket
pthread_mutex_t usuarios_mutex; //Guarantee 2 threads don't both modify user list
pthread_mutex_t canal_mutex; //Guarantee 2 threads don't both modify chat room list
//Must be called by server code before command loop

void inicializar_mutexes();
#endif