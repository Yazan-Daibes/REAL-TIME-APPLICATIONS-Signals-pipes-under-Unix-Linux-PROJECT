#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void p2_picks_integers(int);

#define CHILD2_FILE "./child2.txt"

int main()
{
    //Making P2 sensitive to signal SIGUSR1
    if (signal(SIGUSR1, p2_picks_integers) == SIG_ERR)
    {
        perror("Signal can not set SIGUSR1");
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
void p2_picks_integers(int the_sig)
{
    FILE *fptr_p2;
    int n2;

    //opens CHILD2 file for appending, creates the file if it does not exist 
    if ((fptr_p2 = fopen(CHILD2_FILE, "a")) == NULL)
    {
        perror("CHILD2_FILE");
        exit(-2);
    }

    sleep(1); // sleep to get different seed
    srand(time(0));

    for (int i = 0; i < 10; i++)
    {
        n2 = (int)(1 + rand() % 99); //get random  number
        fprintf(fptr_p2, "%d\n", n2); //print the random numbers in the file in a seperate line
    }

    fclose(fptr_p2); //closing the file
}
