#!/usr/bin/env python3

# Produces a visualisation of a given graph with a given colouring 
# USAGE:
# python3 visualise.py GRAPH COLS OUT.svg
# where GRAPH is the GRAPH input file (beginning with n m q)
# and   COLS is the file with colours (beginning with q) 
# and   OUT.svg is the output .svg file where the graph is drawn

import sys
import math
import colorsys
import html
from random import randint


def read_graph(filename):
    with open(filename) as f:
        first = f.readline().split()

        if len(first) != 3:
            raise ValueError(
                "First line must contain: vertices edges colours"
            )

        n, m, num_colours = map(int, first)

        edges = []
        for _ in range(m):
            line = f.readline()
            if not line:
                raise ValueError("Not enough edges in graph file")

            u, v = map(int, line.split())

            edges.append((u, v))

    return n, m, edges
        #num_colours, edges


def read_colours(filename):
    with open(filename) as f:
        first = f.readline().split()

        if len(first) != 1:
            raise ValueError(
                "First line must contain: number of colours"
            )

        ncol = int(first[0])

        cols = list(map(int, f.readline().split()))
    return ncol, cols

def make_palette(k):
    palette = []

    for i in range(k):
        h = i / max(k, 1)
        r, g, b = colorsys.hsv_to_rgb(h, 0.65, 0.90)

        palette.append(
            f"rgb({int(r*255)},{int(g*255)},{int(b*255)})"
        )

    return palette


def draw_svg(n, edges, colours, num_colours, output):
    width = 3000
    height = 1500


    graph_radius = 300
    vertex_radius = 22

    palette = make_palette(num_colours)

    positions = {}

    # BFS
    root = randint(0, n - 1)
    layers = [[root]]
    seen = {root}
    
    removed = []
    
    while (len(seen) < n):
        l = len(layers) - 1
        layers.append([])
        space = 1
        for v in layers[l]:
            for w in map(lambda x: x[0] if x[1] == v else x[1], filter(lambda e : (e[0] == v and e[1] not in seen) or (e[1] == v and e[0] not in seen), edges)):
                layers[l + 1].append(w)
                seen.add(w)
        if len(layers[-1]) == 0 and len(seen) < n:
            min_not_seen = min(filter(lambda v: v not in seen, range(n)))
            layers[l + 1].append(w)
            seen.add(w)
    
    
    cy = height / (len(layers) + 1)
    y = 0
    for L in layers:
        y += 1
        cx = width / (len(L) + 1)
        x = 0
        for v in L:
            x += 1
            positions[v] = (cx * x, cy * y)
#     
#     for v in range(n):
#         angle = 2 * math.pi * (v) / n - math.pi / 2
# 
#         x = cx + graph_radius * math.cos(angle)
#         y = cy + graph_radius * math.sin(angle)
# 
#         positions[v] = (x, y)

    with open(output, "w") as f:
        f.write(
            f'<svg xmlns="http://www.w3.org/2000/svg" '
            f'width="{width}" height="{height}" '
            f'viewBox="0 0 {width} {height}">\n'
        )

        f.write(
            '<rect width="100%" height="100%" fill="white"/>\n'
        )

        # Edges
        f.write('<g stroke="#555" stroke-width="2">\n')

        for u, v in edges:
            x1, y1 = positions[u]
            x2, y2 = positions[v]
            if abs(y1 - y2) < 1e-6:
                # Same BFS layer: draw an arc.
                mid_x = (x1 + x2) / 2

                # Larger horizontal distance -> taller arc.
                arc_height = abs(x2 - x1) * 0.35
                control_y = y1 + arc_height

                f.write(
                    f'<path d="M {x1:.2f} {y1:.2f} '
                    f'Q {mid_x:.2f} {control_y:.2f} {x2:.2f} {y2:.2f}" '
                    f'fill="none"/>\n'
                )
            else:
                f.write(
                    f'<line x1="{x1:.2f}" y1="{y1:.2f}" '
                    f'x2="{x2:.2f}" y2="{y2:.2f}"/>\n'
                )

        f.write('</g>\n')

        # Vertices
        for v in range(n):
            x, y = positions[v]

            colour_id = colours[v]

            if not 0 <= colour_id < num_colours:
                raise ValueError(
                    f"Vertex {v} has invalid colour {colour_id}"
                )

            fill = palette[colour_id]

            f.write(
                f'<circle cx="{x:.2f}" cy="{y:.2f}" '
                f'r="{vertex_radius}" '
                f'fill="{fill}" stroke="black" stroke-width="2"/>\n'
            )

            f.write(
                f'<text x="{x:.2f}" y="{y + 5:.2f}" '
                f'text-anchor="middle" '
                f'font-family="sans-serif" '
                f'font-size="15" font-weight="bold">'
                f'{html.escape(str(v))}'
                f'</text>\n'
            )

        f.write('</svg>\n')


def main():
    if len(sys.argv) != 4:
        print(
            f"Usage: {sys.argv[0]} GRAPH COLS OUTPUT.svg",
            file=sys.stderr
        )
        sys.exit(1)

    graph_file = sys.argv[1]
    col_file = sys.argv[2]
    output_file = sys.argv[3]
    

    n, m, edges = read_graph(graph_file)
    num_colours, colours = read_colours(col_file)

    draw_svg(
        n,
        edges,
        colours,
        num_colours,
        output_file
    )

    print(f"Wrote {output_file}")


if __name__ == "__main__":
    main()
