# Non-Repetitive Graph Colourings (Experimental)

Tools for generating 3-regular graphs, finding non-repetitive vertex colourings via entropy compression (`entropy_compression.cpp`), and visualising the resulting colourings (`visualise.py`).

## Usage (`find_and_visualise.sh`)

Run a batch of graph generation, colouring, and SVG visualisation tasks with:

```bash
./find_and_visualise.sh <list_file> <out_name>
```

Each line of `<list_file>` specifies a generator (with or without `.cpp`/`.py` extension) followed by its input parameters, for example:

```text
generate_random_regular 100 10
generate_ladder.py 50 6
generate_biased_regular.cpp 150 10 50
```

For the $i$-th line in `<list_file>`, the script:
1. Generates the graph into `<out_name><i>.in`.
2. Runs `./entropy_compression < <out_name><i>.in > log<i>.out`.
3. Extracts the last two lines of `log<i>.out` (the final number of colours $Q$ and the vertex colouring) into `col<i>.out`.
4. Runs `python3 visualise.py <out_name><i>.in col<i>.out <out_name><i>.svg` to render the coloured graph.

## Graph Generators

In all generators below, `q` is the initial number of colours tried by the entropy compression algorithm:

- **`generate_random_regular.cpp`** (`n q`): Generates a random 3-regular graph on `n` vertices.
- **`generate_biased_regular.cpp`** (`n q t`): Generates a random `t`-biased 3-regular graph on `n` vertices (connecting vertex $i$ only to vertices up to $i + t$, producing more short cycles than random regular graphs).
- **`generate_chord.cpp`** (`n q`): Generates a random 3-regular graph on `n` vertices consisting of an `n`-cycle plus a random edge-disjoint perfect matching.
- **`generate_treelike.cpp`** (`r q`): Generates a random 3-regular treelike graph on $3N$ vertices ($N = 3(2^r - 1) + 1$) formed by three copies of depth-`r` 3-regular trees with random edges added between leaves.
- **`generate_ladder.py`** (`half q`): Generates a 3-regular circular ladder (prism) graph on `2 * half` vertices.
- **`generate_prism_cycle.py`** (`N q`): Generates a 3-regular cycle of $N$ diamond/prism gadgets on `4 * N` vertices.

## Entropy Compression Parameters

- To increase the maximum number of vertices that the EC algorithm allows, increase Nmax in the entropy_compression.cpp
- To increase the maximum number of rounds that the EC algorithm tries, increase LIM in the entropy_compression(...) method header.
- To see partial colourings (in the log{i}.out file), compile with -DEBUG
