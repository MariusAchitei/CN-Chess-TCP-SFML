#include "clientSFML.h"

// const int BOARD_SIZE = 8;
// const int SQUARE_SIZE = 100;

clientSFML::clientSFML(int sd, int culoare)
{
    this->culoare = culoare;
    this->sd = sd;

    window_ptr = NULL;
    // window_ptr = new sf::RenderWindow(sf::VideoMode(800, 640, 32), "TileMap Example");
    if (culoare == 1)
        window_ptr = new sf::RenderWindow(sf::VideoMode(BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE), "ALB");
    else
        window_ptr = new sf::RenderWindow(sf::VideoMode(BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE), "NEGRU");

    board_ptr = new sf::RectangleShape[BOARD_SIZE * BOARD_SIZE];

    pion_negru.loadFromFile("img/pawn_black.png");
    pion_alb.loadFromFile("img/pawn_white.png");
    tura_negru.loadFromFile("img/rook_black.png");
    tura_alb.loadFromFile("img/rook_white.png");
    cal_negru.loadFromFile("img/knight_black.png");
    cal_alb.loadFromFile("img/knight_white.png");
    nebun_negru.loadFromFile("img/bishop_black.png");
    nebun_alb.loadFromFile("img/bishop_white.png");
    regina_negru.loadFromFile("img/queen_black.png");
    regina_alb.loadFromFile("img/queen_white.png");
    rege_negru.loadFromFile("img/king_black.png");
    rege_alb.loadFromFile("img/king_white.png");
    blank.loadFromFile("img/blank.png");

    mutare_gresita_buf.loadFromFile("sound/mutare_gresita.wav");
    mutare_gresita.setBuffer(mutare_gresita_buf);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board_ptr[i * BOARD_SIZE + j].setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            board_ptr[i * BOARD_SIZE + j].setPosition(i * SQUARE_SIZE, j * SQUARE_SIZE);
            if ((i + j) % 2 == 0)
                // {
                board_ptr[i * BOARD_SIZE + j].setFillColor(sf::Color{192, 192, 192});
            // }
            else
                // {
                board_ptr[i * BOARD_SIZE + j].setFillColor(sf::Color{47, 79, 79});
            // }
        }
    }
}

void clientSFML::play()
{
    citeste_tabla();
    update_textures();
    display();

    while (!gata)
    {
        printf("HAPCIU\n");
        if (culoare > 0)
            trimite_mutare();
        citeste_tabla();
        update_textures();
        display();
        if (gata)
        {
            printf("----------------\nJOCUL S A INCHEIAT\n");
            if (gata == culoare)
            {
                printf("FELICITARI ATI CASTIGAT\n");
            }
            else
            {
                printf("DIN PACATE ATI PIERDUT, mult noroc data viitoare\n");
            }
            printf("----------------\n");
            break;
        }
        if (culoare < 0)
            trimite_mutare();
        citeste_tabla();
        update_textures();
        display();
        if (gata)
        {
            printf("----------------\nJOCUL S A INCHEIAT\n");
            if (gata == culoare)
            {
                printf("FELICITARI ATI CASTIGAT\n");
            }
            else
            {
                printf("DIN PACATE ATI PIERDUT, mult noroc data viitoare\n");
            }
            printf("----------------\n");
            break;
        }
    }
    printf("FDSUINGDFI\n");
}

void clientSFML::citeste_tabla()
{
    int bytes;
    char fen[150];

    if ((bytes = read(sd, &gata, sizeof(int))) < 0) // vedem daca mai continua jocul
    {
        perror("Eroare la read() de la server.\n");
        // return errno;
    }

    if ((bytes = read(sd, fen, 150)) < 0) // primim tabla de joc
    {
        perror("Eroare la read() de la server.\n");
        // return errno;
    }
    fen[bytes] = '\0';

    int i = 0;
    int m = 0, n = 0;

    while (fen[i] != ' ')
    {
        switch (fen[i])
        {
        case '/':
        {
            m++;
            n = -1;
            break;
        }
        case 'r':
        {
            tabla[m][n] = -4;
            break;
        }
        case 'n':
        {
            tabla[m][n] = -2;
            break;
        }
        case 'b':
        {
            tabla[m][n] = -3;
            break;
        }
        case 'k':
        {
            tabla[m][n] = -6;
            break;
        }
        case 'q':
        {
            tabla[m][n] = -5;
            break;
        }
        case 'p':
        {
            tabla[m][n] = -1;
            break;
        }
        case 'R':
        {
            tabla[m][n] = 4;
            break;
        }
        case 'N':
        {
            tabla[m][n] = 2;
            break;
        }
        case 'B':
        {
            tabla[m][n] = 3;
            break;
        }
        case 'Q':
        {
            tabla[m][n] = 5;
            break;
        }
        case 'K':
        {
            tabla[m][n] = 6;
            break;
        }
        case 'P':
        {
            tabla[m][n] = 1;
            break;
        }

        default:
        {
            int s_gol = fen[i] - '0';
            for (int j = 1; j <= s_gol; j++)
            {
                tabla[m][n] = 0;
                n++;
            }
            n--;
            break;
        }
        }
        i++;
        n++;
    }
    print_matrix(tabla);
}

