#include <pthread.h>

#include "meci.cpp"

/* portul folosit */
#define PORT 2908

// #define START_POSITION "rnbqkbn1/ppppp1pp/4p3/r7/5Q2/5R2/PPPPPPPP/RNB1K1NB w KQkq - 0 1\0"
// #define START_POSITION "rnb1kbnr/ppp1qppp/2p2p2/8/8/2P2P2/PPP1QPPP/RNB1KBNR w KQkq - 0 1\0"
// #define START_POSITION "3k1b2/5ppp/P7/8/8/8/5PPP/4K2Q w KQkq - 0 1\0"
// #define START_POSITION "r3kbnr/pppppp1p/8/8/8/8/PPPPPPPP/RN1QK2R - 0 1\0"
#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\0"

/* codul de eroare returnat de anumite apeluri */

// typedef struct thData
// {
//     int idThread; // id-ul thread-ului tinut in evidenta de acest program
//     int cl;       // descriptorul intors de accept
// } thData;

jucator juc[12];

void *creeaza_joc(void *arg)
{
    int p1 = *(int *)arg, p2 = *(int *)(arg + 4);

    meci m(&juc[p1], &juc[p2], START_POSITION);
    m.print_board();
    m.init(); // anunta jucatorii ca a inceput meciul, da info despre adversari, trimite formatia de start
    m.play();
    m.final();

    return NULL;
}

void *inreg_nume(void *arg)
{
    int index = *(int *)arg;
    int bytes;

    char nume[50];

    if ((bytes = read(juc[index].fds, nume, 50)) == -1)
        perror("Eroare la read() de la client.\n");

    nume[bytes - 1] = '\0';
    strcpy(juc[index].nume, nume);

    juc[index].ready = 1;

    printf("%s\n%d\n", juc[index].nume, juc[index].fds);

    free(arg);
    return NULL;
}

char *conv_addr(struct sockaddr_in address)
{
    static char str[25];
    char port[7];

    strcpy(str, inet_ntoa(address.sin_addr));
    bzero(port, 7);
    sprintf(port, ":%d", ntohs(address.sin_port));
    strcat(str, port);
    return (str);
}

int main()
{

    for (int i = 0; i <= 12; i++)
    {
        juc[i].ready = -1;
    }

    struct sockaddr_in server;
    struct sockaddr_in from;
    fd_set readfds;
    fd_set actfds;
    struct timeval tv;
    int sd, client;
    int optval = 1;
    int fd;
    int nfds;
    unsigned int len;

    int cur_client = 0;

    pthread_t th[12];

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server] Eroare la socket().\n");
        return errno;
    }

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server] Eroare la bind().\n");
        return errno;
    }

    if (listen(sd, 10) == -1)
    {
        perror("[server] Eroare la listen().\n");
        return errno;
    }

    FD_ZERO(&actfds);
    FD_SET(sd, &actfds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    nfds = sd;

    printf("[server] Asteptam la portul %d...\n", PORT);
    fflush(stdout);
    while (1)
    {
        bcopy((char *)&actfds, (char *)&readfds, sizeof(readfds));

        if (select(nfds + 1, &readfds, NULL, NULL, &tv) < 0)
        {
            perror("[server] Eroare la select().\n");
            return errno;
        }

        if (FD_ISSET(sd, &readfds))
        {
            len = sizeof(from);
            bzero(&from, sizeof(from));

            client = accept(sd, (struct sockaddr *)&from, &len);

            if (client < 0)
            {
                perror("[server] Eroare la accept().\n");
                continue;
            }

            // if (nfds < client)
            //     nfds = client;

            FD_SET(client, &actfds);
            printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n", client, conv_addr(from));
            fflush(stdout);

            juc[cur_client].thr = &th[cur_client];
            juc[cur_client].fds = client;
            juc[cur_client].ready = 0;
            printf("%d\n", cur_client);

            int *p_index_jucator = (int *)malloc(sizeof(int));
            *p_index_jucator = cur_client;
            pthread_create(&th[cur_client], NULL, *inreg_nume, p_index_jucator);
            cur_client++;
        }
        int i_p1 = -1;
        for (int i = 0; i <= cur_client; i++)
        {
            if (juc[i].ready == 1 && i_p1 <= -1)
            {
                i_p1 = i;
                continue;
            }
            if (i_p1 >= -1 && juc[i].ready == 1)
            {
                int i_p2 = i;
                juc[i_p1].ready = 0;
                juc[i_p2].ready = 0;

                pthread_join(*juc[i_p1].thr, NULL);
                pthread_join(*juc[i_p2].thr, NULL);

                int i_meci[2];
                i_meci[0] = i_p1;
                i_meci[1] = i;

                printf("oponenti %d-%s [VS] %d-%s\n", i_meci[0], juc[i_p1].nume, i_meci[1], juc[i_p2].nume);

                pthread_create(&th[i_p1], NULL, *creeaza_joc, &i_meci);
            }
        }
    }
    return 0;
}