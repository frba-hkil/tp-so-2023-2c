#include "main.h"


void init(char *config_path) {
	//memoria_logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	memoria_config = iniciar_config(config_path);
	//iniciar_memoria_principal(memoria_config->TAM_MEMORIA, memoria_config->TAM_PAGINA);
	tablas_de_paginacion = list_create();
	archivos_swap = list_create();
	instrucciones_de_procesos = dictionary_create();
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

	iniciar_conexiones(memoria_config, memoria_logger);
	atender_kernel();
	atender_cpu();

	pthread_join(thread_kernel, NULL);
	pthread_join(thread_cpu, NULL);
	log_destroy(memoria_logger);
	free(memoria_principal);
	memoria_eliminar_config(memoria_config);

	return EXIT_SUCCESS;
}
