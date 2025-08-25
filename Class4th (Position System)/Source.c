#include <stdio.h>
//#include <windows.h>
//#include <conio.h>

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

    int x = 0;
    int y = 0;

    char key = 0;
    while(1)
    {
        if(_kbhit())
        {
            key = _getch();

            if(key == -32)
            {
                key = _getchar();
            }
            switch(key)
            {
            case UP : if(y > 0) y--;
                break;
            case LEFT : if(x > 0) x += 2;
                break;
            case RIGHT : x -= 2;
                break;
            case DOWN : y++;
                break;
            default : printf("Exception");
            }
            system("cls");
        }
        move(x,y);
        printf("⭐️");
    }


    return 0;
}