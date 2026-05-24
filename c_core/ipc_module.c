#include "include/eduos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int owner_id;
    int pid;
    char name[64];
    int state;
    int burst_time;
    int remaining_time;
    int memory_req_kb;
    int access_count;
} SharedMetrics;

static SharedMetrics shared_mem;
static int shm_initialized = 0;

void shm_init(int owner_id) {
    memset(&shared_mem, 0, sizeof(SharedMetrics));
    shared_mem.owner_id = owner_id;
    shm_initialized = 1;
    printf("[IPC] Shared memory initialized for owner_id=%d\n", owner_id);
}

int shm_write(PCB *pcb) {
    if (!shm_initialized) {
        fprintf(stderr, "[IPC] ERROR: Shared memory not initialized\n");
        return -1;
    }
    if (pcb->owner_id != shared_mem.owner_id) {
        fprintf(stderr, "[IPC] ACCESS DENIED: PID=%lld owner_id=%d\n",
                (long long)pcb->pid, pcb->owner_id);
        return -1;
    }
    shared_mem.pid = (int)pcb->pid;
    strncpy(shared_mem.name, pcb->name, 63);
    shared_mem.state = pcb->state;
    shared_mem.burst_time = pcb->burst_time;
    shared_mem.remaining_time = pcb->remaining_time;
    shared_mem.memory_req_kb = pcb->memory_req_kb;
    shared_mem.access_count++;
    printf("[IPC] Shared memory written: PID=%lld name=%s\n",
           (long long)pcb->pid, pcb->name);
    return 0;
}

int shm_read(int owner_id, SharedMetrics *out) {
    if (!shm_initialized) {
        fprintf(stderr, "[IPC] ERROR: Shared memory not initialized\n");
        return -1;
    }
    if (owner_id != shared_mem.owner_id) {
        fprintf(stderr, "[IPC] ACCESS DENIED: owner_id=%d\n", owner_id);
        return -1;
    }
    memcpy(out, &shared_mem, sizeof(SharedMetrics));
    printf("[IPC] Shared memory read: PID=%d access_count=%d\n",
           shared_mem.pid, shared_mem.access_count);
    return 0;
}

void pipe_demo() {
    printf("\n=== Pipe Demo (simulated on Windows) ===\n");
    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "PID=1 NAME=calculator STATE=1 BURST=10");
    printf("[IPC Parent] Sending via pipe: %s\n", buffer);
    printf("[IPC Child] Received via pipe: %s\n", buffer);
}

void shm_demo() {
    printf("\n=== Shared Memory Demo ===\n");
    shm_init(1);
    PCB pcb;
    pcb.pid = 42;
    strncpy(pcb.name, "test_process", 63);
    pcb.state = RUNNING;
    pcb.burst_time = 15;
    pcb.remaining_time = 15;
    pcb.memory_req_kb = 256;
    pcb.owner_id = 1;
    shm_write(&pcb);
    SharedMetrics metrics;
    shm_read(1, &metrics);
    printf("[IPC] Read back: PID=%d NAME=%s STATE=%d\n",
           metrics.pid, metrics.name, metrics.state);
    printf("\n[IPC] Testing access control with wrong owner_id:\n");
    pcb.owner_id = 99;
    shm_write(&pcb);
}
