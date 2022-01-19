#include <iostream>
#include <fstream>
#include "mpi.h"
#include <stdio.h>
#include <list>
#include <stdlib.h>
#include <string>
#include <map>

using namespace std;

void printTopology(int rank, int sizeOfWorkersFor0, int sizeOfWorkersFor1, int sizeOfWorkersFor2, int workersFor0[], int workersFor1[], int workersFor2[])
{
  cout << rank << " -> "
       << "0:";
  for (int i = 0; i < sizeOfWorkersFor0 - 1; i++)
  {
    cout << workersFor0[i] << ',';
  }
  cout << workersFor0[sizeOfWorkersFor0 - 1];
  cout << " 1:";
  for (int i = 0; i < sizeOfWorkersFor1 - 1; i++)
  {
    cout << workersFor1[i] << ',';
  }
  cout << workersFor1[sizeOfWorkersFor1 - 1];
  cout << " 2:";
  for (int i = 0; i < sizeOfWorkersFor2 - 1; i++)
  {
    cout << workersFor2[i] << ',';
  }
  cout << workersFor2[sizeOfWorkersFor2 - 1] << endl;
}

int main(int argc, char *argv[])
{
  int numtasks, rank, len;
  char hostname[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(hostname, &len);
  if (rank == 0)
  {
    int worker;

    ifstream MyReadFile("cluster0.txt");

    MPI_Status status;
    int nrOfWorkers;
    int sizeOfWorkersFor1;
    int sizeOfWorkersFor2;
    MyReadFile >> nrOfWorkers;
    int workers[nrOfWorkers];
    int i = 0;
    int N = stoi(argv[1]);
    int V[N];

    for (int i = 0; i < N; i++)
    {
      V[i] = i;
    }

    // ANNOUNCE WHO IS COORD FOR WORKERS
    while (MyReadFile >> worker)
    {
      workers[i] = worker;
      MPI_Send(&rank, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << worker << ")" << endl;
      i++;
    }

    // SEND TO COORD 1
    MPI_Send(&nrOfWorkers, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 1 << ")" << endl;
    MPI_Send(&workers, nrOfWorkers, MPI_INT, 1, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 1 << ")" << endl;
    // SEND TO COORD 2
    MPI_Send(&nrOfWorkers, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 2 << ")" << endl;
    MPI_Send(&workers, nrOfWorkers, MPI_INT, 2, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 2 << ")" << endl;
    // RECV FROM COORD 1
    MPI_Recv(&sizeOfWorkersFor1, 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersOf1[sizeOfWorkersFor1];
    MPI_Recv(&workersOf1, sizeOfWorkersFor1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // RECV FROM COORD 2
    MPI_Recv(&sizeOfWorkersFor2, 1, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersOf2[sizeOfWorkersFor2];
    MPI_Recv(&workersOf2, sizeOfWorkersFor2, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    int totalWorkers = nrOfWorkers + sizeOfWorkersFor1 + sizeOfWorkersFor2;
    int chunk = N / totalWorkers;
    int rest = N % totalWorkers;
    int chunkWithRemainings = chunk + rest;
    int end = chunk * nrOfWorkers + rest;

    // SEND CHUNKS TO COORD 1
    MPI_Send(&chunk, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 1 << ")" << endl;
    MPI_Send(&V[(chunk * nrOfWorkers) + rest], chunk * sizeOfWorkersFor1, MPI_INT, 1, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 1 << ")" << endl;
    // SEND CHUNKS TO COORD 2
    MPI_Send(&chunk, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 2 << ")" << endl;
    MPI_Send(&V[(chunk * nrOfWorkers) + (chunk * sizeOfWorkersFor1) + rest], chunk * sizeOfWorkersFor2, MPI_INT, 2, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 2 << ")" << endl;

    printTopology(rank, nrOfWorkers, sizeOfWorkersFor1, sizeOfWorkersFor2, workers, workersOf1, workersOf2);

    for (int i = 0; i < nrOfWorkers; i++)
    {
      MPI_Send(&nrOfWorkers, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workers, nrOfWorkers, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&sizeOfWorkersFor1, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workersOf1, sizeOfWorkersFor1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&sizeOfWorkersFor2, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workersOf2, sizeOfWorkersFor2, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      if (i == nrOfWorkers - 1)
      {
        MPI_Send(&chunkWithRemainings, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
        cout << "M(" << rank << "," << workers[i] << ")" << endl;
        MPI_Send(&V[i * chunk], chunkWithRemainings, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
        cout << "M(" << rank << "," << workers[i] << ")" << endl;
      }
      else
      {
        MPI_Send(&chunk, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
        cout << "M(" << rank << "," << workers[i] << ")" << endl;
        MPI_Send(&V[i * chunk], chunk, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
        cout << "M(" << rank << "," << workers[i] << ")" << endl;
      }
    }

    fill_n(V, N, 0);

    for (int i = 0; i < nrOfWorkers; i++)
    {
      if (i == nrOfWorkers - 1)
      {
        MPI_Recv(&V[i * chunk], chunkWithRemainings, MPI_INT, workers[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      }
      else
      {
        MPI_Recv(&V[i * chunk], chunk, MPI_INT, workers[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      }
    }

    MPI_Recv(&V[nrOfWorkers * chunk + rest], chunk * sizeOfWorkersFor1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(&V[nrOfWorkers * chunk + rest + chunk * sizeOfWorkersFor1], chunk * sizeOfWorkersFor2, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    cout << "Rezultat: ";
    for (auto const &i : V)
    {
      cout << i << " ";
    }

    MyReadFile.close();
  }
  else if (rank == 1)
  {
    int worker;

    ifstream MyReadFile("cluster1.txt");

    MPI_Status status;
    int nrOfWorkers;
    int sizeOfWorkersFor0;
    int sizeOfWorkersFor2;
    MyReadFile >> nrOfWorkers;
    int workers[nrOfWorkers];
    int i = 0;
    int chunk;
    // ANNOUNCE WHO IS COORD FOR WORKERS
    while (MyReadFile >> worker)
    {
      workers[i] = worker;
      MPI_Send(&rank, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << worker << ")" << endl;
      i++;
    }

    // RECV FROM COORD 0
    MPI_Recv(&sizeOfWorkersFor0, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersOf0[sizeOfWorkersFor0];
    MPI_Recv(&workersOf0, sizeOfWorkersFor0, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // SEND TO COORD 0
    MPI_Send(&nrOfWorkers, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 0 << ")" << endl;
    MPI_Send(&workers, nrOfWorkers, MPI_INT, 0, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 0 << ")" << endl;
    // SEND TO COORD 2
    MPI_Send(&nrOfWorkers, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 0 << ")" << endl;
    MPI_Send(&workers, nrOfWorkers, MPI_INT, 2, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 2 << ")" << endl;
    // RECV FROM COORD 2
    MPI_Recv(&sizeOfWorkersFor2, 1, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersOf2[sizeOfWorkersFor2];
    MPI_Recv(&workersOf2, sizeOfWorkersFor2, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    printTopology(rank, sizeOfWorkersFor0, nrOfWorkers, sizeOfWorkersFor2, workersOf0, workers, workersOf2);

    // RECV ARRAY FROM COORD 0
    MPI_Recv(&chunk, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int V[chunk * nrOfWorkers];
    MPI_Recv(&V, chunk * nrOfWorkers, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    for (int i = 0; i < nrOfWorkers; i++)
    {
      MPI_Send(&sizeOfWorkersFor0, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workersOf0, sizeOfWorkersFor0, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&nrOfWorkers, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workers, nrOfWorkers, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&sizeOfWorkersFor2, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workersOf2, sizeOfWorkersFor2, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&chunk, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&V[i * chunk], chunk, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
    }

    fill_n(V, chunk * nrOfWorkers, 0);

    for (int i = 0; i < nrOfWorkers; i++)
    {
      MPI_Recv(&V[i * chunk], chunk, MPI_INT, workers[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }

    MPI_Send(&V, chunk * nrOfWorkers, MPI_INT, 0, 2, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 0 << ")" << endl;

    MyReadFile.close();
  }
  else if (rank == 2)
  {
    int worker;

    ifstream MyReadFile("cluster2.txt");

    MPI_Status status;
    int nrOfWorkers;
    int sizeOfWorkersFor0;
    int sizeOfWorkersFor1;
    MyReadFile >> nrOfWorkers;
    int workers[nrOfWorkers];
    int i = 0;
    int chunk;
    // ANNOUNCE WHO IS COORD FOR WORKERS
    while (MyReadFile >> worker)
    {
      workers[i] = worker;
      MPI_Send(&rank, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << worker << ")" << endl;
      i++;
    }

    // RECV FROM COORD 0
    MPI_Recv(&sizeOfWorkersFor0, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersOf0[sizeOfWorkersFor0];
    MPI_Recv(&workersOf0, sizeOfWorkersFor0, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // RECV FROM COORD 1
    MPI_Recv(&sizeOfWorkersFor1, 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersOf1[sizeOfWorkersFor1];
    MPI_Recv(&workersOf1, sizeOfWorkersFor1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    printTopology(rank, sizeOfWorkersFor0, sizeOfWorkersFor1, nrOfWorkers, workersOf0, workersOf1, workers);

    // SEND TO COORD 0
    MPI_Send(&nrOfWorkers, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 0 << ")" << endl;
    MPI_Send(&workers, nrOfWorkers, MPI_INT, 0, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 0 << ")" << endl;
    // SEND TO COORD 1
    MPI_Send(&nrOfWorkers, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 1 << ")" << endl;
    MPI_Send(&workers, nrOfWorkers, MPI_INT, 1, 1, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 1 << ")" << endl;

    // RECV ARRAY FROM COORD 0
    MPI_Recv(&chunk, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int V[chunk * nrOfWorkers];
    MPI_Recv(&V, chunk * nrOfWorkers, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    for (int i = 0; i < nrOfWorkers; i++)
    {
      MPI_Send(&sizeOfWorkersFor0, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workersOf0, sizeOfWorkersFor0, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&sizeOfWorkersFor1, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workersOf1, sizeOfWorkersFor1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&nrOfWorkers, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&workers, nrOfWorkers, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&chunk, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
      MPI_Send(&V[i * chunk], chunk, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
      cout << "M(" << rank << "," << workers[i] << ")" << endl;
    }

    fill_n(V, chunk * nrOfWorkers, 0);

    for (int i = 0; i < nrOfWorkers; i++)
    {
      MPI_Recv(&V[i * chunk], chunk, MPI_INT, workers[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }

    MPI_Send(&V, chunk * nrOfWorkers, MPI_INT, 0, 2, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << 0 << ")" << endl;

    MyReadFile.close();
  }
  else
  {
    int coordinator;
    int sizeOfWorkersFor0;
    int sizeOfWorkersFor1;
    int sizeOfWorkersFor2;
    int chunk;
    MPI_Status status;
    MPI_Recv(&coordinator, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // RECV INFO FROM COORD FOR RANK 0
    MPI_Recv(&sizeOfWorkersFor0, 1, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersFor0[sizeOfWorkersFor0];
    MPI_Recv(&workersFor0, sizeOfWorkersFor0, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // RECV INFO FROM COORD FOR RANK 1
    MPI_Recv(&sizeOfWorkersFor1, 1, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersFor1[sizeOfWorkersFor1];
    MPI_Recv(&workersFor1, sizeOfWorkersFor1, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // RECV INFO FROM COORD FOR RANK 2
    MPI_Recv(&sizeOfWorkersFor2, 1, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int workersFor2[sizeOfWorkersFor2];
    MPI_Recv(&workersFor2, sizeOfWorkersFor2, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    printTopology(rank, sizeOfWorkersFor0, sizeOfWorkersFor1, sizeOfWorkersFor2, workersFor0, workersFor1, workersFor2);

    // RECV ARRAY FROM COORD
    MPI_Recv(&chunk, 1, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int V[chunk];
    MPI_Recv(&V, chunk, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    for (int i = 0; i < chunk; i++)
    {
      V[i] = V[i] * 2;
    }

    MPI_Send(&V, chunk, MPI_INT, coordinator, 2, MPI_COMM_WORLD);
    cout << "M(" << rank << "," << coordinator << ")" << endl;
  }

  MPI_Finalize();

  return 0;
}