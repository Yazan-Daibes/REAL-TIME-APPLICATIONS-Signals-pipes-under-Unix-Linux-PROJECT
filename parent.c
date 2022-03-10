// 1180414 Yazan Daibes
// 1181557 Kareen Karam 
// 1180424 Omar Qattosh

//PROJECT 1
//A multi-processing application that simulates a guessing game between 2 processes using signals and pipes.
//The parent should fork 2 children (p1 and p2) and 1 referee process (R)

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void p_intr(int);
void p_quit(int);
void Ref(int);

int p1_p2_fnsh_writ = 0;
int f_des[2]; //array for pipe, [0] for read & [1] for write
int pid_p1, pid_p2, pid_R; //stores the id of the processes

void main(void)
{
    int round = 0; //initialize the round
    int i, status;
    int bigScore1 = 0, bigScore2 = 0; //initialize the total scores
    int score[2]; //stores the sent scores from the referee after tokenization
    static char message[BUFSIZ]; 
    pid_t pid, pid_arr[3];

    // Parent creates the pipe to talk to the Referee (R)
    if (pipe(f_des) == -1)
    {
        perror("Pipe");
        exit(-1);
    }

    printf("\033[0;36m"); // cyan color
    printf(" The Game has started...\n");

    for (i = 0; i < 3; i++)
    {
        pid = fork(); //creating a new process, returns 0 for the child and an integer for the parent

        if (pid == 0 && i == 0) // forking the fist child
        {
            execl("./p1", "p1", NULL); //executing P1 file
            perror("execl -- p1");
            exit(-2);
        }

        if (pid == 0 && i == 1) // forking the second child
        {
            execl("./p2", "p2", NULL); //executing P2 file
            perror("execl -- p2");
            exit(-3);
        }

        if (pid == 0 && i == 2) // forking the referee and make it sensitive to the SIGUSR1 signal
        {
            if (signal(SIGUSR1, Ref) == SIG_ERR) //signal handling, returns -1 (SIG_ERR) on failure
            {
                perror("Signal can not set SIGUSR1");
                exit(SIGUSR1);
            }

            while (1)
            {
                pause(); //waiting for a signal
            }
        }

        else // I'm the parent
        {
            pid_arr[i] = pid;
        }
    }

    //Making the parent process sensitive to the signal SIGINT (To inform the parent that the file is ready)
    if (signal(SIGINT, p_intr) == SIG_ERR)
    {
        perror("Sigset can not set SIGINT");
        exit(SIGINT);
    }

    //Making the parent process sensitive to the signal SIGQUIT (To kill P1, P2, and R)
    if (signal(SIGQUIT, p_quit) == SIG_ERR)
    {
        perror("Signal can not set SIGQUIT");
        exit(SIGQUIT);
    }

    // A loop after each round to check if one of the children has reached the max score (50)
    while (bigScore1 < 50 && bigScore2 < 50)
    {
        //sending the signals to the processes to be informed if the files are ready
        sleep(1);
        kill(pid_arr[0], SIGUSR1); 
        sleep(2);
        kill(pid_arr[1], SIGUSR1);

        pause(); //waiting a signal

        // p1 and p2 has informed the parent that they are ready
        if (p1_p2_fnsh_writ == 2)
        {
            p1_p2_fnsh_writ = 0;

            //the parent writes to the referee the file names using pipe
            if (write(f_des[1], "child1.txt-child2.txt", strlen("child1.txt-child2.txt")) == -1)
            {
                perror("Write");
                exit(-4);
            }

            kill(pid_arr[2], SIGUSR1); //pid_arr[2] is the pid of the refeere R

            sleep(1);

            //the parent read the scores from the referee
            if (read(f_des[0], message, BUFSIZ) != -1)
            {
                round++;
                // printf("Parent: Score = %s\n", message); //the message contains the two scores
                //fflush(stdout);

                // seperate the scores and store them in the score array
                char *token = strtok(message, "-");
                int k = 0;
                while (token)
                {
                    score[k] = atoi(token);
                    token = strtok(NULL, "-");
                    k++;
                }

                bigScore1 = bigScore1 + score[0]; //adding the new round scores to the total score
                bigScore2 = bigScore2 + score[1];

                //Compares the current scores to find the winner in the current round
                if (score[0] > score[1])
                {
                    printf("\033[1;33m");
                    printf("              P1 won ");
                    printf("\033[0;31m"); // red color
                    printf("round #%d ", round);
                    printf("\033[1;33m");
                    printf("with Score = %d\n", score[0]);
                    printf("              P1 BigScore = %d and P2 BigScore = %d\n", bigScore1, bigScore2);
                    fflush(stdout);
                }

                else if (score[0] < score[1])
                {
                    printf("\033[1;33m"); // yellow
                    printf("              P2 won ");
                    printf("\033[0;31m"); // red color
                    printf("round #%d", round);
                    printf("\033[1;33m");
                    printf(" with Score = %d\n", score[1]);
                    printf("              P1 BigScore = %d and P2 BigScore = %d\n", bigScore1, bigScore2);
                    fflush(stdout);
                }

                else
                {
                    printf("\033[0;31m"); // red color
                    printf("              Round #%d ", round);
                    printf("\033[1;33m"); // yellow
                    printf("is a tie\n");
                    printf("              P1 BigScore = %d and P2 BigScore = %d\n", bigScore1, bigScore2);
                    fflush(stdout);
                }

                printf("---------------------------------------------------\n");

                //The game will end if one of the players' total score reaches 50 or more and becomes the winner
                if (bigScore1 >= 50 && bigScore2 >= 50)
                {
                    printf("\033[0;32m"); // green color
                    printf("***********************************************************************************\n");
                    printf("The game winners are P1 : BigScore1 = %d and P2: BigScore2 = %d \n", bigScore1, bigScore2);
                    printf("                  The number of neeeded rounds was = %d                          \n", round);
                    printf("***********************************************************************************\n");
                    fflush(stdout);
                }

                else if (bigScore1 >= 50)
                {
                    printf("\033[0;32m"); // green color
                    printf("***********************************************************************************\n");
                    printf("                  The game winner is P1 : BigScore1 = %d                      \n", bigScore1);
                    printf("                  The number of neeeded rounds was = %d                          \n", round);
                    printf("***********************************************************************************\n");
                    fflush(stdout);
                }

                else if (bigScore2 >= 50)
                {
                    printf("\033[0;32m"); // green color
                    printf("***********************************************************************************\n");
                    printf("                   The game winner is P2 : BigScore2 = %d                      \n", bigScore2);
                    printf("                   The number of neeeded rounds was = %d                          \n", round);
                    printf("***********************************************************************************\n");
                    fflush(stdout);
                }
            }

            else //else for error reading the scores from the referee
            {
                perror("Read");
                exit(-5);
            }
        }
    }

    printf("\033[0;36m"); // cyan color
    printf("***********************************************************************************\n");
    printf("***********************************************************************************\n");
    printf("Done By: 1180414 Yazan Daibes \n         1180424 Omar Qattosh\n         1181557 Kareen Karam\n");
    printf("***********************************************************************************\n");
    printf("***********************************************************************************\n");

    // terminate the program
    kill(pid_arr[0], SIGTERM);
    kill(pid_arr[1], SIGTERM);
    kill(pid_arr[2], SIGTERM);
    kill(getppid(), SIGTERM);
}

