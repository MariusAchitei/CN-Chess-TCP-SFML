#pragma once
#include "utils.cpp"

#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\0"

class meci
{
private:
    int board[8][8];
    jucator *p1;
    jucator *p2;

    // rocadaP1 = 1;
    // rocadaP2 = 1;

    int gata = 0;
    int sah = 0;
    int to_move = 1;
    jucator *castigator;

    // int sahmat = 0;

    int verifica(jucator *p, int sr, int sc, int fr, int fc);
    void inreg_mutare(jucator *p);
    void send_board();

    int pion(int sr, int sc, int fr, int fc);
    int cal(int sr, int sc, int fr, int fc);
    int nebun(int sr, int sc, int fr, int fc);
    int tura(int sr, int sc, int fr, int fc);
    int regina(int sr, int sc, int fr, int fc);
    int rege(int sr, int sc, int fr, int fc);

    int verifica_obstacol(int sr, int sc, int fr, int fc);
    int verifica_obstacol_diag(int sr, int sc, int fr, int fc);

    int is_sah(jucator *p);
    int sahmat(jucator *p);
    void get_rege(int culoare, int *x, int *y);

public:
    meci();
    meci(jucator *p1, jucator *p2);
    meci(jucator *p1, jucator *p2, char *fenn);

    void print_board();

    void init();

    void play();

    void final();
    // ~meci();
};
