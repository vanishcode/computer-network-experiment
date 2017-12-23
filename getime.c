// 返回当前时间 string 模块
// 12.22 by wjh
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "getime.h"

void getime(char* time_buffer)
{
    int h = 0, m = 0, s = 0;
    int t = 0;
    //char time_buffer[8];

    //获取总秒数
    t = time(0);

    s = t % 60;
    m = t % 3600 / 60;

    //1天24h 得到当天小时数+8为东八区区时 避免出现大于24h的情况对24取余
    h = (t % (24 * 3600) / 3600 + 8) % 24;
    
    // %02d是将数字按宽度为2，采用右对齐方式输出，若数据位数不到2位，则左边补0 \r到当前行最左
    sprintf(time_buffer,"%02d:%02d:%02d\r", h, m, s);
    //return time_buffer;
}