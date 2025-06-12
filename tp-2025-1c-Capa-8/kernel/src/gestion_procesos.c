#include "./include/estructuras.h"
#include <stdlib.h>
#include <stdio.h>

int pid_actual = 0;

pcb_t* crear_proceso() {
    pcb_t* nuevo_proceso = malloc(sizeof(pcb_t));
    nuevo_proceso->pid = pid_actual++;
    nuevo_proceso->estado = NEW;
    nuevo_proceso->pc =0;
    nuevo_proceso->me = {0,0,0,0,0,0,0};
    nuevo_proceso->mt = {0,0,0,0,0,0,0};
    nuevo_proceso->tamanio =0 ;
    return nuevo_proceso;
}

void destruir_pcb(pcb_t* proceso){
  free(proceso->pc);
  free(proceso->pid);
  free(proceso->me);
  free(proceso->mt);
  free(proceso->estado);
}

void mover_a_ready(pcb_t* proceso) {
    pthread_mutex_lock(&mutex_cola_ready); //bloqueo el mutex
    list_add(cola_ready, proceso);
    proceso->estado = READY;
    pthread_mutex_unlock(&mutex_cola_ready);
    log_info(logger, "%d, pasa del estado NEW al READY", proceso->pid);
    sem_post(&sem_proceso_en_ready);
}

void cambiar_estado(t_pcb* pcb, estado_proceso nuevo_estado) {
    log_info(logger, "## (%d) Pasa del estado %s al estado %s",
             pcb->pid,
             pcb->estado_actual,
             nuevo_estado;
    pcb->estado_actual = nuevo_estado;
    
      // Encolar en la nueva cola
    switch (nuevo_estado) {
        case NEW:
            pthread_mutex_lock(&mutex_cola_new);
            queue_push(cola_new, pcb);
            pthread_mutex_unlock(&mutex_cola_new);
            break;

        case READY:
            pthread_mutex_lock(&mutex_cola_ready);
            queue_push(cola_ready, pcb);
            pthread_mutex_unlock(&mutex_cola_ready);
            break;

        case BLOCKED:
            pthread_mutex_lock(&mutex_cola_blocked);
            queue_push(cola_blocked, pcb);
            pthread_mutex_unlock(&mutex_cola_blocked);
            break;

        case EXIT:
            pthread_mutex_lock(&mutex_cola_exit);
            queue_push(cola_exit, pcb);
            pthread_mutex_unlock(&mutex_cola_exit);
            break;

        case EXEC:
            // No se encola en EXEC porque se está ejecutando, se gestiona por CPU
            break;
    }
    

}

void solicitar_creacion_proceso_memoria(){

}

void recibir_respuesta_cpu(){

}

void finalizar_proceso(){
}

void finalizar_proceso(){

}
