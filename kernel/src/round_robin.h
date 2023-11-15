#ifndef SRC_ROUND_ROBIN_H_
#define SRC_ROUND_ROBIN_H_

#include <estructuras.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/temporal.h>
#include "ready.h"
#include "kernel_global.h"

void round_robin(t_list*);

void contar_quantum(void);

#endif /* SRC_ROUND_ROBIN_H_ */
