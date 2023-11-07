#include <stdlib.h>
#include <stdio.h>
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

t_sockets sockets;
t_log *logger;

void ejecutarInstrucciones(t_contexto_ejecucion *contexto, int socket_kernel);
void set_registro(t_contexto_ejecucion *contexto, char *reg, u_int32_t val);
void iniciar_hilos_cpu();
void hilo_dispatch();
void hilo_interrupt();
uint32_t get_registro(t_contexto_ejecucion *contexto, char *reg);
uint32_t transform_value(t_contexto_ejecucion *contexto, char *val);