void p_intr(int the_sig)
{
    // if p1_p2_fnsh_writ is 2: that means the parent has recieved the 2 signals from p1 and p2 (P1 and P2 are ready)
    p1_p2_fnsh_writ++;
}

void p_quit(int the_sig)
{
    kill(pid_p1, SIGTERM);
    kill(pid_p2, SIGTERM);
    kill(pid_R, SIGTERM);
}

void Ref(int the_sig)
{
    static char message[BUFSIZ];
    char files[2][20];
    char *tmp;
    int i = 0;
    int num1, num2;
    int score1 = 0, score2 = 0;

    char tmp_score[8];
    char tmp_score1[3];

    char total_score[8];

    FILE *fptr_p1;
    FILE *fptr_p2;

    //Reading a message sent from the parent which contains the files (child1.txt-child2.txt) through pipe
    if (read(f_des[0], message, BUFSIZ) == -1)
    {
        perror("Read");
        exit(-6);
    }

    char *token = strtok(message, "-");
    // sperate the files and store them in the "file" arrray
    while (token != NULL)
    {
        strcpy(files[i], token); //saves the first file in the array
        tmp = strdup(files[i]); //duplicates the first file and stores it in tmp variable
        strcpy(files[i], "./"); //files[i] will contain "./"
        strcat(files[i], tmp); // concatination the "./" with the first file name
        token = strtok(NULL, "-"); //these steps will be repeated until end of message
        i++;
        free(tmp);
    }

    if ((fptr_p1 = fopen(files[0], "r")) == NULL) //files[0] stores child1.txt
    {
        perror("CHILD1_FILE");
        exit(-7);
    }

    if ((fptr_p2 = fopen(files[1], "r")) == NULL) //files[1] stores child2.txt
    {
        perror("CHILD2_FILE");
        exit(-8);
    }

    for (int j = 0; j < 10; j++)
    {
        fscanf(fptr_p1, "%d\n", &num1); //Getting the numbers stored from the 2 files
        fscanf(fptr_p2, "%d\n", &num2);
        //printf("num1 = %d  ", num1);
        //printf("num2 = %d  \n", num2);

	//Comparing the integers line by line and increments the score according to the respective number picked by P1 or P2
        if (num1 > num2)
        {
            score1++;
        }

        else if (num1 < num2)
        {
            score2++;
        }
    }

    //Closing the files
    fclose(fptr_p1); 
    fclose(fptr_p2);

    //delete the files
    unlink(files[0]);
    unlink(files[1]);

    //concatenate score 1 and 2 in order to send them to the parent through the pipe
    sprintf(tmp_score, "%d", score1);
    strcat(tmp_score, "-");
    sprintf(tmp_score1, "%d", score2);
    strcat(tmp_score, tmp_score1);
    
    //Sending the concatenated score to the parent through pipe using f_des[1]
    if (write(f_des[1], tmp_score, strlen(tmp_score)) == -1)
    {
        perror("Write");
        exit(-9);
    }

    printf("\033[0;34m");
    printf("              P1-P2\n");
    printf("              %s\n", tmp_score);
}
