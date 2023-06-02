#include <stdlib.h>
#include "stdio.h"
#include <unistd.h>
#include <sys/sysinfo.h>
#include <signal.h>
#include "tools.h"

//system means memory and cpu
//user means users connected
/*
 * struct memory is store the data
 * necessary to represent a sample
 */
typedef struct memory{
    double physicalMemory;
    double virtualMemory;
    double physicalTotalMemory;
    double virtualTotalMemory;
} MemoryData;

/*
 * this function stores the data that is obtained
 * from sysinfo into MemoryData that includes physical
 * and virtual memory used and total physical and virtual memory
 */
void getMemoryInfo(MemoryData *memData){
    struct sysinfo systemInfo;
    if(sysinfo(&systemInfo) != 0) {
        perror("Couldnt get memory info");
        exit(EXIT_FAILURE);
    }
    double gigabyte = 1024.0*1024.0*1024.0;
    memData->physicalMemory = floorTwoPlaces(((systemInfo.totalram - systemInfo.freeram)*systemInfo.mem_unit)/(gigabyte));
    memData->virtualMemory = floorTwoPlaces(((systemInfo.totalswap - systemInfo.freeswap)*systemInfo.mem_unit)/(gigabyte)
                                               + memData->physicalMemory);
    memData->physicalTotalMemory = floorTwoPlaces(((systemInfo.totalram)*systemInfo.mem_unit)/(gigabyte));
    memData->virtualTotalMemory = floorTwoPlaces(((systemInfo.totalswap)*systemInfo.mem_unit)/(gigabyte)
                                                    + memData->physicalTotalMemory);
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
    MemoryData memData;


    //Write to pipe
    for (int sample = 0; sample < samples; sample++) {
        sleep(delay);
        getMemoryInfo(&memData);
        sprintf(str, "%lf", memData.physicalMemory);
        write(STDOUT_FILENO, str, sizeof(char)*16);
        sprintf(str, "%lf", memData.virtualMemory);
        write(STDOUT_FILENO, str, sizeof(char)*16);
        sprintf(str, "%lf", memData.physicalTotalMemory);
        write(STDOUT_FILENO, str, sizeof(char)*16);
        sprintf(str, "%lf", memData.virtualTotalMemory);
        write(STDOUT_FILENO, str, sizeof(char)*16);
    }
}