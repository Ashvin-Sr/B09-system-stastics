#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "stdio.h"
#include "string.h"
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/resource.h>
#include <math.h>
#include "tools.h"
#include <signal.h>

/*
 * Struct to hold sample data
 * about memory
 */
typedef struct memory{
    double physicalMemory;
    double virtualMemory;
    double physicalTotalMemory;
    double virtualTotalMemory;
} MemoryData;

/* This function prints info about
 * the machine the application is run on
*/
void printMachineInfo(){
    struct utsname machineInfo;
    if(uname(&machineInfo) != 0) {
        perror("\nFailed to obtain machine information\n");
        exit(EXIT_FAILURE);    }
    printf("System Name = %s\nMachine Name = %s\nVersion = %s\nRelease = %s\nArchitecture = %s\n"
            , machineInfo.sysname, machineInfo.nodename, machineInfo.version, machineInfo.release, machineInfo.machine);
}


/*
 * prints the graphics of the cpu usage
 * stored in cpuData of sample 'sample'
 * and all previous samples with option
 * of sequential output dependent 'sequential'
 */
void printCPUUsageGraphics(double cpuStats[], int sample, int samples, bool sequential){
    for(int i = 0; i <= sample; i++){
        if (!sequential || (sample == i)) {
            printf("\t|||");
            for (int j = 0; j < floor(cpuStats[i] * 100); ++j) {
                printf("|");
            }
            printf(" (%.2lf) \n", (cpuStats[i] * 100));
        }
        else
            printf("\n");
    }
    for (int i = 0; i < samples - sample - 1; ++i) {
        printf("\n");
    }
}


/*
 * this function prints the data that was collected in
 * getMemoryInfo for the current sample and all previous samples
 * with an option for graphics and sequential output.
 */
void printMemoryInfo(int samples, int sample, struct memory memoryData[], bool graphics, bool sequential){
    double difference = 0;

    for (int i = 0;i <= sample; i++) {
        if (!sequential || (sample == i)) {
            printf("%.2lf GB / %.2lf GB --  %.2lf GB / %.2lf GB", memoryData[i].physicalMemory,
                   memoryData[i].physicalTotalMemory, memoryData[i].virtualMemory,
                   memoryData[i].virtualTotalMemory);
            if (graphics) {
                if(i != 0)
                    difference = ((memoryData[i].virtualMemory - memoryData[i - 1].virtualMemory));
                printf("|");
                for (int j = 0; j < (fabs(difference*100.0)); ++j) {
                    if (difference > 0)
                        printf("#");
                    else if (difference < 0)
                        printf(":");
                }
                if (difference >= 0)
                    printf("*");
                else if (difference < 0) {
                    printf("@");
                    difference = difference * (-1);
                }
                printf(" %.2lf (%.2lf)", difference, memoryData[i].virtualMemory);
            }
            printf("\n");
        }
        else{
            printf("\n");

        }
    }
    for (int i = 0; i < samples - sample - 1; i++) {
        printf("\n");
    }
}

/*
 * this function prints out all
 * users connected to the current session
 */
void printSessionInfo(int *currentPipe){
    char buff16[32];
    char buff256[256];
    for(;;) {
        read(currentPipe[0], buff16, sizeof(char) * 32);
        if(atoi(buff16) == -1)
            break;
        printf("%s\t", buff16);
        read(currentPipe[0], buff16, sizeof(char) * 32);
        printf("%s ", buff16);
        read(currentPipe[0], buff256, sizeof(char) * 256);
        printf("%s\n", buff256);
    }
}
/*
 * this function returns all true
 * if the parameter string
 * is a number
 */
bool isNumber(char* string)
{
    for (int i=0; string[i] != '\0'; i++)
        if (!isdigit(string[i]))
        {
            return false;
        }
    return true;
}
/*
 * this function sets all the flags
 * that is inputed by the user as command
 * line arguements
 */
void setVariables(int argc, char **argv, int *delay, int *samples,
                  bool *systemUsage, bool *user, bool *graphics, bool *sequential){
    char* temp;
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "--system") == 0)
            *systemUsage = true;

        else if(strcmp(argv[i], "--user") == 0)
            *user = true;

        else if(strcmp(argv[i], "--graphics") == 0)
            *graphics = true;

        else if(strcmp(argv[i], "--sequential") == 0)
            *sequential = true;
        else if(strstr(argv[i], "--tdelay=") != NULL){
            temp = strstr(argv[i], "=");
            *delay = strtol(strtok(temp, "="), NULL, 10);
        }
        else if(strstr(argv[i], "--samples=") != NULL){
            temp = strstr(argv[i], "=");
            *samples = strtol(strtok(temp, "="), NULL, 10);
        }
    }
    if(argc >= 3 && isNumber(argv[1]) && isNumber(argv[2])){// check the first two arguments for ints
        *samples = strtol(argv[1], NULL, 10);
        *delay = strtol(argv[2], NULL, 10);
    }
    else if(argc >= 2 && isNumber(argv[1])) {//check the first arguments for ints
        *samples = strtol(argv[1], NULL, 10);

    }
    if(!*systemUsage && !*user) {
        *systemUsage = true;
        *user = true;
    }
}
/*
 * This function creates and stores a new
 * pipe in currentPipe and also checks for
 * failure of not creaing a pipe
 */
