#include "configuraciones.h"


t_log* iniciarLogger(){
    t_log* logger = log_create("./cfg/memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
    return logger;
}


t_memoria_config* iniciar_config(char *path){
    t_config* config = config_create(path); 

    t_memoria_config* memoria_config = malloc(sizeof(t_memoria_config));
    
    memoria_config->IP_MEMORIA = string_new();
    string_append(&memoria_config->IP_MEMORIA, config_get_string_value(config, "IP_MEMORIA"));
    memoria_config->PUERTO_ESCUCHA = string_new();
    string_append(&memoria_config->PUERTO_ESCUCHA, config_get_string_value(config, "PUERTO_ESCUCHA"));
    memoria_config->IP_FILESYSTEM = string_new();
    string_append(&memoria_config->IP_FILESYSTEM, config_get_string_value(config, "IP_FILESYSTEM"));
    memoria_config->PUERTO_FILESYSTEM = string_new();
    string_append(&memoria_config->PUERTO_FILESYSTEM, config_get_string_value(config, "PUERTO_FILESYSTEM"));
    memoria_config->TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
    memoria_config->TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
    memoria_config->PATH_INSTRUCCIONES = string_new();
    string_append(&memoria_config->PATH_INSTRUCCIONES, config_get_string_value(config, "PATH_INSTRUCCIONES"));
    memoria_config->RETARDO_RESPUESTA = config_get_int_value(config, "RETARDO_RESPUESTA");
    memoria_config->ALGORITMO_REEMPLAZO = string_new();
    string_append(&memoria_config->ALGORITMO_REEMPLAZO, config_get_string_value(config, "ALGORITMO_REEMPLAZO"));

    config_destroy(config);

    log_info(LOGGER,"Me traje todos datos de config");

    return memoria_config;
    
}