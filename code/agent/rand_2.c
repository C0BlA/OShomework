#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define n_stacks 7
#define stack_cap 6

static int this_player;
static int board[stack_cap + 1][n_stacks + 1];

char stack_name(int i)
{
    return 'A' + i - 1;
}

int main()
{
    scanf("%d", &this_player);
    printf("Received player ID: %d\n", this_player); // 디버깅용 출력

    if (this_player != 1 && this_player != 2) {
        printf("Invalid player ID. Exiting.\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    int choice = rand() % n_stacks + 1;  // choice를 int로 변경
    printf("Random choice (int): %d\n", choice); // 디버깅용
    printf("Selected stack: %c\n", stack_name(choice)); // 줄 바꿈 추가
    fflush(stdout); // 버퍼 비우기

    return EXIT_SUCCESS;
}
