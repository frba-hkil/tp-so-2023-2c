#include "main.h"

t_log* LOGGER;
t_memoria_config* MEMORIA_CONFIG;
char *op_strings[] = {"SET", "SUM", "SUB", "JNZ", "SLEEP", "WAIT", "SIGNAL", "MOV_IN", "MOV_OUT", "F_OPEN", "F_CLOSE", "F_SEEK", "F_READ", "F_WRITE", "F_TRUNCATE", "EXIT"};

void init(char *config_path) {
	//memoria_logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	memoria_config = iniciar_config(config_path);
	iniciar_memoria_principal(memoria_config->TAM_MEMORIA, memoria_config->TAM_PAGINA);
	tablas_de_paginacion = list_create();
	archivos_swap = list_create();
	punteros_clock = list_create();
	pthread_mutex_init(&mutex_swap, NULL);
	pthread_mutex_init(&mutex_memoria, NULL);
	cantidad_acceso_disco = 0;
	cantidad_page_fault = 0;
}

int main(int argc, char **argv) {
	memoria_logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	if(argc < 2) {
		log_error(memoria_logger, "Error de parametros. Ejemplo de uso: ./memoria <archivo_configuracion>");
		log_destroy(memoria_logger);
		return EXIT_FAILURE;
	}
	init(argv[1]);

	int socket_memoria = iniciar_modulo_servidor(memoria_config->ip_memoria, memoria_config->puerto_escucha, memoria_logger);
	log_info(memoria_logger, "Memoria iniciado como servidor");

	if(atender_clientes(socket_memoria, procesar_conexiones) == WAIT_CLIENT_ERROR) {
		log_error(memoria_logger, "Error al escuchar clientes... Finalizando servidor");
	}

	log_destroy(memoria_logger);
	free(memoria_principal);
	memoria_eliminar_configuracion(memoria_config);
	cerrar_conexion(socket_memoria);

	return EXIT_SUCCESS;
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
