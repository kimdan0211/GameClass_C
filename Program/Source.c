#include <stdio.h>
//#include <windows.h>

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

void move(int x, int y)
{
    // x와 y축을 설정하는 구조체입니다.
    //COORD position = {x, y};

    // 콘솔 좌표 설정 함수입니다.
    //SetConsolePosition(GetStdHandle(STD_OUTPUT_HANDLE),position);
};

int main(){

    move(5,10);
    printf("⭐️");


    return 0;
}