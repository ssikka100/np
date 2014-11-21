#include "unp.h"
#define SIZE 1024
struct hostent *he;
struct in_addr a;
long addr;

char* byname(char *argv1);
void byaddr(char *argv);
void sig_chld(int signo);
void error(char *msg);

int main(int argc, char **argv)
{

if (argc !=2)
{
    printf("Invalid arguments:Usage <a1client> <IP Address/Domain Name> \n");
    exit(0);
}

int k = atoi(argv[1]);

if(k) // if IP address is entered
{ printf("The IP address and Server name is\n");byaddr(argv[1]);}

if(!k) // if Name is entered
{   printf("The IP address and Server name is\n");
    char *j=byname(argv[1]);
    argv[1]=j;
}


service:
for(;;)
{   //int n;
    char m;
    pid_t childpid;
    int   stat;
    int pfd[2];
    pipe(pfd);
    int nread;
    char buf[MAXLINE];

    printf("Enter the Service wanted: \n(1) Echo \n(2) Time \n(3) Quit \n");
    scanf("%s", &m);
    int n = atoi(&m);

    //user enters quit

    if(n==3)
    exit(0);

    //user enters echo service

    else if(n==1)

    {
            //creating socket for echoclient


            int esockfd;
            struct sockaddr_in servaddr;
            if((esockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
            {error("socket error for echo sock:");}
            bzero(&servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(26262);
            if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
            error("inet_pton error for given IP address");

            if ((connect(esockfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
            error("connect error for echo sock:");


            char connfd[sizeof(&esockfd)];
            char pfdstr_w[sizeof(&pfd[1])];

            memset(connfd,0,sizeof(connfd));
            memset(pfdstr_w,0,sizeof(pfdstr_w));

            sprintf(connfd,"%d",esockfd);  //converting sockfd to char
            sprintf(pfdstr_w,"%d",pfd[1]);    //converting pipefd to char

            printf("\nforking child for echo service....... \n");

            signal (SIGCHLD, sig_chld);

            if( (childpid = fork()) == 0)
            {
                if ( (execlp("xterm", "xterm", "-e", "./a1ech", argv[1], connfd, pfdstr_w, (char *) 0)) < 0)
                    {error("Not able to execute xterm:"); }
            }

            printf("\nchild forked successfully in new terminal!! \n");
            close(esockfd);

            if (childpid < 0)
            {
                error("Error creating child for echo:");
            }
            //else
            if (childpid)
            {
                printf("this is parent window \n \n");


                while(1)
                {
                    char user[MAXLINE];
                    close(pfd[1]);

                    if((nread =read(pfd[0], buf, MAXLINE) >0))
                       {
                        if(buf!=EOF)
                        printf("child status: %s\n", buf);

                       }

                    else
                    {    // reseting for next phase.
                        close(pfd[0]);
                        close(pfd[1]);
                        nread =0;
                        goto service;
                    }
                } // end of while

            } //end of parent block


    continue;
    } //end of if choice



     else if(n==2)

    {


            int tsockfd, n;
            struct sockaddr_in servaddr;
            if((tsockfd = socket(AF_INET, SOCK_STREAM, 0)) <0)
            {error("socket error for time sock:");}
            bzero(&servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(8893);
            if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
            error("inet_pton error for given IP address");

            if ((connect(tsockfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
            error("connect error for time sock:");

            char connfd[sizeof(&tsockfd)];
            char pfdstr[sizeof(&pfd[1])];
            memset(connfd,0,sizeof(connfd));
            memset(pfdstr,0,sizeof(pfdstr));
            sprintf(connfd,"%d",tsockfd);  //converting sockfd to char
            sprintf(pfdstr,"%d",pfd[1]);    //converting pipefd to char

            printf("forking child for time Service \n");

            // child process

            if( (childpid = fork()) == 0)
            {
                if ( (execlp("xterm", "xterm", "-e", "./a1tim", argv[1], connfd, pfdstr, (char *) 0)) < 0)
                    {error("Not able to execute xterm:"); }
            }

            if (childpid < 0)
            {
                error("Error creating child for time:");
            }


            printf("\nchild forked successfully in new terminal!! \n");
            close(tsockfd);

            if (childpid)
            {

                printf("\nthis is parent window \n");



                while(1)
                {
                    char user[MAXLINE];
                    close(pfd[1]);

                    if((nread =read(pfd[0], buf, MAXLINE) >0))
                       {
                        if(buf!=EOF)
                        printf("child status: %s\n", buf);

                       }

                    else
                    {    // reseting for next phase.
                        close(pfd[0]);
                        close(pfd[1]);
                        nread =0;
                        goto service;
                    }
                } // end of while
            } // end of parent body

    continue;
    }

    else
    {

        printf("Invalid choice. \n");
        goto service;
        //break;

    }


} // end of for loop

} // end of main




void sig_chld(int signo)
{

pid_t pid;
int stat;

while ( (pid = waitpid(-1, &stat, WNOHANG)) > 1)
printf("child process %d terminated\n", pid);
return;
}


void error(char *msg)
{
fprintf(stderr, "%s: %s\n", msg, strerror(errno));
exit(0);
}

void byaddr(char *argv) //resolve name of given ip address
{
    addr = inet_addr(argv);
    he = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
    if (he)
    {
        printf("Name: %s\n", he->h_name);
        while (he->h_addr_list[0])
	    printf("IP Address:  %s \n", inet_ntoa(*(struct in_addr *) * he->h_addr_list++));
    }
    else printf("error in resolving domain name \n");
}

char* byname(char *argv1)   //resolve ip address of given name
{
    he = gethostbyname (argv1);
    if (he)
    {
        printf("Name: %s\n", he->h_name);
        while (*he->h_addr_list)
        {
            bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));
            printf("address: %s\n", inet_ntoa(a));
        }
        return inet_ntoa(a);
    }
    else
        {printf("error in resolving");return NULL;}

}


















