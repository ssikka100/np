#include "unp.h"
char recvline[MAXLINE];
char sendline[MAXLINE];
char buff[MAXLINE];


int main(int argc, char **argv)
{
printf("Time Service Started: \n");
int sockfd, pfd_wr;
sockfd = atoi(argv[2]);
pfd_wr= atoi(argv[3]);
char buff[MAXLINE];
int n;                                      //printf("%s \n", argv[3]);

strcpy(buff, "child terminal connected to server ...");
write(pfd_wr, buff, MAXLINE);

while ( (n = read(sockfd, recvline, MAXLINE)) > 0)
            {
                recvline[n] = 0;
                strcpy(buff, "child executing time service ");
                write(pfd_wr, buff, MAXLINE);

                if (fputs(recvline, stdout) == EOF)
                {   strcpy(buff,"fputs error:");
                    write(pfd_wr, buff, MAXLINE);
                    close(sockfd);
                    exit(0);
                }
            }

            if (n == 0)
            {   strcpy(buff,"server terminated prematurely, exiting the child and closing connection!!");
                write(pfd_wr, buff, MAXLINE);
                close(sockfd); //new
                exit(0);}


            if (n < 0)
            {   strcpy(buff,"read error from connected socket:");
                write(pfd_wr, buff, MAXLINE);
                close(sockfd); //new
                exit(0);}

close(sockfd);
close(pfd_wr);  //new
exit(0);         //new

} // end of main
