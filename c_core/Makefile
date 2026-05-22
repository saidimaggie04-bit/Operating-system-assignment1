CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c11
TARGET = eduos
all: $(TARGET)

$(TARGET):main_sim.c process_manager.c thread_manager.c ipc_module.c
    $(CC) $(CFLAGS)  -o $(TARGET) main_sim.c process_manager.c thread_manager.c ipc_module.c

race: thread_manager.c
     $(CC) -Wall -Wextra -pthread  -std=c11  -o race_demo thread_manager.c  -DRACE_CONDITION
fixed: thread_manager.c
  $(CC) -Wall -Wextra -pthread -std=c11 -o fixed_demo thread_manager.c -DFIXED
memcheck: $(TARGET)
  valgrind --leak-check=full ./$(TARGET)

clean:
    rm -f *.o $(TARGET) race_demo fixed_demo
