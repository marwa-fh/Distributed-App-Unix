#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include<time.h>
#include<sys/wait.h>

//function to return string between 2 ":"
char* secondstr(char *msr)
{

    char *target = NULL;
    char *start, *end;

    if ( start = strstr( msr,":" ) )
    {
        start += strlen( ":");
        if ( end = strstr( start,":" ) )
        {
            target = ( char * )malloc( end - start + 1 );
            memcpy( target, start, end - start );
            target[end - start] = '\0';

        }
    }
    return target;
}

int main()
{
//declare nb of client
    int i=0;
    while(i<3)
    {
        i++;

        int ca[2],ac[2],cg[2],gc[2],ag[2],ga[2],child1;
        char msr[50],tcS[50],tm[50],login[50],rt[50],response[50],prdct[10] ;
        //assume we have only 3 users with 3 passwords
        char up1[]="Auth:user1:1111", up2[]="Auth:user2:1234",up3[]="Auth:user3:33";
        char user[20],pass[20];
        char ms[]="Auth:";
        char aup[100],cltt[50];
        char us[30],psw[30],result[70];
        int flag=0;



        if(pipe(ca)==-1 || pipe(ac)==-1 || pipe(cg)==-1 ||pipe(gc)==-1 ||pipe(ag)==-1 || pipe(ga)==-1)
        {
            printf("error in pipe creation\n");
            exit(1);
        }

        if((child1=fork())==-1)
        {
            printf("error in fork creation\n");
            exit(2);

        }


        if(child1==0)
        {

            int nbAttempt=0;
            //Client code
            close(ca[0]);
            close(ac[1]);
            close(ag[1]);
            close(ga[0]);
            close(ag[0]);
            close(ga[1]);
            //printf("Enter username & Password in the format: Auth:username:password \n");
            // scanf("%s",aup);
            printf("\nClient %d: ",i);
            printf("\n\nEnter username: ");
            scanf("%s",us);
            printf("\nEnter password: ");
            scanf("%s",psw);
            strcpy(aup,"Auth:");
            strcat(aup,us);
            strcat(aup,":");
            strcat(aup,psw);
            //client sends Auth messege to authServer
            write(ca[1],aup, (strlen(aup)+1));
            sleep(1);
            read(ac[0],login,  sizeof(login));
            printf("\nMessage 4:\n%s",login);
            fflush(stdout);
            sleep(2);

            strcpy(rt,"Ticket:");
            strcat(rt,us);
            strcat(rt,":");
            char * tn= strrchr(login,':')+1;
            strcat(rt,tn);
            // The client then sends to the Game server the received ticket.
            write(cg[1],rt, (strlen(rt)+1));
            //printf("\nReceived:%s",rt);
            read(gc[0],response,  sizeof(response));
            printf("\nMessage 6:\n%s",response);
            if(response=="ReadyToStart")
            {
                FILE *f=fopen("t1.sh","r");
                while(nbAttempt<10)
                {
                    //clients should predict this word

                    fgets(prdct,11,f);
                    write(cg[1],prdct, (strlen(prdct)+1));
                    nbAttempt++;
                }

                sleep(2);
                read(gc[0],result,sizeof(result));
                printf("\nresult %s\n",result);
                fflush(stdout);
                fclose(f);
            }

            exit(i);
        }



        else
        {
            //AuthServer code

            char *usr,*tcn,win[70],nowin[70];
            char *usr1,*tcn1;
            char tcSaved[70],wclient[10];
            int countr=0;


            int child2=fork();
            if(child2==0)
            {
                //GameServer code
                close(ca[0]);
                close(ca[1]);
                close(ac[0]);
                close(ac[1]);
                close(ag[1]);
                close(ga[0]);
                // receive ticket from AuthServer
                read(ag[0],tm,  sizeof(tm));
                printf("\nMessege 2:\n%s",tm);
                fflush(stdout);
                //save the username and the received ticket.
                usr=secondstr(tm);
                tcn= strrchr(tm,':')+1;

                sleep(2);

                printf("\nTicket messege recieved with user:%s,ticket-nb:%s",usr,tcn);
                fflush(stdout);


                strcpy(tcSaved,"TicketSaved:");
                strcat(tcSaved,usr);
                printf("\nsaved:%s",tcSaved);
                // fflush(stdout);
                sleep(1);
                write(ga[1],tcSaved, (strlen(tcSaved)+1));

                // read the received ticket from client.
                read(cg[0],cltt,  sizeof(cltt));
                printf("\nMessage 5:\nReceived:%s",cltt);
                fflush(stdout);
                //take user and ticket nb
                usr1=secondstr(cltt);
                tcn1= strrchr(cltt,':')+1;
                //send response to client
                //if user or ticket doesnt match
                if(strcmp(usr,usr1)!=0 || strcmp(tcn,tcn1)!=0)
                {
                    char invalid []="InvalidAuthentication";
                    write(gc[1],invalid, (strlen(invalid)+1));
                }
                //otherwise user & tck nb match
                else
                {
                    //number of connected clients having valid tickets is less than 3
                    if(i<3)
                    {
                        char Waiting []="WaitingPlayers";
                        write(gc[1],Waiting, (strlen(Waiting)+1));
                    }
                    else
                    {
                        if(i=3)
                        {
                            //Game server generates a Word
                            char word[]="MysterWord";
                            char Ready []="ReadyToStart";
                            write(gc[1],Ready, (strlen(Ready)+1));
                            //wait(NULL);
                        }
                        else
                        {
                            char  Noplace []=" Noplace";
                            write(gc[1], Noplace, (strlen( Noplace)+1));
                        }
                    }
                    sleep(2);
                    // read the received word from client.
                    char word[]="MysterWord";
                    while(read(cg[0],wclient,  sizeof(wclient))!=0)
                    {
                        //read(cg[0],wclient,  sizeof(wclient));
                        printf("\nClient word\n%s\n-------------------------------------------------------",wclient);
                        countr++;
                        if(strcmp(wclient,word)==0)
                        {
                            flag=1;
                            break;
                        }
                        if (countr==10)
                            break;
                    }

                    if(flag!=1)
                    {
                        strcpy(win,"No winner, closest : ");
                        strcat(win,usr);
                        strcat(win,", Attempts");
                    }
                    else
                    {
                        char co[20];
                        sprintf(co,"%d",countr);
                        strcpy(win,"Winner :");
                        strcat(win,usr);
                        strcat(win,", Attempts :");
                        strcat(win,co);
                    }
                    sleep(1);
                    write(gc[1],win,(strlen(win)+1));
                }
            }


            else
            {


                //AuthenticationServer remaining code
                close(ca[1]);
                close(ac[0]);
                close(ag[0]);
                close(ga[1]);
                char s1[100];
                char msgt[100];
                char *user;
                int r;

                sleep(2);
                //read authMessege from Client
                read(ca[0],msr,  sizeof(msr));

                srand(time(NULL));
                user=secondstr(msr);
                r=rand();
                char snum[20];

                sprintf(snum,"%d",r);
                strcpy(msgt,"Ticket:");
                strcat(msgt,user);
                strcat(msgt,":");
                strcat(msgt,snum);
                printf("\nMessage 1:\nTicket generated by the AuthServer %s",msgt);
                fflush(stdout);
                //send generated ticket with user and random nb to gameServer
                write(ag[1],msgt, (strlen(msgt)+1));
                //recieve ticket saved from game
                // wait(&tcs);
                read(ga[0],tcS, sizeof(tcS));

                printf("\nMessage 3:\nReceived from game %s",tcS);
                fflush(stdout);
                //check if user & password are correct
                if(strcmp(msr,up1)!=0 && strcmp(msr,up2)!=0 && strcmp(msr,up3)!=0)
                {

                    char fail[]="AuthenticationFailure";
                    write(ag[1],fail, (strlen(fail)+1));

                    /* first, find my own process ID */
                    pid_t my_pid = getpid();
                    /* now that i got my PID, send myself the STOP signal. */
                    //kill(my_pid, SIGSTOP);


                }
                else
                {
                    //printf("\nLogin successfull ");
                    char ok[50];
                    strcpy(ok,"OK:");

                    strcat(ok,snum);
                    write(ac[1],ok, (strlen(ok)+1));
                }
                wait(NULL);

            }
        }
    }

    return (0);
}






