/* Generates random treelike on 3N vertices, where N = 3 * ((1 << r) - 1) + 1
 * Graph contains 3 copies of 3-regular trees, with random edges added between the leaves to make the graph 3-regular
 * USAGE
 * Input: r q
 * Output:
 * N m q
 * adjacency list of m edges, each edge on separate line, vertices numbered from 1
 * 
 * m = 3n / 2
 * q is (the first) number of colours that the EC algorithm tries
 */
#include<bits/stdc++.h>

using namespace std;

#define FOR(i,n)	for(int i=0;i<(int)n;i++)
#define ii pair<int, int>
#ifdef EBUG
#define DBG	if(1)
#else
#define DBG	if(0)
#endif
#define SIZE(x) int(x.size())

template<class T>
T get() {
    T a;
    cin >> a;
    return a;
}

default_random_engine gen;
mt19937 gen_mt(gen());


bool valid(vector<ii> &E) {
    set<ii> used;
    for (ii e : E) {
        int a = e.first, b = e.second;
        if (used.count({a, b}) || a == b) return false;
        used.insert({a, b});
    }
    return true;
}

vector<ii> generate(int r) {
    // generates graph with N = 3 * d (d-1)^{r-1} for d = 3
    vector<ii> hrany;
    int N = 3 * ((1 << r) - 1) + 1;
    vector<int> outer;
    
    int s = 0, e = 1;
    
    FOR(i, r) {
        DBG cout << i << "th layer between " << s << " and " << e << endl;
        if (!i) {
            FOR(j, 3) {
                FOR(l, 3) hrany.push_back({N * l, N * l + j + 1});
            }
            s = 1, e = 4;
        }
        else {
            int next = e;
            for (int v = s; v < e; v ++) {
                FOR(j, 3) {
                    hrany.push_back({N * j + v, N * j + next});
                    hrany.push_back({N * j + v, N * j + next + 1});
                }
                next += 2;
            }
            s = e;
            e = next;
        }
        if (i == r - 1) {
            for (int v = s; v < e; v ++) {
                FOR(j, 3) {
                    FOR(l, 2) outer.push_back(N * j + v);
                }
            }
        }
        
    }
    shuffle(outer.begin(), outer.end(), gen_mt);
    FOR(i, SIZE(outer) / 2) {
        hrany.push_back({outer[i], outer[i + SIZE(outer) / 2]});
    }
    
    return hrany;
}

int main() {
    cin.sync_with_stdio(false);
    cout.sync_with_stdio(false);
    int r = get<int>();    
    int seed = time(NULL);
    int target = get<int>();
    int d = 3;
    srand(seed);
    
    
    int n = 3 * (3 * ((1 << r) - 1) + 1);
    
    vector<ii> E = generate(r);
    while (!valid(E)) E = generate(r);
    
    cout << n << " " << n * d / 2 << " " << target << endl;
    for (ii e : E) cout << e.first << " " << e.second << endl;
}
