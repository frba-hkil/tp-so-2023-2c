#ifndef SRC_PAGINAS_H_
#define SRC_PAGINAS_H_

#include "memoria_global.h"
#include <serializador.h>

t_tabla_pagina *crear_tp(uint32_t _pid);
void retornar_nro_frame(t_paquete* paquete);
t_tabla_pagina *buscar_tabla_proceso(uint32_t _pid);
#endif /* SRC_PAGINAS_H_ */
