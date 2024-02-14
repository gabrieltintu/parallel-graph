# Parallel Graph

## Objectives

- Learn how to design and implement parallel programs
- Gain skills in using synchronization primitives for parallel programs
- Get a better understanding of the POSIX threading and synchronization API
- Gain insight on the differences between serial and parallel programs

## Statement

Implement a generic thread pool, then use it to traverse a graph and compute the sum of the elements contained by the nodes.
It is provided a serial implementation of the graph traversal and with most of the data structures needed to implement the thread pool.
The goal is to write the thread pool routines and then use the thread pool to traverse the graph.

## Implementation

### Thread Pool Description

A thread pool contains a given number of active threads that simply wait to be given specific tasks.
The threads are created when the thread pool is created.
Each thread continuously polls the task queue for available tasks.
Once tasks are put in the task queue, the threads poll tasks, and start running them.
A thread pool creates **N** threads upon its creation and does not destroy (join) them throughout its lifetime.
That way, the penalty of creating and destroying threads ad-hoc is avoided.

- `enqueue_task()`: Enqueue task to the shared task queue.
  Uses synchronization.
- `dequeue_task()`: Dequeue task from the shared task queue.
  Uses synchronization.
- `wait_for_completion()`: Wait for all worker threads.
  Uses synchronization.
- `create_threadpool()`: Creates a new thread pool.
- `destroy_threadpool()`: Destroy a thread pool.
  Assumes all threads have been joined.

Notice that the thread pool is completely independent of any given application.
Any function can be registered in the task queue.

Since the threads are polling the task queue indefinitely, it will be needed to define a condition for them to stop once the graph has been traversed completely.
That is, the condition used by the `wait_for_completion()` function.
The recommended way is to note when no threads have any more work to do.
Since no thread is doing any work, no other task will be created.

### Graph Traversal

Once implemented the thread pool, it needs to be tested by doing a parallel traversal of all connected nodes in a graph.
A serial implementation for this algorithm is provided in `src/serial.c`.
To make use of the thread pool, it will be needed to create tasks that will be put in the task queue.
A task consists of 2 steps:

1. Add the current node value to the overall sum.
1. Create tasks and add them to the task queue for the neighbouring nodes.

### Synchronization

For synchronization we can use mutexes, semaphores, spinlocks, condition variables - anything that grinds your gear.
However, we are not allowed to use hacks such as `sleep()`, `printf()` synchronization or adding superfluous computation.

### Input Files

Reading the graphs from the input files is being taken care of the functions implemented in `src/os_graph.c`.
A graph is represented in input files as follows:

- First line contains 2 integers `N` and `M`: `N` - number of nodes, `M` - number of edges
- Second line contains `N` integer numbers - the values of the nodes.
- The next `M` lines contain each 2 integers that represent the source and the destination of an edge.

### Data Structures

#### Graph

A graph is represented internally by the `os_graph_t` structure (see `src/os_graph.h`).

#### List

A list is represented internally by the `os_queue_t` structure (see `src/os_list.h`).

#### Thread Pool

A thread pool is represented internally by the `os_threadpool_t` structure (see `src/os_threadpool.h`).
The thread pool contains information about the task queue and the threads.
