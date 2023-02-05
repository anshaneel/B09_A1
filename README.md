Ansh Aneel

B09_A1

# System Monitoring Tool

  A C program that will report different metrics of the utilization of a given system
  
## Libraries Used

- stdio.h
- stdlib.h
- unistd.h
- string.h
- stdbool.h
- ctype.h
- sys/resource.h
- sys/utsname.h
- sys/sysinfo.h
- sys/types.h
- math.h
- utmp.h

## Functions
### void headerUsage(int samples, int tdelay):
    
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
    * Outputs: 
    * Nbr of samples: [samples] -- every [tdelay] secs
    *           Memory usage: [used_memory] kilobytes
    */
    
    
    
    
### void footerUsage():

   
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
    
    
    
    
### void memoryGraphicsOutput(char memoryGraphics[1024], double memory_current, double* memory_previous, int i):

   
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
    
    
    
    
### void systemOutput(char terminal[1024][1024], bool graphics, int i, double* memory_previous): 
    
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
    
    
    
    
### void userOutput(): 
   
    void userOutput(){
    /**
    * Prints information about current user sessions
    * gets information from utmp.h library and prints to each session to terminal
    *
    */
    
    
    
    
### void CPUGraphics(char terminal[1024][1024], double usage, int i): 
   
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
    
    
    
    
### void CPUOutput(char terminal[1024][1024], bool graphics, int i, long int* cpu_previous, long int* idle_previous): 
   
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
    
    
    
    
### void display(int samples, int tdelay, bool system, bool user, bool graphics, bool sequential): 
   
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
    
    
    
    
### int main(int argc, char *argv[]): 
   
    int main(int argc, char *argv[]){
    /**
    * Entry point of the program, parses command line arguments to pass values to display function
    *
    * @argc: Number of command-line arguments
    * @argv: Array of pointers to command-line arguments as strings
    *
    * Return: 0 on success, non-zero on error
    */
    
    
    
    

## How I solved the problem: 

I approached this problem by splitting it up into chunks and building functionality from the ground up. I started by parsing the command line arguments so I could test the other functions in isolation when implemented, then passed the passed argument to the display function.

The display function is where the program was put together, it would call all the functions based on the arguments passed and print them sample number of times with a delay of tdelay between each iteration. The function refreshes the terminal between iteration if sequential is not selected, it does this by initializing a 2D array to represent a string array and store each line of terminal output, then we print the output with an extra line after the next iteration.

Additionally the graphics option selected applies to memory and cpu output, to configure the visuals I created char arrays to represent strings and initialized them with characters based on the assignment specifications then adding them to the main strings that stores values to print in the terminal.

The cpu visuals are initialized with 3 bars "|||" then an additional bar for every percentage increase in cpu usage
The memory visuals are initialized with a symbol for every 0.01 change in memory usage


