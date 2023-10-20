#include "kernel_conexiones.h"

void iniciar_conexiones_kernel(int* conexiones){

	conexiones[0] = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_dispatch, kernel_logger);
	conexiones[1] = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_interrupt, kernel_logger);
	conexiones[2] = conectar_a_modulo(kernel_config->ip_memoria, kernel_config->puerto_memoria, kernel_logger);
	conexiones[3] = conectar_a_modulo(kernel_config->ip_filesystem, kernel_config->puerto_filesystem, kernel_logger);

}
