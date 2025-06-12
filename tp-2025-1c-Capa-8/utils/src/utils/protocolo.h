//#include "../utils/src/utils/protocolo.h"
// #include "../../include/estructuras.h"
#include <utils/hello.h>
#include "../../kernel/src/include/estructuras.h"

typedef struct {
    int pid;
    int pc;

} pid_y_pc;

void mandar_prceso_a_cpu (pcb_t* proceso, int modulo);

void empaquetar_proceso_ejecucion(t_paquete* paquete,pcb_t* proceso);
pid_y_pc* desempaquetar_pid_y_pc(t_paquete* paquete);
pcb_t* desempaquetar_pcb_t(t_paquete* paquete);
