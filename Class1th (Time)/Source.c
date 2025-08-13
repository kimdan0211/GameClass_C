#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void shuffle(int list[],int length)
{
    int seed;
    int sw;

    for(int i = length -1;i >0;i--)
    {
        seed = rand() % (i + 1);
        sw = list[i];
        list[i] = list[seed];
        list[seed] = sw;
    }
};

void update_health(int size)
{
    for(int i=0;i<size;i++)
    {
        printf("❤️ ");
    }
    printf("\n");
};

int main()
{
#pragma region rand 함수
    // 0 ~ 32767 사이의 난수 값을 반환하는 함수입니다.

    // UTC 기준으로 1970년 1월 1일 0시 0분 초부터 경과된 시간을
    // 초(sec)로 반환하는 함수입니다.

    //srand(time(NULL));
    //int random = rand() % 10 + 1;
    //printf("random : %d\n",random);

#pragma endregion

#pragma region Shuffle Function
    //srand(time(NULL));

    //int size;
    //printf("배열의 크기 : ");
    //scanf("%d",&size);

    //int *arr = (int *)malloc(size * sizeof(int));
    //for(int i = 0;i<size;i++)
    //{
    //    arr[i] = i+1;
    //}

    //printf("현재 배열 : ");
    //for(int i = 0;i<size;i++){
    //    printf("%d ",arr[i]);
    //}
    //printf("\n");

    //shuffle(arr,size);

    //printf("shuffle 배열 : ");
    //for(int i = 0;i<size;i++){
    //    printf("%d ",arr[i]);
    //}
    //printf("\n");

    //free(arr);

#pragma endregion

#pragma region Game
    srand(time(NULL));
    int player_hp = 5;
    int computer = rand()%50+1;
    int answer;

     while(player_hp > 0)
    {
        printf("computer가 가지는 정수는(1~50) : ");
        scanf("%d",&answer);

        if(answer > 50||answer < 1)
        {
            printf("computer가 가지는 정수의 범위는 1~50까지입니다!\n");
            continue;
        }
        
        if(computer > answer)
        {
            printf("UP\n");
            player_hp--;
        }
        else if(computer < answer)
        {
            printf("DOWN\n");
            player_hp--;
        }
        else
        {
            printf("정답!!\n");
            break;
        }
        if(player_hp > 0)
        {
            update_health(player_hp);
        }
        else
        {
            printf("실패! ");
            printf("computer가 가지는 정수는 %d입니다",computer);
        }
    }


#pragma endregion

return 0;
}