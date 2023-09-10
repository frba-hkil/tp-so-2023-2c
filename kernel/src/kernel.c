#include "kernel.h"

int main(int argc, char* argv[]) {
    kernel_logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	if(argc < 1) {
		log_error(kernel_logger, "Error de parametros. Ejemplo de uso: ./kernel <archivo_configuracion>");
		log_destroy(kernel_logger);
		return EXIT_FAILURE;
	}
	kernel_config = kernel_leer_configuracion(argv[1]);

    log_info(kernel_logger, "Hello world!!");
    log_info(kernel_logger, kernel_config->recursos);

    int socket_kernel = iniciar_modulo_servidor(kernel_config->ip_kernel, kernel_config->puerto_escucha, kernel_logger);

    log_info(kernel_logger, "Esperando conexion de consolas");
	if(atender_clientes(socket_kernel) == WAIT_CLIENT_ERROR) {
		log_error(kernel_logger, "Error al escuchar clientes... Finalizando servidor");
	}
	log_destroy(kernel_logger);
	kernel_eliminar_configuracion(kernel_config);
	cerrar_conexion(socket_kernel);

    puts("Hello world!!");
    return EXIT_SUCCESS;
}