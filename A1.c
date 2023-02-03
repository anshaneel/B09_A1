#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <math.h>
#include <utmp.h>

void headerUsage(int samples, int tdelay){

    //Get the memory usage
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    long used_memory = usage.ru_maxrss;

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

void memoryGraphicsOutput(char memoryGraphics[1024], double memory_current, double* memory_previous, int i){

    if (i == 0){ *memory_previous = memory_current; }
        
    // Absolute value of difference in memory usage
    double diff = memory_current - *memory_previous;
    double abs_diff = fabs(diff);
    
    char visual[1024] = "   |";
    int visual_len = (int)( abs_diff / 0.01 );
    char last_char;
    char sign;

    if (diff >= 0) { 
        sign = '#';
        last_char = '*';
        if (visual_len == 0) { last_char = 'o'; }
    }
    else { sign = ':'; last_char = '@'; }

    *memory_previous = memory_current;
    
    for (int i = 4; i < visual_len + 4; i++){
        visual[i] = sign;
    }
    visual[visual_len + 4] = '\0';
    
    strncat(visual , &last_char, 1);

    sprintf(memoryGraphics, "%s %.2f (%.2f)", visual, abs_diff, memory_current);

}

void systemOutput(char terminal[1024][1024], bool graphics, int i, double* memory_previous){

    printf("--------------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");

    struct sysinfo memory;
    sysinfo (&memory); // If sequential we can add it as a paramater and then just print the line

    double total_memory = (double) memory.totalram / (1024 * 1024 * 1024);
    double used_memory =  (double) (memory.totalram - memory.freeram) / (1024 * 1024 * 1024);
    double total_virtual = (double) (memory.totalram + memory.totalswap) / (1024 * 1024 * 1024);
    double used_virtual = (double) (memory.totalram - memory.freeram + memory.totalswap - memory.freeswap) / (1024 * 1024 * 1024);
    
    sprintf(terminal[i], "%.2f GB / %.2f GB -- %.2f GB / %.2f GB", used_memory, total_memory, used_virtual, total_virtual);

    if (graphics){ 
        char graphics_output[1024]; memoryGraphicsOutput(graphics_output, used_memory, memory_previous, i);
        strcat(terminal[i], graphics_output); 
    }

    for (int j = 0; j <= i; j++){
        printf("%s\n", terminal[j]);
    }
}

void userOutput(){

    printf("--------------------------------------------\n");
    printf("### Sessions/users ###\n");

    struct utmp *utmp;
    setutent();

    while ((utmp = getutent()) != NULL) {
        if (utmp -> ut_type == USER_PROCESS) {
            //User, session, host
            printf("%s\t %s (%s)\n", utmp -> ut_user, utmp -> ut_line, utmp -> ut_host);
        }
    }

    endutent();

}

void CPUGraphics(char terminal[1024][1024], double usage, int i){

    int visual_len = (int)(usage) + 12;
    strcpy(terminal[i], "         ");

    for (int j = 9; j < visual_len; j++){
        terminal[i][j] = '|';
    }
    terminal[i][visual_len] = '\0';

    sprintf(terminal[i] + visual_len, " %.2f", usage);

    for (int j = 0; j <= i ; j++){
        printf("%s\n", terminal[j]);
    }
}

void CPUOutput(char terminal[1024][1024], bool graphics, int i, long int* cpu_previous, long int* idle_previous){

    //Ask how he wants us to calculate the CPU usage
    struct sysinfo cpu;
    sysinfo(&cpu);

    FILE *fp = fopen("/proc/stat", "r");
    long int user, nice, system, idle, iowait, irq, softirq;
    fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &idle, &iowait, &irq, &softirq);

    fclose(fp);

    long int cpu_total = user + nice + system + iowait + irq + softirq;

    printf("cup_total: %ld cpu_prev: %ld system: %ld idle: %ld idle_prev: %ld\n", cpu_total, *cpu_previous, system, idle, *idle_previous);

    if (i == -1){
        *cpu_previous = cpu_total;
        *idle_previous = idle;
        return;
    }

    //cpu_use confirmed and 
    double cpu_use = fabs((double)((cpu_total - *cpu_previous) - (idle - *idle_previous)) / ((double)(cpu_total - idle) + 1e-8));
    *cpu_previous = cpu_total;
    *idle_previous = idle;


    printf("--------------------------------------------\n");
    printf("Number of Cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf(" total cpu use: %.2f%%\n", cpu_use);

    if (graphics){ CPUGraphics(terminal, cpu_use, i); }


}

void display(int samples, int tdelay, bool system, bool user, bool graphics, bool sequential){

    char terminal_memory_output[1024][1024];
    char CPU_output[1024][1024];
    double memory_previous;
    long int cpu_previous = 0, idle_previous = 0;


    CPUOutput(CPU_output, graphics, -1, &cpu_previous, &idle_previous);

    for (int i = 0; i < samples; i++){
        if (!sequential){ printf("\033[2J \033[1;1H\n"); }
        else { printf(">>> iteration %d\n", i); }

        headerUsage(samples, tdelay);
        if (system){
            systemOutput(terminal_memory_output, graphics, i, &memory_previous);
            for (int j = 0; j < samples - i - 1; j++){ printf("\n"); }
        }
        if (user){ userOutput(); }
        if (system){ CPUOutput(CPU_output, graphics, i, &cpu_previous, &idle_previous); }

        sleep(tdelay);
    }

    footerUsage();

}

int main(int argc, char *argv[]){

    int samples = 10; int tdelay = 1;
    bool system = true; bool user = true; bool graphics = false; bool sequential = false;

    bool found = false;
    bool user_specified = false, system_specified = false;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--system") == 0 || strcmp(argv[i], "-s") == 0){
            system = true; system_specified = true;
            if (!user_specified){ user = false; }
        }
        else if (strcmp(argv[i], "--user") == 0 || strcmp(argv[i], "-u") == 0){
            user = true; user_specified = true;
            if (!system_specified){ system = false; }
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
        else if (isdigit(*argv[i])){
            if (!found){
                samples = atoi(argv[i]);
                found = true;
            }
            else{ tdelay = atoi(argv[i]); }
        }
    }

    display(samples, tdelay, system, user, graphics, sequential);

    return 0;

}
