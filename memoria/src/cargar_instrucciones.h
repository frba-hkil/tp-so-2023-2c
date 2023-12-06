#ifndef CARGAR_INSTRUCCIONES_H
#define CARGAR_INSTRUCCIONES_H


#include "memoria_global.h"
#include <stdio.h>

t_list *leer_instrucciones(char* nombre_archivo);
FILE* leer_archivo(char* nombre);
t_op_code string_a_op_code(char* str);

#endif
