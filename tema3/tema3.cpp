#include <iostream>
#include <fstream>
#include "mpi.h"
#include <stdio.h>
#include <list>
#include <stdlib.h>
#include <string>
#include <map>

using namespace std;

int main(int argc, char *argv[])
{
  int numtasks, rank, len;
  char hostname[MPI_MAX_PROCESSOR_NAME];
  map<int, list<int> > coordinatorToWorker;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(hostname, &len);
  if (rank == 0)
  {
    // Create a text string, which is used to output the text file
    int worker;

    // Read from the text file
    ifstream MyReadFile("/Users/robert.caplan/projects/Parallel-and-Distributed-Algorithms/tema3/tests/test1/cluster0.txt");

    // Use a while loop together with the getline() function to read the file line by line
    int nrOfWorkers;
    list<int> workersList;
    MyReadFile >> nrOfWorkers;
    cout << nrOfWorkers;
    // getline(MyReadFile, nrOfWorkers);
    while (MyReadFile >> worker)
    {
      // Output the text from the file
      MPI_Send(0, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
      workersList.push_back(worker);
      cout << to_string(worker);
    }

    cout << workersList;

    coordinatorToWorker.insert(pair<int, list<int> >(rank, workersList));

    // cout << coordinatorToWorker;
    // Close the file
    MyReadFile.close();
  }
  else if (rank == 1)
  {
    // Create a text string, which is used to output the text file
    string myText;

    // Read from the text file
    ifstream MyReadFile("/Users/robert.caplan/projects/Parallel-and-Distributed-Algorithms/tema3/tests/test1/cluster1.txt");

    // Use a while loop together with the getline() function to read the file line by line
    while (getline(MyReadFile, myText))
    {
      // Output the text from the file
      cout << myText + "\n";
    }

    // Close the file
    MyReadFile.close();
  }
  else if (rank == 2)
  {
    // Create a text string, which is used to output the text file
    string myText;

    // Read from the text file
    ifstream MyReadFile("/Users/robert.caplan/projects/Parallel-and-Distributed-Algorithms/tema3/tests/test1/cluster2.txt");

    // Use a while loop together with the getline() function to read the file line by line
    while (getline(MyReadFile, myText))
    {
      // Output the text from the file
      cout << myText + "\n";
    }

    // Close the file
    MyReadFile.close();
  }
  else
  {
    int coordinator;
    // MPI_Recv(coordinator, 1, MPI_INT, );
  }

  MPI_Finalize();

  return 0;
}