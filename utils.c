#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void init_board(char *fenn)
{
    int i = 0;
    int m = 0, n = 0;
    int board[8][8];
    while (fenn[i] != ' ')
    {
        switch (fenn[i])
        {
        case '/':
        {
            m++;
            n = -1;
            break;
        }
        case 'r':
        {
            board[m][n] = -4;
            break;
        }
        case 'n':
        {
            board[m][n] = -2;
            break;
        }
        case 'b':
        {
            board[m][n] = -3;
            break;
        }
        case 'k':
        {
            board[m][n] = -6;
            break;
        }
        case 'q':
        {
            board[m][n] = -5;
            break;
        }
        case 'p':
        {
            board[m][n] = -1;
            break;
        }
        case 'R':
        {
            board[m][n] = 4;
            break;
        }
        case 'N':
        {
            board[m][n] = 2;
            break;
        }
        case 'B':
        {
            board[m][n] = 3;
            break;
        }
        case 'Q':
        {
            board[m][n] = 5;
            break;
        }
        case 'K':
        {
            board[m][n] = 6;
            break;
        }
        case 'P':
        {
            board[m][n] = 1;
            break;
        }

        default:
        {
            int s_gol = fenn[i] - '0';
            for (int j = 1; j <= s_gol; j++)
            {
                board[m][n] = 0;
                n++;
            }
            break;
        }
        }
        i++;
        n++;
    }
    for (i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%d, ", board[i][j]);
        }
        printf("\n");
    }
}