#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define SIZE 20
#define EMPTY '.'
#define BLACK 'X'
#define WHITE 'O'

// 터미널 입력 설정
static struct termios orig_term;
static void term_restore(void){ tcsetattr(STDIN_FILENO, TCSANOW, &orig_term); }
static void term_raw(void){
    struct termios t;
    tcgetattr(STDIN_FILENO, &orig_term);
    atexit(term_restore);
    t = orig_term;
    t.c_lflag &= ~(ICANON | ECHO);  // 라인버퍼/에코 해제
    t.c_cc[VMIN]  = 1;              // 최소 1바이트 입력을 기다림
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// 키 입력 파싱
enum { KEY_NONE=0, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE, KEY_ENTER, KEY_Q };
static int read_key(void){
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) != 1) 
        return KEY_NONE;
    if (c=='q' || c=='Q') 
        return KEY_Q;
    if (c==' ') 
        return KEY_SPACE;
    if (c=='\n' || c=='\r') 
        return KEY_ENTER;
    if (c==0x1B){ // ESC [
        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1)!=1) 
            return KEY_NONE;
        if (read(STDIN_FILENO, &seq[1], 1)!=1) 
            return KEY_NONE;
        if (seq[0]=='['){
            if (seq[1]=='A') 
                return KEY_UP;
            if (seq[1]=='B') 
                return KEY_DOWN;
            if (seq[1]=='C') 
                return KEY_RIGHT;
            if (seq[1]=='D') 
                return KEY_LEFT;
        }
        return KEY_NONE;
    }
    return KEY_NONE;
}

// 보드/출력
static char board[SIZE][SIZE];

static void init_board(void){
    for(int y=0;y<SIZE;y++)
        for(int x=0;x<SIZE;x++)
            board[y][x]=EMPTY;
}

static void cls(void){ 
    printf("\033[H\033[J"); 
}

static void draw(int cx,int cy,int turn){
    cls();
    printf("=== 20x20 오목 — 방향키 이동, Space/Enter 착수, q 종료 ===\n");
    printf("선공: Black(%c)  |  후공: White(%c)\n", BLACK, WHITE);
    printf("현재 턴: %s\n\n", (turn%2==1 ? "Black(X)" : "White(O)"));

    printf("   ");
    for(int x=0;x<SIZE;x++) printf("%2d ", x);
    printf("\n");
    for(int y=0;y<SIZE;y++){
        printf("%2d ", y);
        for(int x=0;x<SIZE;x++){
            if (x==cx && y==cy){
                // 커서 하이라이트(대괄호). 빈칸은 '+'로 시각화
                char ch = board[y][x]==EMPTY ? '+' : board[y][x];
                printf("[%c]", ch);
            }else{
                printf(" %c ", board[y][x]);
            }
        }
        printf("\n");
    }
}

// 승리 판정(사용자 코드 기반)
static int check_win(int row, int col, char stone){
    int dx[4] = {1, 0, 1, 1};   // 가로, 세로, ↘, ↗
    int dy[4] = {0, 1, 1, -1};
    for (int dir=0; dir<4; dir++){
        int count=1;
        int x=col+dx[dir], y=row+dy[dir];
        while(x>=0 && x<SIZE && y>=0 && y<SIZE && board[y][x]==stone){ count++; x+=dx[dir]; y+=dy[dir]; }
        x=col-dx[dir]; y=row-dy[dir];
        while(x>=0 && x<SIZE && y>=0 && y<SIZE && board[y][x]==stone){ count++; x-=dx[dir]; y-=dy[dir]; }
        if (count>=5) return 1;
    }
    return 0;
}

int main(void){
    term_raw();
    init_board();

    int cx = SIZE/2, cy = SIZE/2; // 커서 중앙 시작
    int turn = 1;                 // 1부터: 홀수=Black, 짝수=White

    while(1){
        draw(cx, cy, turn);
        int key = read_key();
        if (key==KEY_Q) { printf("종료합니다.\n"); break; }

        if (key==KEY_LEFT  && cx>0)
            cx--;
        else if (key==KEY_RIGHT && cx<SIZE-1)
            cx++;
        else if (key==KEY_UP    && cy>0)
            cy--;
        else if (key==KEY_DOWN  && cy<SIZE-1)
            cy++;

        else if (key==KEY_SPACE || key==KEY_ENTER){
            if (board[cy][cx] != EMPTY) continue; // 이미 돌 있음

            char who = (turn%2==1)? BLACK : WHITE;
            board[cy][cx] = who;

            if (check_win(cy, cx, who)){
                draw(cx, cy, turn);
                printf("%s 승리!\n", (who==BLACK ? "Black(X)" : "White(O)"));
                break;
            }

            // 무승부 체크
            int full=1;
            for(int y=0;y<SIZE;y++){
                for(int x=0;x<SIZE;x++){
                    if (board[y][x]==EMPTY){ full=0; break; }
                }
                if(!full) break;
            }
            if (full){
                draw(cx, cy, turn);
                printf("무승부!\n");
                break;
            }

            turn++; // 턴 교체
        }
    }
    return 0;
}