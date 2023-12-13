#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include "protocolo.h"
#include "paquete.h"
#include "estructuras.h"


t_paquete *serializar_pcb(t_pcb *proceso, t_protocolo protocolo);
t_pcb *deserializar_pcb(t_paquete *paquete);

t_paquete *serializar_contexto_ejecucion(t_contexto_ejecucion*, t_protocolo);
void deserializar_contexto_ejecucion(t_contexto_ejecucion*, t_paquete*);

t_paquete *serializar_instruccion(t_instruccion *instruccion, t_protocolo protocolo);
t_instruccion *deserializar_instruccion(t_paquete *paquete);
t_paquete *serializar_instrucciones(t_list *instrucciones, t_protocolo protocolo);
t_list *deserializar_instrucciones(t_list *datos, uint32_t longitud_datos);
t_paquete *serializar_valores_enteros(t_list* valores, t_protocolo protocolo);
t_list *deserializar_valores_enteros(t_paquete *paquete);


#endif /* SERIALIZADOR_H_ */
