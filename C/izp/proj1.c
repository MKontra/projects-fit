#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Projekt 1: 
// Ciselne prevody medzi sustavami
//
// autor: M.Kontra (xkontr00@stud.fit.vutbr.cz)


// navratove hodnoty funkcii
#define RET_OK  0
#define RET_HLP  1
#define RET_OWF  2
#define RET_RNG  3
#define RET_ARG  4
#define RET_ALK  5
#define RET_INP  6
#define RET_ERR  7

typedef struct parametre
{
  unsigned int zaklad;		//zaklad sustavy do ktorej budeme prevadzat

  unsigned int zarovnanie;	// pocet miest zarovnania

} Params, *PParams;


//definicie funkcii

int nacti_vstup (char **riadok);
int spracuj_parametre (PParams p, int count, char **vector);
int spracuj_vstup (char *str, unsigned long long *value);
int checkRange (char c, int base);
int retnallong (char *str, int base, unsigned long long *value);
int retnaint (char *str, int base, unsigned int *value);
int dlzka_retazca (char *str);
int otoc_retazec (char *str);

//funkcia nacti_vstup prime ako parameter ukazatel (char *) a alokuje a nacita donho riadok textu zo  stdin

int
nacti_vstup (char **riadok)
{
  *riadok = NULL;
  int c;
  int i = 0;
  c = getchar ();
  if ((c == '\n') || (c == EOF))
    {
      return RET_INP;
    }
  else
    {
      ungetc (c, stdin);
    }
  while (((c = getchar ()) != EOF) && (c != '\n'))
    {
      if ((i % 16) == 0)
	{
	  *riadok = realloc (*riadok, i + 16);
	  if (*riadok == NULL)
	    {
	      return RET_ALK;
	    }
	  *(*(riadok) + i) = c;
	}
      else
	{
	  *(*(riadok) + i) = c;
	};
      i++;
    }


  *(*(riadok) + i) = '\0';
  *riadok = realloc (*riadok, i);
  return RET_OK;
}

//spracuje retazec zo vstupu 

int
spracuj_vstup (char *str, unsigned long long *cislo)
{
  int ret;
  unsigned int base;
  unsigned long long num;
  ret = retnaint (str, 10, &base);
  if (ret != RET_OK)
    {
      return RET_INP;
    }



  while ((*str != '\0') && (*str == ' '))
    str++;
  while (isalnum (*str++));
  str--;



  ret = retnallong (str, base, &num);
  if (ret != RET_OK)
    {
      return RET_INP;
    }
  *cislo = num;



  while ((*str != '\0') && (*str == ' '))
    str++;
  while (isalnum (*str++));
  while ((*str == '\n') || (*str == ' '))
    str++;


  if (*str != '\0')
    {
      return RET_INP;
    }



  return RET_OK;
}

// skontroluje ci dany znak patri do danej sustavy

int
checkRange (char c, int base)
{
  if ((base > 36) || (base < 2))
    {
      return RET_ARG;
    }
  if ((isdigit (c)) && ((c - '0') < base))
    return RET_OK;
  if ((toupper (c) >= 'A') && (toupper (c) <= 'Z')
      && (((toupper (c) - 'A') + 10) < base))
    return RET_OK;

  return RET_RNG;
}

// retazec na cislo

int
retnaint (char *str, int base, unsigned int *value)
{
  unsigned long long i;
  unsigned int ret;
  ret = retnallong (str, base, &i);
  *value = (unsigned int) i;
  return ret;
}

//funkcia prevedie retazec na vstupe na cislo unsigned long long

int
retnallong (char *str, int base, unsigned long long *value)
{
  unsigned long long i = 0;
  unsigned long long owf = i;
  unsigned int cishod;
  unsigned long long maxcis = ULLONG_MAX / base;

  while (isspace (*str++));
  str--;
  if (*str == '\0')
    {
      return RET_ARG;
    }

//      if(*(--str) == '-') {sig*=-1;}

  while ((*str != '\0') && (*str != ' '))
    {
      if (checkRange (*str, base))
	{
	  return RET_RNG;
	}
      owf = i;
      if (i > maxcis)
	{
	  return RET_OWF;
	}
      if (isdigit (*str))
	{
	  cishod = (*str - '0');
	}
      else
	{
	  cishod = ((toupper (*str) - 'A') + 10);
	}

      i = i * base + cishod;

      if (owf > i)
	{
	  return RET_OWF;
	}


      str++;
    }

  *value = i;

  return RET_OK;
}

// vypise pomoc

