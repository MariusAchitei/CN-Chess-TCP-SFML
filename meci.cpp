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
    p1->rocada_mare = 1;
    p2->rocada_mare = 1;

    p1->rocada_mica = 1;
    p2->rocada_mica = 1;

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
            n--;
            break;
        }
        }
        i++;
        n++;
    }
}

void meci::init()
{
    int alb = 1, negru = -1;
    p1->oponent = p2;
    p2->oponent = p1;

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

    // if (write(p1->fds, START_POSITION, strlen(START_POSITION)) == -1)
    //     perror("Eroare la scris catre client");
    // if (write(p2->fds, START_POSITION, strlen(START_POSITION)) == -1)
    //     perror("Eroare la scris catre client");

    send_board();
}

void meci::play()
{

    while (!gata)
    {
        inreg_mutare(p1);
        if (sahmat(p2))
        {
            gata = 1;
            castigator = p1;
            break;
        }
        to_move = to_move * -1;
        send_board();
        print_board();

        inreg_mutare(p2);
        if (sahmat(p1))
        {
            gata = -1;
            castigator = p2;
            break;
        }
        send_board();
        print_board();
        to_move = to_move * -1;
    }
}

void meci::inreg_mutare(jucator *p)
{
    char mutare[5];
    int bytes, ok = 0, aux, sah = -1;
    int verif = 0; // voi folosi asta pentru rocada

    // loop pentru a inregistra mutari, pana apare cea buna
    do
    {
        if ((bytes = read(p->fds, mutare, 50)) == -1)
            perror("Eroare la read() de la client.\n");
        mutare[4] = '\0';
        printf("am primit mutarea [%s] de la [%d]\n", mutare, p->culoare);

        // transform mutarea din sir de caractere in coordonate
        int sr = mutare[0] - '0', sc = mutare[1] - '0', fr = mutare[2] - '0', fc = mutare[3] - '0';

        verif = verifica(p, sr, sc, fr, fc);
        if (!verif || abs(board[fr][fc]) == 6) // tinta sa nu fie rege sau mutare legala
        {
            printf("mutare invalida\n");
            if (write(p->fds, &ok, sizeof(int)) == -1)
                perror("Eroare la scris catre client");
            continue;
        }
        // rocada returneaza 2
        if (verif == 2)
        {
            ok = 1;
            printf("mutare efectuata\n");
            if (write(p->fds, &ok, sizeof(int)) == -1)
                perror("Eroare la scris catre client");
            break;
        }

        aux = board[fr][fc];
        board[fr][fc] = board[sr][sc];
        board[sr][sc] = 0;
        if (is_sah(p))
        {
            board[sr][sc] = board[fr][fc];
            board[fr][fc] = aux;
            printf("mutare invalida, Intrati in SAH de la adversar\n");
            if (write(p->fds, &sah, sizeof(int)) == -1)
                perror("Eroare la scris catre client");
            continue;
        }

        // verific daca un pion a ajuns la capat
        if (abs(board[fr][fc]) == 1 && (fr == 0 || fr == 7))
        {
            // dau direct regina
            if (fr == 0)
                board[fr][fc] = 5;
            else
                board[fr][fc] = -5;
        }

        // daca a fost mutat regele nu se mai paote face rocada
        if (abs(board[fr][fc]) == 6)
        {
            p->rocada_mare = 0;
            p->rocada_mica = 0;
        }
        if (p->culoare == -1 && board[fr][fc] == -4)
        {
            if (sr == 0 && sc == 0)
                p->rocada_mare = 0;
            else if (sr == 0 && sc == 7)
                p->rocada_mica = 0;
        }
        if (p->culoare == 1 && board[fr][fc] == 4)
        {
            if (sr == 7 && sc == 0)
                p->rocada_mare = 0;
            else if (sr == 7 && sc == 7)
                p->rocada_mica = 0;
        }

        ok = 1;
        printf("mutare efectuata\n");
        if (write(p->fds, &ok, sizeof(int)) == -1)
            perror("Eroare la scris catre client");
        break;
        // am efectuat mutarea verificam sahmat
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

    if (write(p2->fds, &gata, sizeof(int)) == -1)
        perror("Eroare la scris catre client");
    if (write(p1->fds, &gata, sizeof(int)) == -1)
        perror("Eroare la scris catre client");

    if (write(p2->fds, fenn, strlen(fenn)) == -1)
        perror("Eroare la scris catre client");
    if (write(p1->fds, fenn, strlen(fenn)) == -1)
        perror("Eroare la scris catre client");

    printf("Stare: [%s]\n", fenn);
}

int meci::verifica(jucator *p, int sr, int sc, int fr, int fc)
{
    // verific daca facem rocada
    if (abs(board[sr][sc] + board[fr][fc]) == 10)
    {
        if (is_sah(p))
            return 0;
        if (p->culoare == -1)
        {
            if (fr == 0 && fc == 0 && p->rocada_mare)
            {
                if (tura(fr, fc, sr, sc))
                {
                    board[sr][sc - 2] = -4;
                    board[sr][sc - 1] = -6;
                    board[sr][sc] = 0;
                    board[fr][fc] = 0;
                    if (is_sah(p))
                    {
                        board[sr][sc] = -6;
                        board[fr][fc] = -4;
                        board[sr][sc - 2] = 0;
                        board[sr][sc - 1] = 0;
                        return 0;
                    }
                    p->rocada_mare = 0;
                    p->rocada_mica = 0;
                    return 2;
                }
                else
                    return 0;
            }
            if (fr == 0 && fc == 7 && p->rocada_mica)
                if (tura(fr, fc, sr, sc))
                {
                    board[sr][sc + 2] = -4;
                    board[sr][sc + 1] = -6;
                    board[sr][sc] = 0;
                    board[fr][fc] = 0;
                    if (is_sah(p))
                    {
                        board[sr][sc] = -6;
                        board[fr][fc] = -4;
                        board[sr][sc + 2] = 0;
                        board[sr][sc + 1] = 0;
                        return 0;
                    }
                    p->rocada_mare = 0;
                    p->rocada_mica = 0;
                    return 2;
                }
                else
                    return 0;
        }
        if (p->culoare == 1)
        {
            if (fr == 7 && fc == 0 && p->rocada_mare)
            {
                if (tura(fr, fc, sr, sc))
                {
                    board[sr][sc - 2] = 4;
                    board[sr][sc - 1] = 6;
                    board[sr][sc] = 0;
                    board[fr][fc] = 0;
                    if (is_sah(p))
                    {
                        board[sr][sc] = 6;
                        board[fr][fc] = 4;
                        board[sr][sc - 2] = 0;
                        board[sr][sc - 1] = 0;
                        return 0;
                    }
                    p->rocada_mare = 0;
                    p->rocada_mica = 0;
                    return 2;
                }
                else
                    return 0;
            }
            if (fr == 7 && fc == 7 && p->rocada_mica)
            {
                if (tura(fr, fc, sr, sc))
                {
                    board[sr][sc + 2] = 4;
                    board[sr][sc + 1] = 6;
                    board[sr][sc] = 0;
                    board[fr][fc] = 0;
                    if (is_sah(p))
                    {
                        board[sr][sc] = 6;
                        board[fr][fc] = 4;
                        board[sr][sc + 2] = 0;
                        board[sr][sc + 1] = 0;
                        return 0;
                    }
                    p->rocada_mare = 0;
                    p->rocada_mica = 0;
                    return 2;
                }
                else
                    return 0;
            }
        }
    }

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

    // if (abs(board[fr][fc]) == 6)
    //     return 0;

    // verificam daca mutarea este specifica piesei
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

    return 1;
}

int meci::is_sah(jucator *p)
{
    int x_rege, y_rege;
    get_rege(p->culoare, &x_rege, &y_rege);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] * p->culoare >= 0)
                continue;
            if (verifica(p->oponent, i, j, x_rege, y_rege))
            {
                sah = i * 10 + j; // in variabila sah punem locatia atacului
                return 1;
            }
        }
    return 0;
}

