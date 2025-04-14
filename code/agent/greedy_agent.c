#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define n_stacks 7
#define stack_cap 6

static int this_player;
static int board[stack_cap + 1][n_stacks + 1];
static int top[n_stacks + 1] = {0, 1, 1, 1, 1, 1, 1, 1};

int const upward = 1;
int const downward = -1;
int const leftward = -1;
int const rightward = 1;

int _count(int stack, int level, int hdir, int vdir, int player) {
    int count = 0;
    while (1 <= stack && stack <= n_stacks &&
           1 <= level && level <= stack_cap) {
        if (board[level][stack] == player) {
            count++;
            stack += hdir;
            level += vdir;
        } else {
            break;
        }
    }
    return count;
}

int check_possible(int x, int y) {
    return (x >= 1 && x <= stack_cap && y >= 1 && y <= n_stacks);
}

int count_down(int stack, int level, int player) {
    return _count(stack, level + downward, 0, downward, player);
}
int count_left(int stack, int level, int player) {
    return _count(stack + leftward, level, leftward, 0, player);
}
int count_right(int stack, int level, int player) {
    return _count(stack + rightward, level, rightward, 0, player);
}
int count_down_left(int stack, int level, int player) {
    return _count(stack + leftward, level + downward, leftward, downward, player);
}
int count_down_right(int stack, int level, int player) {
    return _count(stack + rightward, level + downward, rightward, downward, player);
}
int count_up_left(int stack, int level, int player) {
    return _count(stack + leftward, level + upward, leftward, upward, player);
}
int count_up_right(int stack, int level, int player) {
    return _count(stack + rightward, level + upward, rightward, upward, player);
}

int greedy_find_3(int target_player) {
    for (int stack = 1; stack <= n_stacks; stack++) {
        if (top[stack] > stack_cap) continue;
        int level = top[stack];

        if (count_down(stack, level, target_player) == 3) return stack;
        if (count_left(stack, level, target_player) + count_right(stack, level, target_player) == 3) return stack;
        if (count_down_left(stack, level, target_player) + count_up_right(stack, level, target_player) == 3) return stack;
        if (count_up_left(stack, level, target_player) + count_down_right(stack, level, target_player) == 3) return stack;
    }
    return 0;
}


int greedy_offense(int player) {
    return greedy_find_3(player);
}

int greedy_defense(int player) {
    return greedy_find_3(3 - player);
}

// 연속 2개 + 양쪽 비어있음
int greedy_attack_2_open(int player) {
    for (int i = 1; i <= stack_cap; i++) {
        for (int j = 1; j <= n_stacks; j++) {
            // 수평 체크: O O _ O or _ O O O
            if (j <= n_stacks - 3) {
                if (board[i][j] == player && board[i][j+1] == player && board[i][j+2] == 0 && board[i][j+3] == player &&
                    top[j+2] == i) return j+2;

                if (board[i][j] == 0 && board[i][j+1] == player && board[i][j+2] == player && board[i][j+3] == player &&
                    top[j] == i) return j;
            }
        }
    }
    return 0;
}

int greedy_defend_2_open(int player) {
    int opponent = 3 - player;
    for (int i = 1; i <= stack_cap; i++) {
        for (int j = 1; j <= n_stacks - 3; j++) {
            // O O _ O (상대)
            if (board[i][j] == opponent && board[i][j+1] == opponent &&
                board[i][j+2] == 0 && board[i][j+3] == opponent &&
                top[j+2] == i) {
                return j+2;
            }

            // _ O O O
            if (board[i][j] == 0 && board[i][j+1] == opponent &&
                board[i][j+2] == opponent && board[i][j+3] == opponent &&
                top[j] == i) {
                return j;
            }
        }
    }
    return 0;
}

// 갭이 있는 3개 (예: OXO, XOX)
int greedy_gap_three(int player) {
    for (int i = 1; i <= stack_cap; i++) {
        for (int j = 1; j <= n_stacks; j++) {
            if (j <= n_stacks - 3) {
                // O X O _
                if (board[i][j] == player && board[i][j+1] == 0 && board[i][j+2] == player && board[i][j+3] == player &&
                    top[j+1] == i) return j+1;

                // _ O X O
                if (board[i][j] == player && board[i][j+1] == player && board[i][j+2] == 0 && board[i][j+3] == player &&
                    top[j+2] == i) return j+2;
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
            if (board[i][j] != 0 && top[j] == 1)
                top[j] = i + 1;
        }
    }

    int choice;

    if ((choice = greedy_defense(this_player))) {
        printf("%c", 'A' + choice - 1);
        return EXIT_SUCCESS;
    }

    if ((choice = greedy_offense(this_player))) {
        printf("%c", 'A' + choice - 1);
        return EXIT_SUCCESS;
    }

    if ((choice = greedy_defend_2_open(this_player))) {
        printf("%c", 'A' + choice - 1);
        return EXIT_SUCCESS;
    }

    if ((choice = greedy_attack_2_open(this_player))) {
        printf("%c", 'A' + choice - 1);
        return EXIT_SUCCESS;
    }

    if ((choice = greedy_gap_three(this_player))) {
        printf("%c", 'A' + choice - 1);
        return EXIT_SUCCESS;
    }

    if ((choice = greedy_gap_three(3 - this_player))) {
        printf("%c", 'A' + choice - 1);
        return EXIT_SUCCESS;
    }

    srand(time(NULL));
    do {
        choice = rand() % n_stacks + 1;
    } while (board[1][choice] != 0);

    printf("%c", 'A' + choice - 1);
    return EXIT_SUCCESS;
}
