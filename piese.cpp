int meci::pion(int sr, int sc, int fr, int fc)
{
    int culoare = (board[sr][sc] < 0) ? -1 : 1;
    // if (abs(sr - fr) > 2)
    //     return 0;
    if (culoare > 0 && fr >= sr) // conditii pentru dat inapoi
        return 0;
    if (culoare < 0 && fr <= sr)
        return 0;
    if (abs(sr - fr) == 2)
    {
        if (culoare < 0 && sr == 1 || culoare > 0 && sr == 6)
            if (board[fr][fc] == 0)
                if (sc == fc)
                    return 1;
        return 0;
    }
    if (abs(sr - fr) == 1)
    {
        if (abs(sc - fc) == 1 && board[fr][fc] * culoare < 0)
            return 1;
        // mutari in diagonala doar cand se iau piese
        // diferenta de o coloana si culoare diferita la finish
        if (sc == fc && board[fr][fc] == 0)
            return 1;
    }
    return 0;
}
int meci::cal(int sr, int sc, int fr, int fc)
{
    if ((abs(fr - sr) == 1 && abs(fc - sc) == 2) || (abs(fr - sr) == 2 && abs(fc - sc) == 1))
        return 1;
    return 0;
}
int meci::nebun(int sr, int sc, int fr, int fc)
{
    if (abs(sr - fr) == abs(sc - fc))
        if (verifica_obstacol_diag(sr, sc, fr, fc))
            return 1;
    return 0;
}
int meci::tura(int sr, int sc, int fr, int fc)
{
    if (sr == fr || sc == fc)
        if (verifica_obstacol(sr, sc, fr, fc))
            return 1;
    return 0;
}
int meci::regina(int sr, int sc, int fr, int fc)
{
    // verific mutare de nebun
    if (abs(sr - fr) == abs(sc - fc))
        if (verifica_obstacol_diag(sr, sc, fr, fc))
            return 1;

    // verific mutare de tura
    if (sr == fr || sc == fc)
        if (verifica_obstacol(sr, sc, fr, fc))
            return 1;
    return 0;
}
int meci::rege(int sr, int sc, int fr, int fc)
{
    int i, j;

    // verificam in jurul destinatie sa nu fie regele advers
    for (i = fr - 1; i <= fr + 1; i++)
        for (j = fc - 1; j <= fc + 1; j++)
        {
            if (i < 0 || i > 7 || j < 0 || j > 7)
                continue;
            if (abs(board[i][j]) == 6)
                if (!(i == sr && j == sc))
                    return 0;
        }
    if (abs(sr - fr) <= 1 && abs(sc - fc) <= 1)
        return 1;

    return 0;
}

int meci::verifica_obstacol(int sr, int sc, int fr, int fc)
{
    int s, f, aux, dinamic;
    if (sr == fr) // parcurgem coloane
    {
        dinamic = 0;
        s = sc;
        f = fc;
        aux = sr;
    }
    if (sc == fc) // parcurgem randuri
    {
        dinamic = 1;
        s = sr;
        f = fr;
        aux = sc;
    }

    int i = (s < f) ? s + 1 : s - 1;

    while (i != f)
    {
        if (dinamic)
        {
            if (board[i][aux] != 0)
                return 0;
        }
        else if (board[aux][i] != 0)
            return 0;
        if (s < f)
            i++;
        else
            i--;
    }

    return 1;
}

int meci::verifica_obstacol_diag(int sr, int sc, int fr, int fc)
{
    int j = (sc < fc) ? sc + 1 : sc - 1;
    int i = (sr < fr) ? sr + 1 : sr - 1;

    while (i != fr && j != fc)
    {
        if (board[i][j] != 0)
            return 0;
        j = (sc < fc) ? j + 1 : j - 1;
        i = (sr < fr) ? i + 1 : i - 1;
    }
    return 1;
}