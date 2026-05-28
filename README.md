# EduOS - Operating System Simulator

**Project Title:** EduOS Multi-Component OS Simulator
**Module Code:** 351 CS 2104
**Module Name:** Operating Systems
**Semester:** III
**Student Name:** [Your Full Name]
**Registration Number:** [Your Reg Number]

## Prerequisites
- GCC compiler with pthread support
- Python 3.8+
- pip install -r python_schedular/requirements.txt

## Build Instructions

### C Part
cd c_core
gcc -Wall -Wextra -pthread -o eduos main_sim.c process_manager.c thread_manager.c ipc_module.c
./eduos

### Python Part
cd python_schedular
pip install -r requirements.txt
py scheduler_sim.py --random 10 --seed 42
py scheduler_sim.py --file sample_processes.csv

### Run Full System
cd controller
py main_controller.py

## Directory Structure
- c_core/ - C source files
- c_core/include/ - Header files (eduos.h)
- python_schedular/ - Python scheduling algorithms
- controller/ - Python-C integration bridge
- docs/screenshots/ - Simulator screenshots

## Screenshots
See docs/screenshots/ folder for simulator running screenshots.

## Challenges Encountered
1. PCB Serialization - wrote custom JSON serializer in C
2. Race Conditions - demonstrated and fixed using pthread_mutex_t
3. Python-C Integration - used subprocess.Popen and JSON bridge

## References
- Operating Systems: Three Easy Pieces (OSTEP)
- Linux man pages
- POSIX pthread documentation

## Valgrind Output
0 errors from 0 contexts (suppressed: 0 from 0)
