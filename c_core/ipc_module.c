#include "include/eduos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/* Shared memory structure */
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

/* Simulated shared memory using global struct */
static SharedMetrics shared_mem;
static int shm_initialized = 0;

/* Initialize shared memory */
void shm_init(int owner_id) {
    memset(&shared_mem, 0, sizeof(SharedMetrics));
    shared_mem.owner_id = owner_id;
    shm_initialized = 1;
    printf("[IPC] Shared memory initialized for owner_id=%d\n", owner_id);
}

/* Write to shared memory with access control */
int shm_write(PCB *pcb) {
    if (!shm_initialized) {
        fprintf(stderr, "[IPC] ERROR: Shared memory not initialized\n");
        return -1;
    }
    /* Access control check */
    if (pcb->owner_id != shared_mem.owner_id) {
        fprintf(stderr, "[IPC] ACCESS DENIED: PID=%d owner_id=%d\n",
                pcb->pid, pcb->owner_id);
        return -1;
    }
    shared_mem.pid = pcb->pid;
    strncpy(shared_mem.name, pcb->name, 63);
    shared_mem.state = pcb->state;
    shared_mem.burst_time = pcb->burst_time;
    shared_mem.remaining_time = pcb->remaining_time;
    shared_mem.memory_req_kb = pcb->memory_req_kb;
    shared_mem.access_count++;
    printf("[IPC] Shared memory written: PID=%d name=%s\n",
           pcb->pid, pcb->name);
    return 0;
}

/* Read from shared memory */
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

/* Pipe demo - parent sends PCB data to child */
void pipe_demo() {
    printf("\n=== Anonymous Pipe Demo ===\n");
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    /* Create sample PCB data to send */
    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "PID=1 NAME=calculator STATE=1 BURST=10");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        /* Child process - reads from pipe */
        close(pipefd[1]);
        char recv_buf[256];
        read(pipefd[0], recv_buf, sizeof(recv_buf));
        printf("[IPC Child] Received via pipe: %s\n", recv_buf);
        close(pipefd[0]);
        exit(0);
    } else {
        /* Parent process - writes to pipe */
        close(pipefd[0]);
        write(pipefd[1], buffer, strlen(buffer) + 1);
        printf("[IPC Parent] Sent via pipe: %s\n", buffer);
        close(pipefd[1]);
        wait(NULL);
    }
}

/* Demo shared memory IPC */
void shm_demo() {
    printf("\n=== Shared Memory Demo ===\n");

    /* Initialize shared memory */
    shm_init(1);

    /* Create a PCB to write */
    PCB pcb;
    pcb.pid = 42;
    strncpy(pcb.name, "test_process", 63);
    pcb.state = RUNNING;
    pcb.burst_time = 15;
    pcb.remaining_time = 15;
    pcb.memory_req_kb = 256;
    pcb.owner_id = 1;

    /* Write to shared memory */
    shm_write(&pcb);

    /* Read back */
    SharedMetrics metrics;
    shm_read(1, &metrics);
    printf("[IPC] Read back: PID=%d NAME=%s STATE=%d\n",
           metrics.pid, metrics.name, metrics.state);

    /* Test access control - wrong owner */
    printf("\n[IPC] Testing access control with wrong owner_id:\n");
    pcb.owner_id = 99;
    shm_write(&pcb);
}
