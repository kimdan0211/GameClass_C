#include <stdio.h>
#define SIZE 10

int main()
{
#pragma region GetAsyncKeyState
    // Window API에서 제공하는 입력 처리 함수로, 지정된 키의 상태를
    // 비동적으로 확인할 깨 사용되는 함수입니다.

    // Key State

    // 0x0000 : 이전에 누른 적이 없고, 호출 시점에도 눌려있지 않은 상태

    // 0x0001 : 이전에 누른 적이 있고, 호출 시점에는 눌려있지 않은 상태

    // 0x8000 : 이전에 누른 적이 없고, 호출 시점에는 눌려있는 상태

    // 0x8001 : 이전에 누른 적이 있고, 호출 시점에도 눌려있는 상태

    const char* text[SIZE] = {
        "ㅇㅇ",
        "ㅂㅂ",
        "ㅈㅈ",
        "ㄷㄷ",
        "ㄱㄱ",
        "ㅅㅅ",
        "ㄴㄴ",
        "ㄹㄹ",
        "ㅊㅊ",
        "ㅌㅌ"
    };

#pragma endregion


    return 0;
}