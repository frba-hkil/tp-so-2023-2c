#ifndef KERNEL_CONFIG_H_
#define KERNEL_CONFIG_H_

#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct {
	char *ip_kernel;
	char *puerto_escucha;
	char *ip_memoria;
	char *puerto_memoria;
	char *ip_filesystem;
	char *puerto_filesystem;
	char *ip_cpu;
	char *puerto_cpu_dispatch;
	char *puerto_cpu_interrupt;
	char *algoritmo_planificacion;
	double quantum;
	char* recursos;
	char* instancias_recursos;
	int grado_multiprogramacion;
} t_kernel_config;

t_kernel_config *kernel_leer_configuracion(char *path);
void kernel_eliminar_configuracion(t_kernel_config *config);

#endif /* KERNEL_CONFIG_H_ */
