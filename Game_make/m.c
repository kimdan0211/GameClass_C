#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define n 20
#define EMPTY '.'

static char board[n][n];

static void new_board() // 초기화
{
    for(int y=0;y<n;y++){
        for(int x=0;x<n;x++){
            board[y][x] = EMPTY;
        }
    }
}

static void BLACK(){

}

int main(){



    return 0;
}