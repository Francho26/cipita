#ifndef IO_H
#define IO_H

#include <commons/config.h>
#include <commons/log.h>

extern int PUERTO_KERNEL;
extern char* IP_KERNEL;
extern char* LOG_LEVEL;

// Funciones para inicializar log y configuración
t_log* inicializar_io_log();
t_config* inicializar_io_config(void);

#endif