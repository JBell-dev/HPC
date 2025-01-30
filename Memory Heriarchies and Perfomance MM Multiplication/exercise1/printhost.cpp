#include <unistd.h>
#include <stdio.h> //since c is allowed by the exercise, i will use C output functions
//Sources: 
//https://man7.org/linux/man-pages/man2/gethostname.2.html
//https://stackoverflow.com/questions/27914311/get-computer-name-and-logged-user-name
#define MAXHOSTNAME 256
int main() {
    char hostname[MAXHOSTNAME]; 
    gethostname(hostname, MAXHOSTNAME);
    printf("Hostname: %s\n", hostname); 
    return 0;
}

//we compile with gcc printhost.cpp -o printhost
//srun --nodes=1 --time=00:05:00  --pty bash -i