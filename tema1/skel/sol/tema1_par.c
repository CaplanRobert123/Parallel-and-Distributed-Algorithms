#include <stdlib.h>
#include "genetic_algorithm.h"
#include <pthread.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	// array with all the objects that can be placed in the sack
	sack_object *objects = NULL;

	// number of objects
	int object_count = 0;

	// maximum weight that can be carried in the sack
	int sack_capacity = 0;

	// number of generations
	int generations_count = 0;

	// number of threads
	int P;

	if (!read_input(&objects, &object_count, &sack_capacity, &generations_count, &P, argc, argv))
	{
		return 0;
	}

	// declar bariera
	pthread_barrier_t barrier;

	individual *current_generation = (individual *)calloc(object_count, sizeof(individual));
	individual *next_generation = (individual *)calloc(object_count, sizeof(individual));
	individual *tmp = NULL;

	// initializez generatiile
	for (int i = 0; i < object_count; ++i)
	{
		current_generation[i].fitness = 0;
		current_generation[i].chromosomes = (int *)calloc(object_count, sizeof(int));
		current_generation[i].chromosomes[i] = 1;
		current_generation[i].count_chromosomes = 0;
		current_generation[i].index = i;
		current_generation[i].chromosome_length = object_count;

		next_generation[i].fitness = 0;
		next_generation[i].chromosomes = (int *)calloc(object_count, sizeof(int));
		next_generation[i].index = i;
		next_generation[i].chromosome_length = object_count;
	}

	pthread_t threads[P];
	pthread_barrier_init(&barrier, NULL, P);

	args_struct args[P];

	// initializezi argumentele date ca parametrii functiei paralelizate
	for (int i = 0; i < P; i++)
	{
		args[i].P = P;
		args[i].barrier = &barrier;
		args[i].params.objects = objects;
		args[i].params.object_count = object_count;
		args[i].params.generations_count = generations_count;
		args[i].params.sack_capacity = sack_capacity;
		args[i].params.current_generation = current_generation;
		args[i].params.next_generation = next_generation;
		args[i].params.tmp = tmp;
		args[i].id = i;
	}

	// creez thread-urile
	for (int i = 0; i < P; i++)
	{
		pthread_create(&threads[i], NULL, run_genetic_algorithm_par, &args[i]);
	}

	// pentru a finaliza operatiile
	for (int id = 0; id < P; id++)
	{
		pthread_join(threads[id], NULL);
	}

	pthread_barrier_destroy(&barrier);
	free(current_generation);
	free(next_generation);
	free(objects);
	return 0;
}
