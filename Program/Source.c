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
    srand(time(NULL));

    int size;
    printf("배열의 크기 : ");
    scanf("%d",&size);

    int *arr = (int *)malloc(size * sizeof(int));
    for(int i = 0;i<size;i++)
    {
        arr[i] = i+1;
    }

    printf("현재 배열 : ");
    for(int i = 0; i<size;i++){
        printf("%d ",arr[i]);
    }
    printf("\n");

    shuffle(arr,size);

    printf("shuffle 배열 : ");
    for(int i = 0; i<size;i++){
        printf("%d ",arr[i]);
    }
    printf("\n");

    free(arr);
    

#pragma endregion


return 0;
}