#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define n_stacks 7  // Row
#define stack_cap 6 // Col
int move = 1;       // 몇번째 움직임인지지
static int board[stack_cap + 1][n_stacks + 1] = {0};

void clear()
{
    printf("\033[2J\033[H"); // 화면 지우기 + 커서를 맨 위로 이동
    fflush(stdout);
}

char stack_name(int i)
{
    return 'A' + i - 1;
}

int stack_num(char c)
{
    return c - 'A' + 1;
}
void playing(const char *player_exec, int player_id);
void drop_stone(int position, int player_id);
void print_board();
void check_ending();

int main(int argc, char *argv[])
{
    if (argc != 5 || strcmp(argv[1], "-X") != 0 || strcmp(argv[3], "-Y") != 0)
    {
        fprintf(stderr, "Usage: %s -X <agent-binary> -Y <agent-binary>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *player1_exec = argv[2];
    const char *player2_exec = argv[4];

    while (move < 100)
    {
        clear();
        print_board();
        if (move % 2 == 1)
        {
            playing(player1_exec, 1);
        }
        else
        {
            playing(player2_exec, 2);
        }

        move++;
    }
}

void playing(const char *player_exec, int player_id)
{
    int pipe_[2];
    pipe(pipe_);

    pid_t pid = fork();
    if (pid == 0)
    { // 자식 프로세스 (플레이어 실행)
        close(pipe_[0]);
        dup2(pipe_[1], STDOUT_FILENO); // 표준 출력을 파이프에 연결
        close(pipe_[1]);

        execl(player_exec, player_exec, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    else
    { // 부모 프로세스 (결과 읽기)
        close(pipe_[1]);

        char position_alpa;
        if (read(pipe_[0], &position_alpa, 1) > 0)
        {
            int position_num = stack_num(position_alpa);
            if (position_num >= 1 && position_num <= n_stacks)
            {
                drop_stone(position_num, player_id);
            }
            else
            {
                printf("Winner: %c\n", (player_id == 1) ? 'Y' : 'X');
                exit(EXIT_SUCCESS);
            }
        }
        close(pipe_[0]);
    }
}

void print_board()
{
    int turn = move % 2;
    printf("move: %d\n", move);
    printf("turn:%c\n", (turn == 1) ? 'X' : 'Y');
    for (int R = 1; R <= stack_cap; R++)
    {
        for (int L = 1; L <= n_stacks; L++)
        {
            printf("%d", board[R][L]);
        }
        printf("\n");
    }
}

void drop_stone(int position, int player_id)
{
    position -= 1; // 배열 인덱스를 맞추기 위해 1 빼줌

    if (board[0][position] != 0)
    { // 해당 칸이 가득 찼다면 패배
        printf("winner player:%c\n", (player_id == 1) ? 'Y' : 'X');
        exit(EXIT_SUCCESS);
    }

    for (int i = stack_cap - 1; i >= 0; i--)
    { // 배열 인덱스는 0부터 시작하므로 stack_cap - 1 부터 검사
        if (board[i][position] == 0)
        {
            board[i][position] = player_id;
            break; // 돌이 한 칸만 내려오도록 break 추가
        }
    }
    print_board();
}