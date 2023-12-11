#ifndef CONFIGURACIONES_H
#define CONFIGURACIONES_H
#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <commons/string.h>



typedef struct 
{
    char* IP_MEMORIA;
    char* PUERTO_ESCUCHA;
    char* IP_FILESYSTEM;
    char* PUERTO_FILESYSTEM;
    uint32_t TAM_MEMORIA;
    uint32_t TAM_PAGINA;
    char* PATH_INSTRUCCIONES;
    uint32_t RETARDO_RESPUESTA;
    char* ALGORITMO_REEMPLAZO;
}t_memoria_config;

extern t_log* LOGGER;
t_log* iniciarLogger();
t_memoria_config* iniciar_config(char *path);
void memoria_eliminar_config(t_memoria_config*);

#endif
