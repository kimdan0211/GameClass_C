#include <stdio.h>
#include <termios.h>
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
        "명탐정 코난입니다",
        "코난이요?",
        "네 코난이요",
        "유명한 탐정님은 어딨나요",
        "네 탐정 유명한입니다",
        "사건 의뢰 가능한가요",
        "얼마까지 알아보고 오셨죠",
        "얼마까지 가능한가요",
        "선제시 ㄴㄴ",
        "안녕히 계세요"
    };

    struct termios originalTerm, rawTerm;
    tcgetattr(0, &originalTerm); // tcgetattr : 현재 터미널 속성을 가져와 originalTerm에 저장
    rawTerm = originalTerm; // 복사본 생성 : 원래 속성은 보관하고, 수정은 rawTerm에서 진행
    rawTerm.c_lflag &= ~(ICANON | ECHO); 
    // ICANON : 줄 단위 입력 모드 → 비활성화 (엔터 없이 입력 처리)
    // ECHO   : 입력한 키 화면 표시 → 비활성화 (눌러도 화면에 안 보임)
    tcsetattr(0, TCSANOW, &rawTerm); // tcsetattr : 바뀐 속성(rawTerm)을 즉시(TCSANOW) 적용

    int count = 0;
    while(count < SIZE){
        if(count %2 == 0){
            printf("탐정 : %s\n",text[count]);
        }
        else{ 
            printf("의뢰인 : %s\n",text[count]);
        }
        count ++;
        if(count < SIZE){
            while(getchar() != ' '); // getchar : 키 하나 입력 받기 (엔터 없이 동작)
        }
    }
    printf("대화가 끝났습니다.\n");
    tcsetattr(0, TCSANOW, &originalTerm); // tcsetattr : 프로그램 종료 전, 원래 속성(originalTerm)으로 복구
#pragma endregion


    return 0;
}