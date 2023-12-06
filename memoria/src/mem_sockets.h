#ifndef SRC_MEM_SOCKETS_H_
#define SRC_MEM_SOCKETS_H_

#include <stdlib.h>

typedef struct {
	int s_escucha;
	int s_kernel;
	int s_cpu;
	int s_fs;
} t_socket_m;

extern t_socket_m sockets_m;

#endif /* SRC_MEM_SOCKETS_H_ */
