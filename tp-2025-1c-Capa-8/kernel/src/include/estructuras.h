#ifndef ESTRUCTURAS_HILOS
#define ESTRUCTURAS_HILOS

#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/collections/queue.h>


//STRUCTURAS
//---------------------------------------------------------------
typedef enum {
	NEW,
	READY,
	BLOCKED,
	EXEC,
	EXIT, // <--- revisar nombre, conflicto con syscall EXIT
	SUSP_BLOKED,
	SUSP_READY
} estado_proceso;

typedef struct{
	int pid;
	int pc;
	int me[7];
	int mt[7];
	estado_proceso estado;
} pcb_t;

typedef enum {
    NOOP,
    WRITE,
    READ,
    GOTO,
    IO_SYS,
    INIT_PROC_SYS,
    DUMP_MEMORY_SYS,
    EXIT_SYS
} instruccion_t;
// typedef enum {
//     INIT_PROC,
//     //EXIT, // <--- revisar nombre, conflicto con syscall EXIT
//     IO,
//     DUMP_MEMORY
// } syscall_type;

typedef struct {
    char* nombre_io;
    int socket_io;
    t_queue* cola_bloqueados;
    bool ocupado;
} dispositivo_io_t;

// typedef struct {
//     int pid;
//     syscall_type tipo;

//     // Parámetros opcionales según la syscall
//     char* nombre_archivo;     // INIT_PROC
//     int tamanio;              // INIT_PROC

//     char* dispositivo;        // IO
//     int duracion;             // IO
// } t_syscall;

typedef struct {
    pcb_t* pcb;
    char dispositivo[16];
    int duracion;
} t_io_request;

typedef struct{
	t_log* logger;
	int puerto;
} argumentos_conexiones;
typedef struct{
	char* ip;
	t_log* logger;
	int puerto;
} argumentos_memoria;

typedef enum {
    RESPUESTA_EXIT,
    RESPUESTA_IO
} tipo_respuesta_cpu;

typedef struct {
    int pid;
    tipo_respuesta_cpu tipo;
    char dispositivo[32];  // solo si es IO
    int duracion;          // en milisegundos, solo si es IO
} t_respuesta_cpu;

extern t_list* cola_new;
extern t_list* cola_ready;
extern t_list* cola_exec;
extern t_list* cola_blocked;
extern t_list* cola_exit;
extern t_queue* cola_io;
extern t_list* dispositivos_io;
//-----------------------------------------------------------------

//SEMAFOROS
//-------------------------------------------------------------------------
extern pthread_mutex_t mutex_cola_new;
extern pthread_mutex_t mutex_cola_ready;
extern pthread_mutex_t mutex_cola_exec;
extern pthread_mutex_t mutex_cola_blocked;
extern pthread_mutex_t mutex_cola_exit;
extern pthread_mutex_t mutex_cola_io;

extern sem_t sem_procesos_en_ready;
extern sem_t sem_procesos_en_new;
extern sem_t multiprogramacion_disponible;
//----------------------------------------------------------------------------

#endif