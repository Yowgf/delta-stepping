# Delta-Stepping

Parallel Delta-Stepping SSSP algorithm with the bucket fusion technique invented by Zhang et al. (CGO 2020). Implemented in C++, using OpenMP.

Delta-stepping is an algorithm originally suggested by Meyer et al. in 1998 as an alternative to Dijkstra's purely sequential approach to [SSSP](https://en.wikipedia.org/wiki/Shortest_path_problem). The central idea is that of priority coarsening. That is, one can give up on the strict order a bit, without making the algorithm incorrect.

SSSP tries to calculate the distances of all nodes to a fixed source node. During Delta-stepping, to each node we attribute a ordering "category", instead of a fixed position. More concretely, each node is put in a bucket of width _delta_, according to its distance to the source node. For example, nodes with distance 0 to 1000 to the source node can all be placed in the bucket (read category) _0_, if we take a _delta_ of 1000.

This observation opens opportunity to parallelize the code. Specifically, we can work concurrently with different nodes of the same bucket. In practice, the algorithm can achieve significant speedups over Dijkstra's.

## Compile instructions

To compile the code, just type ```make``` in the project's main directory. The executable file ```delta-stepping``` should be created in the folder ```build```. From there, it should be intuitive how to run the algorithm.

The main implementation of the algorithm can be found in the file ```lib/main/appliance/Alg/delta-stepping.cpp```.