void clientSFML::update_textures()
{
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            switch (tabla[i][j])
            {
            case -1:
                piese[i][j].setTexture(pion_negru);
                break;

            case -2:
                piese[i][j].setTexture(cal_negru);
                break;

            case -3:
                piese[i][j].setTexture(nebun_negru);
                break;
            case -4:
                piese[i][j].setTexture(tura_negru);
                break;
            case -5:
                piese[i][j].setTexture(regina_negru);
                break;
            case -6:
                piese[i][j].setTexture(rege_negru);
                break;
            case 1:
                piese[i][j].setTexture(pion_alb);
                break;
            case 2:
                piese[i][j].setTexture(cal_alb);
                break;
            case 3:
                piese[i][j].setTexture(nebun_alb);
                break;
            case 4:
                piese[i][j].setTexture(tura_alb);
                break;
            case 5:
                piese[i][j].setTexture(regina_alb);
                break;
            case 6:
                piese[i][j].setTexture(rege_alb);
                break;
            case 0:
                piese[i][j].setTexture(blank);
                break;

            default:
                break;
            }

            // piese[i][j].setTextureRect(rect);
            // piese[i][j].setSize(sf::Vector2f(rect.width, rect.height));

            sf::Vector2f piecePosition;
            piecePosition.x = (j * SQUARE_SIZE);
            piecePosition.y = (i * SQUARE_SIZE);
            piese[i][j].setPosition(piecePosition);
        }
}

void clientSFML::display()
{
    int move = 0;

    if (window_ptr->isOpen())
    {

        window_ptr->clear();

        // Draw each square to the window_ptr->
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                window_ptr->draw(board_ptr[i * BOARD_SIZE + j]);
                // window_ptr->draw(piese[i][j]);
            }
        }
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                // window_ptr->draw(board[i][j]);
                window_ptr->draw(piese[i][j]);
            }
        }

        window_ptr->display();
    }
}

void clientSFML::trimite_mutare()
{
    char buf[10];
    int ok = 0, bytes, move = 0;
    do
    {
        printf("Introduceti mutarea:\n");
        move = citeste_mutare();
        printf("Am pimit de la citire ecran {%d}\n", move);
        buf[0] = move / 1000 + '0';
        buf[1] = move / 100 % 10 + '0';
        buf[2] = move / 10 % 10 + '0';
        buf[3] = move % 10 + '0';
        buf[4] = '\0';
        printf("am trimis mutarea{%s}\n", buf);
        if (write(sd, buf, strlen(buf)) <= 0)
        {
            perror("Eroare la write() spre server.\n");
            // return errno;
        }
        if ((bytes = read(sd, &ok, sizeof(int))) == -1)
            perror("Eroare la citit() de la server.\n");
        if (ok < 1)
        {
            mutare_gresita.play();
            if (ok == -1)
                printf("Sunteti in SAH, alegeti alta mutare\n");
            else
                printf("Mutare inaccesibila\n");
        }
        else
        {
            printf("mutare efectuata\n");
        }
    } while (ok < 1);
}

int clientSFML::citeste_mutare()
{
    int move = 0;

    while (window_ptr->isOpen())
    {
        sf::Event event;
        if (window_ptr->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window_ptr->close();
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    int y = event.mouseButton.y / SQUARE_SIZE;
                    int x = event.mouseButton.x / SQUARE_SIZE;
                    std::cout
                        << "the left button was pressed" << std::endl;
                    std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                    if (!move)
                    {
                        // if (a[event.mouseButton.y / SQUARE_SIZE][event.mouseButton.x / SQUARE_SIZE] == 0)
                        // continue;
                        if (tabla[y][x] * culoare <= 0)
                        {
                            mutare_gresita.play();
                            continue;
                        }
                        move = y * 10 + x;
                        printf("%d  %d\n", x, y);
                    }
                    else
                    {
                        if (move == y * 10 + x)
                        {
                            move = 0;
                            continue;
                        }
                        move *= 100;
                        move += y * 10 + x;
                        printf("%d  %d\n", x, y);
                        printf("Mutarea: {%d}\n", move);

                        int sr = move / 1000, sc = move / 100 % 10, fr = move / 10 % 10, fc = move % 10;

                        piese[fr][fc].setTexture(*(piese[sr][sc].getTexture()));

                        piese[sr][sc].setTexture(blank);

                        return move;
                        move = 0;
                        break;
                    }
                }
                break;
            default:
                break;
            }
        }
    }
}