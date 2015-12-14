#ifndef _LIST
#define _LIST

#include <stdlib.h>
#include <stdbool.h>

typedef struct node_struct node; //El nodo de cada lista

typedef struct node_struct { //La estructura del nodo

	void* data; 
	node* next; 

} node_str; 

typedef struct { //La estructura de la lista

	node* head; 

	void (*free_data)(void*); //Funcion que libera los datos en el nodo

} list;

node* new_node(void* data); //Agregar un nuevo nodo

list* new_list(void (*free_fnc)(void*)); //Crear una nueva lista vacia. Pasar un puntero para liberar datos en la lista

void add_to_list(list* l, void* data); //Agregar nodos a la lista y retornar el nuevo head

void remove_front(list* l); //Remover un nodo del frente y libera el nodo. No se hace free data porque necesitamos remover un usuario de multimples listas 

int list_size(list* l); //Contar el numero de nodos de una lista

void remove_node(list* l, void* data); //Remover un nodo de la lista

bool is_empty(list* l); //Saber si la lista es vacia

#endif