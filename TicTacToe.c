#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Considering the 8 winning combinations (3 rows, 3 columns and 2 diagonals), for each player this keeps eight 4-bit counters.
// Each counter keeps track of how many pieces the player has in a set of 3 aligned squares (using the winMatrix).
// Initial value is set to 1, because it is easier to check for 4
uint32_t playerSum[2] = { 0x11111111, 0x11111111 };

//  Within each hexadecimal value, the winning combinations are listed for rows, then columns, then the diagonals.
uint32_t winMatrix[9] = {
    0x10010010, 0x10001000, 0x10000101,
    0x01010000, 0x01001011, 0x01000100,
    0x00110001, 0x00101000, 0x00100110
    };

int board[9];

static void play(int player, int position)
{
    playerSum[player]+=winMatrix[position];
    board[position]=player;
}

static void undoPlay(int player, int position)
{
    playerSum[player]-=winMatrix[position];
    board[position]=-1;
}

static bool winFor(int player)
{
    if (playerSum[player] & 0x44444444)
        return 1;
    return 0;
}

static bool draw()
{
    if (playerSum[0] + playerSum[1] == 0x55555555)
        return 1;
    return 0;
}

static bool gameOver()
{
    return (winFor(0) || winFor(1) || draw());
}

static void printBoard ()
{
    char cellMark[] = {' ', 'O', 'X'};
    
    printf ("\n");
    for (int i = 0; i < 3; ++i) {
        printf ("|");
        for (int j = 0; j < 3; ++j)
            printf (" %c |", cellMark[board[3*i+j] + 1]);
        printf ("\n");
        if (i < 2)
            printf ("-------------");
        printf ("\n");
    }
}

int iterations = 0;

int negamaxAlphaBeta(int player, int alpha, int beta, bool makeMove)
{
    int opponent = 1 - player;
    
    if (winFor(player)) return 1;
    else if (winFor(opponent)) return -1;
    else if (draw()) return 0;
    
    int bestMove = 0;
    for (int position = 0; position < 9; ++position) {
        if (board[position] == -1) {
            play(player, position);
            int value = -negamaxAlphaBeta(opponent, -beta, -alpha, 0);
            
            undoPlay(player, position);
            
            if (value > alpha) {
                alpha = value;
                bestMove = position;
                if (alpha > beta) {
                    break;
                }
            }
        }
    }
    
    if (makeMove) {
        play(player, bestMove);
    }
    return alpha;
}


static int getUserMove()
{
    int cell;
    int tooManyChars = 0;
    
    do {
        printf("Choose an empty cell between 1 to 9: ");
        cell = getchar();
        cell -= '0';
        tooManyChars = 0;
        while (getchar() != '\n') tooManyChars++;
        
    } while (tooManyChars > 0 || cell < 1 || cell > 9 || board[cell -1] != -1);
    
    return (cell -1);
}


#define MAX_INFINITY 100;

int main (void)
{
    enum players {UserO = 0, ComputerX = 1};
    memset (board, -1, sizeof (board));
    printBoard();
    
    int alpha = - MAX_INFINITY;
    int beta = MAX_INFINITY;
    int result;
    do {
        int position = getUserMove();
        play(UserO, position);
        printBoard();
        
        result = negamaxAlphaBeta(ComputerX, alpha, beta, 1);
        printBoard();
        
    } while (!gameOver());
    
    if (result == 1)
        printf ("X wins\n");
    else if (result == - 1 )
        printf ("O wins\n");
    else
        printf ("draw\n");

    return 0;
}
