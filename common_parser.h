#ifndef COMMON_PARSER_H
#define COMMON_PARSER_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common_resizable_buffer.h"

#define ERROR -1
#define MAX_BUF_SIZE 32

typedef struct parser {
	//	SOLO LO AGREGUE PARA PODER COMPILAR CON MAKE
	int dummie;
}parser_t;

void parser_create(parser_t *self);
void parser_destroy(parser_t *self);

/*Lee del archivo en tandas de 32 bytes hasta encontrar un \n y 
luego guarda en remaining lo que estaba despues del \n 
que no fue procesado */
int parser_run(FILE* file, resizable_buffer_t * buf, char * remaining);


#endif
