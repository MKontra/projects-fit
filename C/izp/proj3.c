#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

//Navratove hodnoty

#define RET_OK  0		//all ok
#define RET_FILE  1		//chyba pri otvarani suboru
#define RET_ARGS  2		//chyba parametrov prikazoveho riadku
#define RET_ALK  3		//chyba pri alokacii
#define RET_INP  4		//chybny vstup alebo format dat v subore
#define RET_ERR  5		//nespecifikovana chyba

#define SUDOKU 10

typedef struct matrix		//struktura matice
{
  int n;			//row
  int m;			//col
  int **add;			//ukazatel na pole
} TMatrix, *p_TMatrix;

typedef struct params		//struktura parametrov
{
  int function;			//kod funkcie
  TMatrix mat1;			//matica 1
  TMatrix mat2;			//matica 2
} TParams, *p_TParams;

//Definicie

int freeMat (p_TMatrix ptm);
int allocMat (p_TMatrix ptm);
int vypisMat (p_TMatrix ptm);
int readandallocMat (FILE * fp, p_TMatrix ptm);

int
allocMat (p_TMatrix ptm)
{
  int i;

  /*najskor sa pole alokuje NULL mi, ak zlyha alokovanie niektoreho z
     mozeme to vyuzit na zistenie ci uz bol dany riadok alokovany
     (nakolko by obsahoval neinicializovanu hodnotu) */

  if ((ptm->add = calloc (ptm->n, sizeof (int *))) == NULL)	//alokujeme pole ukazatelov
    return RET_ALK;
  for (i = 0; i < ptm->n; i++)	//alokujeme samotne riadky
    if ((*(ptm->add + i) = (int *) malloc (ptm->m * sizeof (int))) == NULL)
      {
	freeMat (ptm);		//ak niekde nastala chyba tak odalokujem co sme naalokovali
	return RET_ALK;
      }
  return RET_OK;
}

int
freeMat (p_TMatrix ptm)
{
  int i;

  //Uvolnime riadky

  if (ptm->add == NULL)
    return RET_ALK;
  for (i = 0; i < ptm->n; i++)
    if (*(ptm->add + i) != NULL)
      free (*(ptm->add + i));

  //Potom pole ukazatelov na ne

  if (ptm->add != NULL)
    {
      free (ptm->add);
      ptm->add = NULL;
    }

  return RET_OK;

}

int
zeroMat (p_TMatrix ptm)
{
  int i, j;

  //vynuluje maticu

  for (i = 0; i < ptm->n; i++)
    for (j = 0; j < ptm->m; j++)
      ptm->add[i][j] = 0;
  return RET_OK;
}


int
readandallocMat (FILE * fp, p_TMatrix ptm)
{
  int i, j;
  int code;
  int c;
  int already = 0;

  //nacitame rozmery
  if ((fscanf (fp, "%d %d", &i, &j)) != 2)
    return RET_INP;

  //ak sa za nimy nachadza nieco ine ako novy riadok vrat chybu
  while ((c = fgetc (fp)) != '\n')
    if (c != ' ' && c != '\t')
      return RET_INP;

  //chybne rozmery
  if (i <= 0 || j <= 0)
    {
      return RET_INP;
    }


  ptm->n = i;
  ptm->m = j;

  //alokuj maticu
  if ((code = allocMat (ptm)) != RET_OK)
    return code;


  //nacitaj hodnoty do pola
  for (i = 0; i < ptm->n; i++)
    {
      for (j = 0; j < ptm->m; j++)
	{
	  if (((code = fscanf (fp, "%d", &(ptm->add[i][j]))) == EOF)
	      || (code != 1))
	    return RET_FILE;

	}


      //kontrola konca riadku matice
      if ((c = fgetc (fp)) != '\n')
	{
	  if (c == EOF && i == ptm->n - 1)
	    return RET_OK;
	  fscanf (fp, "%*[^\n]");
	  if (already != 1)
	    {
	      fprintf (stderr,
		       "Varovanie: Vstupna matica obsahuje viac hodnot nez kolko je uvedene v rozsahu\n");
	      already = 1;
	    }
	}

    }
  return RET_OK;
}


