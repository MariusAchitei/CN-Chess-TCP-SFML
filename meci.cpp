#include "meci.h"

#include "piese.cpp"

meci::meci()
{
    printf("\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\nAAAAAAAAAAAAAA\n");
}

meci::meci(jucator *p1, jucator *p2)
{
    this->p1 = p1;
    this->p2 = p2;
    // init_board(START_POSITION);
}

meci::meci(jucator *p1, jucator *p2, char *fenn)
{
    this->p1 = p1;
    this->p2 = p2;
    // init_board(fenn);
    int i = 0;
    int m = 0, n = 0;

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
}

void meci::init()
{
    int alb = 1, negru = 2;

    if (write(p1->fds, &alb, sizeof(int)) == -1)
        perror("Eroare la scris catre client");
    if (write(p2->fds, &negru, sizeof(int)) == -1)
        perror("Eroare la scris catre client");

    p1->culoare = 1;
    p2->culoare = -1;

    // trimitem jucatorilor numele adversarilor

    if (write(p1->fds, p2->nume, 50) == -1)
        perror("Eroare la scris catre client");
    if (write(p2->fds, p1->nume, 50) == -1)
        perror("Eroare la scris catre client");

    if (write(p1->fds, START_POSITION, strlen(START_POSITION)) == -1)
        perror("Eroare la scris catre client");
    if (write(p2->fds, START_POSITION, strlen(START_POSITION)) == -1)
        perror("Eroare la scris catre client");
}

void meci::play()
{

    while (!sahmat)
    {
        inreg_mutare(p1);
        to_move = to_move * -1;
        send_board();
        print_board();

        inreg_mutare(p2);
        send_board();
        print_board();
        to_move = to_move * -1;
    }
}

void meci::inreg_mutare(jucator *p)
{
    char mutare[5];
    int bytes, ok = 0;

    do
    {
        if ((bytes = read(p->fds, mutare, 50)) == -1)
            perror("Eroare la read() de la client.\n");
        mutare[4] = '\0';
        printf("am primit mutarea [%s] de la [%d]\n", mutare, p->culoare);

        if (!(ok = verifica(p, mutare)))
        {
            printf("mutare invalida\n");
            if (write(p->fds, &ok, sizeof(int)) == -1)
                perror("Eroare la scris catre client");
        }
        else
        {
            printf("mutare efectuata\n");
            if (write(p->fds, &ok, sizeof(int)) == -1)
                perror("Eroare la scris catre client");
        }
    } while (!ok);
}

void meci::send_board()
{
    char fenn[100], aux;
    int k = 0, empty_s = 0;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            aux = int_to_fenn(board[i][j]);
            if (aux)
            {
                if (empty_s)
                {
                    fenn[k] = '0' + empty_s;
                    k++;
                    empty_s = 0;
                }
                fenn[k] = aux;
                k++;
            }
            else
                empty_s++;
        }
        if (empty_s)
        {
            fenn[k] = '0' + empty_s;
            k++;
            empty_s = 0;
        }
        fenn[k] = '/';
        k++;
    }
    fenn[k - 1] = ' ';
    if (to_move == 1)
        fenn[k] = 'w';
    else
        fenn[k] = 'b';
    fenn[k + 1] = '\0';

    if (write(p2->fds, fenn, strlen(fenn)) == -1)
        perror("Eroare la scris catre client");
    if (write(p1->fds, fenn, strlen(fenn)) == -1)
        perror("Eroare la scris catre client");

    printf("Stare: [%s]\n", fenn);
}

int meci::verifica(jucator *p, char *mutare)
{
    int fail = 0;
    int sr = mutare[0] - '0', sc = mutare[1] - '0', fr = mutare[2] - '0', fc = mutare[3] - '0';

    // daca piesa si culoarea nu au acelasi semn mutarea este invalida

    if ((board[sr][sc] * p->culoare) <= 0)
        return 0;

    // piesa trebuie asezata pe un patrat gol sau unul cu piesa adversa
    if ((board[fr][fc] * board[sr][sc]) > 0)
        return 0;

    // verifica daca piesa este in afara tablei
    if (fr > 7 || fr < 0 || fc > 7 || fc < 0)
        return 0;

    if (sr == fr && sc == fc)
        return 0;

    switch (abs(board[sr][sc]))
    {
    case 1:
        if (!pion(sr, sc, fr, fc))
            return 0;
        break;
    case 2:
        if (!cal(sr, sc, fr, fc))
            return 0;
        break;
    case 3:
        if (!nebun(sr, sc, fr, fc))
            return 0;
        break;
    case 4:
        if (!tura(sr, sc, fr, fc))
            return 0;
        break;
    case 5:
        if (!regina(sr, sc, fr, fc))
            return 0;
        break;
    case 6:
        if (!rege(sr, sc, fr, fc))
            return 0;
        break;

    default:
        break;
    }

    board[fr][fc] = board[sr][sc];
    board[sr][sc] = 0;
    return 1;
}

void meci::print_board()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%d, ", board[i][j]);
        }
        printf("\n");
    }
}

// meci::~meci()