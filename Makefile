CC = gcc -Wall
CFLAGS = -lm


systemStats.out:main.c cpuStats memStats sesInfo tools.o
	$(CC) $< tools.o -o $@ $(CFLAGS)
cpuStats:cpuStats.c
	$(CC) -Wall $< -o $@ $(CFLAGS)
memStats:memoryStats.c tools.o
	$(CC) -Wall $^ -o $@ $(CFLAGS)
sesInfo:sessionInfo.c
	$(CC) -Wall $< -o $@
tools.o: tools.c
	$(CC) -Wall -c $< -o $@

.PHONY: clean
clean:
	rm *.o