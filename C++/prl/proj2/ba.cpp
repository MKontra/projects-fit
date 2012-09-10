#include <cstdio>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include "math.h"

using namespace std;

#define TAG 0

#define _p 0
#define _g 1
#define _s 2

//operator bodka v kruzku
int
dotproduct (int op1, int op2)
{
  if (op1 == _p)
    return op2;
  else if (op2 == _p)
    return op1;
  else if (op1 == _g)
  {
    return _g;
  }
  else				//op1 == _s
  {
    return _s;
  }
}

int
rev_dotproduct (int op1, int op2)
{
  return dotproduct (op2, op1);
}

char
out_d (int op)
{
  if (op == _p)
    return 'p';
  else if (op == _g)
    return 'g';
  else
    return 's';
}



int
main (int argc, char *argv[])
{
  setbuf (stdout, 0);
  int numprocs;
  int myid;

  int x;
  int y;

  int d;
  int scand;

  MPI_Status stat;
  //inicializacia
  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myid);

  int bitov = (numprocs + 1) / 2;
  int xs[bitov];
  int ys[bitov];

  int dpth = floor (log2 (numprocs + 1));	//hlbka
  int lastinxmino = (2 * (bitov >> 1)) - 2 > 0 ? (2 * (bitov >> 1)) - 2 : 0;	//index posledneho procesora stomu o jeden nizsieho
  //nacitanie vstupu
  if (myid == 0)
  {
    //cout << "bitov: "<< bitov << " " << "depth: "<<dpth<<endl;
    char input[] = "numbers";
    char number;
    int invar = 0;
    fstream fin;
    fin.open (input, ios::in);

    for (int i = 0; i < (2 * bitov); i++)
    {
      fin >> number;
      if (number == '1')
	number = 1;
      else
	number = 0;
      if (!fin.good ())
      {
	cout << "error reading input\n";
	return -1;
      };
      if (i < bitov)
	xs[i] = number;
      else
	ys[i - bitov] = number;
    }

    //rozposlanie
    int i, dst;
    for (i = 0, dst = numprocs - 1; i < bitov; i++, dst--)
    {
      MPI_Send (&ys[i], 1, MPI_INT, dst, TAG, MPI_COMM_WORLD);
      MPI_Send (&xs[i], 1, MPI_INT, dst, TAG, MPI_COMM_WORLD);
    }
  }

  scand = d = _p;		//vsetci okrem prvkou ktore nesu hodnoty si nastavia propagate

  if (myid < numprocs && myid > (2 * (bitov >> 1)) - 2)
  {
    MPI_Recv (&y, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
    MPI_Recv (&x, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);

    //vypocitaj d
    if (x == 1 && y == 1)
    {
      d = _g;
    }
    else if (x == 0 && y == 0)
    {
      d = _s;
    }
    else
    {
      d = _p;
    }
    scand = d;
    //cout << "received x "<<x<< " y " << y << " d "<< out_d(d) << " rank: " << myid <<endl;

  }

  //scan s dotproduct
  //A UPSWEEP   
  int lchildval;		//uchovanie si hodnoty laveho syna, znizuje pocet sendov a receivov
  int rchildval;
  if (myid > (2 * (bitov >> 1)) - 2)	//posledny riadok nepracuje s detmin
  {
    MPI_Send (&d, 1, MPI_INT, (myid - 1) / 2, TAG, MPI_COMM_WORLD);
  }
  else if (myid == 0)		//root
  {
    MPI_Recv (&lchildval, 1, MPI_INT, 2, TAG, MPI_COMM_WORLD, &stat);
    MPI_Recv (&rchildval, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD, &stat);
    scand = dotproduct (lchildval, rchildval);
    //cout << "rank: "<< myid <<" step: " << out_d(scand) << " input: "<< out_d(rchildval) << " input: "<< out_d(lchildval) <<endl;

  }
  else				//zvysok stromu
  {
    MPI_Recv (&rchildval, 1, MPI_INT, 2 * myid + 1, TAG, MPI_COMM_WORLD,
	      &stat);
    MPI_Recv (&lchildval, 1, MPI_INT, 2 * myid + 2, TAG, MPI_COMM_WORLD,
	      &stat);
    scand = dotproduct (lchildval, rchildval);
    //cout << "rank: "<< myid <<" step: " << out_d(scand) << " input: "<< out_d(rchildval) << " input: "<< out_d(lchildval) <<endl;
    MPI_Send (&scand, 1, MPI_INT, (myid - 1) / 2, TAG, MPI_COMM_WORLD);
  }

  MPI_Barrier (MPI_COMM_WORLD);

  if (myid == 0)
  {
    d = scand;
  };				//uloz overflow

  //B DOWNSWEEP
  if (myid > (2 * (bitov >> 1)) - 2)	//posledny riadok nepracuje s detmin
  {
    MPI_Recv (&scand, 1, MPI_INT, (myid - 1) / 2, TAG, MPI_COMM_WORLD, &stat);
  }
  else if (myid == 0)
  {
    scand = _p;
    MPI_Send (&scand, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
    rchildval = dotproduct (rchildval, scand);
    MPI_Send (&rchildval, 1, MPI_INT, 2, TAG, MPI_COMM_WORLD);
  }
  else				//zvysok stromu
  {

    MPI_Recv (&scand, 1, MPI_INT, (myid - 1) / 2, TAG, MPI_COMM_WORLD, &stat);

    MPI_Send (&scand, 1, MPI_INT, myid * 2 + 1, TAG, MPI_COMM_WORLD);
    rchildval = dotproduct (rchildval, scand);
    MPI_Send (&rchildval, 1, MPI_INT, myid * 2 + 2, TAG, MPI_COMM_WORLD);

  }

  if (myid < numprocs && myid > (2 * (bitov >> 1)) - 2)
  {
    if ((x == 1) xor (y == 1) xor (scand == _g))
    {
      printf ("%d:%d\n", myid == 0 ? 0 : numprocs - myid + lastinxmino, 1);
    }
    else
    {
      printf ("%d:%d\n", myid == 0 ? 0 : numprocs - myid + lastinxmino, 0);
    }
  }

  MPI_Barrier (MPI_COMM_WORLD);
  if (myid == 0)
  {
    if (d == _g)
      printf ("overflow\n");
  };
  MPI_Finalize ();
}
