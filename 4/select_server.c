#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>

//max socket fd can process
#define MAX_FD        1024
//!server ip
#define SERVER_IP     "127.0.0.1"
//!server bind port default
#define SERVER_PORT   11111

//macros for simple log
#define log_msg(level,fmt,...)   do { fprintf(stderr,"[%s][%s %s - line:%d]"fmt"\n",level,__DATE__,__TIME__,__LINE__,##__VA_ARGS__);}while (0);

#define debug(fmt,...)            log_msg("DEBUG",fmt,##__VA_ARGS__)
#define info(fmt,...)           log_msg("INFO ",fmt,##__VA_ARGS__)
#define warn(fmt,...)           log_msg("WARN ",fmt,##__VA_ARGS__)
#define error(fmt,...)          log_msg("ERROR",fmt,##__VA_ARGS__)

//create an socket and liten at the specified port
int create_and_listen(const char *szIP,uint16_t nPort)
{
    //socket
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd < 0)
    {
        error("socket failed,errno=%d",errno);
        return -1;
    }

    //addr structure
    struct sockaddr_in sa;
    memset(&sa,0,sizeof(sa));
    sa.sin_addr.s_addr=inet_addr(szIP);
    sa.sin_port       =htons(nPort);
    sa.sin_family     =AF_INET;

    //rause the addr
    int rause = 1;
    int optlen= 4;
    if (setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,(const char *)&rause,optlen) == -1)
    {
        error("setsockopt failed,errno=%d",errno);
        close(server_fd);
        return -1;
    }

    //bind
    if (bind(server_fd,(struct sockaddr *)&sa,sizeof(sa)) == -1)
    {
        error("bind failed,errno=%d",errno);
        close(server_fd);
        return -1;
    }

    //listen
    if (listen(server_fd,15) == -1)
    {
        error("listen failed,errno=%d",errno);
        close(server_fd);
        return -1;
    }

    info("create_and_listen end ,server_fd=%d",server_fd);

    return server_fd;
}

//handle client request
int doClient(int client_fd)
{
#if 1
    int result = 0;
    struct sockaddr_in client_sa;
    memset(&client_sa,0,sizeof(client_sa));
    socklen_t socklen = 0;
    //getpeername
    if (getpeername(client_fd,(struct sockaddr *)&client_sa,&socklen) == -1)
    {
        error("getpeername failed,client_fd=%d,errno=%d",client_fd,errno);
        return -1;
    }
    char buf[1024]={0};
    int read_size=0;
    int send_size=0;
    read_size=recv(client_fd,buf,sizeof(buf)-1,0);
    if (read_size < 0)
    {
        info("read failed,client_fd",client_fd);
    }
    else if (read_size==0)
    {
        info("read EOF,client_fd",client_fd);
    }
    else
    {
        send_size=send(client_fd,buf,read_size,0);
        if (send_size==read_size)
        {
            info("recv and echo msg success,msg=%s",buf);
            result = 1;
        }
        else
        {
            error("send failed,read_size=%d,send_size=%d,msg=%s",read_size,send_size,buf);
        }
    }
    return result==1?0:-1;
#endif
}

int main(int argc,char *argv[])
{
    const char *szIP=SERVER_IP;
      uint16_t nPort=SERVER_PORT;
    int server_fd = create_and_listen(szIP,nPort);
    if (server_fd < 0)
    {
        exit(0);
    }

    //fd set
    int fdset[MAX_FD] = {0};
    fd_set rdset;
    fd_set wrset;
    int max_fd = 0;
    memset(fdset,0xff,sizeof(fdset));
    while (1)
    {
        int fdset_c=0;
        FD_ZERO(&rdset);
        FD_SET(server_fd,&rdset);
        max_fd = server_fd;
        for (int i=0;i<MAX_FD;++i)
        {
            if (fdset[i]!=-1)
            {
                fdset_c++;
                if (i>max_fd)
                {
                    max_fd=i;
                }
                FD_SET(i,&rdset);
            }
        }
        debug("fdset_c=%d,max_fd=%d",fdset_c,max_fd);
        struct timeval tv;
        tv.tv_sec=1;
        tv.tv_usec=0;
        int select_return = select(max_fd+1,&rdset,NULL,NULL,&tv);
        if (select_return == -1)
        {
            error("select failed,errno=%d",errno);
            break;
        }
        else if(select_return <= 0)
        {
            info("[main] do loop report,server listen ip=%s,port=%d",szIP,nPort);
        }
        else
        {
            if (FD_ISSET(server_fd,&rdset))
            {
                struct sockaddr_in client_sa;
                socklen_t socklen = 0;
                memset(&client_sa,0,sizeof(client_sa));
                //accept
                int client_fd=accept(server_fd,(struct sockaddr *)&client_sa,&socklen);
                if (client_fd < 0)
                {
                    error("accept failed,errno=%d",errno);
                    break;
                }
                else if (client_fd >= MAX_FD)
                {
                    warn("client_fd >= MAX_FD,client_fd=%d,max_fd=%d",client_fd,MAX_FD);
                    close(client_fd);
                    //continue;
                }
                else
                {
                    info("accept from %s:%d",inet_ntoa(client_sa.sin_addr),ntohs(client_sa.sin_port));
                    fdset[client_fd]=0;
                }
            }
            for (int i=0;i<MAX_FD;++i)
            {
                if (fdset[i]!=-1 && FD_ISSET(i,&rdset))
                {
                    if (doClient(i) < 0)
                    {
                        close(i);
                        fdset[i]=-1;
                    }
                }
            }
        }
    }

    info("[main] end");
    //close
    close(server_fd);
    return 0;
}