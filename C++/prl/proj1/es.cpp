#include <cstdio>
#include <iostream>
#include <fstream>
#include <mpi.h>

using namespace std;

#define TAG 0

int main(int argc, char *argv[] )
{
    //setbuf(stdout, 0);
    int numprocs;
    int myid;
    
    int hasx = 0;
    int x;
    int hasy = 0;
    int y;

    int zcount = 0;
    int zrecv = 0;
    int z;
    int newc;
    int c = 1;
    
    MPI_Status stat;
    //inicializacia
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    numprocs = numprocs - 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    int * nums;
    int * numssorted;
    //nacitanie vstupnych dat
    if (myid == 0)
    {
	nums = new int[numprocs];
	numssorted = new int[numprocs];
	char input[] = "numbers";
	int number;
	int invar = 0;
	fstream fin;
	fin.open(input, ios::in);
	
	while(fin.good())
	{
            number = fin.get();
	    if (!fin.good()) break;
	    nums[invar] = number;
	    cout << nums[invar] << " ";
	    invar++;
	}
        cout << endl;


    }
    
    //double starttime, endtime;
    //starttime = MPI_Wtime();
    
    for ( int k = 1; k <= 2*numprocs; k++ )
    {

	int h = 1;
	if ( k > numprocs ) h = k - numprocs;
        //POROVNANIE
	if ( (myid >= h) && hasx && hasy)
	{
	    if ( x > y ) c = c + 1;
	}

        //POSUV

	if ( (myid >= h) && (myid <= min(numprocs - 1, k)) && hasy )
	{
	  MPI_Send(&y, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);
	}
	if ( (myid >= h + 1) && ( myid <= min(numprocs, k) ) )
	{
          MPI_Recv(&y, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat);
          hasy = 1;
	} 

        //MASTER: NOVE DATA
	if ( (myid == 0) && (k <= numprocs) )
	{
	    MPI_Send(&nums[k-1], 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
	    MPI_Send(&nums[k-1], 1, MPI_INT, k, TAG, MPI_COMM_WORLD);
	}
	if ( (myid == 1) && ( k <= numprocs ) )
	{
          MPI_Recv(&y, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
          hasy = 1;
	}
	if ( (myid == k) && ( k <= numprocs ) )
	{
          MPI_Recv(&x, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
          hasx = 1;
	}

        // ALEBO RADENIE PODLA C
        //  KTO MA CAKAT HODNOTU
        if ( (k > numprocs))
        {
          MPI_Bcast( ( h == myid ) ? &c : &zrecv,1, MPI_INT, h, MPI_COMM_WORLD);
          if ( h == myid ) zrecv = c;
        }
	//ZASLI ZCOUNT, POCET VYSKYTOV DANEHO PRVKU NA POZICII
        if ( ( k > numprocs ) && ( zrecv == myid ) )
        {
          MPI_Send(&zcount,1, MPI_INT, h, TAG, MPI_COMM_WORLD);
          zcount++;
        }
        //ODOSIELATEL VYPOCITA NOVEHO PRIJEMCU
        if ( ( k > numprocs ) && ( h == myid) )
        {
          MPI_Recv(&newc, 1, MPI_INT, zrecv, TAG, MPI_COMM_WORLD, &stat);
          newc = newc + c;
        }
        //REBROADCAST RANKU PRIJEMCU
        if ( k > numprocs )
        {
          MPI_Bcast( ( h == myid ) ? &newc : &zrecv,1, MPI_INT, h, MPI_COMM_WORLD);
          if ( h == myid ) zrecv = newc;
        }
        
        //ODOSLANIE PRVKU        
        if ( ( k > numprocs ) && ( h == myid ) )
        {
          MPI_Send(&x,1, MPI_INT, newc, TAG, MPI_COMM_WORLD);
        }
        //PRIJATIE PRVKU
        if ( ( k > numprocs ) && ( zrecv == myid) )
        {
          MPI_Recv(&z, 1, MPI_INT, h, TAG, MPI_COMM_WORLD, &stat);
        }        

    }
    
    for ( int k = 1; k <= numprocs; k++ )
    {
      MPI_Send(&z, 1, MPI_INT, (myid+1) % (numprocs+1), TAG, MPI_COMM_WORLD);
      if ( myid == 0 )
      { 
        MPI_Recv(&numssorted[k-1], 1, MPI_INT, numprocs, TAG, MPI_COMM_WORLD, &stat);
      } else
        if ( (myid > 1) && (myid<= numprocs) )
      {
        MPI_Recv(&z, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat);
      } 
    }
    
    //endtime = MPI_Wtime();
    //if ( myid != 0 ) printf("Rank %d took %f seconds\n", myid, endtime-starttime);
    
    if (myid == 0 )
    {
	for ( int i = numprocs; i > 0 ; i-- )
	    cout << numssorted[i-1] << endl;
        delete nums;
    }
    MPI_Finalize();
}
