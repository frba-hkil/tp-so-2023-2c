#ifndef SRC_PAGINAS_H_
#define SRC_PAGINAS_H_

#include "memoria_global.h"
#include <serializador.h>
#include <mem_sockets.h>

t_tabla_pagina *crear_tp(uint32_t _pid, int);
void retornar_nro_frame(t_paquete* paquete);
t_tabla_pagina *buscar_tabla_proceso(uint32_t _pid);
void manejar_page_fault(t_paquete* paquete);
void asignar_frame(t_entrada_tabla_pagina *etp, uint32_t pid);
void fifo(t_entrada_tabla_pagina *new_etp, uint32_t pid);
void lru(t_entrada_tabla_pagina *new_etp, uint32_t pid);
uint32_t buscar_pid_en_frame(uint32_t nro_frame);
void eliminar_estructuras(t_paquete* paquete);
t_entrada_tabla_pagina *buscar_entrada(t_direccion_fisica* df, uint32_t pid);
void escribir_en_memoria(uint32_t *valor, t_direccion_fisica* df, uint32_t pid);
uint32_t *leer_de_memoria(t_direccion_fisica* df, uint32_t pid);
void liberar_espacio_usuario(t_tabla_pagina* tp);
void eliminar_tabla_de_pagina(t_tabla_pagina *tp);
void eliminar_tabla_de_pagina_proceso(void *tp);
void actualizar_swap(uint32_t pid, uint32_t nro_pagina, void* frame);
#endif /* SRC_PAGINAS_H_ */
