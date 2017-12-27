/**
 * 12.27 udp 回声服务器
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
#define BUF_SIZE 100

int main(){

    //创建套接字
    int sock = socket(PF_INET, SOCK_DGRAM, 0);

    //服务器地址信息
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(1234);

    //不断获取用户输入并发送给服务器，然后接受服务器数据
    struct sockaddr fromAddr;
    socklen_t addrLen = sizeof(fromAddr);
    while(1){
        char buffer[BUF_SIZE] = {0};
        printf("Input a string: ");
        gets(buffer);
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&fromAddr, &addrLen);
        buffer[strLen] = 0;
        printf("Message form server: %s\n", buffer);
    }

    close(sock);
    return 0;
}