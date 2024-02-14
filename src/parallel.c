// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "log/log.h"
#include "utils.h"

#define NUM_THREADS		4

static int sum;
static os_graph_t *graph;
static os_threadpool_t *tp;
/* TODO: Define graph synchronization mechanisms. */
pthread_mutex_t graph_mutex;

/* TODO: Define graph task argument. */

static void process_node(unsigned int idx);

static void process_node_helper(void *idx)
{
	unsigned int ui_idx = *(unsigned int *)idx;

	process_node(ui_idx);
}

static void process_node(unsigned int idx)
{
	/* TODO: Implement thread-pool based processing of graph. */
	pthread_mutex_lock(&graph_mutex);

	os_node_t *node;
	os_task_t *task;

	node = graph->nodes[idx];
	sum += node->info;
	graph->visited[idx] = DONE;

	for (unsigned int i = 0; i < node->num_neighbours; i++) {
		if (graph->visited[node->neighbours[i]] == NOT_VISITED) {
			graph->visited[node->neighbours[i]] = PROCESSING;
			task = create_task(process_node_helper, (void *)&node->neighbours[i], NULL);
			enqueue_task(tp, task);
		}
	}
	tp->first_added = 1;
	pthread_cond_broadcast(&tp->cond_dequeue);
	pthread_mutex_unlock(&graph_mutex);
}

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	DIE(input_file == NULL, "fopen");

	graph = create_graph_from_file(input_file);

	/* TODO: Initialize graph synchronization mechanisms. */
	tp = create_threadpool(NUM_THREADS);

	if (pthread_mutex_init(&graph_mutex, NULL) != 0) {
		perror("Failed to initialize mutex");
		exit(EXIT_FAILURE);
	}


	process_node(0);
	wait_for_completion(tp);
	destroy_threadpool(tp);

	printf("%d", sum);

	pthread_mutex_destroy(&graph_mutex);

	return 0;
}
