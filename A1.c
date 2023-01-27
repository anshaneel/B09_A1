#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>

//TO DO: fix delay and samples command line parsing, figure out this screen refreshing thing
void headerUsage(){
//Get the memory usage
struct sysinfo memInfo;
long used_memory = (info.totalram - info.freeram) / 1024;

printf("Nbr of samples: %d -- every %d secs\n Memory usage: %ld kilobytes\n", samples, tdelay, used_memory);

}

void footerUsage(){

    struct utsname sysinfo;
    uname(&sysinfo);

    printf("--------------------------------------------\n");
    printf("### System Information ###\n");
    printf(" System Name = %s\n", sysinfo.sysname);
    printf(" Machine Name = %s\n", sysinfo.nodename);
    printf(" Version = %s\n", sysinfo.version);
    printf(" Release = %s\n", sysinfo.release);
    printf(" Architecture = %s\n", sysinfo.machine);
    printf("--------------------------------------------\n");

}

void systemOutput(){

    printf("--------------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");

    struct sysinfo memory;
    sysinfo (&memory);

    double total_memory = memory.totalram * memory.mem_unit / (1024 * 1024 * 1024);
    double used_memory = (memory.totalram - memory.freeram) * memory.mem_unit / (1024 * 1024 * 1024);
    double total_virtual = (memory.totalram + memory.totalswap) * memory.mem_unit / (1024 * 1024 * 1024);
    double used_virtual = (memory.totalram - memory.freeram + memory.totalswap - memory.freeswap) * memory.mem_unit / (1024 * 1024 * 1024);

    printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB\n", used_memory, total_memory, used_virtual, total_virtual);


}

void userOutput(){
    //Ask question about what exactly to print out in terms of user information (user, session, terminal, IP)???
    printf("--------------------------------------------\n");
    printf("### Sessions/users ###\n");

    struct utmp *utmp;
    setutent();

    while ((utmp = getutent()) != NULL) {
        if (utmp -> ut_type == USER_PROCESS) {
            //User, session, terminal
            printf("%s\t %s (%s)", utmp -> ut_user, utmp -> ut_session, utmp -> ut_line);
        }
    }

    endutent();

}

void CPUOutput(){

    //Ask how he wants us to calculate the CPU usage
    struct sysinfo cpu;
    sysinfo (&cpu);
    double use = ((double)(cpu.uptime - cpu.idle) / (double)cpu.uptime) * 100;

    printf("--------------------------------------------\n");
    printf("Number of Cores: %d\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf(" total cpu use: %d%\n", use);

}

void graphicsOutput(){

}

void display(int samples, int tdelay, bool system, bool user, bool graphics, bool sequential){

    for (int i = 0; i < samples; i++){
        
        if (!sequential){
        system("clear");
        }
        headerUsage();
        if (system){
            systemOutput();
        }
        if (user){
            userOutput();
        }
        if (system){

        }
        if (graphics){
            graphicsOutput();
        }
        sleep(tdelay);
    }

    footerUsage();

}
int main(int argc, char *argv[]){

    int samples = 10; int tdelay = 1;
    bool system = false; bool user = false; bool graphics = false; bool sequential = false;

    if (argc == 1){
        display(samples, tdelay, true, true, false, false);
    }

    bool found = false;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--system") == 0 || strcmp(argv[i], "-s") == 0){
            system = true;
        }
        else if (strcmp(argv[i], "--user") == 0 || strcmp(argv[i], "-u") == 0){
            user = true;
        }
        else if (strcmp(argv[i], "--graphics") == 0 || strcmp(argv[i], "-g") == 0){
            graphics = true;
        }
        else if (strcmp(argv[i], "--sequential") == 0 || strcmp(argv[i], "-seq") == 0){
            sequential = true;
        }
        else if (strncmp(argv[i], "--samples=", 10) == 0){
            sscanf(argv[i], "%d", &samples);
            found = true;
        }
        else if (strncmp(argv[i], "--tdelay=", 9) == 0){
            sscanf(argv[i], "%d", &tdelay);
        }
        else if (isdigit(argv[i])){
            if (!found){
                samples = atoi(argv[i]);
                found = true;
            }
            else{
                tdelay = atoi(argv[i]);
            }
        }

    }

    display(samples, tdelay, system, user, graphics, sequential);
}
