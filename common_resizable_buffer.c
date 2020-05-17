#include "common_resizable_buffer.h"

//FUNCIONES PRIVADAS
static int resize_if_needed(resizable_buffer_t *self, int new_size) {
	int status;
	if (new_size > self->size) {
		status = resizable_buffer_resize(self, new_size);
		if (status == ERROR) return ERROR;
	}
	return 0;
}

//FUNCIONES PUBLICAS
int resizable_buffer_create(resizable_buffer_t *self, int size){
	self->buffer = malloc(sizeof(char) * size);
	if (self->buffer == NULL) return ERROR;
	self->size = size;
	memset(self->buffer, 0, size);
	return 0;
}

void resizable_buffer_destroy(resizable_buffer_t *self){
	free(self->buffer);
}

int resizable_buffer_resize(resizable_buffer_t *self, int new_size){
	int old_size = self->size;
	char *result;
	result = realloc(self->buffer, new_size);
	if (result == NULL) {
		return ERROR;		
	} 
	self->buffer = result;
	self->size = new_size;
	memset(&self->buffer[old_size], 0, new_size - old_size);
	return 0;
}

int resizable_buffer_save(resizable_buffer_t *self, char *word){
	int status;
	status = resizable_buffer_n_save(self, word, strlen(word));
	return status;
}

int resizable_buffer_n_save(resizable_buffer_t *self, char *word, int n){
	int new_size = strlen(self->buffer) + n + 1;
	int status;
	void *ptr_status;

	status = resize_if_needed(self, new_size);
	if (status == ERROR) return ERROR;

	ptr_status = strncat(self->buffer, word, n);
	if (ptr_status == NULL) {
		return ERROR;
	} 

	return 0;
}

int resizable_buffer_byte_save(resizable_buffer_t *self, char* buf, int size) {
	int status;
	int old_size = self->size;
	int	new_size = self->size + size;

	status = resize_if_needed(self, new_size);
	if (status == ERROR) return ERROR;

	memcpy(&self->buffer[old_size], buf, size);

	return 0;
}

bool resizable_buffer_is_empty(resizable_buffer_t *self) {
	if (*self->buffer == '\0') {
		return true;
	} else {
		return false;
	}
}
