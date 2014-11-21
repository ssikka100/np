#include "unp.h"
#include <time.h>

char buf[MAXLINE+1];

void error(char *msg)
{
fprintf(stderr, "%s: %s\n", msg, strerror(errno));
exit(0);
}

void *doit1(void *arg)  //echo routine for server
{   int connfd;
    connfd=*(int*)arg;

    pthread_detach(pthread_self());
    free(arg);      //close(echosockfd);

    ssize_t n;

    again:
    while ( (n = read(connfd, buf, MAXLINE)) > 0)
        {   write(connfd, buf, n);
            memset(buf,0,sizeof(buf));
            printf("Server connected for echo service \n");
        }

    if (n < 0 && errno == EINTR)
    goto again;
    else if (n < 0)
    error("str_echo_read error:");

    else if (n==0)
    printf("\n echo client window closed/crashed \n");
    close(connfd);
    return (NULL);

}

void *doit(void *arg)   //time routine for server
{
    int connfd;
    connfd=*(int*)arg;

    pthread_detach(pthread_self());
    free(arg);

    char buff[MAXLINE];
    time_t	ticks;

    for(;;)
{

    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));

    fd_set timeser;
    FD_ZERO(&timeser);
    FD_SET(connfd,&timeser);
    struct timeval timesleep;
    timesleep.tv_sec = 5;
    printf("Server connected for time service ....\n");

    select((connfd+1),&timeser,NULL,NULL,&timesleep);

    // if client window crashes/closed
    if(FD_ISSET(connfd,&timeser))
    {
        printf("\n time client window closed/crashed \n");
        close(connfd);
        return (NULL);
    }

    write(connfd, buff, strlen(buff));
}

    close(connfd);
    return (NULL);

}


void timesrv(int *timesockfd) //creating listening socket for time service
{
struct sockaddr_in servaddr;
int listenfd, sk ;
const int on =1;
if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
error("listenfd error:");
//bzeros(&servaddr, sizeof(servaddr));
memset(&servaddr, 0,sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
servaddr.sin_port = htons(8893);
setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
if((sk=bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
    {error("bind error for timesock");}
if((sk=listen(listenfd, LISTENQ)) < 0)
    {error("listen error for timesock");}
    *timesockfd = listenfd;
}

void echosrv(int *echosockfd)   //creating listening socket for echo service
{
struct sockaddr_in servaddr;
int listenfd1, sk1 ;
const int on =1;
if((listenfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
error("listenfd error:");
//bzeros(&servaddr, sizeof(servaddr));
memset(&servaddr, 0,sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
servaddr.sin_port = htons(26262);
setsockopt(listenfd1, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
if(sk1=(bind(listenfd1, (SA *) &servaddr, sizeof(servaddr))) < 0)
    {error("bind error for echosock");}
if(sk1=(listen(listenfd1, LISTENQ)) < 0)
    {error("listen error for echosock");}
    //printf("listenfd1 %d", listenfd1);
    *echosockfd = listenfd1;
}

int main(int argc, char **argv)
{
int timesockfd, echosockfd, *iptr,*iptr1;


//create respective sockets
timesrv(&timesockfd);
echosrv(&echosockfd);

fd_set service_fds;
int maxfd; ;

maxfd=max(timesockfd,echosockfd);

maxfd=maxfd+1;


for(;;)
{

FD_ZERO(&service_fds);
FD_SET(echosockfd, &service_fds);
FD_SET(timesockfd, &service_fds);

int i;
for (i=0;i<maxfd;i++)
{
    FD_ZERO(&service_fds);
    FD_SET(echosockfd, &service_fds);
    FD_SET(timesockfd, &service_fds);

    int sel;
    if((sel=select(maxfd, &service_fds, NULL, NULL, NULL)) <0)
    {error("select failed:");}


        if(FD_ISSET(echosockfd,&service_fds))
        {
            //setting parameters for echo service

            struct sockaddr_in cliaddr;
            socklen_t clilen;
            iptr1 = malloc(sizeof(int));
            for ( ; ; )
            {
                iptr1 = malloc(sizeof(int));
                pthread_t t2;
                clilen = sizeof(cliaddr);
                if ( (*iptr1 = accept (echosockfd, (SA *) &cliaddr, &clilen)) < 0)
                    {if (errno == EINTR) // interupt
                    continue;
                    else
                    error("connfd error for echo");}

                    if((pthread_create(&t2, NULL, doit1, iptr1))<0)
                    {error("could not create thread:");}

                break;
                //close (connfd);
             } //end of for loop


        }


            if(FD_ISSET(timesockfd,&service_fds))
        {

            for (;;)
            {
                pthread_t t1;
                iptr = malloc(sizeof(int));
                *iptr = accept(timesockfd, (SA *) NULL, NULL);
                if((pthread_create(&t1, NULL, doit, iptr))<0)
                {error("could not create thread:");}
                break;
            }

        }// end of if


      } // end of inner for

    } //end of outer for

} //end of main

