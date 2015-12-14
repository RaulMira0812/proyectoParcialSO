#ifndef SERVER_COMMANDS
#define SERVER_COMMANDS
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "constants.h"

//quitar espacio al final de la cadena
void partir_cadena(char* cadena); 

//validar solo caracteres permitidos
bool validar_cadena(char* cadena);

//verificar caracter por caracter
bool validar_caracter(char c);

//obtener comando
char* obtener_sgt_cmd(int socket, char* cmd, bool* err);


