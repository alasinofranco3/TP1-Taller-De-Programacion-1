#ifndef COMMON_RESIZABLE_BUFFER_H
#define COMMON_RESIZABLE_BUFFER_H 

#define ERROR -1
#define EXTRA_SPACE 30

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct resizable_buffer{
	int size;
	char *buffer;
}resizable_buffer_t;

int resizable_buffer_create(resizable_buffer_t *self, int size);
void resizable_buffer_destroy(resizable_buffer_t *self);

/*En caso de error destruye el buffer*/
int resizable_buffer_resize(resizable_buffer_t *self, int new_size);

/*En caso de no contar con espacio suficiente para guardar la nueva palabra
(string) llama a resize con el tamanio nuevo + 1.Luego guarda la palabra
En caso de error destruye el buffer*/
int resizable_buffer_save(resizable_buffer_t *self, char *word);

/*En caso de no contar con espacio suficiente para guardar los n caracteres 
de la palabra (word) llama a resize con el tamanio nuevo + 1.Luego guarda
los caracteres en el buffer 
En caso de error destruye el buffer*/
int resizable_buffer_n_save(resizable_buffer_t *self, char *word, int n);

/*Se utiliza para guardar cadenas de bytes*/
int resizable_buffer_byte_save(resizable_buffer_t *self, char* buf, int size);

#endif

