#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define n_stacks 7
#define stack_cap 6

static int this_player;
static int board[stack_cap + 1][n_stacks + 1];
static int top[n_stacks + 1] = {0};


int stack_num(char c) {
    return c - 'A' + 1;
}

void check_level();
int check_possible(int x, int y);
char stack_name(int i);
int check_line(int player, int num_of_stone, int num_of_open_space);
int check_attack(int player, int stones, int opens);
int check_defend(int player, int stones, int opens);
int detect_gap_three(int player);

int main() {
    int i, j;
    scanf("%d", &this_player);
    if (this_player != 1 && this_player != 2) return EXIT_FAILURE;

    for (i = stack_cap; i >= 1; i--) {
        for (j = 1; j <= n_stacks; j++) {
            scanf("%d", &(board[i][j]));
        }
    }

    check_level();

    int move;


    move = check_defend(this_player, 3, 1);
    if (move > 0 && top[move] != 0) {
        printf("%c", stack_name(move));
        return EXIT_SUCCESS;}

    move = check_attack(this_player, 3, 1);
    if (move > 0 && top[move] != 0) {
        printf("%c", stack_name(move));
        return EXIT_SUCCESS;}

    move = detect_gap_three(3 - this_player);
    if (move > 0 && top[move] != 0) {
        printf("%c", stack_name(move));
        return EXIT_SUCCESS;}
    
    move = detect_gap_three(this_player);
    if (move > 0 && top[move] != 0) {
        printf("%c", stack_name(move));
        return EXIT_SUCCESS;}




    move = check_attack(this_player, 2, 2);
    if (move > 0 && top[move] != 0) {
        printf("%c", stack_name(move));
        return EXIT_SUCCESS;}

    move = check_defend(this_player, 2, 2);
    if (move > 0 && top[move] != 0) {
        printf("%c", stack_name(move));
        return EXIT_SUCCESS;}


    srand(time(NULL));
    do {
        if (top[3] || top[4] || top[5]) {
            move = 3 + (rand() % 3);  // 가운데부터
        } else {
            move = rand() % n_stacks + 1;
        }
    } while (top[move] == 0);
    printf("%c", stack_name(move));
    return EXIT_SUCCESS;
}

    





void check_level() { //몇번째가 맨 위인지
    for (int j = 1; j <= n_stacks; j++) {
        top[j] = 0; 
        for (int i = stack_cap; i >= 1; i--) {
            if (board[i][j] == 0) {
                top[j] = i;
                break;
            }
        }
    }
}

int check_possible(int x, int y) { //판 안에 있는지 확인
    return (x >= 1 && x <= stack_cap && y >= 1 && y <= n_stacks);
}

char stack_name(int i) {
    return 'A' + i - 1;
}

int check_line(int player, int num_of_stone, int num_of_open_space) {
    int checker[4][2] = {
        {1, 0}, {0, 1}, {1, 1}, {1, -1}
    };

    for (int i = 1; i <= stack_cap; i++) {
        for (int j = 1; j <= n_stacks; j++) {
            for (int d = 0; d < 4; d++) {
                int dx = checker[d][0];
                int dy = checker[d][1];
                int count = 0;
                int empty_x = -1, empty_y = -1;
                int valid = 1;

                for (int k = 0; k < 4; k++) {
                    int x = i + k * dx;
                    int y = j + k * dy;

                    if (!check_possible(x, y)) {
                        valid = 0;
                        break;
                    }

                    if (board[x][y] == player) {
                        count++;
                    } else if (board[x][y] == 0 && top[y] == x && top[y] != 0) {
                        empty_x = x;
                        empty_y = y;
                    } else {
                        valid = 0;
                        break;
                    }
                }

                if (count >= 4) {
                    return -1; // 즉시 승리
                }

                if (valid && count == num_of_stone && empty_x != -1 && top[empty_y] != 0) {
                    return empty_y;
                }
            }
        }
    }

    return 0;
}

int check_attack(int player, int stones, int opens) {
    return check_line(player, stones, opens);
}

int check_defend(int player, int stones, int opens) {
    return check_line(3 - player, stones, opens);
}

int detect_gap_three(int player) {
    int checker[4][2] = {
        {0, 1}, {1, 0}, {1, 1}, {1, -1}
    };

    for (int i = 1; i <= stack_cap; i++) {
        for (int j = 1; j <= n_stacks; j++) {
            for (int d = 0; d < 4; d++) {
                int dx = checker[d][0], dy = checker[d][1];
                int x1 = i, y1 = j;
                int x2 = i + dx, y2 = j + dy;
                int x3 = i + 2*dx, y3 = j + 2*dy;
                int x4 = i + 3*dx, y4 = j + 3*dy;

                if (check_possible(x1, y1) && check_possible(x2, y2) &&
                    check_possible(x3, y3) && check_possible(x4, y4)) {

                    if (board[x1][y1] == player && board[x2][y2] == player &&
                        board[x3][y3] == 0 && board[x4][y4] == player &&
                        top[y3] == x3 && top[y3] != 0) return y3;

                    if (board[x1][y1] == player && board[x2][y2] == 0 &&
                        board[x3][y3] == player && board[x4][y4] == player &&
                        top[y2] == x2 && top[y2] != 0) return y2;
                }
            }
        }
    }

    return 0;
}
