#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define log_msg(level,fmt,...)   do { fprintf(stderr,"[%s][%s %s - line:%d]"fmt"\n",level,__DATE__,__TIME__,__LINE__,##__VA_ARGS__);}while (0);

#define debug(fmt,...)          log_msg("DEBUG",fmt,##__VA_ARGS__)
#define info(fmt,...)           log_msg("INFO ",fmt,##__VA_ARGS__)
#define warn(fmt,...)           log_msg("WARN ",fmt,##__VA_ARGS__)
#define error(fmt,...)          log_msg("ERROR",fmt,##__VA_ARGS__)

#define checkif(cond,fmt,...)   do\
                                {\
                                if(!(cond))\
                                    {\
                                    error(fmt,##__VA_ARGS__);\
                                    exit(0);\
                                    }\
                                } while(0);

//default server bind address info
#define SERVER_IP      "127.0.0.1"
#define SERVER_PORT    11111

int main()
{
    //socket
    int client_fd = socket(AF_INET,SOCK_STREAM,0);
    checkif(client_fd>0,"socket failed , error=%d",errno);
    info("socket success, socket=%d",client_fd);
    struct sockaddr_in sa;
    memset(&sa,0,sizeof(sa));
    sa.sin_family        = AF_INET;
    sa.sin_port          = htons(SERVER_PORT);
    sa.sin_addr.s_addr   = inet_addr(SERVER_IP);

    //bind
    if (connect(client_fd,(struct sockaddr *)&sa,sizeof(struct sockaddr_in)) != 0)
    {
        error("connect failed ,socket=%d, error=%d",client_fd,errno);
        exit(0);
    }

    info("connect to server success, socket=%d,server_ip=%s,server_port=%d",client_fd,SERVER_IP,SERVER_PORT);

    while(1)
    {
        char szMsg[128] = {0};
        printf("please input an string : \n");
        if (NULL == gets(szMsg))
        {
            break;
        }
        ssize_t send_size = send(client_fd,szMsg,strlen(szMsg),0);
        if (send_size > 0)
        {
            info("send msg to server,msg=%s ,size=%d",szMsg,send_size);
        }
        else
        {
            warn("send msg to server failed ,size=%d ,errno=%d",send_size,errno);
            break;
        }
        //recv
        char szBuf[128] = {0};
        ssize_t recv_size = recv(client_fd,szBuf,sizeof(szBuf)-1,0);
        if (recv_size > 0)
        {
            info("recv msg from server success ,msg=%s,recv_size=%d",szBuf,recv_size);
        }
        else
        {
            warn("recv msg from server failed,recv_size=%d ,errno=%d",recv_size,errno);
            break;
        }
    }

    //close
    info("client end");
    //close server
    close(client_fd);
    return 0;
}