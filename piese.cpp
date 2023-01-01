int meci::pion(int sr, int sc, int fr, int fc)
{
    return 1;
}
int meci::cal(int sr, int sc, int fr, int fc)
{
    if ((abs(fr - sr) == 1 && abs(fc - sc) == 2) || (abs(fr - sr) == 2 && abs(fc - sc) == 1))
        return 1;
    return 0;
}
int meci::nebun(int sr, int sc, int fr, int fc)
{
    return 1;
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
    return 1;
}
int meci::rege(int sr, int sc, int fr, int fc)
{
    return 1;
}

int meci::verifica_obstacol(int sr, int sc, int fr, int fc)
{
    int s, f, aux, dinamic;
    if (sr == fr)
    {
        dinamic = 1;
        s = sc;
        f = fc;
        aux = sr;
    }
    if (sc == fc)
    {
        dinamic = 0;
        s = sr;
        f = fr;
        aux = sc;
    }
    // if (sr == fr)
    // {
    int i, j;
    if (f < s)
    {
        i = f;
        j = s - 1;
    }
    else
    {
        i = s + 1;
        j = f;
    }

    for (i; i < j; i++)
    {
        if (dinamic)
            if (board[aux][i] != 0)
                return 0;
            else if (board[i][aux] != 0)
                return 0;
        // }
        // if (sc == fc)
        // {
        //     int i, j;
        //     if (fr < sr)
        //     {
        //         i = fr + 1;
        //         j = sr;
        //     }
        //     else
        //     {
        //         i = sr + 1;
        //         j = fr;
        //     }

        //     for (i; i < j; i++)
        //     {
        //         if (board[i][sc] != 0)
        //             return 0;
        //     }
        // }
        return 1;
    }