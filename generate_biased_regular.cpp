/* Generates random 3-regular on n vertices that is t-biased (biased towards more cycles)
 * Graph is obtained from a random matching where ith vertex can be connected to vertices from 0 to i + t inclusive
 * Setting t = n yields a random regular; t small yields many short cycles
 * WARNING: setting t very small might blows up the time needed to generate the graph
 * USAGE
 * Input: n q t
 * Output:
 * N m q
 * adjacency list of m edges, each edge on separate line, vertices numbered from 0
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


template<class T>
T get() {
    T a;
    cin >> a;
    return a;
}

default_random_engine gen;
mt19937 gen_mt(gen());


bool valid(int n, int d, vector<ii> &E) {
    set<ii> used;
    for (ii e : E) {
        int a = e.first, b = e.second;
        if (used.count({a, b}) || a == b) return false;
        used.insert({a, b});
    }
    return true;
}

vector<ii> generate(int n, int d, int t) {
    vector<int> degs(n, d);
    vector<ii> res;
    FOR(i, n) {
        while (degs[i] > 0) {
            int match = gen_mt() % min(n, i + t);
            int where = i + t;
            while (degs[match] <= 0 + (match == i ? 1 : 0)) {
                match = gen_mt() % min(n, where);
                where ++;
            }
            res.push_back({i, match});
            degs[i] --;
            degs[match] --;
        }
    }
    return res;
}

int main() {
    cin.sync_with_stdio(false);
    cout.sync_with_stdio(false);
    int n = get<int>();
    int d = 3;
    int target = get<int>();
    int t = get<int>();
    
    int seed = time(NULL);
    srand(seed);
    
    vector<int> degs;
    FOR(i, n) {
        FOR(j, d) degs.push_back(i);
    }
    
    
    vector<ii> E = generate(n, d, t);
    while (!valid(n, d, E)) E = generate(n, d, t);
    
    cout << n << " " << n * d / 2 << " " << target << endl;
    for (ii e : E) cout << e.first << " " << e.second << endl;
}
