// 显示当前时间，此程序的模块化实现见 gettime.c
// 12.22 by wjh
#include <time.h>
#include <stdio.h>
#include <unistd.h>
// 让光标别瞎他喵的乱闪
#define HIDE_CURSOR() printf("\033[?25l")

int main()
{
    int h = 0, m = 0, s = 0;
    int t = 0;
    while (1)
    {
        HIDE_CURSOR();
        t = time(0); //获取总秒数
        s = t % 60;
        m = t % 3600 / 60;                     
        //1h= 3600s不足1h的除60即为分钟

        h = (t % (24 * 3600) / 3600 + 8) % 24; 
        //1天24h 得到当天小时数+8为东八区区时 避免出现大于24h的情况对24取余

        printf("%02d:%02d:%02d\r", h, m, s); 
        // %02d是将数字按宽度为2，采用右对齐方式输出，若数据位数不到2位，则左边补0 \r到当前行最左

        usleep(700); 
        //程序执行较快 对时间进行控制 执行挂起1s
    }
    return 0;
}