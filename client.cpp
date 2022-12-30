#include <SFML/Graphics.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#include "utils.c"

#define IP "127.0.0.1"
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main(int argc, char *argv[])
{
  char *ip = IP;
  int sd;                    // descriptorul de socket
  struct sockaddr_in server; // structura folosita pentru conectare
                             // mesajul trimis
  int nr = 0;
  char buf[150];

  int bytes;

  /* exista toate argumentele in linia de comanda? */
  // if (argc != 3)
  // {
  //   printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
  //   return -1;
  // }

  /* stabilim portul */
  port = atoi(argv[2]);

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
  server.sin_port = htons(2908);

  /* ne conectam la server */
  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("Eroare la connect().\n");
    return errno;
  }
  printf("--\nConectarea a reusit\n--\n");

  /* citirea mesajului */
  printf("Introduceti un nume: ");
  fflush(stdout);
  fgets(buf, sizeof(buf), stdin);

  /* trimiterea mesajului la server */
  if (write(sd, buf, strlen(buf)) <= 0)
  {
    perror("Eroare la write() spre server.\n");
    return errno;
  }
  printf("Am trimis %s\n", buf);
  printf("--\nSe cauta oponent..\n--\n");

  /* citirea raspunsului dat de server
     (apel blocant pina cind serverul raspunde) */

  if ((read(sd, &nr, sizeof(int))) < 0) // primim mesajul ca incepe jocul
  {
    perror("Eroare la read() de la server.\n");
    return errno;
  }
  if ((bytes = read(sd, buf, 50)) < 0) // primim numele adversarului
  {
    perror("Eroare la read() de la server.\n");
    return errno;
  }
  buf[bytes] = '\0';

  printf("Oponentul a fost gasit! -- %s\n", buf);

  if ((bytes = read(sd, buf, 150)) < 0) // primim numele adversarului
  {
    perror("Eroare la read() de la server.\n");
    return errno;
  }

  init_board(buf);

  char *mutare;

  if (nr == 1)
  {
    printf("Veti juca cu piesele albe, aveti prima mutare\n");
    printf("Introduceti mutarea:\n");
    buf[0] = '\0';
    fgets(buf, sizeof(buf), stdin);
    if (write(sd, buf, strlen(buf)) <= 0)
    {
      perror("Eroare la write() spre server.\n");
      return errno;
    }
  }
  else
  {
    printf("Veti juca cu piesele negre, aveti a doua mutare\n");
  }
  /* afisam mesajul primit */

  for (int i = 0; i <= 10; i++)
  {
    buf[0] = '\0';
    if ((bytes = read(sd, buf, 50)) < 0) // primim numele adversarului
    {
      perror("Eroare la read() de la server.\n");
      return errno;
    }
    buf[bytes] = '\0';

    printf("\n%s\n", buf);
    printf("Introduceti mutarea:\n");
    buf[0] = '\0';
    fgets(buf, sizeof(buf), stdin);
    if (write(sd, buf, strlen(buf)) <= 0)
    {
      perror("Eroare la write() spre server.\n");
      return errno;
    }
  }

  printf("Mesajul primit este: %d\n", nr);

  /* inchidem conexiunea, am terminat */
  close(sd);
}
