#include <stdio.h>
#include <stdlib.h>    // atexit, rand, system
#include <time.h>      // time
#include <windows.h>   // Sleep
#include <conio.h>     // _getch()

#define SIZE  20
#define EMPTY '.'
#define BLACK 'X'
#define WHITE 'O'

#define AI_ENABLED  1
#define AI_IS_WHITE 1   // 1: 컴퓨터=백(O), 0: 컴퓨터=흑(X)

enum {
    KEY_NONE=0, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE, KEY_Q, KEY_U
};

static const int DIR4_DR[4] = { 1, 0, 1, -1 }; // 세로, 가로, ↘, ↗
static const int DIR4_DC[4] = { 0, 1, 1,  1 };

// 터미널 I/O (Windows에서는 별도 raw 모드 불필요)
static void tty_restore(void) { /* no-op on Windows */ }
static void tty_enable_raw(void) {
    atexit(tty_restore);
    // 필요 시 콘솔 모드 조정 가능하지만, _getch()로 충분
}

// 단일 키 읽기 (방향/공백/u/q)
static int read_key(void){
    int ch = _getch();              // 블로킹 1글자 입력
    if (ch=='q'||ch=='Q') return KEY_Q;
    if (ch==' ')         return KEY_SPACE;
    if (ch=='u'||ch=='U') return KEY_U;

    // 방향키는 0 또는 224(0xE0) 후에 실제 코드가 옴
    if (ch==0 || ch==0xE0){
        int ch2 = _getch();
        switch (ch2){
            case 72: return KEY_UP;    // ↑
            case 80: return KEY_DOWN;  // ↓
            case 77: return KEY_RIGHT; // →
            case 75: return KEY_LEFT;  // ←
            default: return KEY_NONE;
        }
    }
    return KEY_NONE;
}

// 보드/이력/렌더링
typedef struct { int row, col; char stone; } Move;

static char g_board[SIZE][SIZE];
static Move g_hist[SIZE*SIZE];
static int   g_hist_top = 0;

static void board_init(void){
    for(int y=0;y<SIZE;y++)
        for(int x=0;x<SIZE;x++)
            g_board[y][x] = EMPTY;
}

static void screen_clear(void){ system("cls"); }

static void draw(int cx,int cy,int turn){
    screen_clear();
    printf("방향키: 이동 | Space: 착수 | u: 되돌리기 | q: 종료\n");
    printf("현재 턴: %s   (사람=흑 X, 컴퓨터=백 O)\n\n", (turn%2==1?"Black(X)":"White(O)"));
    printf("   "); for(int x=0;x<SIZE;x++) printf("%2d ", x); printf("\n");
    for(int y=0;y<SIZE;y++){
        printf("%2d ", y);
        for(int x=0;x<SIZE;x++){
            if (x==cx && y==cy){
                char ch = (g_board[y][x]==EMPTY?'+':g_board[y][x]);
                printf("[%c]", ch);
            } else {
                printf(" %c ", g_board[y][x]);
            }
        }
        printf("\n");
    }
}

// 규칙 유틸 (연결 길이/승리 판단 등)

// 양방향 연결 길이(자기 자신 포함)
static int line_len(int row,int col,int dr,int dc,char s){
    int cnt=1;
    int r=row+dr, c=col+dc;
    while(0<=r&&r<SIZE&&0<=c&&c<SIZE&&g_board[r][c]==s){ cnt++; r+=dr; c+=dc; }
    r=row-dr; c=col-dc;
    while(0<=r&&r<SIZE&&0<=c&&c<SIZE&&g_board[r][c]==s){ cnt++; r-=dr; c-=dc; }
    return cnt;
}

// 착수 지점 기준 5목(이상) 여부
static int has_five(int row,int col,char s){
    for(int k=0;k<4;k++){
        if (line_len(row,col,DIR4_DR[k],DIR4_DC[k],s) >= 5) return 1;
    }
    return 0;
}

// 패턴 판별 (열린4/열린3) - 한 방향

// .XXXX. (중심 가정 포함)
static int is_open_four(int row,int col,int dr,int dc,char s){
    char a[9];
    for(int k=-4;k<=4;k++){
        int r=row+k*dr, c=col+k*dc;
        a[k+4]=(0<=r&&r<SIZE&&0<=c&&c<SIZE)? g_board[r][c]:'#';
    }
    a[4]=s; // 중심 착수 가정

    // 길이6 슬라이딩 창, 중심 포함
    for(int t=-4; t<=-1; t++){
        int L=t+4;
        if(!(t<=0 && 0<=t+5)) continue;
        if (a[L]==EMPTY && a[L+1]==s && a[L+2]==s && a[L+3]==s && a[L+4]==s && a[L+5]==EMPTY)
            return 1;
    }
    return 0;
}

