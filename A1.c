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

//TO DO: CPU, graphics, sequential?

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
    
    snprintf(visual + strlen(visual), sizeof(visual) - strlen(visual), "%*c", visual_len, sign);
    strncat(visual , &last_char, 1);

    sprintf(memoryGraphics, "%s %.2f (%.2f) ", visual, abs_diff, memory_current);
        
}

void systemOutput(char terminal[1024][1024], bool graphics, int i, double* memory_previous){

    printf("--------------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");

    struct sysinfo memory;
    sysinfo (&memory); // If sequential we can add it as a paramater and then just print the line

    double total_memory = memory.totalram * memory.mem_unit / (1024 * 1024 * 1024);
    double used_memory = (memory.totalram - memory.freeram) * memory.mem_unit / (1024 * 1024 * 1024);
    double total_virtual = (memory.totalram + memory.totalswap) * memory.mem_unit / (1024 * 1024 * 1024);
    double used_virtual = (memory.totalram - memory.freeram + memory.totalswap - memory.freeswap) * memory.mem_unit / (1024 * 1024 * 1024);
    
    sprintf(terminal[i], "%.2f GB / %.2f GB -- %.2f GB / %.2f GB", used_memory, total_memory, used_virtual, total_virtual);

    if (graphics){ 
        char graphics_output[1024]; memoryGraphicsOutput(graphics_output, used_memory, memory_previous, i);
        strcat(terminal[i], graphics_output); 
    }

    strcat(terminal[i], "\n");

    for (int j = 0; j <= i; j++){
        printf("%s", terminal[i]);
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

void CPUOutput(){

    //Ask how he wants us to calculate the CPU usage
    struct sysinfo cpu;
    sysinfo(&cpu);
    //Need to fix this 100 is a placeholder
    double use = ((double)(cpu.uptime - 100) / (double)cpu.uptime) * 100;

    printf("--------------------------------------------\n");
    printf("Number of Cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf(" total cpu use: %f%%\n", use);

    FILE *fp = fopen("/proc/stat", "r");


}

void display(int samples, int tdelay, bool system, bool user, bool graphics, bool sequential){

    char terminal_memory_output[1024][1024];
    double memory_previous;

    for (int i = 0; i < samples; i++){
        if (!sequential){
            printf("\033[2J \033[1;1H\n");
        }
        else { printf(">>> iteration %d\n", i); }

        headerUsage(samples, tdelay);

        if (system){
            systemOutput(terminal_memory_output, graphics, i, &memory_previous);
            for (int j = 0; j < samples - i - 1; j++){ printf("\n"); }
        }
        if (user){
            userOutput();
        }
        if (system){
            CPUOutput();
        }
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
            system = true;
            system_specified = true;
            if (!user_specified){ user = false; }
        }
        else if (strcmp(argv[i], "--user") == 0 || strcmp(argv[i], "-u") == 0){
            user = true;
            user_specified = true;
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

    return 1;

}
