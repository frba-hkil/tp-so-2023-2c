#include "kernel.h"

int main(int argc, char* argv[]) {
    kernel_logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	if(argc < 1) {
		log_error(kernel_logger, "Error de parametros. Ejemplo de uso: ./kernel <archivo_configuracion>");
		log_destroy(kernel_logger);
		return EXIT_FAILURE;
	}
	kernel_config = kernel_leer_configuracion(argv[1]);

    //iniciar_conexiones_kernel(sockets);

    iniciar_consola();
    sleep(1);
    escuchar_consola();


    log_destroy(kernel_logger);
    kernel_eliminar_configuracion(kernel_config);
    //pthread_join(console_thread, NULL);
    return EXIT_SUCCESS;
}

