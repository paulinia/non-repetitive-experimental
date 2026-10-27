/* Generates random 3-regular graphs on n vertices (n even) (or 3-regular-apart-from-one-vertex when n is odd)
 * USAGE
 * Input: n q
 * Output:
 * n m q
 * adjacency list of m edges, each edge on separate line, vertices numbered from 0
 * 
 * m = floor(3n / 2)
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

default_random_engine gen;
mt19937 gen_mt(gen());

bool valid(int n, int d, vector<int> &degs) {
    set<ii> used;
    for (int i = 0; i + 1 < n * d; i += 2) {
        int a = min(degs[i], degs[i + 1]), b = max(degs[i], degs[i + 1]);
        if (used.count({a, b}) || degs[i] == degs[i + 1]) return false;
        used.insert({a, b});
    }
    return true;
}

int main() {
    int n, target;
    cin >> n >>  target;
    int d = 3;
    int seed = time(NULL);
    srand(seed);
    
    vector<int> degs;
    FOR(i, n) {
        FOR(j, d) degs.push_back(i);
    }
    
    
    shuffle(degs.begin(), degs.end(), gen_mt);
    while (!valid(n, d, degs)) shuffle(degs.begin(), degs.end(), gen_mt);
    
    cout << n << " " << n * d / 2 << " " << target << endl;
    for (int i = 0; i + 1 < n * d; i += 2) cout << degs[i] << " " << degs[i + 1] << endl;
}
