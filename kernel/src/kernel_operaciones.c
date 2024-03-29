#include "kernel_operaciones.h"

void escuchar_consola(void) {

	generador_de_id = 0;
	plani_running = 0;
	command_handlers[0] = iniciar_proceso;
	command_handlers[1] = finalizar_proceso;
	command_handlers[2] = detener_planificacion;
	command_handlers[3] = iniciar_planificacion;
	command_handlers[4] = cambiar_multiprogramacion;
	cola_new = queue_create();
	cola_exit = queue_create();
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_plani_running, NULL);
	sem_init(&sem_planificacion_l, 0, 0);
	sem_init(&sem_planificacion_c, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, kernel_config->grado_multiprogramacion);
	sem_init(&sem_new, 0, 0);
	sem_init(&sem_exit, 0, 0);
	sem_init(&sem_sleep, 0, 0);
	cargar_recursos();
	//sleep(1);
	pthread_create(&plani_largo_thread, NULL, (void*) plani_largo_pl, NULL);
	pthread_create(&plani_corto_thread, NULL, (void*) plani_corto_pl, kernel_config->algoritmo_planificacion);

	while(1){
		sem_wait(&sem_consola);

		//pthread_mutex_lock(&mutex_inst_consola);
		command_handlers[codigo_consola](parametros_consola);
		//pthread_mutex_unlock(&mutex_inst_consola);

	}
}

void iniciar_proceso(char** parametros) {
	t_pcb* new_pcb;

	cargar_instrucciones(parametros[0], generador_de_id, atoi(parametros[1]));
	new_pcb = crear_pcb(generador_de_id++, atoi(parametros[1]), atoi(parametros[2]));

	pthread_mutex_lock(&mutex_new);
	queue_push(cola_new, new_pcb);
	pthread_mutex_unlock(&mutex_new);
	sem_post(&sem_new);
	log_info(kernel_logger, "Se crea el proceso <%d> en NEW", new_pcb->contexto->pid);

}

void finalizar_proceso(char** parametros) {
	//si esta en running(en cpu), mando una interrupcion para terminar
	//espero (hago un sem_wait) hasta que la cpu me devuelta el contexto
	//si esta en cualquier otro estado lo termino en el instante
	//funcion: liberar recursos y cambiar estado a EXIT
	//signal(grado multiprogramacion)
	fin_proc(atoi(parametros[0]));
}

void iniciar_planificacion(char** parametros) {

	if(!plani_running) {
		pthread_mutex_lock(&mutex_plani_running);
		plani_running = 1;
		pthread_mutex_unlock(&mutex_plani_running);
		sem_post(&sem_planificacion_l);
		sem_post(&sem_planificacion_c);
		pthread_cond_signal(&cond_plani_running);
		//sem_post(&sem_sleep);
	}

}

void detener_planificacion(char** parametros) {

	if(plani_running) {
		pthread_mutex_lock(&mutex_plani_running);
		plani_running = 0;
		pthread_mutex_unlock(&mutex_plani_running);
	}
}

void cambiar_multiprogramacion(char** parametros) {
	sem_destroy(&sem_grado_multiprogramacion);
	sem_init(&sem_grado_multiprogramacion, 0, atoi(parametros[0]));
}

void cargar_instrucciones(char* fpath, uint32_t pid, uint32_t proc_size) {
	t_paquete* packet = crear_paquete(INICIALIZACION_DE_PROCESO, buffer_vacio());

	agregar_a_paquete(packet, &pid, sizeof(uint32_t));
	agregar_a_paquete(packet, &proc_size, sizeof(uint32_t));
	agregar_a_paquete(packet, fpath, strlen(fpath) + 1);

	enviar_paquete(packet, sockets[SOCK_MEM]);
}

void cargar_recursos(void) {
	char** nombres_recursos = string_get_string_as_array(kernel_config->recursos);
	char** instancias_recursos = string_get_string_as_array(kernel_config->instancias_recursos);
	int i, cant_recursos = string_array_size(nombres_recursos);
	int32_t *instancia_recurso;

	recursos_sistema = dictionary_create();
	colas_blocked = dictionary_create();

	for(i = 0 ; i < cant_recursos ; i++) {
		instancia_recurso = malloc(sizeof(int32_t));
		*instancia_recurso = atoi(instancias_recursos[i]);
		dictionary_put(recursos_sistema, nombres_recursos[i], instancia_recurso);

		t_queue *cola_blocked = queue_create();
		dictionary_put(colas_blocked, nombres_recursos[i], cola_blocked);

	}
}