// .XXX. / .SS.S. / .S.SS. (중심 가정 포함, 엄격)
static int is_open_three(int row,int col,int dr,int dc,char s){
    if (is_open_four(row,col,dr,dc,s)) return 0; // 이미 열린4면 삼으로 카운트 X

    char a[9];
    for(int k=-4;k<=4;k++){
        int r=row+k*dr, c=col+k*dc;
        a[k+4]=(0<=r&&r<SIZE&&0<=c&&c<SIZE)? g_board[r][c]:'#';
    }
    a[4]=s;

    // 1) .XXX.
    for(int t=-4; t<=0; t++){
        int L=t+4;
        if(!(t<=0 && 0<=t+4)) continue;
        int bad=0, sc=0;
        for(int i=L;i<=L+4;i++){
            if (a[i]!=s && a[i]!=EMPTY){ bad=1; break; }
            if (a[i]==s) sc++;
        }
        if (bad) continue;
        if (!(a[L]==EMPTY && a[L+4]==EMPTY)) continue;
        if (sc!=3) continue;
        if (a[L+1]==s && a[L+2]==s && a[L+3]==s) return 1;
    }

    // 2) .SS.S. / .S.SS.
    for(int t=-4; t<=-1; t++){
        int L=t+4;
        if(!(t<=0 && 0<=t+5)) continue;
        int bad=0, sc=0;
        for(int i=L;i<=L+5;i++){
            if (a[i]!=s && a[i]!=EMPTY){ bad=1; break; }
            if (a[i]==s) sc++;
        }
        if (bad) continue;
        if (!(a[L]==EMPTY && a[L+5]==EMPTY)) continue;
        if (sc!=3) continue;

        int t1 = (a[L+1]==s && a[L+2]==s && a[L+3]==EMPTY && a[L+4]==s); // .SS.S.
        int t2 = (a[L+1]==s && a[L+2]==EMPTY && a[L+3]==s && a[L+4]==s); // .S.SS.
        if (t1 || t2) return 1;
    }
    return 0;
}

// 금수/합법 판단

// 흑 3-3 금수(즉시 5목은 예외). 장목/4-4/3-4는 허용.
static int is_black_double_three(int row, int col) {
    if (g_board[row][col] != EMPTY) return 0;

    int cnt_open3 = 0;
    g_board[row][col] = BLACK; // 임시 착수

    for (int k=0; k<4; k++) {
        // 4연결 이상의 축은 삼으로 보지 않음(중복 위협 방지)
        if (line_len(row,col,DIR4_DR[k],DIR4_DC[k],BLACK) >= 4) continue;
        if (is_open_three(row,col,DIR4_DR[k],DIR4_DC[k],BLACK)) {
            cnt_open3++;
            if (cnt_open3 >= 2) { g_board[row][col] = EMPTY; return 1; }
        }
    }
    g_board[row][col] = EMPTY;
    return 0;
}

// 착수 합법성 (흑 3-3만 체크)
static int is_legal_move(char who, int row, int col){
    if (row<0 || row>=SIZE || col<0 || col>=SIZE) return 0;
    if (g_board[row][col] != EMPTY) return 0;

    if (who==BLACK){
        // 즉시 5목이면(예외 허용)
        g_board[row][col] = BLACK;
        int win_now = has_five(row,col,BLACK);
        g_board[row][col] = EMPTY;
        if (!win_now && is_black_double_three(row,col)) return 0;
    }
    return 1;
}

// 보조 유틸

static int board_is_empty(void){
    for(int y=0;y<SIZE;y++)
        for(int x=0;x<SIZE;x++)
            if(g_board[y][x]!=EMPTY) return 0;
    return 1;
}

static int manhattan(int r,int c,int r2,int c2){
    int dr = r-r2; if(dr<0) dr=-dr;
    int dc = c-c2; if(dc<0) dc=-dc;
    return dr+dc;
}

// 내가 r,c에 둘 때 4방향 중 최대 연결 길이
static int best_line_if_put(int r,int c,char who){
    int best=0;
    g_board[r][c]=who;
    for(int k=0;k<4;k++){
        int L = line_len(r,c,DIR4_DR[k],DIR4_DC[k],who);
        if (L>best) best=L;
    }
    g_board[r][c]=EMPTY;
    return best;
}

// 초보 AI (아주 단순한 휴리스틱)
// 우선순위: 내 즉승(5) → 상대 즉승 차단(5) → 상대 4 완성 차단(=현재 3에서 4) → 휴리스틱(최대 연결+주변/중앙)

