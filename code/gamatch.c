#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define n_stacks 7  // Row
#define stack_cap 6 // Col
int move = 1;       // 몇번째 움직임인지지
pid_t signal_pid = -1; // signal에서 pid 확인용
static int board[stack_cap + 1][n_stacks + 1] = {0};
int current_player = 0; // 현재 플레이어
char current_position = 0; // 플레이어가 선택한 위치

void clear()
{
    //system("clear"); // 시스템 호출을 사용하여 화면 지우기
    const char *ANSI_CLEAR ="\033[1;1H\033[2J";
    write(1, ANSI_CLEAR, strlen(ANSI_CLEAR));
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
void drop(int position, int player_id);
void print_board();
void check_ending();
void ctrl_c_handler(int sig);
void timeout_handler(int sig);



int main(int argc, char *argv[])
{
    signal(SIGINT, ctrl_c_handler);
    if (argc != 5 || strcmp(argv[1], "-X") != 0 || strcmp(argv[3], "-Y") != 0)
    {
        fprintf(stderr, "Usage: %s -X <agent-binary> -Y <agent-binary>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *player1_exec = argv[2];
    const char *player2_exec = argv[4];

    while (1)
    {
        if (move > stack_cap * n_stacks)
        {
            clear();
            print_board();
            printf("Draw\n");
            exit(EXIT_SUCCESS);
        }

        clear();
        print_board();
        if (move % 2 == 1)
        {
            playing(player1_exec, 1);
            check_ending();
        }
        else
        {
            playing(player2_exec, 2);
            check_ending();
        }

        move++;
    }
}

void playing(const char *player_exec, int player_id)
{
    current_player = player_id; // 현재 플레이어 설정
    int Order_In[2]; //플레이어 선택을 받음
    int Board_out[2]; // 플레이어에게 순서 ,보드 상태를 전달

    if(pipe(Order_In)<0 || pipe(Board_out)<0) // 파이프 생성
    {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }


    if (pid == 0)
    { //자식 프로세스
        close(Order_In[0]);
        dup2(Order_In[1], STDOUT_FILENO); // 표준 출력을 파이프에 연결
        close(Order_In[1]);

        close(Board_out[1]);
        dup2(Board_out[0], STDIN_FILENO);
        close(Board_out[0]);

        execl(player_exec, player_exec, NULL);

        perror("execl failed");    
        exit(EXIT_FAILURE);
    }
    else {
        signal_pid = pid;
        close(Order_In[1]);
        close(Board_out[0]);

        char info_buff[2048];
        int len = 0;

        //ID
        info_buff[len++] = player_id +'0';
        info_buff[len++] = '\n';

        //Board
        for(int i = stack_cap; i >= 1; i--){
            for( int j = 1; j <= n_stacks; j++){
                info_buff[len++] = board[i][j] + '0';
                info_buff[len++] = ' ';
            }
            info_buff[len++] = '\n'; //
        }

        ssize_t Sent_size = 0;
        while(Sent_size < len){
            ssize_t n = write(Board_out[1], info_buff + Sent_size, len - Sent_size);
            if (n <= 0) {
                perror("write failed");
                kill(pid, SIGKILL); // 자식 프로세스 종료
                waitpid(pid, NULL, 0); // 자식 프로세스 종료 대기
                fprintf(stderr, "write failed\n");
                exit(EXIT_FAILURE);
            }
            Sent_size += n;
        }


        close(Board_out[1]);

        signal(SIGALRM, timeout_handler);
        alarm(3);

        // 플레이어 돌 위치 받기
        char position_alpa;
        ssize_t bytesRead = read(Order_In[0], &position_alpa, sizeof(position_alpa));
        alarm(0);
        current_position = position_alpa; // 플레이어가 선택한 위치 저장

        close(Order_In[0]);

        // printf("position: %c\n", position_alpa); //디버깅용


        if (bytesRead > 0){
            int position_num = stack_num(position_alpa);
            drop_stone(position_num, player_id);
        }
        else {
            fprintf(stderr, "Error reading from player %d\n", player_id);
            kill(pid, SIGKILL); // 자식 프로세스 종료
            waitpid(pid, NULL, 0); // 자식 프로세스 종료 대기
            exit(EXIT_FAILURE);
        }

    }

    // 자식 프로세스가 종료될 때까지 기다림
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
    {
        int exit_status = WEXITSTATUS(status);
        if (exit_status != 0)
        {
            fprintf(stderr, "Player %d exited with status %d\n", player_id, exit_status);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Player %d runtime out!(error)\n", player_id);
        exit(EXIT_FAILURE);
    }

    signal_pid = -1; // 다음 플레이어를 위해 pid 초기화


}




void drop_stone(int position, int player_id)
{
    if (board[1][position] != 0)
    { // 맨 윗돌까지 찼는데 넣었으면 패배
        printf("Stack %d is full\n", position);
        printf("winner player: %c\n", (player_id == 1) ? 'Y' : 'X');
        exit(EXIT_SUCCESS);
    }

    if (position < 1 || position > n_stacks)
    { // 잘못된 위치에 넣었으면 패배
        printf("Wrong position: %d\n", position);
        printf("winner player: %c\n", (player_id == 1) ? 'Y' : 'X');
        exit(EXIT_SUCCESS);
    }

    drop(position, player_id);

    check_ending();
}

void check_ending(){
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
            if (board[i][j] != 0)
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

                        if (x < 1 || x > stack_cap || y < 1 || y > n_stacks || board[x][y] != board[i][j])
                            break;

                        count++;
                    }

                    if (count >= 4)
                    {
                        printf("winner player: %c\n", (board[i][j] == 1) ? 'X' : 'Y');
                        exit(EXIT_SUCCESS);
                    }
                }
            }
        }
    }

}

void print_board()
{
    clear();
    int turn = move % 2;
    printf("player %c's position: %c\n", (current_player==1 ? 'X' : 'Y'), current_position);
    printf("move: %d\n", move);
    for (int R = 1; R <= stack_cap; R++)
    {
        for (int L = 1; L <= n_stacks; L++)
        {
            printf("%d ", board[R][L]);
        }
        printf("\n");
    }
    printf("A B C D E F G\n");

}

void drop(int position, int player_id)
{

    int destination = 0; // 떨어질 위치

    for (int i = stack_cap; i >= 1; i--)
    {
        if (board[i][position] == 0)
        {
            destination = i;
            break;
        }
    }

    for (int i = 1; i <= destination; i++)
    {
        board[i][position] = player_id;
        clear();
        print_board();
        usleep(140000);
        if(i != destination)
        {
            board[i][position] = 0;
        }
    }
}



void ctrl_c_handler(int sig)
{
    if(signal_pid >0)
    {
        kill(signal_pid, SIGINT);
        waitpid(signal_pid, NULL, 0);
    }
    printf("\nReceived Ctrl+C\n");
    exit(EXIT_SUCCESS);
}

void timeout_handler(int sig)
{
    if (signal_pid > 0)
    {
        kill(signal_pid, SIGKILL); // 자식 프로세스 종료
        waitpid(signal_pid, NULL, 0); // 자식 프로세스 종료 대기
    }
    printf("Player %d timed out\n", current_player);
    printf("winner player: %c\n", (current_player == 1) ? 'Y' : 'X');
    exit(EXIT_SUCCESS);
}
