#ifndef MEMORIA_GLOBAL_H_
#define MEMORIA_GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/bitarray.h>
#include <commons/temporal.h>
#include <commons/string.h>
#include <math.h>
#include <protocolo.h>
#include <conexiones.h>
#include <estructuras.h>
#include <serializador.h>
#include "configuraciones.h"

t_log *memoria_logger;
t_memoria_config *memoria_config;
t_socket sockets;
void *memoria; //representa el espacio de usuario
uint32_t cantidad_frames; // tam_memoria / tam_pagina
t_bitarray *bitmap_frames;
t_temporal *timer_lru;
void (*reemplazar_pagina)(t_entrada_tabla_pagina*, uint32_t);
t_queue *paginas_en_memoria;

t_list *tablas_de_paginas; //lista donde se almacena la tabla de pagina x proceso
t_list *bitmap_marcos;
t_list *archivos_swap;
t_dictionary *instrucciones_de_procesos; //memoria de instrucciones
pthread_mutex_t mutex_swap;
pthread_mutex_t mutex_memoria;
uint32_t cantidad_acceso_disco;
uint32_t cantidad_page_fault;



#endif /* MEMORIA_GLOBAL_H_ */
