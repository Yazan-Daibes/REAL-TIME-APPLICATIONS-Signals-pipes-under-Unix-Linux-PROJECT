#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define CHILD1_FILE "./child1.txt"

void p1_picks_integers(int);

int main()
{
    //making P1 sensitive to signal SIGUSR1
    if (signal(SIGUSR1, p1_picks_integers) == SIG_ERR) //SIG_ERR = -1
    {
        perror("Sigset can not set SIGUSR1");
        exit(SIGUSR1);
    }

    while (1)
    {
        pause();
        kill(getppid(), SIGINT); //Sending the signal to the parent process to inform that the file is ready
    }

    return 0;
}

//A function which is used to pick 10 random integers in the range (1, 100)
void p1_picks_integers(int the_sig)
{
    FILE *fptr_p1;
    int n1;
    
    //opens CHILD1 file for appending, creates the file if it does not exist 
    if ((fptr_p1 = fopen(CHILD1_FILE, "a")) == NULL) 
    {
        perror("CHILD1_FILE");
        exit(-3);
    }

    sleep(1); // sleep to get different seed
    srand(time(0));

    for (int i = 0; i < 10; i++)
    {
        n1 = (int)(1 + rand() % 99); //get random  number
        fprintf(fptr_p1, "%d\n", n1); //print the random numbers in the file in a seperate line
    }

    fclose(fptr_p1); //closing the file
}
