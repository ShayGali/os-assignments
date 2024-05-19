#include <stdio.h>
#include <stdlib.h>
#define EMPTY '_'
#define COMPUTER 'X'
#define PLAYER 'O'

/**
 * check if we get a permutation of 1-9
 * @param input the input string
 */
void is_valid_input(const char *input) {
    int counter[9] = {0};
    while (*input) {
        if (*input < '1' || *input > '9') {
            printf("Invalid input - input should be between 1 and 9\n");
            printf("got: %c\n", *input);
            exit(1);
        }

        counter[*input - '1']++;
        if (counter[*input - '1'] > 1) {
            printf("Invalid input - duplicate number %c\n", *input);
            exit(1);
        }
        input++;
    }

    // check if we have all the numbers
    for (int i = 0; i < 9; i++) {
        if (counter[i] != 1) {
            printf("Invalid input - missing number %d\n", i + 1);
            exit(1);
        }
    }
}

/**
 * check if the move is a winning move
 */
int check_if_win(char board[3][3], int x, int y) {
    // check the row of x - the y is changing
    if (board[x][0] == board[x][1] && board[x][1] == board[x][2]) {
        return 1;
    }
    // check the column of y - the x is changing
    if (board[0][y] == board[1][y] && board[1][y] == board[2][y]) {
        return 1;
    }
    // main diagonal
    if (x == y) {
        if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
            return 1;
        }
    }

    // secondary diagonal
    if (x + y == 2) {
        if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
            return 1;
        }
    }

    return 0;
}

/**
 * print the board
 */
void print_board(char board[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

/**
 * make a move for the computer
 * @param board the board
 * @param input a pointer to the input string (will be incremented until a valid move is found)
 * @return 1 if the move is a winning move, 0 otherwise
 */
int make_computer_move(char board[3][3], const char **input) {
    int x, y, computer_move;
    while (1) {
        computer_move = (**input - '1');
        x = computer_move / 3;
        y = computer_move % 3;
        if (board[x][y] == EMPTY) {
            board[x][y] = COMPUTER;
            break;
        }
        (*input)++;
    }
    printf("Computer played: (%d, %d)\n", x, y);
    return check_if_win(board, x, y);
}

/**
 * make a move for the user
 * @param board the board
 * @return 1 if the move is a winning move, 0 otherwise
 */
int make_user_move(char board[3][3]) {
    int x, y, user_input;
    while (1) {
        printf("Enter your move: (1-9)\n");
        if (scanf("%d", &user_input) != 1) {
            printf("Invalid input\n");
            // clear the buffer
            while (getchar() != '\n');
            continue;
        }
        if (user_input < 1 || user_input > 9) {
            printf("Invalid input - input should be between 1 and 9\n");
            continue;
        }
        x = (user_input - 1) / 3;
        y = (user_input - 1) % 3;
        if (board[x][y] == EMPTY) {
            board[x][y] = PLAYER;
            break;
        }
        printf("Cell already taken\n");
    }
    printf("You played: (%d, %d)\n", x, y);
    return check_if_win(board, x, y);
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <numbers>\n", argv[0]);
        return 1;
    }

    is_valid_input(argv[1]);
    const char *input = argv[1];
    // fill the board with empty cells
    char board[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = EMPTY;
        }
    }

    for (int round = 0; round < 9; round++) {
        if (round % 2 == 0) {
            int is_won = make_computer_move(board, &input);
            if (is_won) {
                printf("\n\n\033[0;31mComputer win!\033[0m\n\n");
                print_board(board);
                printf("\n");
                return 0;
            }
        } else {
            int is_won = make_user_move(board);
            if (is_won) {
                printf("\n\n\033[0;32mYou win!\033[0m\n\n");
                print_board(board);
                printf("\n");
                return 0;
            }
        }

        print_board(board);
        printf("\n");
    }

    // if we reached here, it's a draw
    printf("\n\n\033[0;33mDRAW!\033[0m\n\n");
    print_board(board);
    printf("\n");
    return 0;
}
