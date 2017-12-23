/**
 * socket实验 时间同步
 * 12.22
 * server.c
 * macOS 10.12
 * by wjh
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#include "getime.h"

#define BUFFER_SIZE 100

int main()
{
    
    // 存储时间数组
    char time_buffer[8];

    // 调用getime，获取当前时间
    getime(time_buffer);
    printf("server:%s\n",time_buffer);

    // 创建套接字
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // 绑定IP,Port,Socket(上面的)
    struct sockaddr_in serverAddress;

    // 初始化为0
    memset(&serverAddress, 0, sizeof(serverAddress));

    // 开始设置
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(1234);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 绑定
    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    // 进入监听状态，但并不接收请求
    listen(serverSocket, 20);

    // 接收请求
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);

    // 发送时间数据
    write(clientSocket,time_buffer,sizeof(time_buffer));

    // 关闭
    close(clientSocket);
    close(serverSocket);

    return 0;
}