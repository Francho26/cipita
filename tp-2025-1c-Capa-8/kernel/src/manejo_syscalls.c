#include "./include/estructuras.h"
#include <unistd.h>

void atender_syscall(int socket_consola) {
    t_syscall* syscall = recibir_syscall(socket_consola);

    switch (syscall->tipo) {
        case INIT_PROC:
            pcb_t* pcb = crear_pcb(syscall->pid, syscall->size);
            log_info(logger, "## (%d) Se crea el proceso - Estado: NEW", pcb->pid);
            pthread_mutex_lock(&mutex_cola_new);
            queue_push(cola_new, pcb);
            pthread_mutex_unlock(&mutex_cola_new);
            break;

        case EXIT:
            // Manejar la finalización del proceso
            finalizar_proceso(syscall->pid);
            break;

        case IO:
            // Manejar la solicitud de IO
            manejar_solicitud_io(syscall->pid, syscall->dispositivo, syscall->duracion);
            break;

        default:
            log_warning(logger, "Syscall desconocida recibida");
            break;
    }

    destruir_syscall(syscall);
}


void destruir_syscall(t_syscall* syscall) {
    if (!syscall) return;

    if (syscall->nombre_archivo)
        free(syscall->nombre_archivo);

    if (syscall->dispositivo)
        free(syscall->dispositivo);

    free(syscall);
}

t_syscall* recibir_syscall(int socket_cpu) {
    t_syscall* syscall = calloc(1, sizeof(t_syscall));

    int tipo_syscall;
    recv(socket_cpu, &tipo_syscall, sizeof(int), 0);
    syscall->tipo = tipo_syscall;

    recv(socket_cpu, &syscall->pid, sizeof(int), 0);

    switch (syscall->tipo) {
        case SYSCALL_INIT_PROC: {
            // Recibir string archivo
            int len_archivo;
            recv(socket_cpu, &len_archivo, sizeof(int), 0);

            syscall->nombre_archivo = malloc(len_archivo);
            recv(socket_cpu, syscall->nombre_archivo, len_archivo, 0);

            // Recibir tamaño
            recv(socket_cpu, &syscall->tamanio, sizeof(int), 0);
            break;
        }

        case SYSCALL_IO: {
            int len_disp;
            recv(socket_cpu, &len_disp, sizeof(int), 0);

            syscall->dispositivo = malloc(len_disp);
            recv(socket_cpu, syscall->dispositivo, len_disp, 0);

            recv(socket_cpu, &syscall->duracion, sizeof(int), 0);
            break;
        }

        case SYSCALL_EXIT:
        case SYSCALL_DUMP_MEMORY:
            // Solo reciben PID, ya lo hicimos arriba
            break;

        default:
            log_error(logger, "Syscall desconocida recibida");
            free(syscall);

