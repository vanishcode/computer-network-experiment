/**
 * socket实验 时间同步
 * 12.23
 * 客户端
 * macOS 10.12
 * by wjh
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define HIDE_CURSOR() printf("\033[?25l")
#define CLEAR() printf("\033[2J")

int main()
{
    char time_buffer[8];
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));           //每个字节都用0填充
    serv_addr.sin_family = AF_INET;                     //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //具体的IP地址
    serv_addr.sin_port = htons(1234);                   //端口

    while (1)
    {
        HIDE_CURSOR();
        //创建套接字
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        //读取服务器传回的数据
        read(sock, time_buffer, sizeof(time_buffer));

        //sprintf(time_buffer, "%02d:%02d:%02d\r", h, m, s);

        printf("Server Time: %s\r", time_buffer);
        //CLEAR();
        usleep(1000);
        memset(time_buffer, 0, 8);
        //关闭套接字
        close(sock);
    }

    return 0;
}