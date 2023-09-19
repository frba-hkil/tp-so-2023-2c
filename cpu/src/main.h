#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/error.h>
#include "../../utils/src/utils.h"
#include "../../utils/src/estructuras.h"


void processInstruction(char *code);uint32_t get_registro(t_pcb *pcb, char *reg);
uint32_t transform_value(t_pcb *pcb, char *val);