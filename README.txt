int cpuCores()
returns the amount of cores of the cpu on the device running the program

double floorTwoPlaces(double number)
which is floored to two decimal places returns a double of parameter number

void getCPUUsage(struct cpuData *cpuStats, int sample)
this function stores a sample of the CPU usage in the parameter cpuData of the current sample 'sample' current total and idle times

void printCPUUsageGraphics(struct cpuData *cpuStats, int sample, int samples, bool sequential)
prints the graphics of the cpu usage stored in cpuData of sample 'sample' and all previous samples with option of sequential output dependent 'sequential'

void getCPUUsage(struct cpuData *cpuStats, int sample)
this function stores a sample of the CPU usage in the parameter cpuData of the current sample 'sample' and the current total and idle times

void printCPUUsageGraphics(struct cpuData *cpuStats, int sample, int samples, bool sequential)
prints the graphics of the cpu usage stored in cpuData of sample 'sample' and all previous samples with option of sequential output dependent 'sequential'

void getMemoryInfo(struct memory *memoryData)
this function stores the data that is obtained from sysinfo into memoryData that includes physical and virtual memory used and total physical and virtual memory

void printMemoryInfo(int samples, int sample, struct memory memoryData[], bool graphics, bool sequential)
this function prints the data that was collected in getMemoryInfo for the current sample and all previous samples with an option for graphics and sequential output.

void printMachineInfo()
this function prints info about the machine the application is run on

void printSessionInfo()
this function prints out all users connected to the current session

bool isNumber(char* string)
this function returns all true if the parameter string is a number

void setVariables(int argc, char **argv, int *delay, int *samples, bool *systemUsage, bool *user, bool *graphics, bool *sequential)
this function sets all the flags that is inputted by the user as command
line arguments

void newPipe(int *currentPipe)
This function creates and stores a new pipe in currentPipe and also checks for failure of not creaing a pipe

void newExecutable(int *currentPipe, char *path, char *samples, char *delay)
This function creates a new executable given a path 'path' and needed command line arguments samples and delay

static void catch_function(int signo)
Signal handler function for SIGINT

void readCpuStats(double *cpuStat, int *cpuPipe)
This function reads from the pipe cpuPipe and stores the information in cpuStat

void readMemStats(MemoryData *memStats, int *memPipe)
This function reads from the pipe memPipe and stores the information in memStats

How did I solve the problem

First I sectioned the big problem into smaller tasks: creating a proccess for each piece of data
obtaining needed data, writing back the data from the process storing needed data and printing the needed data.

To create a process for each piece of data, I first made a pipe to for each piece of data, then proceeded to fork
the main process for each pipe and then had 3 seperate executables that would collect the data for me that would overwrite 
each child process.

For the cpu process, I had a function that would collect the total cpu usage and in the main of the executable I would write this to a pipe.
For the mem process, I had also a function that collected all the data and I used the main of the executable to write this to a pipe.
For the ses process, I again, had a function that would collect sessions and I used the main of the executable to write this to a pipe.

To tackle the obtained data I used all the libraries and structs that were posted on quercus. Some of them included needing to read 
through files(cpu usage) while others involved structs and calling functions on the structs(memory, users and machine info). For each 
piece of data I would have a separate function that collects the information for me if there wasn’t a way to collect the information. 
For the specific case of CPU usage where I needed to collect the data my formula was adding all the cpu states time except for idle and 
iowait, for the total time I added all the states together. To get the current sample, I would do delta(usage)/delta(total) delta 
being current sample - previous sample.

With the collected data I would then using the pipe that was created before the forks, I would write to the pipe which then the main process
had access to. With access to the pipe, I would read all the data coming out of the pipe and then proceed to figure out how to store it.

To store the data, for sessions and machine info, there were structs that automatically stored for memory and sessions and  
memory I created my own struct. For cpu i created an array that stores the samples in an array which was collected from the pipe.
For memory I created a struct that stored only the information I needed being physical,
virtual, physical total and virtual total and had an array of memoryData of size samples where I stored all the samples data.
To print the data, I just figured out the order of the print statements and how to format the output and just placed all the data in the 
right places. For the sequential part, all I did was not clear the screen on each iteration. For graphics for the cpu and memory I solved 
each differently. For graphics I printed the difference of virtual memory from the previous and current sample. Depending on the difference in memory
I would print : for every decrease by 0.01 and print # for every increase by 0.01 and end it with * and @ for positive and negative 
increase respectively. For cpuI would print ||| then for every whole percent (not rounded) I would print an extra |||

How to run the program

When compiling the c file compile with the math library EX gcc “name of file”.c -lm
There is also a makefile to compile the executable. To complie type: make systemStats.out
When running the execution EX ./a.out
There are flags that can be run with the execution
The first two arguments are for sample and delay if they are typed meaning if two integers 
are typed the first is for the sample and the second is for the delay. If only one integer 
is typed then it would be set to samples.

If no integers are typed for the first two elements the default is samples = 10 and delay = 1.
These are the flags
--system
This is used to only print system information
--user
This is used to only print user information
--graphics 
This is used to print a graphical representation of data
--sequenital
This is used to print all the data sequential without a refresh of the screen or cursor
--tdelay=N
This is used to also set delay where N is replaced with number for delay
--samples=N
This is used to also set the number samples where N is replaced with the number of samples desired
