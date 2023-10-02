#include "kernel_config.h"

t_kernel_config *kernel_leer_configuracion(char *path) {

	t_config *config = config_create(path);
	t_kernel_config *kernel_config = malloc(sizeof(t_kernel_config));

	kernel_config->ip_kernel = string_duplicate(config_get_string_value(config, "IP_KERNEL"));
	kernel_config->puerto_escucha = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA"));
	kernel_config->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
	kernel_config->puerto_memoria = string_duplicate(config_get_string_value(config, "PUERTO_MEMORIA"));
	kernel_config->ip_filesystem = string_duplicate(config_get_string_value(config, "IP_FILESYSTEM"));
	kernel_config->puerto_filesystem = string_duplicate(config_get_string_value(config, "PUERTO_FILESYSTEM"));
	kernel_config->ip_cpu = string_duplicate(config_get_string_value(config, "IP_CPU"));
	kernel_config->puerto_cpu_dispatch = string_duplicate(config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
	kernel_config->puerto_cpu_interrupt = string_duplicate(config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
	kernel_config->algoritmo_planificacion = string_duplicate(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	kernel_config->quantum = config_get_double_value(config, "QUANTUM");
	kernel_config->recursos = string_duplicate(config_get_string_value(config, "RECURSOS"));
	kernel_config->instancias_recursos = string_duplicate(config_get_string_value(config, "INSTANCIAS_RECURSOS"));
	kernel_config->grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION_INI");

	config_destroy(config);

	return kernel_config;
}

void kernel_eliminar_configuracion(t_kernel_config *config) {
	free(config->ip_kernel);
	free(config->puerto_escucha);
	free(config->ip_memoria);
	free(config->puerto_memoria);
	free(config->ip_filesystem);
	free(config->puerto_filesystem);
	free(config->ip_cpu);
	free(config->puerto_cpu_dispatch);
	free(config->puerto_cpu_interrupt);
	free(config->algoritmo_planificacion);
    free(config->recursos);
    free(config->instancias_recursos);
	free(config);
}
