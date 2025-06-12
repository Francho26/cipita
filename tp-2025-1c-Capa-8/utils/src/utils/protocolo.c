#include "protocolo.h"
#include <utils/hello.h>

void mandar_proceso_a_cpu(pcb_t* proceso, int modulo){
    t_paquete* paquete = crear_paquete(PROCESO);
    empaquetar_proceso_ejecucion(paquete,proceso);
    enviar_paquete(paquete, modulo);
    eliminar_paquete(paquete);    
    return;
}

void empaquetar_proceso_ejecucion(t_paquete* paquete, pcb_t* proceso){
    int i;
    agregar_a_paquete(paquete, &(proceso->pid), sizeof(int)); 
    agregar_a_paquete(paquete, &(proceso->pc), sizeof(int));
    for(i=0;i<7;i++){
        agregar_a_paquete(paquete, &(proceso->me[i]), sizeof(int));
    }
    for(i=0;i<7;i++){
        agregar_a_paquete(paquete, &(proceso->mt[i]), sizeof(int));
    }
    agregar_a_paquete(paquete, &(proceso->estado), sizeof(estado_proceso));
    return;
}
//recibir paquete
//actualizar pb
//antes de reci 
/*pid_y_pc* datos = desempaquetar_pid_y_pc(paquete);
printf("Recibí pid=%d, pc=%d\n", datos->pid, datos->pc);
free(datos);*/

pcb_t* desempaquetar_pcb_t(t_paquete* paquete) {
    pcb_t* proceso = malloc(sizeof(pcb_t));

    int offset = 0,i;   
    memcpy(&(proceso->pid), paquete->buffer->stream + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&(proceso->pc), paquete->buffer->stream + offset, sizeof(int));
    offset += sizeof(int);
    for(i=0;i<7;i++){
        memcpy(&(proceso->me[i]),paquete->buffer->stream + offset,sizeof(int));
        offset+=sizeof(int);
    }
    for(i=0;i<7;i++){
        memcpy(&(proceso->mt[i]),paquete->buffer->stream + offset,sizeof(int));
        offset+=sizeof(int);
    }
    memcpy(&(proceso->estado),paquete->buffer->stream + offset,sizeof(estado_proceso));
    offset+=sizeof(estado_proceso);
    eliminar_paquete(paquete);
    return proceso;
}
// opcode recibir oppa
 ////
/*kernel*/
//mandar_prceso_a_cpu(proceso, cpu);