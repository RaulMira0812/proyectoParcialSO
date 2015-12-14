#ifndef _LIST
#define _LIST

#include <stdlib.h>
#include <stdbool.h>

typedef struct nodo_struct nodo; //El nodo de cada lista

typedef struct nodo_struct { //La estructura del nodo

	void* valor; 
	node* nodo_siguiente; 

} nodo_strc; 

typedef struct { //La estructura de la lista

	nodo* primer_nodo; 

	void (*free_data)(void*); //Funcion que libera los datos en el nodo

} lista;

nodo* nuevo_nodo(void* valor); //Agregar un nuevo nodo

lista* nueva_lista(void (*free_fnc)(void*)); //Crear una nueva lista vacia. Pasar un puntero para liberar datos en la lista

void agregar_nodo(lista* l, void* valor); //Agregar un nodo a la lista y retornar el nuevo head

void remover_primer_nodo(lista* l); //Remover un nodo del frente y libera el nodo. No se hace free data porque necesitamos remover un usuario de multimples listas 

int tamanio(lista* l); //Contar el numero de nodos de una lista

void remover_nodo(lista* l, void* valor); //Remover un nodo de la lista

bool es_vacia(lista* l); //Saber si la lista es vacia

#endif