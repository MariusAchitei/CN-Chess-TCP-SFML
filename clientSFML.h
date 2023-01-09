#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <unistd.h> //pentru read write

#include "utils.cpp"

const int BOARD_SIZE = 8;
const int SQUARE_SIZE = 100;

class clientSFML
{
private:
    int culoare;
    int tabla[8][8];
    int sd;
    int gata = 0;
    int selected = -1;

    sf::RenderWindow *window_ptr;

    sf::RectangleShape *board_ptr;
    sf::Sprite piese[BOARD_SIZE][BOARD_SIZE];

    sf::Texture pion_negru;
    sf::Texture pion_alb;
    sf::Texture tura_negru;
    sf::Texture tura_alb;
    sf::Texture cal_negru;
    sf::Texture cal_alb;
    sf::Texture nebun_negru;
    sf::Texture nebun_alb;
    sf::Texture regina_negru;
    sf::Texture regina_alb;
    sf::Texture rege_negru;
    sf::Texture rege_alb;
    sf::Texture blank;

    sf::SoundBuffer mutare_gresita_buf;
    sf::Sound mutare_gresita;

    sf::SoundBuffer mutare_buf;
    sf::Sound mutare;

    sf::SoundBuffer start_game_buf;
    sf::Sound start_game;

    sf::SoundBuffer sah_mat_buf;
    sf::Sound sah_mat;

    int citeste_mutare();
    void citeste_tabla();

public:
    clientSFML(int sd, int culoare);
    // ~clientSFML();

    void trimite_mutare();
    void display();
    void update_textures();
    void play();
};
