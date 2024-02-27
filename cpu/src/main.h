#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/error.h>
#include "../../utils/src/utils.h"
#include "../../utils/src/conexiones.h"
#include "../../utils/src/estructuras.h"
#include "../../utils/src/serializador.h"
#include <pthread.h>


char *nombres_registros[4] = {"AX", "BX", "CX", "DX"};
typedef struct {
    int dispatch;
    int interrupt;
    int memoria;
} t_sockets;


bool desalojar = false;
bool terminar = false;
uint32_t page_size;
uint32_t nro_pagina;
uint32_t desplazamiento;
t_sockets sockets;
t_log *logger;

void ejecutarInstrucciones(t_contexto_ejecucion *contexto, int socket_kernel);
void set_registro(t_contexto_ejecucion *contexto, char *reg, u_int32_t val);
void iniciar_hilos_cpu();
void hilo_dispatch();
void hilo_interrupt();
char *getInstruccionLabel(t_op_code iId);
uint32_t get_registro(t_contexto_ejecucion *contexto, char *reg);
uint32_t transform_value(t_contexto_ejecucion *contexto, char *val);
void leer_de_memoria(t_contexto_ejecucion *contexto, char* reg, uint32_t *nro_frame, uint32_t desplazamiento);
uint32_t *solicitar_nro_frame(uint32_t *n_pagina, uint32_t pid);
uint32_t get_page_size(void);
void escribir_en_memoria(t_contexto_ejecucion *contexto, char* reg, uint32_t *nro_frame, uint32_t desplazamiento);
