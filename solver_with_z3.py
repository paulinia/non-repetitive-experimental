from itertools import combinations
import random
import networkx as nx
import matplotlib.pyplot as plt
from z3 import Solver, Int, Or, sat, unsat


def coloration_r_non_repetitive_z3(adj, r, q):
    """
    adj : 0-1 adj matrix of the graph
    r   : maximal period for the forbidden squares
    q   : number of vailable colors

    outputs a color assignment with colors in {0, ..., q-1},
    or None if the graph is not colorable.
    """
    n = len(adj)

    if r < 0 or q < 0:
        raise ValueError("r et q doivent être positifs ou nuls.")
    if any(len(ligne) != n for ligne in adj):
        raise ValueError("La matrice doit être carrée.")
    if any(adj[v][v] for v in range(n)):
        raise ValueError("Le graphe doit être sans boucle.")
    if any(bool(adj[u][v]) != bool(adj[v][u])
           for u in range(n) for v in range(u)):
        raise ValueError("La matrice doit être symétrique.")

    if n == 0:
        return []
    if q == 0:
        return None
    if r == 0:
        return [0] * n

    q = min(q, n)
    Neighbors = [
        [u for u in range(n) if adj[v][u]]
        for v in range(n)
    ]

    solveur = Solver()
    couleurs = [Int(f"c_{v}") for v in range(n)]

    # one color between 0 and q-1 per vertex.
    for c in couleurs:
        solveur.add(c >= 0, c < q)

    # We can assume that v_0 is colored 0
    solveur.add(couleurs[0] == 0)
    for v in range(1, n):
        solveur.add(Or(
            couleurs[v] == 0,
            Or([
                couleurs[u] == couleurs[v] - 1
                for u in range(v)
            ])
        ))

    # paths on 2k vertices :
    limit = 2 * min(r, n // 2)
    path = []
    visit = [False] * n

    def explorer(v):
        path.append(v)
        visit[v] = True
        length = len(path)

        # A path and its mirror provide the same constraint.
        if length % 2 == 0 and path[0] < path[-1]:
            k = length // 2
            solveur.add(Or([
                couleurs[path[i]] != couleurs[path[i + k]]
                for i in range(k)
            ]))

        if length < limit:
            for u in Neighbors[v]:
                if not visit[u]:
                    explorer(u)

        visit[v] = False
        path.pop()

    if limit >= 2:
        for depart in range(n):
            explorer(depart)

    result = solveur.check()

    if result == unsat:
        return None
    if result != sat:
        raise RuntimeError(
            f"Z3 n'a pas conclu : {solveur.reason_unknown()}"
        )

    modele = solveur.model()
    return [modele.eval(c).as_long() for c in couleurs]


def arbre_binaire_aleatoire(n, seed=None):
    """
    adj matrix of a random tree with vertices of degree 1 or 3
    """
    if n < 4 or n % 2 != 0:
        raise ValueError("n must be at least 4 and even")

    rng = random.Random(seed)
    adj = [[0] * n for _ in range(n)]

    def add_edge(u, v):
        adj[u][v] = adj[v][u] = 1

    for v in (1, 2, 3):
        add_edge(0, v)

    leafs = [1, 2, 3]
    prochain = 4

    while prochain < n:
        i = rng.randrange(len(leafs))
        v = leafs[i]
        leafs[i] = leafs[-1]
        leafs.pop()

        add_edge(v, prochain)
        add_edge(v, prochain + 1)

        leafs.extend([prochain, prochain + 1])
        prochain += 2

    return adj

def tree_with_cycle_leafs(n, seed=None):
    adj = arbre_binaire_aleatoire(n, seed)
    leafs = []

    def parcourir(v, parent):
        enfants = [
            u for u in range(n)
            if adj[v][u] and u != parent
        ]
        if not enfants:
            leafs.append(v)
        else:
            for u in enfants:
                parcourir(u, v)

    parcourir(0, -1)

    # Relier les leafs en un cycle.
    for i, u in enumerate(leafs):
        v = leafs[(i + 1) % len(leafs)]
        adj[u][v] = adj[v][u] = 1

    return adj


def cycle(n):
    adj = [[0] * n for _ in range(n)]
    vertices = list(range(n))
    random.shuffle(vertices)

    start = 0
    while start < n:
        remaining = n - start

        # Leave either no vertices or at least 3 for the next cycles.
        sizes = [
            k for k in range(3, remaining + 1)
            if remaining - k == 0 or remaining - k >= 3
        ]
        k = random.choice(sizes)
        cycle = vertices[start:start + k]

        for i in range(k):
            u, v = cycle[i], cycle[(i + 1) % k]
            adj[u][v] = adj[v][u] = 1

        start += k

    low_deg = list(range(n))
    cpt = 0
    while(cpt<n):
        adj[low_deg[cpt]][low_deg[cpt+1]] = 1
        adj[low_deg[cpt+1]][low_deg[cpt]] = 1
        cpt += 2
    return adj

def cubic_wall(h, w, seed=None):
    """Return the adjacency matrix of a wall completed to a cubic graph."""
    if h < 2 or w < 2:
        raise ValueError("h and w must both be at least 2.")

    rng = random.Random(seed)
    rows, cols = h + 1, 2 * w + 2
    neighbors = {v: set() for v in range(rows * cols)}

    def add_edge(u, v):
        neighbors[u].add(v)
        neighbors[v].add(u)

    # Horizontal edges and alternating vertical edges.
    for r in range(rows):
        for c in range(cols):
            u = r * cols + c
            if c + 1 < cols:
                add_edge(u, u + 1)
            if r + 1 < rows and (r + c) % 2 == 0:
                add_edge(u, u + cols)

    # Remove degree-1 corners.
    leaves = [v for v in neighbors if len(neighbors[v]) < 2]
    while leaves:
        v = leaves.pop()
        if v not in neighbors:
            continue
        for u in list(neighbors[v]):
            neighbors[u].remove(v)
            if len(neighbors[u]) < 2:
                leaves.append(u)
        del neighbors[v]

    boundary = [v for v in neighbors if len(neighbors[v]) == 2]

    # Find a perfect matching using only edges absent from the wall.
    # Randomized backtracking: the matching is not sampled uniformly.
    def find_matching(remaining):
        if not remaining:
            return []

        u = min(
            remaining,
            key=lambda v: len(remaining - neighbors[v] - {v})
        )
        candidates = sorted(remaining - neighbors[u] - {u})
        rng.shuffle(candidates)

        for v in candidates:
            result = find_matching(remaining - {u, v})
            if result is not None:
                return [(u, v)] + result

        return None

    matching = find_matching(set(boundary))
    if matching is None:
        raise ValueError("No suitable perfect matching exists.")

    for u, v in matching:
        add_edge(u, v)

    # Relabel the surviving vertices as 0, ..., n-1.
    vertices = sorted(neighbors)
    index = {v: i for i, v in enumerate(vertices)}
    adj = [[0] * len(vertices) for _ in vertices]

    for u in vertices:
        for v in neighbors[u]:
            adj[index[u]][index[v]] = 1

    assert all(sum(row) == 3 for row in adj)
    return adj

def random_subcubic_graph(n, p, seed=None):
    """Generate G(n,p), make it subcubic, and return its adjacency matrix."""
    G = nx.gnp_random_graph(n, p, seed=seed)
    H = nx.Graph()
    ports = {}

    for v in G:
        neighbors = list(G.neighbors(v))
        d = len(neighbors)

        if d <= 3:
            H.add_node((v, 0))
            for u in neighbors:
                ports[v, u] = (v, 0)
        else:
            # Replace v by a cycle of length d.
            cycle = [(v, i) for i in range(d)]
            H.add_edges_from(
                (cycle[i], cycle[(i + 1) % d])
                for i in range(d)
            )
            for i, u in enumerate(neighbors):
                ports[v, u] = cycle[i]

    for u, v in G.edges():
        H.add_edge(ports[u, v], ports[v, u])

    return nx.to_numpy_array(H, dtype=int).tolist()

    
def draw_graph(adj, colors=None):
    G = nx.Graph()
    G.add_nodes_from(range(len(adj)))
    G.add_edges_from(
        (u, v)
        for u in range(len(adj))
        for v in range(u + 1, len(adj))
        if adj[u][v]
    )

    pos = nx.spring_layout(G, seed=42)
    nx.draw(
        G, pos,
        with_labels=True,
        node_color=colors if colors is not None else "lightblue",
        cmap=plt.cm.tab10 if colors is not None else None,
        node_size=400,
    )
    plt.show()

#adj = cycle(40)
#adj = tree_with_cycle_leafs(8, seed=42)
#adj = cubic_wall(2, 8)
#G = nx.dodecahedral_graph()
#G = nx.circular_ladder_graph(10)
#G = nx.tutte_graph()
#G = nx.frucht_graph()
#G = nx.desargues_graph()
#G = nx.pappus_graph()
G = nx.moebius_kantor_graph()
adj = nx.to_numpy_array(G, dtype=int).tolist()
c = coloration_r_non_repetitive_z3(adj, r=6, q=5)
print(adj)
print("coloring : ", c)
draw_graph(adj)
random.seed(1014)

for i in range(200):
    print(i)
    #adj = cycle(50)
    adj = random_subcubic_graph(15, 0.7)
    print(len(adj))
    c = coloration_r_non_repetitive_z3(adj, r=6, q=6)
    if c is None:
        print(adj)

        print("coloring : ", c)


        draw_graph(adj)



        