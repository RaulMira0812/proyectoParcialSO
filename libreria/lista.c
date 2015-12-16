#include "../libreria_h/lista.h"

nodo* nuevo_nodo(void* valor) { //Agregar un nuevo nodo a la listaa
	nodo* n = (nodo*)malloc(sizeof(nodo)); 
	n->valor = valor; 
	n->nodo_siguiente = NULL; 
	return n; 
}

lista* nueva_lista(void (*free_fnc)(void*)) {//Crear una nueva lista; free_fnc es un puntero que permite liberar datos en la listaa
	lista* l = (lista*) malloc(sizeof(lista)); 
	l->free_data = free_fnc; 
	l->primer_nodo = NULL; 
	return l; 
}

void agregar_nodo(lista* l, void* valor) { //Agregar nodos a la listaa y retornan si 
	nodo* new_primer_nodo = nuevo_nodo(valor); 
	new_primer_nodo -> nodo_siguiente = l->primer_nodo; 
	l->primer_nodo = new_primer_nodo; 
}

void remover_primer_nodo(lista* l) { //Remover el primer_nodo de la listaa. Se libera memoria
	if (l->primer_nodo == NULL) {
		return; 
	}

	nodo* anterior_primer_nodo = l->primer_nodo; 
	l->primer_nodo = anterior_primer_nodo->nodo_siguiente; 
	free(anterior_primer_nodo); //Remover el viejo primer_nodo
}

void remover_nodo(lista* l, void* valor){ //Remover un nodo de la listaa sin liberar memoria
	nodo* iter = l->primer_nodo; 
	nodo* nodo_eliminar; 
	if (!iter) //listaa vacia
		return; 

	if (iter->valor == valor) { //Si encuentra los datos en el primer nodo, se lo elimina
		remover_primer_nodo(l); 
		return; 
	}

	while (iter->nodo_siguiente && iter->nodo_siguiente->valor != valor) {
		iter = iter->nodo_siguiente;
	}

	//Si no encuantra el nodo a remover
	if (iter->nodo_siguiente==NULL)
		return; 

	//Se encuentra el nodo a remover
	nodo_eliminar = iter->nodo_siguiente; 
	iter->nodo_siguiente = iter->nodo_siguiente->nodo_siguiente; 

	free(nodo_eliminar); 
}

int tamanio(lista* l) { //Contar el numero de nodos de una listaa

	if (l == NULL) {
		return 0; 
	}

	nodo* tmp = l->primer_nodo; 
	int cnt = 0; 

	while (tmp != NULL) {
		cnt++; 
		tmp = tmp->nodo_siguiente; 
	}

	return cnt; 
}

bool es_vacia(lista* l) {
	if (l->primer_nodo == NULL)
		return true; 
	return false; 
}