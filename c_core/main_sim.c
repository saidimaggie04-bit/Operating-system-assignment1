#include "include/eduos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

pid_t edu_fork(PCB *parent);
void edu_exec(pid_t pid, char *prog_name);
int edu_wait(pid_t parent_pid);
void edu_exit(pid_t pid, int exit_code);
void edu_ps();
void write_pcb_snapshot();

int main() {
    printf("=== EduOS Simulator Starting ===\n\n");
    PCB parent;
    parent.pid = 0;
    strncpy(parent.name, "init", 63);
    parent.state = RUNNING;
    parent.priority = 0;
    parent.burst_time = 20;
    parent.arrival_time = 0;
    parent.remaining_time = 20;
    parent.memory_req_kb = 512;
    parent.thread_count = 1;
    parent.owner_id = 1;
    parent.creation_time = time(NULL);
    pid_t p1 = edu_fork(&parent);
    pid_t p2 = edu_fork(&parent);
    pid_t p3 = edu_fork(&parent);
    edu_exec(p1, "calculator");
    edu_exec(p2, "text_editor");
    edu_exec(p3, "file_manager");
    printf("\n--- Process Table ---\n");
    edu_ps();
    write_pcb_snapshot();
    printf("PCB snapshot saved to pcb_snapshot.json\n");
    edu_exit(p1, 0);
    edu_exit(p2, 0);
    edu_exit(p3, 0);
    edu_wait(parent.pid);
    printf("\n--- Final Process Table ---\n");
    edu_ps();
    printf("\n=== EduOS Simulation Complete ===\n");
    return 0;
}
