#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/log.h>
#include "configuraciones.h"
#include "../../utils/src/conexiones.h"

t_log* LOGGER;
t_memoria_config* MEMORIA_CONFIG;
char *op_strings[] = {"SET", "SUM", "SUB", "JNZ", "SLEEP", "WAIT", "SIGNAL", "MOV_IN", "MOV_OUT", "F_OPEN", "F_CLOSE", "F_SEEK", "F_READ", "F_WRITE", "F_TRUNCATE", "EXIT"};

int main() {

    LOGGER=iniciarLogger();
    MEMORIA_CONFIG = iniciarConfig();

    printf("El IP es: %s El puerto es: %s", MEMORIA_CONFIG->IP_FILESYSTEM, MEMORIA_CONFIG->PUERTO_ESCUCHA);

    iniciar_modulo_servidor(MEMORIA_CONFIG->IP_MEMORIA, MEMORIA_CONFIG->PUERTO_ESCUCHA, LOGGER);

    log_info (LOGGER, "Iniciamos server");

    conectar_a_modulo(MEMORIA_CONFIG->IP_FILESYSTEM, MEMORIA_CONFIG->PUERTO_FILESYSTEM, LOGGER);

    log_info(LOGGER, "Logramos ser cliente");
    return 0;
}

t_list *crear_instrucciones(FILE* proc) { //crea la lista de instrucciones dado un puntero al archivo de un proceso

	char *linea, **tokens;
	t_instruccion *inst;
	t_list *instrucciones = list_create();

	while(!feof(proc)) {
		linea = string_new();
		tokens = string_array_new();

		fscanf(proc, "%s\n", linea);
		tokens = string_n_split(linea, 3, " ");

		switch(string_array_size(tokens)) {
		case 1:
			inst = crear_instruccion(string_a_op_code(tokens[0]), "\0", "\0");
			break;
		case 2:
			inst = crear_instruccion(string_a_op_code(tokens[0]), tokens[1], "\0");
			break;
		case 3:
			inst = crear_instruccion(string_a_op_code(tokens[0]), tokens[1], tokens[2]);
		}

		list_add(instrucciones, inst);
		free(linea);
		string_array_destroy(tokens);
	}

	return instrucciones;
}

t_op_code string_a_op_code(char* str) {
	int op_codes_size = string_array_size(op_strings);
	t_op_code i;

	for(i = 0; i < op_codes_size; i++) {
		if(string_equals_ignore_case(str, op_strings[i]))
			return i;
	}
	return -1;
}
