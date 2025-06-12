#ifndef CPU_H
#define CPU_H

#include <commons/log.h>
#include <commons/config.h>

extern char* IP_MEMORIA;
extern int PUERTO_MEMORIA;
extern char* IP_KERNEL;
extern int PUERTO_KERNEL_DISPATCH;
extern int PUERTO_KERNEL_INTERRUPT;
extern int ENTRADAS_TLB;
extern char* REEMPLAZO_TLB;
extern int ENTRADAS_CACHE;
extern char* REEMPLAZO_CACHE;
extern int RETARDO_CACHE;
extern char* LOG_LEVEL;

#define MAX_ARGS 3
#define MAX_ARG_LENGTH 64
#define MAX_LINEA 256

typedef struct {
  char* ip;
  int puerto;
  char* identificador;
  t_log* logger;
} argumentos_conexion;

// typedef enum {
//   NOOP,
//   WRITE,
//   READ,
//   GOTO,
//   IO,
//   INIT_PROC,
//   DUMP_MEMORY,
//   EXIT
// } instruccion_t;

typedef struct{
  instruccion_t tipo;
  int nargs;
  char args[MAX_ARGS][MAX_ARG_LENGTH];
} instruccion_t_completa;

// Funciones de inicialización
t_log* inicializar_cpu_log(char* identificador_cpu);
t_config* inicializar_cpu_config(void);

// Carga argumentos para hilos
argumentos_conexion* cargar_argumentos(char* ip, t_log* logger, int  puerto, char* identificador);

// Hilos
void* conectar_kernel_dispatch(void* args);
void* conectar_kernel_interrupt(void* args);

// Funciones para instrucciones
instruccion_t_completa* parsear_instruccion(const char* linea_original);
instruccion_t instruccion_from_str(const char* str);
const char* instruccion_to_string(instruccion_t tipo);


#endif