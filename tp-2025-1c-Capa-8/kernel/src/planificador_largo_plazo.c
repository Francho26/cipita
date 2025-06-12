#include "./include/estructuras.h"
#include <unistd.h>

comparar_por_tamanio(void* a, void* b) {
    return ((pcb_t*)a)->tamanio < ((pcb_t*)b)->tamanio;
}

// void* planificador_largo_plazo(void* args) {

//     printf("Sistema en estado STOP. presione enter para continuar ... \n");
//     getchar();
//     printf("Planificacion iniciada");


//     while(1) {
//         sem_wait(&sem_procesos_en_new);

//         Boolean cola_vacia = list_is_empty(cola_ready);

//         if(algoritmo == "FIFO" ) {
//         agregar_a_new_FIFO(pcb);

//         } else if (algoritmo == "PMCP") {
//             agregar_a_new_PCMP(pcb);
//         }

//         pthread_mutex_lock(&mutex_cola_new);
//         t_pcb* pcb = list_remove(cola_new, 0);
//         pthread_mutex_unlock(&mutex_cola_new);

//         log_info(logger, "%s Se crea el proceso - Estado: NEW", pcb->pid);

//         //mover a ready

//         if(cola_vacia){
//                 //Logica  de preguntar a memoria, hacer cuando tengamos paquetes listos
//         }


//         pthread_mutex_lock(&mutex_cola_ready);
//         list_add(cola_ready, pcb);
//         pthread_mutex_unlock(&mutex_cola_ready);

//         sem_post(&sem_procesos_en_ready)

       



//     }

//     void agregar_a_new_FIFO(t_pcb* pcb) {
//     pthread_mutex_lock(&mutex_cola_new);
//     list_add(cola_new, pcb);
//     pthread_mutex_unlock(&mutex_cola_new);

//     log_info(logger, "Nuevo proceso PID: %d agregado a NEW", pcb->pid);
//     sem_post(&hay_proceso_en_new);

//     void agregar_a_new_PCMP(t_pcb* pcb) {
//     pthread_mutex_lock(&mutex_cola_new);
//     list_add(cola_new, pcb);
//     list_sort(cola_new, comparar_por_tamanio);
//     pthread_mutex_unlock(&mutex_cola_new);

//     log_info(logger, "Nuevo proceso PID: %d agregado a NEW", pcb->pid);
//     sem_post(&hay_proceso_en_new);
// }


// }
// }
void* planificador_largo_plazo_fifo(void* arg) {
    printf("Sistema en estado STOP. presione enter para continuar ... \n");
    getchar();
    printf("Planificacion iniciada");

    while (1) {
        sem_wait(&sem_multiprogramacion); // Controla el grado de multiprogramación

        pthread_mutex_lock(&mutex_cola_new);
        if (!queue_is_empty(cola_new)) {
            pcb_t* pcb = queue_pop(cola_new);
            pthread_mutex_unlock(&mutex_cola_new);

            // Solicitar a Memoria la creación del proceso
            if (solicitar_creacion_proceso_memoria(pcb)) {
                cambiar_estado(pcb, READY);
                pthread_mutex_lock(&mutex_cola_ready);
                queue_push(cola_ready, pcb);
                pthread_mutex_unlock(&mutex_cola_ready);
                sem_post(&sem_procesos_en_ready)
                log_info(logger, "## (%d) Pasa del estado NEW al estado READY", pcb->pid);
            } else {
                // Manejar el caso en que Memoria no pueda crear el proceso
                log_error(logger, "## (%d) Memoria no pudo crear el proceso", pcb->pid);
                destruir_pcb(pcb);
            }
        } else {
            pthread_mutex_unlock(&mutex_cola_new);
        }

        sleep(1); // Evita el consumo excesivo de CPU
    }
}
