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
    /**
    * Retrieves and prints the current memory usage in kilobytes
    * 
    * @samples: number of samples 
    * @tdelay: time delay between each sample in seconds
    *
    * The function uses the getrusage function from the sys/resource.h library to retrieve information about the memory usage
    * of the calling process and then prints the result to the terminal.
    *
    * Outputs: Nbr of samples: [samples] -- every [tdelay] secs
    *           Memory usage: [used_memory] kilobytes
    */

    // Get information about the utilization of memory in kilobytes and store it in used_memory
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    long used_memory = usage.ru_maxrss;

    // Print the number of samples, tdelay, and memory usgae
    printf("Nbr of samples: %d -- every %d secs\n Memory usage: %ld kilobytes\n", samples, tdelay, used_memory);

}

void footerUsage(){
    /**
     * Retrives and prints system information
     *
     * The function gets information about the system using uname function from sys/utsname.h library
     * 
     * Output:
     * --------------------------------------------
     * ### System Information ###
     *  System Name = [sysname]
     *  Machine Name = [nodename]
     *  Version = [version]
     *  Release = [release]
     *  Architecture = [machine]
     *  --------------------------------------------
     *
     */
    
    // Retrive System information
    struct utsname sysinfo;
    uname(&sysinfo);

    // Prints relavent information such as system name, Machine name, Version, Releasem Architecture
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
    /**
    * Outputs a graphical representation of the change in memory
    * 
    * @memoryGraphics: char array of size 1024 to store the output
    * @memory_current: double representing the current memory usage
    * @memory_previous: a pointer to a double representing the previous memory usage
    * @i: integer representing the index of current iteration
    *
    *
    */

    // If first iteration then store previous memory usage as current memeory to prevent seg fault
    if (i == 0){ *memory_previous = memory_current; }
        
    // Absolute value of difference in memory usage
    double diff = memory_current - *memory_previous;
    double abs_diff = fabs(diff);
    
    // initialize the stting for viuals and make the length add one for every 0.01 change in memory
    char visual[1024] = "   |";
    int visual_len = (int)( abs_diff / 0.01 );
    char last_char;
    char sign;

    // Chppse the appropriate sign and last character based on the difference in memory usage
    // Positive non zero difference makes sign '#' and last character '*'. positive zero changes the last char to 'o'
    // Negative difference makes the sign ':' and last character '@'
    if (diff >= 0) { 
        sign = '#';
        last_char = '*';
        if (visual_len == 0) { last_char = 'o'; }
    }
    else { sign = ':'; last_char = '@'; }

    // Set previous memory to current for the next iteration
    *memory_previous = memory_current;
    
    // Create the graphics starting at 4 to account for the starting of visuals ("   |")
    for (int i = 4; i < visual_len + 4; i++){
        visual[i] = sign;
    }
    visual[visual_len + 4] = '\0';
    
    strncat(visual , &last_char, 1);

    // Add the graphics to the output string
    sprintf(memoryGraphics, "%s %.2f (%.2f)", visual, abs_diff, memory_current);

}

void systemOutput(char terminal[1024][1024], bool graphics, int i, double* memory_previous){
    /**
    * Function Gets memory information of function and stores it in terminal then prints all memory information thus far
    * Gets information using sysinfo function from sys/sysinfo.h library
    *
    * @terminal: array of strings for the output
    * @graphics: boolean value indicaing if graphics option has been selected
    * @i: int value indicating the current iteration
    * @memory_previous: pointer to double that contains the last memory usage calculated
    *
    */

    // Divider
    printf("--------------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");

    // Gets memory
    struct sysinfo memory;
    sysinfo (&memory);

    // Calculate the memory usgae and total memory in GB
    double total_memory = (double) memory.totalram / (1024 * 1024 * 1024);
    double used_memory =  (double) (memory.totalram - memory.freeram) / (1024 * 1024 * 1024);
    double total_virtual = (double) (memory.totalram + memory.totalswap) / (1024 * 1024 * 1024);
    double used_virtual = (double) (memory.totalram - memory.freeram + memory.totalswap - memory.freeswap) / (1024 * 1024 * 1024);
    
    // Add the memmory usage to terminal
    sprintf(terminal[i], "%.2f GB / %.2f GB -- %.2f GB / %.2f GB", used_memory, total_memory, used_virtual, total_virtual);

    // If graphics is enabled then add the visual from the graphics function
    if (graphics){ 
        char graphics_output[1024]; memoryGraphicsOutput(graphics_output, used_memory, memory_previous, i);
        strcat(terminal[i], graphics_output); 
    }

    // Prints terminal
    for (int j = 0; j <= i; j++){
        printf("%s\n", terminal[j]);
    }
}

void userOutput(){
    /**
    * Prints information about current user sessions
    * gets information from utmp.h library and prints to each session to terminal
    *
    */

    printf("--------------------------------------------\n");
    printf("### Sessions/users ###\n");

    // Initalize and open utmp
    struct utmp *utmp;
    setutent();

    while ((utmp = getutent()) != NULL) {
        // Checks for user process
        if (utmp -> ut_type == USER_PROCESS) {
            // Prints the User, session, host
            printf("%s\t %s (%s)\n", utmp -> ut_user, utmp -> ut_line, utmp -> ut_host);
        }
    }

    // close utmp
    endutent();

}

