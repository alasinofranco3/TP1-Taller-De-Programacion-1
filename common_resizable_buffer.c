#include "common_resizable_buffer.h"

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
		resizable_buffer_destroy(self);
		return ERROR;		
	} 
	self->buffer = result;
	self->size = new_size;
	memset(&self->buffer[old_size], 0, new_size - old_size);
	return 0;
}

int resizable_buffer_save(resizable_buffer_t *self, char *word){
	int new_size = strlen(self->buffer) + strlen(word);
	int status;
	void *ptr_status;

	if (new_size >= self->size - 1) {
		//printf("agrandamos el buffer\n");
		status = resizable_buffer_resize(self, new_size + 1);
		if (status == ERROR) return ERROR;
	}

	ptr_status = strncat(self->buffer, word, strlen(word));
	if (ptr_status == NULL) {
		resizable_buffer_destroy(self);
		return ERROR;
	} 

	return 0;
}

int resizable_buffer_n_save(resizable_buffer_t *self, char *word, int n){
	int new_size = strlen(self->buffer) + n;
	int status;
	void *ptr_status;

	if (new_size >= self->size - 1) {
		//printf("agrandamos el buffer\n");
		status = resizable_buffer_resize(self, new_size + 1);
		if (status == ERROR) return ERROR;
	}

	ptr_status = strncat(self->buffer, word, n);
	if (ptr_status == NULL) {
		resizable_buffer_destroy(self);
		return ERROR;
	} 

	return 0;
}

int resizable_buffer_byte_save(resizable_buffer_t *self, char* buf, int size) {
	int status;
	int old_size = self->size;
	int	new_size = self->size + size;

	if (new_size > self->size) {
		status = resizable_buffer_resize(self, new_size);
		if (status == ERROR) return ERROR;
	}

	for (int i = 0 ; i < size; i++) {
		self->buffer[i + old_size] = buf[i];
	}

	return 0;
}
