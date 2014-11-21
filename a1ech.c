#include "unp.h"

void str_icli(FILE *fp, int sockfd, int pfd)  //using select command
{
int maxfdp1;
fd_set rset;
char buff[MAXLINE];
char sendline[MAXLINE], recvline[MAXLINE];

FD_ZERO(&rset);
for ( ; ; )
{
    FD_SET(fileno(fp), &rset);
    FD_SET(sockfd, &rset);
    maxfdp1 = max(fileno(fp), sockfd) + 1;
    select(maxfdp1, &rset, NULL, NULL, NULL);

if (FD_ISSET(sockfd, &rset)) { /* socket is readable */
    if (read(sockfd, recvline, MAXLINE) == 0)
    {strcpy(buff, "server terminated prematurely, exiting the child and closing connection!!");
    write(pfd, buff, MAXLINE);
    close(sockfd);
        exit(0);}

    fputs(recvline, stdout);
}

if (FD_ISSET(fileno(fp), &rset))
{
    if (fgets(sendline, MAXLINE, fp) == NULL)
    return;

    write(sockfd, sendline, sizeof(sendline));
    strcpy(buff, "child executing echo service");
    write(pfd, buff, MAXLINE);
}

    } //end of for loop
    close(sockfd);
}  // end of function.

int main(int argc, char **argv)
{
printf("Echo Service Started: \nInput your string: \n");
int sockfd, pfd_wr;
sockfd = atoi(argv[2]);
pfd_wr = atoi(argv[3]);
char buff[MAXLINE];
strcpy(buff, "child now accepting input from user!!");
write(pfd_wr, buff, MAXLINE);

str_icli(stdin, sockfd,pfd_wr);

close(pfd_wr);
exit(0);
}














//close(pfd[0]);
//strcpy(buf, recvline);
//write(pfd[1], buf,strlen(buf)+1);
//close(pfd[1]);
//
//exit(0);






//while (fgets(sendline, MAXLINE, fp) != NULL){
//write(sockfd, sendline, 1);
//sleep(1);
//write(sockfd, sendline + 1, strlen(sendline) - 1);
//if (Readline(sockfd, recvline, MAXLINE) == 0)
//err_quit("str_cli: server terminated prematurely");
//Fputs(recvline, stdout);}

//printf("sockfd is \n");
//printf("%d \n", sockfd);
//sleep(135);
//sockfd=atoi(argv[1]);
//argv[1]="127.0.0.1";


//struct sockaddr_in servaddr;
//sockfd = socket(AF_INET, SOCK_STREAM, 0);
//bzero(&servaddr, sizeof(servaddr));
//servaddr.sin_family = AF_INET;
//servaddr.sin_port = htons(SERV_PORT);
//
////IP Address of server is in argv[1]
//if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
//error("inet_pton error for given IP address");
//
//if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
//        error("connect error:");
//