int meci::sahmat(jucator *p)
{
    if (!is_sah(p))
        return 0;

    int x_atac = sah / 10, y_atac = sah % 10;
    int x_rege, y_rege;
    get_rege(p->culoare, &x_rege, &y_rege);

    // verificam daca piesa care ataca poate fi capturata
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            // verificare de mutat regele
            if (verifica(p, x_rege, y_rege, i, j))
            {
                int aux = board[i][j];
                board[i][j] = board[x_rege][y_rege];
                board[x_rege][y_rege] = 0;

                if (!is_sah(p))
                {
                    board[x_rege][y_rege] = board[i][j];
                    board[i][j] = aux;
                    return 0;
                }
                board[x_rege][y_rege] = board[i][j];
                board[i][j] = aux;
            }
            // daca piesa este de aceeasi culoare sau este 0
            if (board[i][j] * p->culoare <= 0)
                continue;

            // verificam daca piesa care ataca poate fi capturata
            if (verifica(p, i, j, x_atac, y_atac))
            {
                int aux = board[x_atac][y_atac];
                board[x_atac][y_atac] = board[i][j];
                board[i][j] = 0;
                if (!is_sah(p))
                {
                    board[i][j] = board[x_atac][y_atac];
                    board[x_atac][y_atac] = aux;
                    return 0;
                }
                board[i][j] = board[x_atac][y_atac];
                board[x_atac][y_atac] = aux;
            }
        }

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            // verificare de blocare cu alta piesa
            if (verifica(p->oponent, x_atac, y_atac, i, j))
            {
                for (int m = 0; m < 8; m++)
                    for (int n = 0; n < 8; n++)
                    {
                        if (verifica(p, m, n, i, j))
                        {
                            int aux = board[i][j];
                            board[i][j] = board[m][n];
                            board[m][n] = 0;
                            if (!verifica(p->oponent, x_atac, y_atac, x_rege, y_rege))
                            {
                                if (!is_sah(p))
                                {
                                    board[m][n] = board[i][j];
                                    board[i][j] = aux;
                                    return 0;
                                }
                            }
                            board[m][n] = board[i][j];
                            board[i][j] = aux;
                        }
                    }
            }
    return 1;
}

void meci::get_rege(int culoare, int *x, int *y)
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (board[i][j] * culoare == 6) // gasim coordonatele regelui
            {
                *x = i;
                *y = j;
                return;
                // break;
            }
}

void meci::print_board()
{
    print_matrix(board);
}

void meci::final()
{
    send_board();
    print_board();
}
// meci::~meci()