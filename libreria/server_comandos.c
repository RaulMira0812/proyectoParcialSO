#include "../libreria_h/server_comandos.h"


void partir_cadena(char* cadena) {
	int len = strlen(cadena);
	int i = len - 1;
	
	while (i >=0 && (cadena[i] == '\n' || cadena[i] == ' ' || cadena[i] == '\t')) {
		--i;
	}
	
	cadena[i+1] = '\0';
}




bool validar_cadena(char* cadena) {
	if (cadena == NULL) {
		printf("Error cadena incorrecta.\n");
		return false;
	}
	int i;
	for (i=0; i < strlen(cadena); i++) {
		if (!caracter_cadena(cadena[i]))
			return false;
	}
	return true;
}

bool validar_caracter(char c) {
	if ('a' <= c && c <= 'z')
		return true;
	if ('A' <= c && c <= 'Z')
		return true;
	if ('0' <= c && c <= '9')
		return true;
	if (c == '-' || c == '_' || c == '*' || c == '&' || c == '#')
		return true;
	return false;
}

char* obtener_sgt_cmd(int socket, char* cmd, bool* err) {
	int n_bytes = -1;
	char buffer[BUFFER_SIZE];
	*err = false;

	if ((n_bytes = recv(socket, buffer, BUFFER_SIZE, 0)) <= 0) {
		*err = true;
		return NULL;
	}
	return cmd;
}

