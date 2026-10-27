/* Generates random 3-regular chord graph on n vertices
 * The graph consists of a cycle + random matching (that does not duplicate cycle edges)
 * USAGE
 * Input: n q
 * Output:
 * n m q
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

vector<ii> generate(int n) {
    vector<ii> hrany;
    vector<int> outer;
    FOR(i, n) {
        hrany.push_back({i, (i + 1) % n});
        outer.push_back(i);
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
    int n = get<int>();    
    int seed = time(NULL);
    int target = get<int>();
    int d = 3;
    srand(seed);
    
    vector<ii> E = generate(n);
    while (!valid(E)) E = generate(n);
    
    cout << n << " " << n * d / 2 << " " << target << endl;
    for (ii e : E) cout << e.first << " " << e.second << endl;
}
