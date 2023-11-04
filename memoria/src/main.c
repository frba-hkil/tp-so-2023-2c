#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/log.h>
#include "configuraciones.h"
#include "../../utils/src/conexiones.h"

t_log* LOGGER;
t_memoria_config* MEMORIA_CONFIG;

int main() {

    LOGGER=iniciarLogger();
    MEMORIA_CONFIG = iniciarConfig();

    printf("El IP es: %s El puerto es: %s", MEMORIA_CONFIG->IP_FILESYSTEM, MEMORIA_CONFIG->PUERTO_ESCUCHA);

    iniciar_modulo_servidor(MEMORIA_CONFIG->IP_MEMORIA, MEMORIA_CONFIG->PUERTO_ESCUCHA, LOGGER);

    log_info (LOGGER, "Iniciamos server");

    conectar_a_modulo(MEMORIA_CONFIG->IP_FILESYSTEM, MEMORIA_CONFIG->PUERTO_FILESYSTEM, LOGGER);

    log_info(LOGGER, "Logramos ser cliente");
    return 0;
}
