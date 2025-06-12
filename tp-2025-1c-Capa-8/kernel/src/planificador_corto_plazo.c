#include "./include/estructuras.h"
#include <unistd.h>

// void* planificador_corto_plazo(void* arg) {  //argumento para pasarle una funcion a pthread_create()
//     while(1) {
//         sem_wait(&sem_procesos_en_ready);

//         pthread_mutex_lock(&mutex_cola_ready);
//         pcb_t* proceso = list_remove(cola_ready, 0);
//         pthread_mutex_unlock(&mutex_cola_ready);

//         pthread_mutex_lock(&mutex_cola_exec);
//         list_add(cola_exec, proceso);
//         proceso->estado = EXEC;
//         log_info(logger, "%d, pasa del estado READY al EXEC", proceso->pid);
//         pthread_mutex_unlock(&mutex_cola_exec);

//         //enviar cpu
//         // t_paquete* paquete = crear_paquete()
//     }
//     return NULL;
// }

void* planificador_corto_plazo_fifo(void* arg) {
    while (1) {
        sem_wait(&sem_procesos_en_ready);
        pthread_mutex_lock(&mutex_cola_ready);
        if (!queue_is_empty(cola_ready)) {
            pcb_t* pcb = queue_pop(cola_ready);
            pthread_mutex_unlock(&mutex_cola_ready);

            cambiar_estado(pcb, EXEC);
            log_info(logger, "## (%d) Pasa del estado READY al estado EXEC", pcb->pid);

            // Enviar PCB a la CPU
            enviar_pcb_a_cpu(pcb);

            // Esperar respuesta de la CPU
            t_respuesta_cpu* respuesta = recibir_respuesta_cpu();

            if (respuesta->tipo == EXIT) {
                cambiar_estado(pcb, EXIT);
                log_info(logger, "## (%d) Pasa del estado EXEC al estado EXIT", pcb->pid);
                destruir_pcb(pcb);
            } else if (respuesta->tipo == IO) {
                cambiar_estado(pcb, BLOCKED);
                log_info(logger, "## (%d) Pasa del estado EXEC al estado BLOCKED", pcb->pid);
                // Enviar a la cola de IO correspondiente
                encolar_proceso_en_io(pcb, respuesta->dispositivo, respuesta->duracion);
            }

            free(respuesta);
        } else {
            pthread_mutex_unlock(&mutex_cola_ready);
            sleep(1); // Evita el consumo excesivo de CPU
        }
    }
}

void* planificador_io(void* arg) {
    char* dispositivo = (char*) arg;
    t_queue* cola_io = obtener_cola_io(dispositivo);

    while (1) {
        pthread_mutex_lock(&mutex_cola_io);
        if (!queue_is_empty(cola_io)) {
            t_io_request* solicitud = queue_pop(cola_io);
            pthread_mutex_unlock(&mutex_cola_io);

            log_info(logger, "## (%d) - Bloqueado por IO: %s", solicitud->pcb->pid, dispositivo);
            sleep(solicitud->duracion); // Simula la operación de IO

            cambiar_estado(solicitud->pcb, READY);
            pthread_mutex_lock(&mutex_cola_ready);
            queue_push(cola_ready, solicitud->pcb);
            pthread_mutex_unlock(&mutex_cola_ready);
            log_info(logger, "## (%d) finalizó IO y pasa a READY", solicitud->pcb->pid);

            free(solicitud);
        } else {
            pthread_mutex_unlock(&mutex_cola_io);
            sleep(1); // Evita el consumo excesivo de CPU
        }
    }
}

