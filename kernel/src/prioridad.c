#include "prioridad.h"

t_pcb* prioridades(t_list* procesos_en_ready) {
	t_pcb *pcb;


	if(list_size(lista_ready) >= 2) {
		pcb = list_get_minimum(lista_ready, mayor_prioridad);
		list_remove_element(lista_ready, pcb);
	}
	else {
		pcb = list_remove(lista_ready, 0);
	}
	return pcb;
}

void* mayor_prioridad(void* pcb1, void* pcb2) {
	t_pcb*_pcb1 = (t_pcb*)pcb1;
	t_pcb*_pcb2 = (t_pcb*)pcb2;

	return (_pcb1->prioridad < _pcb2->prioridad) ? _pcb1 : _pcb2;
}
