#include <stdio.h>
#include <conio.h>
#include <windows.h>

int screen_index;
HANDLE screen[2];

void initialize()
{
    CONSOLE_CURSOR_INFO cursor;

    // 화면 버퍼를 2개 생성합니다.
    screen[0] = CreateConsoleScreenBuffer
    (
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        CONSOLE_TEXTMODE_BUFFER, NULL
    );

    screen[1] = CreateConsoleScreenBuffer
    (
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        CONSOLE_TEXTMODE_BUFFER, NULL
    );

    cursor.dwSize = 1;
    cursor.bVisible = FALSE;

    SetConsoleCursorInfo(screen[0], &cursor);
    SetConsoleCursorInfo(screen[1], &cursor);
};

void flip()
{
    SetConsoleActiveScreenBuffer(screen[screen_index]);

    screen_index = !screen_index;
}

void clear()
{
    COORD position = {0, 0};

    DWORD dword;

    CONSOLE_SCREEN_BUFFER_INFO console;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(handle, &console);
}

int main()
{

    return 0;
}