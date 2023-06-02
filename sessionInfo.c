#include <stdlib.h>
#include "stdio.h"
#include <unistd.h>
#include <sys/sysinfo.h>
#include <utmp.h>
#include <signal.h>

/*
 * this function prints info about the
 * machine the application is run on
 */
void printSessionInfo(){
    char buff256[256];
    char buff16[32];
    struct utmp *sessions;
    setutent();
    sessions = getutent();
    while(sessions != NULL){
        if(sessions->ut_type == USER_PROCESS) {
            sprintf(buff16, "%s", sessions->ut_user);
            write(STDOUT_FILENO, buff16, sizeof(char)*32);
            sprintf(buff16, "%s", sessions->ut_line);
            write(STDOUT_FILENO, buff16, sizeof(char)*32);
            sprintf(buff256, "%s", sessions->ut_host);
            write(STDOUT_FILENO, buff256, sizeof(char)*256);
        }
        sessions = getutent();
    }
    endutent();
    sprintf(buff16, "-1");
    write(STDOUT_FILENO, buff16, sizeof(char)*32);
}

int main (int argc, char **argv){

    //Block SIGINT so it does not interrupt
    sigset_t blockSet;
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGINT);
    sigprocmask(SIG_BLOCK, &blockSet, NULL);

    int samples = atoi(argv[0]);
    int delay = atoi(argv[1]);


    //Write to pipe
    for (int sample = 0; sample < samples; sample++) {
        sleep(delay);
        printSessionInfo();
    }
}