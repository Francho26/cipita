#ifndef MEMORIA_H
#define MEMORIA_H

#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <pthread.h>

typedef struct{
  int puerto;
  t_log* logger;
} argumentos_servidor_memoria;

typedef struct{
  int pid;
  int paginas_asign;
  int* marcos_asign;
  char** instrucciones;
} proceso_mem;

extern int PUERTO_MEMORIA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern int ENTRADAS_POR_TABLA;
extern int CANTIDAD_NIVELES;
extern int RETARDO_MEMORIA;
extern char* PATH_SWAPFILE;
extern int RETARDO_SWAP;
extern char* LOG_LEVEL;
extern char* DUMP_PATH;
extern char* PATH_INSTRUCCIONES;

t_config* inicializar_config(void);
t_log* inicializar_log(void);

argumentos_servidor_memoria* cargar_argumentos(t_log* logger, int puerto);
void* atender_cliente(void* arg);

extern void* espacio_usuario;
extern t_bitarray* bitmap_marcos;
extern t_log* logger;
extern t_list* lista_procesos;

/*
SEMAFOROS
*/

extern pthread_mutex_t* acc_lista_proc;

#endif MEMORIA_H