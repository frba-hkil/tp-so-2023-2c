#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/error.h>
#include "../../utils/src/utils.h"
#include "../../utils/src/estructuras.h"

char *nombres_registros[4] = {"AX", "BX", "CX", "DX"};

void procesarInstrucciones(char *code, t_pcb *pcb);
void ejecutarInstruccion(char *tipo, char **args, t_pcb *pcb);
void set_registro(t_pcb *pcb, char *reg, u_int32_t val);
uint32_t get_registro(t_pcb *pcb, char *reg);
uint32_t transform_value(t_pcb *pcb, char *val);