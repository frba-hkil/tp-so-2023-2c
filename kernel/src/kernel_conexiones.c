#include "kernel_conexiones.h"

void iniciar_conexiones_kernel(void){

	int socket_cpu_dispatch, socket_cpu_interrupt, socket_memoria, socket_filesystem;

	socket_cpu_dispatch = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_dispatch, kernel_logger);
	socket_cpu_interrupt = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_interrupt, kernel_logger);
	socket_memoria = conectar_a_modulo(kernel_config->ip_memoria, kernel_config->puerto_memoria, kernel_logger);
	socket_filesystem = conectar_a_modulo(kernel_config->ip_filesystem, kernel_config->puerto_filesystem, kernel_logger);

}