void CPUGraphics(char terminal[1024][1024], double usage, int i){
    /**
    * Prints a graphical representation of CPU usage
    *
    * @terminal: array of strings that stores the output to print to the terminal
    * @usage: current cpu usage
    * @i: current iteration of program
    *
    * The graphical represntation for usage always starts with '|||' then adds another bar for every percent change in usage
    *
    */

    // Calculates the length of the visual string adding 12 for starting characters
    int visual_len = (int)(usage) + 12;

    // Creates graphics and stores in terminal
    strcpy(terminal[i], "         ");

    for (int j = 9; j < visual_len; j++){
        terminal[i][j] = '|';
    }
    terminal[i][visual_len] = '\0';

    // Adds the usage to string
    sprintf(terminal[i] + visual_len, " %.2f", usage);

    // Prints history of cpu usage and graphics
    for (int j = 0; j <= i ; j++){
        printf("%s\n", terminal[j]);
    }
}

void CPUOutput(char terminal[1024][1024], bool graphics, int i, long int* cpu_previous, long int* idle_previous){
    /**
    * Prints information about the current CPU usage of the system
    *
    * @terminal: An array of strings that stores the terminal output
    * @graphics: A boolean value indicating whether graphics option has been selected
    * @i: An integer representing the 
    * @cpu_previous: A pointer to a long int to store the previous CPU usage 
    * @idle_previous: A pointer to a long int to store the previous idle time
    *
    * Function gets CPu usage using sysinfo function from sys/sysinfo.h library
    *
    */

    // Gets system info
    struct sysinfo cpu;
    sysinfo(&cpu);
    
    // Opens proc stat file with cpu usage
    FILE *fp = fopen("/proc/stat", "r");
    long int user, nice, system, idle, iowait, irq, softirq;
    fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(fp);

    // Calculates total usage of cpu
    long int cpu_total = user + nice + system + iowait + irq + softirq;

    // If -1 is passed as iteration then we only define previous values to prevent seg fault
    if (i == -1){
        *cpu_previous = cpu_total;
        *idle_previous = idle;
        return;
    }

    // Cpu value calculations
    long int total_prev = *cpu_previous + *idle_previous;
    long int total_cur = idle + cpu_total;
    double totald = (double) total_cur - (double) total_prev;
    double idled = (double) idle - (double) *idle_previous;
    double cpu_use = fabs((1000 * (totald - idled) / (totald + 1e-6) + 1) / 10);

    if (cpu_use > 100){ cpu_use = 100; }

    // Makes the previous usage equal to the current for the next iteration
    *cpu_previous = cpu_total;
    *idle_previous = idle;

    // Prints the number of cores and cpu usage
    printf("--------------------------------------------\n");
    printf("Number of Cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf(" total cpu use: %.2f%%\n", cpu_use);

    // If graphics have been slected then call the graphics function to add the visuals
    if (graphics){ CPUGraphics(terminal, cpu_use, i); }


}

void display(int samples, int tdelay, bool system, bool user, bool graphics, bool sequential){
    /**
    * Outputs all the system information according to the command line arguments selected by user
    * 
    * @samples: the number of times the information will be displayed
    * @tdelay: the time delay between each sample in seconds
    * @system: boolean value indicating whether systems information has been selected
    * @user: boolean value indicating whether user information has been selected
    * @graphics: boolean value indicating whether graphics output has been selected
    * @sequential: boolean value indicating whether equential output has been selected
    * 
    * Displays header, system output, user output, cpu output, and footer
    * Graphics adds visuals to memeory and cpu usage
    * Equential prints information in sequential manner
    */

    // Initialize variables for terminal and cpu output, aswell as previous values for memeory and cpu usage
    char terminal_memory_output[1024][1024];
    char CPU_output[1024][1024];
    double memory_previous;
    long int cpu_previous = 0, idle_previous = 0;

    // Initialize the cpu information with -1 iteration
    CPUOutput(CPU_output, graphics, -1, &cpu_previous, &idle_previous);
    sleep(1);
    // Loop samples number of times
    for (int i = 0; i < samples; i++){
        // If sequential is selected then we do not reset terminal between iterations and state iteration number
        if (!sequential){ printf("\033[2J \033[1;1H\n"); }
        else { printf(">>> iteration %d\n", i); }

        // Displays header information
        headerUsage(samples, tdelay);

        // If system is slected diplays systems information usinf systemOutput function
        if (system){
            systemOutput(terminal_memory_output, graphics, i, &memory_previous);
            for (int j = 0; j < samples - i - 1; j++){ printf("\n"); }
        }
        
         // If user is slected diplays user information usinf userOutput function
        if (user){ userOutput(); }
        // Displays cpu information using CPUOutput function is system is selected
        if (system){ CPUOutput(CPU_output, graphics, i, &cpu_previous, &idle_previous); }

        // Delay the output for tdelay seconds
        sleep(tdelay);
    }

    // Displays footer
    footerUsage();

}

int main(int argc, char *argv[]){
    /**
    * Entry point of the program, parses command line arguments to pass values to display function
    *
    * @argc: Number of command-line arguments
    * @argv: Array of pointers to command-line arguments as strings
    *
    * Return: 0 on success, non-zero on error
    */

    // Default values if not specified
    int samples = 10; int tdelay = 1;
    bool system = true; bool user = true; bool graphics = false; bool sequential = false;

    // boolean values to check if arguments have been seen previously
    bool found = false;
    bool user_specified = false, system_specified = false;

    // Parse command line arguments
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
            // Gets integer in string, and sets found to true to indicate that samples have been seen
            sscanf(argv[i], "%d", &samples);
            found = true;
        }
        else if (strncmp(argv[i], "--tdelay=", 9) == 0){
            sscanf(argv[i], "%d", &tdelay);
        }
        // If integer is passed as command line argument the first is samples and the second is delay
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
