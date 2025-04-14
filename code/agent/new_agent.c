#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define n_stacks 7
#define stack_cap 6

int this_player;
int other_player;

static int board[stack_cap + 1][n_stacks + 1];




char stack_name(int i)
{
    return 'A' + i - 1;
}

int stack_num(char c)
{
    return c - 'A' + 1;
}


int is_full(int stack);


int main()
{
    scanf("%d", &this_player);
    // printf("Received player ID: %d\n", this_player); // 디버깅용 출력

    int other_player = 3 - this_player; 
    
    if (this_player != 1 && this_player != 2) {
        printf("Invalid player ID. Exiting.\n");
        return EXIT_FAILURE;
    }

    for (int l = stack_cap; l >= 1; l--)
    {
        for (char s = 'A'; s <= 'G'; s++)
        {
            scanf("%d", &(board[l][stack_num(s)]));
        }
    }


    


    srand(time(NULL));
    int choice = rand() % n_stacks + 1;
    printf("%c", stack_name(choice));

    return EXIT_SUCCESS;
}



int is_full(int stack) { //꽉차면 1 리턴
    return board[0][stack] != 0;
}


void defend_check(){
    int checker[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };
    for (int i = 1; i <= stack_cap; i++)
    {
        for (int j = 1; j <= n_stacks; j++)
        {
            if (board[i][j] == other_player)
            {
                for (int k = 0; k < 4; k++)
                {
                    int count = 1;
                    int x = i;
                    int y = j;

                    while (1)
                    {
                        x += checker[k][0];
                        y += checker[k][1];

                        if (x < 1 || x > stack_cap || y < 1 || y > n_stacks || board[x][y] != other_player)
                            break;

                        count++;
                    }

                    if (count == 2)
                    {
                        for (int l = 1; l <= stack_cap; l++)
                        {
                            if (board[l-1][j] != 0 && board[l][j] == 0)
                            {
                                printf("%c", stack_name(j));
                                return;
                            }
                        }
                    }
                    if(count == 3)
                    {
                        for (int l = 1; l <= stack_cap; l++)
                        {
                            if (board[l][j] == 0)
                            {
                                printf("%c", stack_name(j));
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

}