void newPipe(int *currentPipe){
    if(pipe(currentPipe) == -1){
        perror("Pipe could not be created");
        exit(EXIT_FAILURE);
    }
}
/*
 * This function creates a new executable
 * given a path 'path' and needed command line
 * arguments samples and delay
 */
void newExecutable(int *currentPipe, char *path, char *samples, char *delay){
    if(fork() == 0) {
        close(currentPipe[0]);
        dup2(currentPipe[1], STDOUT_FILENO);
        if(execl(path, samples, delay, NULL) == -1) {
            perror("Could not excute an excutable");
            exit(EXIT_FAILURE);
        }
    }
}
/*
 * Signal handler function for SIGINT
 */
static void catch_function(int signo){
    int num;
    printf("Are you sure you want to quit?[Yes = 1/ No = 0]");
    scanf("%d", &num);
    if(num == 1)
        exit(1);
}
/*
 * This function reads from the pipe cpuPipe
 * and stores the information in cpuStat
 */
void readCpuStats(double *cpuStat, int *cpuPipe){
    char buff16[16];
    read(cpuPipe[0], buff16, sizeof(char)*16);
    *cpuStat = strtod(buff16, NULL);
}
/*
 * This function reads from the pipe memPipe
 * and stores the information in memStats
 */
void readMemStats(MemoryData *memStats, int *memPipe){
    char buff16[16];
    read(memPipe[0], buff16, sizeof(char)*16);
    memStats->physicalMemory = strtod(buff16, NULL);
    read(memPipe[0], buff16, sizeof(char)*16);
    memStats->virtualMemory = strtod(buff16, NULL);
    read(memPipe[0], buff16, sizeof(char)*16);
    memStats->physicalTotalMemory = strtod(buff16, NULL);
    read(memPipe[0], buff16, sizeof(char)*16);
    memStats->virtualTotalMemory = strtod(buff16, NULL);
}
int main(int argc, char **argv){

    //Setup Catch signal SIGINT
    if(signal(SIGINT, catch_function)){
        fputs("Error creating a signal", stderr);
        return EXIT_FAILURE;
    }

    //Block signal SIGTSTP
    sigset_t blockSet;
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGTSTP);
    sigprocmask(SIG_BLOCK, &blockSet, NULL);

    //declare variables for flags
    int samples = 10;
    int delay = 1;
    bool systemUsage = false;
    bool user = false;
    bool graphics = false;
    bool sequential = false;

    setVariables(argc, argv, &delay, &samples,
                 &systemUsage, &user, &graphics, &sequential);

    char strSamples[16];
    char strDelay[16];

    sprintf(strSamples, "%d", samples);
    sprintf(strDelay, "%d", delay);

    //Setup all pipes
    int cpuPipe[2];
    int memPipe[2];
    int sesPipe[2];

    //Setup variables to store samples
    int cpuCore;
    char buff16[16];
    double cpuStats[samples];
    MemoryData memStats[samples];
    struct rusage memoryUsage;

    //Get system information
    if(getrusage(RUSAGE_SELF, &memoryUsage) != 0) {
        perror("Could obtain information of system");
        exit(EXIT_FAILURE);
    }

    //Initialize new Pipes
    newPipe(cpuPipe);
    newPipe(memPipe);
    newPipe(sesPipe);

    //Execute new executables
    newExecutable(cpuPipe, "cpuStats", strSamples, strDelay);
    newExecutable(memPipe, "memStats", strSamples, strDelay);
    newExecutable(sesPipe, "sesInfo", strSamples, strDelay);

    //Close file descriptors
    close(cpuPipe[1]);
    close(memPipe[1]);
    close(sesPipe[1]);

    //Read amount of cores
    read(cpuPipe[0], buff16, sizeof(char)*16);
    cpuCore = atoi(buff16);

    for(int sample = 0; sample < samples; sample++){
        //Collect new sample
        readCpuStats(&cpuStats[sample], cpuPipe);
        readMemStats(&memStats[sample], memPipe);


        if (!sequential) {
                printf("\033[0;0H\033[2J"); // Reset cursor
        }

        printf("Nbr of samples: %d -- every %d secs\n "
               "Memory usage: %ld kilobytes\n"
               "---------------------------------------\n", samples, delay, memoryUsage.ru_maxrss);

        if(systemUsage) {
            printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
            printMemoryInfo(samples, sample, memStats, graphics, sequential);
            printf("---------------------------------------\n");
        }

        if (user){
            printf("### Sessions/Users ###\n");
            printSessionInfo(sesPipe);
            printf("---------------------------------------\n");
        }

        if(systemUsage) {
            printf("Number of cores: %d\n"
                   "total cpu use = %.2f%%\n", cpuCore, cpuStats[sample] * 100);
            if(graphics)
                printCPUUsageGraphics(cpuStats, sample, samples, sequential);
        }

        if(sequential)
            printf("Iteration = %d\n", sample + 1);
        printf("---------------------------------------\n");
        printMachineInfo();
        printf("---------------------------------------\n");

    }
}
