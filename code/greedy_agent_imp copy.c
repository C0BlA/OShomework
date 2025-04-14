#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


#define n_stacks 7
#define stack_cap 6

static int this_player;
static int board[stack_cap + 1][n_stacks + 1];
static int top[n_stacks + 1] = {0, 1, 1, 1, 1, 1, 1, 1};

int const upward = 1;
int const downward = -1;
int const leftward = -1;
int const rightward = 1;

char stack_name(int i)
{
    return 'A' + i - 1;
}

int stack_num(char c)
{
    return c - 'A' + 1;
}



int check_possible(int x, int y) {
    return (x >= 1 && x <= stack_cap && y >= 1 && y <= n_stacks);
}

int check_defend(int player, int num_of_stone, int num_of_open_space) {
    int other = 3 - player;
    return check_line(other, num_of_stone, num_of_open_space);
}

int check_attack(int player, int num_of_stone, int num_of_open_space) {
    return check_line(player, num_of_stone, num_of_open_space);
}

int check_line(int player, int num_of_stone, int num_of_open_space) {
    int checker[4][2] = {
        {1, 0}, 
        {0, 1}, 
        {1, 1}, 
        {1, -1}
    };

    for (int i = 1; i <= stack_cap; i++) {
        for (int j = 1; j <= n_stacks; j++) {
            if (board[i][j] != player) continue;

            for (int d = 0; d < 4; d++) {
                int dx = checker[d][0], dy = checker[d][1];
                int count = 1;

                int start_x = i, start_y = j;
                int finish_x = i, finish_y = j;

                while (check_possible(finish_x + dx, finish_y + dy) && board[finish_x + dx][finish_y + dy] == player) {
                    finish_x += dx;
                    finish_y += dy;
                    count++;
                }

                while (check_possible(start_x - dx, start_y - dy) && board[start_x - dx][start_y - dy] == player) {
                    start_x -= dx;
                    start_y -= dy;
                    count++;
                }

                if (count == num_of_stone) {
                    int open_space = 0;
                    int open_left_x = start_x - dx, open_left_y = start_y - dy;
                    int open_right_x = finish_x + dx, open_right_y = finish_y + dy;

                    if (check_possible(open_left_x, open_left_y) && board[open_left_x][open_left_y] == 0 && top[open_left_y] == open_left_x)
                        open_space++;
                    if (check_possible(open_right_x, open_right_y) && board[open_right_x][open_right_y] == 0 && top[open_right_y] == open_right_x)
                        open_space++;

                    if (open_space >= num_of_open_space) {
                        if (open_space == 2) {
                            return (rand() % 2) ? open_left_y : open_right_y;
                        } else {
                            return (board[open_left_x][open_left_y] == 0) ? open_left_y : open_right_y;
                        }
                    }
                }
            }
        }
    }
    return 0;
}


int detect_gap_three(int player) {
    int checker[4][2] = {
        {0, 1}, 
        {1, 0}, 
        {1, 1}, 
        {1, -1}
    };

    for (int i = 1; i <= stack_cap; i++) {
        for (int j = 1; j <= n_stacks; j++) {
            for (int d = 0; d < 4; d++) {
                int dx = checker[d][0], dy = checker[d][1];
                // 하나하나 검사
                int x1 = i, y1 = j;
                int x2 = i + dx, y2 = j + dy;
                int x3 = i + 2*dx, y3 = j + 2*dy;
                int x4 = i + 3*dx, y4 = j + 3*dy;

                // 돌, 돌, 빈칸, 돌
                if (check_possible(x1, y1) && 
                    check_possible(x2, y2) &&
                    check_possible(x3, y3) && 
                    check_possible(x4, y4)) {
                    if (board[x1][y1] == player && 
                        board[x2][y2] == player &&
                        board[x3][y3] == 0 
                        && board[x4][y4] == player &&
                        top[y3] == x3)
                        return y3;
                }

                // 돌, 빈칸, 돌, 돌
                if (check_possible(x1, y1) && 
                    check_possible(x2, y2) &&
                    check_possible(x3, y3) && 
                    check_possible(x4, y4)) {
                    if (board[x1][y1] == player && 
                        board[x2][y2] == 0 &&
                        board[x3][y3] == player && 
                        board[x4][y4] == player &&
                        top[y2] == x2)
                        return y2;
                }
            }
        }
    }
    return 0;
}

int main() {
    int i, j;
    scanf("%d", &this_player);

    if (this_player != 1 && this_player != 2)
        return EXIT_FAILURE;

    for (i = stack_cap; i > 0; i--) {
        for (j = 1; j <= n_stacks; j++) {
            scanf("%d", &(board[i][j]));
            if (board[i][j] != 0 && top[j] == 1) {
                top[j] = i + 1;
            }
        }
    }
    int move = check_attack(this_player, 3, 1);  // 세로 3개 연속
    if (move) {
        char position = stack_name(move);
        printf("%c", position); 
        return EXIT_SUCCESS;
    }
    move = check_defend(this_player, 3, 1);  // 상대가 세로 3개 연속을 만들려 한다면 막을 위치
    if(move) {
        char position = stack_name(move);
        printf("%c", position); 
        return EXIT_SUCCESS;
    }

    move = check_attack(this_player, 2, 2);  // 3연속 돌이 놓여질 수 있는지 확인
    if (move) {
        char position = stack_name(move);
        printf("%c", position); 
        return EXIT_SUCCESS;
    }


    move = check_defend(this_player, 2, 2);  // 상대가 3연속을 만들어 놓으려 한다면 막을 위치
    if (move) {
        char position = stack_name(move);
        printf("%c", position); 
        return EXIT_SUCCESS;
    }


    move = detect_gap_three(this_player); 
    if (move) {
        char position = stack_name(move);
        printf("%c", position); 
        return EXIT_SUCCESS;
    }

    move = detect_gap_three(3 - this_player);
    if (move) {
        char position = stack_name(move);
        printf("%c", position);
        return EXIT_SUCCESS;
    }

    srand(time(NULL)); 
    int choice;

    do {
        if (board[1][3] == 0 || board[1][4] == 0 || board[1][5] == 0) {
            choice = 3 + (rand() % 3);  // 3, 4, 5 중 선택
        } else {
            choice = rand() % n_stacks + 1;  // 1~7 중 선택
        }
    } while (board[1][choice] != 0);

    char position = stack_name(choice);
    printf("%c", position); 
    return EXIT_SUCCESS;
}