//sucet matic
int
sucet (p_TMatrix ptm1, p_TMatrix ptm2)
{
  int code, i = 0, j = 0;

  if (ptm1->n != ptm2->n || ptm1->m != ptm2->m)
    return RET_INP;

  TMatrix tmp;
  tmp.n = ptm1->n;
  tmp.m = ptm1->m;
  if ((code = allocMat (&tmp)) != RET_OK)
    return code;

  for (i = 0; i < ptm1->n; i++)
    for (j = 0; j < ptm1->m; j++)
      tmp.add[i][j] = ptm1->add[i][j] + ptm2->add[i][j];
  vypisMat (&tmp);
  freeMat (&tmp);
  return RET_OK;
}

//sucin
int
sucin (p_TMatrix ptm1, p_TMatrix ptm2)
{
  int code, i = 0, j = 0, k = 0;

  if (ptm1->m != ptm2->n)
    return RET_INP;

  TMatrix tmp;
  tmp.n = ptm1->n;
  tmp.m = ptm2->m;

  if ((code = allocMat (&tmp)) != RET_OK)
    return code;

  zeroMat (&tmp);


  for (i = 0; i < tmp.n; i++)
    for (j = 0; j < tmp.m; j++)
      for (k = 0; k < ptm1->m; k++)
	tmp.add[i][j] += (ptm1->add[i][k] * ptm2->add[k][j]);


  vypisMat (&tmp);
  freeMat (&tmp);
  return RET_OK;
}

//podmatica
int
submatrix (p_TMatrix ptm1, p_TMatrix ptm2)
{
  int i = 0, j = 0, k = 0, l = 0, isSub = 0;

  //prechadza kazdy index vstupnej matice a hlada zadanu podmaticu
  //ak su rozmery matice 2 vacsie ako matice 1 cyklus sa nevykona ani raz
  for (i = 0; i < ptm1->n - ptm2->n + 1; i++)
    for (j = 0; j < ptm1->m - ptm2->m + 1; j++)
      {
	isSub = 1;
	for (k = 0; k < ptm2->n; k++)
	  for (l = 0; l < ptm2->m; l++)
	    if (ptm1->add[i + k][j + l] != ptm2->add[k][l])
	      isSub = 0;
	if (isSub == 1)
	  {
	    printf ("true\n");
	    return RET_OK;
	  }
      }
  printf ("false");
  return RET_OK;
}


//krizova rotacia
int
crot (p_TMatrix ptm1)
{
  int code, i = 0, j = 0;
  int l;


  TMatrix tmp1;
  tmp1.n = ptm1->n;
  tmp1.m = ptm1->m;

  //pomocna matica 1
  if ((code = allocMat (&tmp1)) != RET_OK)
    return code;
  zeroMat (&tmp1);


  //spravnim indexovanim naplnime pomocnu maticu posunutymi riadkami
  for (i = 0; i < tmp1.n; i++)
    {
      l = ptm1->add[i][0];
      l = l % (ptm1->m);
      for (j = 0; j < ptm1->m; j++)
	tmp1.add[i][j] = ptm1->add[i][(j - l + ptm1->m) % (ptm1->m)];
    }


  //podobne posunieme stlpce
  TMatrix tmp2;
  tmp2.n = ptm1->n;
  tmp2.m = ptm1->m;
  if ((code = allocMat (&tmp2)) != RET_OK)
    return code;
  zeroMat (&tmp2);

  for (i = 0; i < tmp1.m; i++)
    {
      l = tmp1.add[0][i];
      l = l % (ptm1->n);
      for (j = 0; j < ptm1->n; j++)
	tmp2.add[j][i] = tmp1.add[(j - l + ptm1->n) % (ptm1->n)][i];
    }
  vypisMat (&tmp2);
  freeMat (&tmp1);
  freeMat (&tmp2);
  return RET_OK;
}

//Orba

//definicia smerov posuvania
enum smery
{
  start = 5,
  up_right = 3,
  right = 6,
  down = 8,
  down_left = 7
};

