#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include <unistd.h>
#include <sys/sysinfo.h>
#include <signal.h>

/*
 * returns the amount of cores of the cpu
 * on the device running the program
 */
int cpuCores(){
    int cores = -2;
    FILE *cpu = fopen("/proc/stat", "r");
    if(cpu == NULL)
        perror("Did not open CPU file");
    char temp[8];
    while(strcmp(temp, "intr") != 0 ) {
        fscanf(cpu, "%8s %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d", temp);
        cores ++;
    }
    fclose(cpu);
    return cores;
}

/*
 * this function stores a sample of the CPU usage
 * in the parameter cpuData of the current sample
 * 'sample' current total and idle times
 */
double getCPUUsage(int sample, long *prevIdle, long *prevTotal){
    long temp_cpu_info[10];
    long idle = 0;
    long total = 0;

    double currUsage = 0;

    FILE *cpu = fopen("/proc/stat", "r");
    if(cpu == NULL) {
        perror("Did not open CPU file");
        exit(EXIT_FAILURE);    }
    fscanf(cpu, "%*s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &temp_cpu_info[0], &temp_cpu_info[1], &temp_cpu_info[2],
           &temp_cpu_info[3], &temp_cpu_info[4], &temp_cpu_info[5], &temp_cpu_info[6], &temp_cpu_info[7], &temp_cpu_info[8],
           &temp_cpu_info[9]);
    for (int i = 0; i < 10; i++){
        total += temp_cpu_info[i];
        if(i == 3 || i == 4)
            idle += temp_cpu_info[i];
    }

    fclose(cpu);
    if(sample != -1) {
        if (((double) (total) - (double)(*prevTotal)) != 0)// checks to not divide by zero if there is no increase
            currUsage =
                    ((double) (total - idle) - (double)(*prevTotal - *prevIdle)) /
                    ((double) (total) - (double)(*prevTotal));// calculated usage for this sample
        else
            currUsage = 0;
    }

    *prevTotal = total;
    *prevIdle = idle;

    return currUsage;
}

int main (int argc, char **argv){
    //Block SIGINT so it does not interrupt
    sigset_t blockSet;
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGINT);
    sigprocmask(SIG_BLOCK, &blockSet, NULL);

    int samples = atoi(argv[0]);
    int delay = atoi(argv[1]);
    char str[16];

    double currUsage = 0;
    long prevTotal = 0;
    long prevIdle = 0;

    sprintf(str, "%d", cpuCores());
    write(STDOUT_FILENO, str, sizeof(char)*16);

    currUsage = getCPUUsage(-1, &prevIdle, &prevTotal);

    //Write to pipe
    for (int sample = 0; sample < samples; sample++) {
        sleep(delay);
        currUsage = getCPUUsage(sample, &prevIdle, &prevTotal);
        sprintf(str, "%lf", currUsage);
        write(STDOUT_FILENO, str, sizeof(char)*16);
    }
}