static int ai_pick_move(int* out_r, int* out_c, char who){
    if (board_is_empty()){ *out_r = SIZE/2; *out_c = SIZE/2; return 1; }

    char opp = (who==BLACK?WHITE:BLACK);

    // 1) 내 즉시 승리
    for(int r=0;r<SIZE;r++){
        for(int c=0;c<SIZE;c++){
            if(!is_legal_move(who,r,c)) continue;
            g_board[r][c]=who;
            int win = has_five(r,c,who);
            g_board[r][c]=EMPTY;
            if(win){ *out_r=r; *out_c=c; return 1; }
        }
    }
    // 2) 상대 즉시 승리 차단
    for(int r=0;r<SIZE;r++){
        for(int c=0;c<SIZE;c++){
            if(!is_legal_move(who,r,c)) continue;
            g_board[r][c]=opp;
            int opp_win = has_five(r,c,opp);
            g_board[r][c]=EMPTY;
            if(opp_win){ *out_r=r; *out_c=c; return 1; }
        }
    }
    // 3) 상대가 다음 수에 4 만드는 자리(=현재 3 → 4) 차단
    for(int r=0;r<SIZE;r++){
        for(int c=0;c<SIZE;c++){
            if(!is_legal_move(who,r,c)) continue;
            int opp_len_if_put = best_line_if_put(r,c,opp);
            if (opp_len_if_put >= 4){ *out_r=r; *out_c=c; return 1; }
        }
    }
    // 4) 휴리스틱: 내 최대 연결 + 주변/중앙 선호
    int bestR=-1, bestC=-1, bestScore=-1, bestTie=1e9;
    int centerR=SIZE/2, centerC=SIZE/2;

    for(int r=0;r<SIZE;r++){
        for(int c=0;c<SIZE;c++){
            if(!is_legal_move(who,r,c)) continue;

            int L = best_line_if_put(r,c,who);

            // 8이웃에 돌 있으면 가산점
            int around=0;
            for(int dr=-1; dr<=1; dr++){
                for(int dc=-1; dc<=1; dc++){
                    if (dr==0 && dc==0) continue;
                    int rr=r+dr, cc=c+dc;
                    if (0<=rr&&rr<SIZE&&0<=cc&&cc<SIZE && g_board[rr][cc]!=EMPTY){ around=1; goto AROUND_DONE; }
                }
            }
        AROUND_DONE:;
            int score = L*10 + around;
            int tie   = manhattan(r,c,centerR,centerC);
            if (score>bestScore || (score==bestScore && tie<bestTie)){
                bestScore = score; bestTie=tie; bestR=r; bestC=c;
            }
        }
    }
    if (bestR!=-1){ *out_r=bestR; *out_c=bestC; return 1; }

    // 5) 마지막: 랜덤 합법 수
    int empties[SIZE*SIZE][2], ecnt=0;
    for(int r=0;r<SIZE;r++)
        for(int c=0;c<SIZE;c++)
            if (g_board[r][c]==EMPTY && is_legal_move(who,r,c)){
                empties[ecnt][0]=r; empties[ecnt][1]=c; ecnt++;
            }
    if (ecnt>0){
        int k = rand()%ecnt;
        *out_r = empties[k][0]; *out_c = empties[k][1];
        return 1;
    }
    return 0;
}

// 메인
int main(void){
    srand((unsigned)time(NULL));
    tty_enable_raw();
    board_init();

    int cx=SIZE/2, cy=SIZE/2;
    int turn=1; // 1: 흑(사람), 2: 백(컴퓨터)

    while(1){
        draw(cx,cy,turn);
        int key=read_key();

        if (key==KEY_Q){
            printf("종료합니다.\n");
            break;
        } else if (key==KEY_U){
            if (g_hist_top>0){
                Move m=g_hist[--g_hist_top];
                g_board[m.row][m.col]=EMPTY;
                turn--;
                printf("마지막 수를 되돌렸습니다.\n");
                fflush(stdout);
            } else {
                printf("되돌릴 수가 없습니다.\n");
                fflush(stdout);
            }
            continue;
        }

        // 사람이 두는 경우(흑) 또는 AI 비활성/AI가 흑인 경우
        if (turn%2==1 || !AI_ENABLED || !AI_IS_WHITE){
            if      (key==KEY_LEFT  && cx>0)         cx--;
            else if (key==KEY_RIGHT && cx<SIZE-1)    cx++;
            else if (key==KEY_UP    && cy>0)         cy--;
            else if (key==KEY_DOWN  && cy<SIZE-1)    cy++;
            else if (key==KEY_SPACE){
                if (g_board[cy][cx]!=EMPTY) continue;

                char who = (turn%2==1)? BLACK:WHITE;
                if (!is_legal_move(who, cy, cx)){
                    printf("해당 칸은 둘 수 없습니다(흑 3-3 금수 등).\n");
                    fflush(stdout);
                    continue;
                }

                g_board[cy][cx]=who;
                g_hist[g_hist_top++] = (Move){cy,cx,who};

                if (has_five(cy,cx,who)){
                    draw(cx,cy,turn);
                    printf("%s 승리!\n", (who==BLACK?"Black(X)":"White(O)"));
                    break;
                }
                turn++;
            }
            continue;
        }

        // 컴퓨터(백) 차례
        if (AI_ENABLED && AI_IS_WHITE && turn%2==0){
            int ar=-1, ac=-1;
            if (ai_pick_move(&ar,&ac,WHITE)){
                Sleep(120); // 시각적 딜레이 (ms)
                g_board[ar][ac]=WHITE;
                g_hist[g_hist_top++] = (Move){ar,ac,WHITE};

                if (has_five(ar,ac,WHITE)){
                    draw(ac,ar,turn);
                    printf("White(O) 승리! (컴퓨터)\n");
                    break;
                }
                turn++;
                cx = ac; cy = ar; // 커서를 착수 위치로 이동
            } else {
                draw(cx,cy,turn);
                printf("둘 곳이 없습니다. 무승부.\n");
                break;
            }
        }
    }
    return 0;
}