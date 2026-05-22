#include "include/eduos.h"

int main(void){
srand(time(NULL);
printf("===EduOS core simulator starting ===\n\n");

//create init process (PID)
PCB init ={0};
init.pid = 1;
strcpy(init.name,'init");
init.state = RUNNING;
init.priority = 0;
init.burst_time = 100;
init.remaining_time = 100;
init.arrival_time = 0;
init.creation_time = time(NULL);

process_table[0] = init;
process_count = 1;
next_pid = 2;

edu_ps();
//test fork + exec
pid_t child1 = edu_fork(&process_table[0]);
edu_exec(child1,"ls");

pid_tchild2 = edu_fork(&process_table[0]);
edu_exec(child2,"firefox");


edu_ps();

//test exit
edu_exit(child1,0);
edu_ps();

edu_exit(child2,0);
edu_ps();
printf("simulator finished.\n");
return 0;

}

