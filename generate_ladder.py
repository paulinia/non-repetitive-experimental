# generates 3-regular ladder (torus) with 2half vertices
# USAGE: n/2 q
# OUTPUT:
# n m q
# adjacency list, each edge on a separate line, vertices numbered from 0
# +-+
# | |
# +-+
# | |
# +-+
# ...
# wraps back up to make it 3-regular
half, q = map(int, input().split())

print(2 * half, half * 3, q)
for i in range(half - 1):
    print(i, i + half)
    print(i, i + 1)
    print(i + half, i + 1 + half)
    
print(0, half - 1)
print(half, 2 * half - 1)
print(half - 1, 2 * half - 1)