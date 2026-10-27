# generates the 3-regular prism-cycle of 4N vertices
#   +     +
#  /|\   /|\
#-+ | +-+ | +-...
#  \|/   \|/
#   +     +
# USAGE: N q
# OUTPUT:
# n m q
# adjacency list, each edge on a separate line, vertices numbered from 0
#
# where m = 3n/2 = 6N
N, q = map(int, input().split())

print(4 * N, 6 * N, q)
for i in range(N):
    next = (i + 1) % N
    print(4 * i, 4 * i + 1)
    print(4 * i, 4 * i + 2)
    print(4 * i + 1, 4 * i + 2)
    print(4 * i + 1, 4 * i + 3)
    print(4 * i + 2, 4 * i + 3)
    print(4 * i + 3, next * 4)
    
    