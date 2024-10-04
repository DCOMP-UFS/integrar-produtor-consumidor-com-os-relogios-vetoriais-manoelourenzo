#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <processes.h>


int main(int argc, char* argv[]) 
{
  int my_rank;               

  MPI_Init(NULL, NULL); 
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

  if (my_rank == 0) { 
    process0(my_rank);
  } else if (my_rank == 1) {  
    process1(my_rank);
  } else if (my_rank == 2) {  
    process2(my_rank);
  }

  MPI_Finalize(); 

  return 0; 
}
