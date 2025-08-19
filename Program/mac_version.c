#include <stdio.h>
#include <termios.h>
#include <stdlib.h>

// mac 터미널의 화살표 키는 ESC '[' 'A/B/C/D' 시퀀스로 들어옵니다.
#define KEY_UP     1001
#define KEY_LEFT   1002
#define KEY_RIGHT  1003
#define KEY_DOWN   1004

// ----- 커서 이동 (Windows 형태 유지) -----
void move(int x, int y)
{
    // ANSI는 1-based 좌표 (행=y, 열=x)
    // 사용성 위해 x,y를 그대로 받되 내부에서 +1 보정
    printf("\033[%d;%dH", y + 1, x + 1);
    fflush(stdout);
}

// ----- 터미널 상태 저장/복원 -----
static struct termios g_original;

static void restore_terminal(void) {
    tcsetattr(0, TCSANOW, &g_original);     // 원래 설정 복원
    printf("\033[?25h");                    // 커서 표시
    fflush(stdout);
}

static void set_raw_terminal(void) {
    struct termios raw;
    tcgetattr(0, &g_original);              // tcgetattr : 현재 터미널 속성 저장
    raw = g_original;                       // 복사본 생성
    raw.c_lflag &= ~(ICANON | ECHO);        // ICANON/ECHO 비활성화 (엔터 없이, 에코 끔)
    tcsetattr(0, TCSANOW, &raw);            // tcsetattr : 즉시 적용
    printf("\033[?25l");                    // 커서 숨김
    fflush(stdout);
}

// ----- 키 입력 하나 읽기 (화살표 시퀀스 해석) -----
static int read_key(void) {
    int c = getchar();
    if (c == 27) {          // ESC
        int c1 = getchar(); // 보통 '['
        if (c1 == '[') {
            int c2 = getchar(); // 'A','B','C','D'
            if (c2 == 'A') return KEY_UP;
            if (c2 == 'B') return KEY_DOWN;
            if (c2 == 'C') return KEY_RIGHT;
            if (c2 == 'D') return KEY_LEFT;
        }
        return 27;          // 기타 ESC
    }
    return c;               // 일반 키
}

int main(void){
    // 초기 화면 정리
    printf("\033[2J\033[H"); // 화면 지우고 (1,1)로
    set_raw_terminal();
    atexit(restore_terminal); // 비정상 종료에도 복원을 보장

    int x = 5, y = 10;      // 초기 좌표
    move(x, y);
    printf("⭐️");

    printf("\033[1;1H");    // 안내 문구
    printf("[방향키] 이동, [Space] 종료\n");
    fflush(stdout);

    while(1){
        int key = read_key();
        if (key == ' ') {   // Space: 종료
            break;
        }
        // 이전 위치 지우고
        move(x, y);
        printf("  ");

        // 방향키 처리 (경계는 간단히 0 이상만 보장)
        if (key == KEY_UP    && y > 0) y--;
        if (key == KEY_DOWN)           y++;
        if (key == KEY_LEFT  && x > 0) x--;
        if (key == KEY_RIGHT)          x++;

        // 새 위치에 출력
        move(x, y);
        printf("⭐️");
        fflush(stdout);
    }

    // atexit에서 복원됨
    return 0;
}