int
printHelp (int code)
{
  switch (code)
    {
    case 0:
      printf ("Vsetko OK\n");
      break;
    case 1:
      printf ("Napoveda :\n");
      break;
    case 2:
      printf ("doslo k preteceniu cisla");
      break;
    case 3:
      printf ("Sustava alebo znak mimo rozsah rozsahu\n");
      break;
    case 4:
      printf ("chyba argumentov prikazovej riadky\n");
      break;
    case 5:
      printf ("chyba pri alokacii pamate\n");
      break;
    case 6:
      printf ("chyba pri zadavani vstupu\n");
      break;
    case 7:
      printf ("Nespecificka chyba.\n");
      break;


    }

printf
    ("\nproj1 [-h] cielovy_zaklad [zarovnanie]\n-h - Program vypise help(toto okno)\ncileovy zaklad  - cislo 2-36 urcujuce cielovu sustavu\n\ninden");
  printf ("zarovnanie -  volitelne zarovnanie nulami\n\n");
  printf ("Po spusteni program poziada o 2 cisla v desiatkovej sustave\n");
  printf
    ("Prve urcuje zaklad sustavy cisla a druhe  je samotne cislo.\nPre zadanie pouzivame cisla 0-9 a pismena a-z,A-Z,not case sensitive\n");

return RET_OK;

}

// spracuje parametre prikazovej riadky

int
spracuj_parametre (PParams p, int count, char **args)
{
  if ((count < 2) || (count > 3))
    {
      return RET_ARG;
    }
  if ((count == 2) && (*args[1] == '-') && (*(args[1] + 1) == 'h'))
    {
      printHelp (RET_HLP);
      exit (0);
    }
  else if (count == 2 && *args[1] != '-')
    {
      if (retnaint (args[1], 10, &p->zaklad) != RET_OK)
	{
	  return RET_ARG;
	}
      else
	return RET_OK;
    }
  else if (count == 3)
    {
      if ((retnaint (args[1], 10, &p->zaklad) != RET_OK)
	  || (retnaint (args[2], 10, &p->zaklad) != RET_OK))
	{
	  return RET_ARG;
	}
      else
	return RET_OK;
    }
  return RET_OK;
}

//skontroluje ci parametre su v danom rozsahu

int
kontrola_hodnot (PParams p)
{
  if ((p->zaklad > 36) || (p->zaklad < 2))
    {
      return RET_RNG;
    }
  return RET_OK;
}

// vytlaci cislo v zadanej sustave

int
vytlac_cislo (unsigned long long cislo, int zaklad, int zarovnanie)
{
  unsigned int c;
  unsigned int i = 0;
  char *outstr = (char *) malloc (sizeof (char));
  while (cislo > 0)
    {
      *(outstr + i) = ((c = (cislo % zaklad)) < 10) ? c + '0' : c - 10 + 'A'; 
      cislo /= zaklad;
      if (i % 16 == 0)
	{
	  outstr = realloc (outstr, (i + 1 + 16) * sizeof (char));
	}
      i++;
    }
  if (zarovnanie > i)
    {
      outstr = realloc (outstr, (zarovnanie + 1) * sizeof (char));
      for (; i <= zarovnanie; i++)
	*(outstr + i) = '0';
      *(outstr + i - 1) = '\0';
    }
  else
    {
      *(outstr + i) = '\0';
    }
  otoc_retazec (outstr);
  for (i = 0; *(outstr + i) != '\0'; i++)
    putchar (*(outstr + i));
  free (outstr);
  return RET_OK;
}

//vrati dlzku retazca

int
dlzka_retazca (char *str)
{
  char *c = str;
  while (*str++);
  return str - c - 1;
}

// otoci retazec

int
otoc_retazec (char *str)
{
  char c;
  int i, j;
  for (i = 0, j = dlzka_retazca (str) - 1; i < j; i++, j--)
    {
      c = *(str + i);
      *(str + i) = *(str + j);
      *(str + j) = c;
    }



  return RET_OK;
}



int
main (int argc, char *args[])
{

  unsigned int zaklad = 0;	//zaklad sustavy do ktorej budeme prevadzat

  unsigned int zarovnanie = 0;	// pocet miest zarovnania

  unsigned long long cislo = 0;	//cislo zo vstupu v 10-ej sustave

  char *vstup = NULL;		// Riadok zo vstupu

  int code;			// Navraovy kod

  Params p = { zaklad = 0, zarovnanie = 0 };	//Struktura paramerov

  if ((code = spracuj_parametre (&p, argc, args)) != RET_OK)
    {
      printHelp (code);
      return EXIT_FAILURE;
    }

  if ((code = kontrola_hodnot (&p)) != RET_OK)
    {
      printHelp (code);
      return EXIT_FAILURE;
    }

  if ((code = nacti_vstup (&vstup)) != RET_OK)
    {
      printHelp (code);
      free (vstup);
      return EXIT_FAILURE;
    }

  if ((code = spracuj_vstup (vstup, &cislo)) != RET_OK)
    {
      printHelp (code);
      free (vstup);
      return EXIT_FAILURE;
    }

  free (vstup);
  vytlac_cislo (p.zaklad, 10, 0);
  putchar (' ');


  if ((code = vytlac_cislo (cislo, p.zaklad, p.zarovnanie)) != RET_OK)
    {
      printHelp (code);
      free (vstup);
      return EXIT_FAILURE;
    }

putchar('\n');


  return EXIT_SUCCESS;
}
