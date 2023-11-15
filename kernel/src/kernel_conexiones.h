#ifndef SRC_KERNEL_CONEXIONES_H_
#define SRC_KERNEL_CONEXIONES_H_

#include "kernel_global.h"

#define SOCK_CPU_DISPATCH 0
#define SOCK_CPU_INT 1
#define SOCK_MEM 2
#define SOCK_FS 3

void iniciar_conexiones_kernel(int*);

#endif /* SRC_KERNEL_CONEXIONES_H_ */
