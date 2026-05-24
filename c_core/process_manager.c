#include "include/eduos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PROCESSES 50
static PCB pcb_table[MAX_PROCESSES];
static int pcb_count = 0;
static int next_pid = 1;

static void print_timestamp() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);
}

pid_t edu_fork(PCB *parent) {
    if (pcb_count >= MAX_PROCESSES) {
        fprintf(stderr, "ERROR: PCB table full\n");
        return -1;
    }
    PCB child;
    memcpy(&child, parent, sizeof(PCB));
    child.pid = next_pid++;
    child.state = READY;
    child.creation_time = time(NULL);
    pcb_table[pcb_count++] = child;
    print_timestamp();
    printf("edu_fork: created PID=%lld\n", (long long)child.pid);
    return child.pid;
}

void edu_exec(pid_t pid, char *prog_name) {
    for (int i = 0; i < pcb_count; i++) {
        if (pcb_table[i].pid == pid) {
            strncpy(pcb_table[i].name, prog_name, 63);
            pcb_table[i].burst_time = 10;
            pcb_table[i].remaining_time = 10;
            print_timestamp();
            printf("edu_exec: PID=%lld runs '%s'\n", (long long)pid, prog_name);
            return;
        }
    }
}

int edu_wait(pid_t parent_pid) {
    print_timestamp();
    printf("edu_wait: PID=%lld waiting\n", (long long)parent_pid);
    for (int i = 0; i < pcb_count; i++)
        pcb_table[i].state = TERMINATED;
    return 0;
}

void edu_exit(pid_t pid, int exit_code) {
    for (int i = 0; i < pcb_count; i++) {
        if (pcb_table[i].pid == pid) {
            pcb_table[i].state = TERMINATED;
            pcb_table[i].exit_code = exit_code;
            print_timestamp();
            printf("edu_exit: PID=%lld code=%d\n", (long long)pid, exit_code);
            return;
        }
    }
}

void edu_ps() {
    printf("\n%-6s %-20s %-10s %-8s %-10s\n",
           "PID","NAME","STATE","PRIORITY","BURST");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < pcb_count; i++) {
        char *s;
        switch(pcb_table[i].state) {
            case NEW: s="NEW"; break;
            case READY: s="READY"; break;
            case RUNNING: s="RUNNING"; break;
            case WAITING: s="WAITING"; break;
            case TERMINATED: s="TERMINATED"; break;
            default: s="UNKNOWN"; break;
        }
        printf("%-6lld %-20s %-10s %-8d %-10d\n",
               (long long)pcb_table[i].pid,
               pcb_table[i].name, s,
               pcb_table[i].priority,
               pcb_table[i].burst_time);
    }
    printf("\n");
}

void write_pcb_snapshot() {
    FILE *f = fopen("pcb_snapshot.json","w");
    if (!f) { perror("fopen"); return; }
    fprintf(f,"[\n");
    for (int i = 0; i < pcb_count; i++) {
        fprintf(f,"  {\"pid\":%lld,\"name\":\"%s\",\"state\":%d,"
                "\"priority\":%d,\"burst_time\":%d,\"arrival_time\":%d,"
                "\"remaining_time\":%d,\"memory_req_kb\":%d,\"owner_id\":%d}%s\n",
                (long long)pcb_table[i].pid, pcb_table[i].name,
                pcb_table[i].state, pcb_table[i].priority,
                pcb_table[i].burst_time, pcb_table[i].arrival_time,
                pcb_table[i].remaining_time, pcb_table[i].memory_req_kb,
                pcb_table[i].owner_id, i<pcb_count-1?",":"");
    }
    fprintf(f,"]\n");
    fclose(f);
}
