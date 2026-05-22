#ifndef EDUOS_H
#define EDUOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

/*process states */
#define NEW        0
#define READY      1
#define RUNNING    2
#define WAITING    3
#define TERMINATED 4

/*Thread pool size*/
#define THREAD_POOL_SIZE 4

/*Process Control Block*/
typedef struct {
    pid_t pid;             /*unique process ID*/
    char name[64];         /*process name / program */
    int state;             /*NEW|READY|RUNNING|WAITING|TERMINATED*/
    int priority;          /*0 = highest */
    int burst_time;         /* total CPU time needed*/
    int arrival_time;      /* clock tick of arrival */
    int remaining_time;    /*used by preemptive algorithms*/
    int memory_req_kb;     /*memory footprint in KB*/
    int thread_count;      /*threads spawned by process*/
    time_t creation_time;  /*wall-clock timestamp*/
    int exit_code;         /*exit status*/
    int owner_id;          /*for access control check*/
    }PCB;
    #endif /*EDUOS_H */