int
plough (p_TMatrix ptm1)
{

  int code = 0, i = 0, j = 0, k = 0, l = 0;

  TMatrix tmp;
  tmp.n = ptm1->n;
  tmp.m = ptm1->m;
//Alokacia pomocnej matice
  if ((code = allocMat (&tmp)) != RET_OK)
    return code;
  zeroMat (&tmp);

  code = start;

  /* v zdrojovej matici sa pohybujeme premennymi k,l
     v kazdej iteracii zistujeme ci nedoslo k zmene podmienky daneho stavu
     ak ano prepneme na novy smer */

  for (i = 0; i < ptm1->n; i++)
    for (j = 0; j < ptm1->m; j++)
      if (!((k == ptm1->n - 1) && (l == ptm1->m - 1)))
	{
	  switch (code)
	    {
	    case start:
	      tmp.add[i][j] = ptm1->add[k][l];
	      code = right;
	      break;

	    case right:
	      l++;
	      tmp.add[i][j] = ptm1->add[k][l];
	      if (k == 0)
		code = down_left;
	      if (k == ptm1->n - 1)
		code = up_right;
	      break;

	    case down_left:
	      k++;
	      l--;
	      tmp.add[i][j] = ptm1->add[k][l];
	      if ((k == ptm1->n - 1))
		code = right;
	      else if ((l == 0) && (k != ptm1->n - 1))
		code = down;
	      else
		code = down_left;
	      break;

	    case down:
	      k++;
	      tmp.add[i][j] = ptm1->add[k][l];
	      if (l == 0)
		code = up_right;
	      if (l == ptm1->m - 1)
		code = down_left;
	      break;

	    case up_right:
	      k--;
	      l++;
	      tmp.add[i][j] = ptm1->add[k][l];
	      if ((l == ptm1->m - 1))
		code = down;
	      else if ((k == 0) && (k != ptm1->m - 1))
		code = right;
	      else
		code = up_right;
	      break;

	    }
	}
  vypisMat (&tmp);
  freeMat (&tmp);
  return RET_OK;
}

//pomocna funkcia
//sluzi na vynulovanie vektora(pola) cisel
//vyuzita pri sudoku
int
memcln (int *field, int length)
{
  int i;
  for (i = 0; i < length; i++)
    field[i] = 0;
  return RET_OK;
}

int
sudoku (p_TMatrix ptm1)
{
  int i, j, k, l;
  int nbrs[SUDOKU];		//bitove pole


  if (ptm1->n != 9 || ptm1->m != 9)
    {
      printf ("false\n");	//kontrola rozmerov
      return RET_OK;
    }

  //riadky
  for (i = 0; i < 9; i++)
    {
      memcln (nbrs, SUDOKU);
      for (j = 0; j < 9; j++)
	if (nbrs[ptm1->add[i][j]] > 0 && ptm1->add[i][j] < 10)
	  {
	    printf ("false\n");
	    return RET_OK;
	  }
	else
	  nbrs[ptm1->add[i][j]]++;
    }

  //stlpce
  for (i = 0; i < 9; i++)
    {
      memcln (nbrs, SUDOKU);
      for (j = 0; j < 9; j++)
	if (nbrs[ptm1->add[j][i]] > 0 && ptm1->add[i][j] < 10)
	  {
	    printf ("false\n");
	    return RET_OK;
	  }
	else
	  nbrs[ptm1->add[j][i]]++;
    }

  //stvorce 3x3
  // vonkajsie cykli posuvaju o 3
  for (i = 0; i < 9; i += 3)
    for (j = 0; j < 9; j += 3)
      {
	//vnutorne po jednom prechadzaju kazdym prvkom
	memcln (nbrs, SUDOKU);
	for (k = i; k < i + 3; k++)
	  for (l = j; l < j + 3; l++)
	    if (nbrs[ptm1->add[k][l]] > 0 && ptm1->add[i][j] < 10)
	      {
		printf ("false\n");
		return RET_OK;
	      }
	    else
	      nbrs[ptm1->add[k][l]]++;
      }

  printf ("true\n");
  return RET_OK;
  return RET_OK;
}

int
errmsg (int code)
{

  switch (code)
    {
    case 0:
      break;
    case 1:
      printf
	("Chyba pri otvarani suboru. Subor neexistuje alebo je poskodeny\n");
      break;
    case 2:
      printf ("Nespravny pocet parametrov alebo nespravny tvar\n");
      break;
    case 3:
      printf
	("Chyba pri alokacii pamete. Pravdepodobne nerealne rozmery matice\n");
      break;
    case 4:
      printf
	("Nespravny tvar dat vo vstupnom subore. Malo vstupnych dat zle zadane rozmery\n");
      break;
    case 5:
      printf ("Nespecifikovana chyba\n");
      break;
    }
  printf ("\nProgram na vykonavanie elementarnych operaacii nad maticami\n\n"
	 "pouzitie: proj3 [-h|-sucet|-sucin|-crot|-plough|-sudoku|-submatrix] subor1 [subor2]\n\n"
	 "parametre:\n\n"
	 "-h = vypise napovedu a skonci\n"
	 "-sucet = pozaduje 2 vstupne subory scita, matice\n"
	 "-sucin = ako sucet lenze nasobi\n"
	 "-crot = krizova rotacia\n"
	"-plough = tzv. orba\n"
	 "-sudoku = zistuje platnost riesenia sudoku 9x9\n"
	 "-submatrix = funkcia podmatice");
  return code;
}

