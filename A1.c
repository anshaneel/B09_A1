#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>

// Function to parse command line arguments
void parseArgs(int argc, char *argv[], int *samples, int *tdelay, int *graphics, int *sequential, int *sys, int *user) {
    int opt;
    while ((opt = getopt(argc, argv, "s:t:gus")) != -1) {
        switch (opt) {
            case 's':
                *samples = atoi(optarg);
                break;
            case 't':
                *tdelay = atoi(optarg);
                break;
            case 'g':
                *graphics = 1;
                break;
            case 'u':
                *sequential = 1;
                break;
            case 's':
                *sys = 1;
                break;
            case 'u':
                *user = 1;
                break;
            default:
                printf("Usage: %s [-s samples] [-t tdelay] [-g graphics] [-s system] [-u user]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind < argc) {
        if (argc - optind == 2) {
            *samples = atoi(argv[optind]);
            *tdelay = atoi(argv[optind+1]);
        } else {
            printf("Usage: %s [-s samples] [-t tdelay] [-g graphics] [-s system] [-u user]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

// Function to get number of connected users
int getConnectedUsers() {
    struct utmp *entry;
    int count = 0;
    setutent();
    while ((entry = getutent()) != NULL) {
        if (entry->ut_type == USER_PROCESS) {
            count++;
        }
    }
    endutent();
    return count;
}

// Function to get number of sessions per user
void getUserSessions(int *sessions) {
    struct utmp *entry;
    setutent();
    while ((entry = getutent()) != NULL) {
        if (entry->ut_type == USER_PROCESS) {
            sessions[entry->ut_user]++;
        }
    }
    endutent();
}

// Function to get system usage
void getSystemUsage(float *cpu, int *memory) {
    FILE *file;
    char line[100];
    int i;

    // Get CPU usage
    file = fopen("/proc/stat", "r");
    fgets(line, 100, file);
    fclose(file);
    sscanf(line, "cpu %f %*f %*f %*f", &cpu);

    // Get memory usage
    file = fopen("/proc/meminfo", "r");
    for (i = 0; i < 2; i++) {
        fgets(line, 100, file);
    }
    sscanf(line, "MemTotal: %d kB", &memory[0]);
    for (i = 0; i < 3; i++) {
        fgets(line, 100, file);
    }
    sscanf(line, "MemFree: %d kB", &memory[1]);
    fclose(file);
}

// Function to print system usage
void printSystemUsage(float cpu, int *memory) {
    printf("Memory usage: %d kilobytes\n", memory[0]);
    printf("Free memory: %d kilobytes\n", memory[1]);
    printf("CPU usage: %.2f%%\n", cpu);
}

// Function to print user usage
void printUserUsage(int users, int *sessions) {
    int i;
    printf("Number of connected users: %d\n", users);
    printf("Sessions per user:\n");
    for (i = 0; i < 32; i++) {
        if (sessions[i] > 0) {
            printf("%c: %d\n", i, sessions[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    int samples = 10, tdelay = 1, graphics = 0, sequential = 0, sys = 0, user = 0;
    int i, users, memory[2], sessions[32] = {0};
    float cpu;

    parseArgs(argc, argv, &samples, &tdelay, &graphics, &sequential, &sys, &user);

    for (i = 0; i < samples; i++) {
        if (sys) {
            getSystemUsage(&cpu, memory);
            if (!sequential) {
                printf("Sample %d\n", i+1);
                printSystemUsage(cpu, memory);
                printf("------------------------------\n");
            } else {
                printf("Sample %d: CPU usage: %.2f%%, Memory usage: %d kB, Free memory: %d kB\n", i+1, cpu, memory[0], memory[1]);
            }
        }
        if (user) {
            users = getConnectedUsers();
            getUserSessions(sessions);
            if (!sequential) {
                printf("Sample %d\n", i+1);
                printUserUsage(users, sessions);
                printf("------------------------------\n");
            } else {
                                printf("Sample %d: Number of connected users: %d\n", i+1, users);
                for (int j = 0; j < 32; j++) {
                    if (sessions[j] > 0) {
                        printf("User %c: %d sessions\n", j, sessions[j]);
                    }
                }
            }
        }
        sleep(tdelay);
    }

    if (graphics) {
        // Graphical output for memory usage
        printf("Graphical representation of memory usage:\n");
        float percent = ((float)memory[1] / (float)memory[0]) * 100;
        int blocks = (int)(percent / 5);
        for (i = 0; i < blocks; i++) {
            printf("::::::@");
        }
        for (i = 0; i < 20 - blocks; i++) {
            printf("######*");
        }
        printf("\n");

        // Graphical output for CPU usage (if samples > 1)
        if (samples > 1) {
            printf("Graphical representation of CPU usage:\n");
            blocks = (int)(cpu / 5);
            for (i = 0; i < blocks; i++) {
                printf("||||");
            }
            printf("\n");
        }
    }

    return 0;
}

