#include "cargar_instrucciones.h"


char *op_strings[] = {"SET", "SUM", "SUB", "JNZ", "SLEEP", "WAIT", "SIGNAL", "MOV_IN", "MOV_OUT", "F_OPEN", "F_CLOSE", "F_SEEK", "F_READ", "F_WRITE", "F_TRUNCATE", "EXIT"};

t_list *leer_instrucciones(char* nombre_archivo) { //crea la lista de instrucciones dado un puntero al archivo de un proceso
    FILE* archivo = leer_archivo(nombre_archivo);
    log_info(memoria_logger, "%s", nombre_archivo);
	char *linea, **tokens;
	t_instruccion *inst;
	t_list *instrucciones = list_create();
	size_t len;

	while(!feof(archivo)) {
		linea = string_new();
		tokens = string_array_new();
		len = 0;

		getline(&linea, &len, archivo);
		//log_info(memoria_logger, "%s", linea);
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

FILE* leer_archivo(char* nombre){
    FILE *archivo = fopen(nombre, "r");

    return archivo;
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
