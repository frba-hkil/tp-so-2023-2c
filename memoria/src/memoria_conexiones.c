#include "memoria_conexiones.h"

void iniciar_conexiones(t_memoria_config* cfg, t_log* logger) {
	//sockets_m.s_fs = conectar_a_modulo(cfg->IP_FILESYSTEM, cfg->PUERTO_FILESYSTEM, logger);
	sockets_m.s_escucha = iniciar_modulo_servidor("127.0.0.1", cfg->PUERTO_ESCUCHA, logger);

	sockets_m.s_cpu = esperar_cliente(sockets_m.s_escucha);
	sockets_m.s_kernel = esperar_cliente(sockets_m.s_escucha);

	log_info(logger, "todos los modulos conectados");
}
