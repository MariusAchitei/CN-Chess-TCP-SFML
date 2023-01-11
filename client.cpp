#include <SFML/Graphics.hpp>

// #include "utils.cpp"
#include "clientSFML.cpp"

#define IP "127.0.0.1"
#define PORT 2908

int board[8][8];

/* portul de conectare la server*/
// int port;

void trimite_nume(int sd)
{
  char buf[50];
  printf("Introduceti un nume: ");
  fgets(buf, sizeof(buf), stdin);

  /* trimiterea mesajului la server */
  if (write(sd, buf, strlen(buf)) <= 0)
  {
    perror("Eroare la write() spre server.\n");
    // return errno;
  }
  printf("Am trimis %s\n", buf);
  printf("--\nSe cauta oponent..\n--\n");
}

// citesc semnalul de inceput, culoarea, numele adversarului
void init_meci(int sd, int *culoare)
{
  int nr, bytes;
  char buf[150];
  if ((read(sd, &nr, sizeof(int))) < 0) // primim mesajul ca incepe jocul
  {
    perror("Eroare la read() de la server.\n");
    // return errno;
  }

  *culoare = nr; // pastram culoarea in referinta

  if ((bytes = read(sd, buf, 50)) < 0) // primim numele adversarului
  {
    perror("Eroare la read() de la server.\n");
    // return errno;
  }
  buf[bytes] = '\0';

  printf("Oponentul a fost gasit! -- %s\n", buf);
}

int main(int argc, char *argv[])
{
  char *ip = IP;
  int sd;                    // descriptorul de socket
  struct sockaddr_in server; // structura folosita pentru conectare
                             // mesajul trimis
  int nr = 0;
  char buf[150];

  int bytes;

  /* cream socketul */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Eroare la socket().\n");
    return errno;
  }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(ip);
  /* portul de conectare */
  server.sin_port = htons(PORT);

  /* ne conectam la server */
  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("Eroare la connect().\n");
    return errno;
  }
  printf("--\nConectarea a reusit\n--\n");
  fflush(stdout);
  /* citirea mesajului */

  trimite_nume(sd);

  int culoare, gata = 0;

  init_meci(sd, &culoare);

  if (culoare == 1)
    printf("Veti juca cu piesele albe, aveti prima mutare\n");
  else
    printf("Veti juca cu piesele negre, aveti a doua mutare\n");

  clientSFML client(sd, culoare);
  client.play();

  /* afisam mesajul primit */

  // printf("Mesajul primit este: %d\n", nr);

  /* inchidem conexiunea, am terminat */
  close(sd);
}
