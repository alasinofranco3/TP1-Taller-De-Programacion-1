#include "common_parser.h"


void parser_create(parser_t *self) {
	self->dummie = 0;
}

void parser_destroy(parser_t *self) {
	self->dummie = 0;
}

int parser_run(FILE* file, resizable_buffer_t * buf, char * remaining) {
	int status;
	char* delim = NULL;
	int buf_len = MAX_BUF_SIZE;
	char aux_buffer[MAX_BUF_SIZE];
	memset(aux_buffer, 0, buf_len);

	while (!feof(file) && delim == NULL) {
		status = resizable_buffer_save(buf, aux_buffer);
		if (status == ERROR) return ERROR;
		status = fread(aux_buffer, 1, buf_len - 1, file);
		delim = strchr(aux_buffer, '\n');
	}

	if (delim != NULL) {
		*delim = '\0';
		status = resizable_buffer_save(buf, aux_buffer);
		
		if (status == ERROR) return ERROR;
		memset(remaining, 0, MAX_BUF_SIZE);
		strncpy(remaining, delim + 1, strlen(delim + 1));
	} else {
		printf("Se encontro el fin de archivo\n");
	}

	return 0;
}


