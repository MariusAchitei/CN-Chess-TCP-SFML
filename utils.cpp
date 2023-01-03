#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#include <iostream>

extern int errno;
// #include <iostream>

struct jucator
{
    char nume[50];
    jucator *oponent;
    int fds; // file descriptor
    int ready;
    int culoare;
    pthread_t *thr;
};

char int_to_fenn(int piesa)
{
    switch (piesa)
    {
    case -1:
        return 'p';
        break;

    case -2:
        return 'n';
        break;

    case -3:
        return 'b';
        break;
    case -4:
        return 'r';
        break;
    case -5:
        return 'q';
        break;
    case -6:
        return 'k';
        break;
    case 1:
        return 'P';
        break;
    case 2:
        return 'N';
        break;
    case 3:
        return 'B';
        break;
    case 4:
        return 'R';
        break;
    case 5:
        return 'Q';
        break;
    case 6:
        return 'K';
        break;
    case 0:
        return 0;
        break;

    default:
        break;
    }
    return 0;
}

void print_board(char *fenn)
{
    // int** board = new int*[8];
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
            n--;
            break;
        }
        }
        i++;
        n++;
    }

    print_matrix(board);
}

void print_matrix(int board[8][8])
{
    int i, j;

    std::cout << std::endl;
    for (i = -1; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (i == -1) // pe prima linie afisez numarul coloanei
            {
                if (j == 0)
                    std::cout << "    ";
                else
                    std::cout << j - 1 << " ";
                continue;
            }
            if (!i) // pe a doua linie separatori
            {
                if (j == 0)
                    std::cout << "    ";
                else
                    std::cout << "= ";
                continue;
            }
            if (!j) // pe prima coloana nr linieri si separatori
            {
                std::cout << i - 1 << " | ";
                continue;
            }
            char aux = (board[i - 1][j - 1]) ? int_to_fenn(board[i - 1][j - 1]) : '-';

            std::cout << aux << " ";
            // printf("%c ", int_to_fenn(board[i - 1][j - 1]));
        }
        printf("\n");
    }
}