int
vypisMat (p_TMatrix ptm)
{
  int i, j;
  printf ("%d %d\n", ptm->n, ptm->m);
  for (i = 0; i < ptm->n; i++)
    {
      for (j = 0; j < ptm->m; j++)
	printf ("%d ", ptm->add[i][j]);
      printf ("\n");
    }
  return RET_OK;
}

int
processParams (p_TParams ptp, int count, char **vector)
{
  int code;
  FILE *fp = NULL;
  switch (count)
    {

    case 1:return RET_ARGS;
    case 2:
      if (strcmp (vector[1], "-h") == 0)
      {ptp->function = 0;
      return RET_OK;}
      return RET_ARGS;

    case 3:
      if (strcmp (vector[1], "-crot") == 0)
	ptp->function = 4;
      else if (strcmp (vector[1], "-plough") == 0)
	ptp->function = 5;
      else if (strcmp (vector[1], "-sudoku") == 0)
	ptp->function = 6;
      else
	return RET_ARGS;

      fp = fopen (vector[2], "rb");
      if (fp == NULL)
	return RET_FILE;
      if ((code = readandallocMat (fp, &(ptp->mat1))) != RET_OK)
	{

	  if (ptp->mat1.add != NULL)
	    {
	      freeMat (&(ptp->mat1));
	    }
	  fclose (fp);
	  return code;
	}
      fclose (fp);
      (ptp->mat2).add = NULL;


      return RET_OK;

    case 4:
      if (strcmp (vector[1], "-add") == 0)
	ptp->function = 1;
      else if (strcmp (vector[1], "-mult") == 0)
	ptp->function = 2;
      else if (strcmp (vector[1], "-submatrix") == 0)
	ptp->function = 3;
      else
	return RET_ARGS;

      fp = fopen (vector[2], "rb");
      if (fp == NULL)
	return RET_FILE;
      if ((code = readandallocMat (fp, &(ptp->mat1))) != RET_OK)
	{
	  if (ptp->mat1.add != NULL)
	    {
	      freeMat (&(ptp->mat1));
	    }
	  fclose (fp);
	  return code;
	}
      fclose (fp);

      fp = fopen (vector[3], "rb");
      if (fp == NULL)
	return RET_FILE;
      if ((code = readandallocMat (fp, &(ptp->mat2))) != RET_OK)
	{
	  if (ptp->mat2.add != NULL)
	    {
	      freeMat (&(ptp->mat2));
	    }
	  fclose (fp);
	  return code;
	}
      fclose (fp);

      return RET_OK;

    default:
      return RET_ARGS;

    }

  return RET_ERR;


}

int
main (int argc, char *argv[])
{
  int code;
  TParams tp = {.mat1.add = NULL,.mat2.add = NULL };
  if ((code = processParams (&tp, argc, argv)) != RET_OK)
    {
      errmsg (code);
      return EXIT_FAILURE;
    };
  switch (tp.function)
    {
    case 0:
      errmsg (RET_OK);
      break;
    case 1:
      code = sucet (&(tp.mat1), &(tp.mat2));
      break;
    case 2:
      code = sucin (&(tp.mat1), &(tp.mat2));
      break;
    case 3:
      code = submatrix (&(tp.mat1), &(tp.mat2));
      break;
    case 4:
      code = crot (&(tp.mat1));
      break;
    case 5:
      code = plough (&(tp.mat1));
      break;
    case 6:
      code = sudoku (&(tp.mat1));
      break;
    default:
      return EXIT_FAILURE;
    }

  if (tp.mat1.add != NULL)
    {
      freeMat (&(tp.mat1));
    }


  if (tp.mat2.add != NULL)
    {
      freeMat (&(tp.mat2));
    }


  if (code != RET_OK)
    {
      errmsg (code);
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
