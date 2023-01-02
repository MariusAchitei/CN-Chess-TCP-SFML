#include <SFML/Graphics.hpp>

#include "utils.cpp"

#define IP "127.0.0.1"
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
// extern int errno;

/* portul de conectare la server*/
int port;

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

  if ((bytes = read(sd, &nr, sizeof(int))) < 0) // primim mesajul gata
  {
    perror("Eroare la read() de la server.\n");
    // return errno;
  }

  if ((bytes = read(sd, buf, 150)) < 0) // primim tabla de joc
  {
    perror("Eroare la read() de la server.\n");
    // return errno;
  }

  print_board(buf); // afisam tabla initiala
}

void primeste_mutare()
{
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
  fflush(stdout);
  /* citirea mesajului */

  trimite_nume(sd);

  /* citirea raspunsului dat de server
     (apel blocant pana cand serverul raspunde) */

  // citim semnalul de pornire, citim numele adversarului, afisez tabela
  int culoare, gata = 0;

  init_meci(sd, &culoare);

  if (culoare == 1)
    printf("Veti juca cu piesele albe, aveti prima mutare\n");
  else
    printf("Veti juca cu piesele negre, aveti a doua mutare\n");
  /* afisam mesajul primit */

  while (!gata)
  {
    if (culoare > 0)
      citeste_mutare(sd);

    // primeste_mutare(sd, buf);
    if ((bytes = read(sd, &gata, sizeof(int))) < 0) // vedem daca mai continua jocul
    {
      perror("Eroare la read() de la server.\n");
      return errno;
    }
    buf[0] = '\0';
    if ((bytes = read(sd, buf, 150)) < 0) // primim tabla de joc
    {
      perror("Eroare la read() de la server.\n");
      return errno;
    }

    buf[bytes] = '\0';
    print_board(buf);
    printf("\n%s\n", buf);

    if (gata != 0)
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
      citeste_mutare(sd);

    if ((bytes = read(sd, &gata, sizeof(int))) < 0) // vedem daca mai continua jocul
    {
      perror("Eroare la read() de la server.\n");
      return errno;
    }

    buf[0] = '\0';
    if ((bytes = read(sd, buf, 150)) < 0) // primim tabla de joc
    {
      perror("Eroare la read() de la server.\n");
      return errno;
    }
    print_board(buf);
    printf("\n%s\n", buf);

    if (gata != 0)
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

  printf("Mesajul primit este: %d\n", nr);

  /* inchidem conexiunea, am terminat */
  close(sd);
}
