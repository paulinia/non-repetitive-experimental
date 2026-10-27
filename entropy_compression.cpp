/* INPUT:
 * n m q
 * a_1 b_1
 * ...
 * a_m b_m
 * (adjacency list of the graph, vertices numbered from 0)
 * 
 * OUTPUT:
 * Q -- the resulting number of colours
 * q_1 q_2 ... q_n -- q_i is the colour of ith vertex
 * The algorithm runs for LIM = 20000 steps before giving up and increasing q by one; Q is the resulting number of colours
 * 
 * Compile with -DEBUG to get logs
 */
#include<bits/stdc++.h>

using namespace std;

#define FOR(i,n)	for(int i=0;i<(int)n;i++)
#define ii pair<int, int>
#define lli long long int
// DEBUG macro, compile with -DEBUG
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

template <class T, class U>
ostream& operator<<(ostream& out, const pair<T, U> &par) {
    out << "[" << par.first << ";" << par.second << "]";
    return out;
}

template <class T>
ostream& operator<<(ostream& out, const vector<T>& v) {
    FOR(i, v.size()){
      if(i) out << " ";
      out << v[i];
    }
    out << endl;
    return out;
}

default_random_engine gen;
mt19937 gen_mt(gen());

lli b = 433;
lli mod = 29496756437;
#define Nmax 1000
// ensure b < q
// Nmax is the maximal size of the graph. Change to increase limit. Set to constant for compiler optimizations

struct path {
    int L;
    vector<int> V;
    bitset<Nmax> in;
    
    path(int v) {
        L = 1;
        in = bitset<Nmax>(0);
        in.set(v);
        V = {v};
    }
    
    path() {
        L = 0;
        in = 0;
    }
    
    path(vector<int> &v) {
        V = v;
        L = SIZE(V);
        in = 0;
        for (int u : V) in.set(u);
    }
    
    path(path &P, int u) {
        V = P.V;
        V.push_back(u);
        in = P.in;
        in.set(u);
        L = P.L + 1;
    }
    
    path(pair<path, path> &Ps) {
        L = Ps.first.L + Ps.second.L;
        in = Ps.first.in | Ps.second.in;
        V = Ps.first.V;
        for (int v : Ps.second.V) V.push_back(v);
    }
};

struct col_path {
    path P;
    lli col_type;
    lli pw_last;
    
    col_path(path &p, vector<int> &col) {
        P = p;
        col_type = 0;
        lli pw = 1;
        FOR(i, P.L) {
            col_type = (col_type + (((lli)col[P.V[i]] * pw) % mod)) % mod;
            pw = (pw * b) % mod;
        }
        pw_last = pw;
    }
    
    col_path(col_path &cP, int v, lli c) {
        P = path(cP.P, v);
        col_type = (cP.col_type + (c * cP.pw_last)) % mod;
        pw_last = (cP.pw_last * b) % mod;
        // DBG cout << "Extend " << cP.P << " -> " << P << " by col " << c << endl;
    }
    
    vector<col_path> extend(vector<vector<int> > &hrany, vector<int> &col) {
        int v = P.V.back();
        vector<col_path> to_ext;
        for (int u : hrany[v]) {
            if (P.in[u]) continue;
            if (col[u] == -1) continue;
            to_ext.push_back(col_path(*this, u, col[u]));
        }
        return to_ext;
    }
    
    int last() {
        return P.V[P.L - 1];
    }
    
    bitset<Nmax> in() {
        return P.in;
    }
};

ostream& operator<<(ostream& out, const col_path &P) {
    cout << "[L = " << P.P.L << "; col_type = " << P.col_type << " V = {";
    for (int v : P.P.V) cout << v << ", ";
    cout << "}]";
    return out;
}



path find_shortest(int n, int q, vector<vector<int> > &hrany, vector<int> &colouring) {
    vector<vector<col_path> > last(q);
    unordered_map<int, int> id_by_colour;
    FOR(i, n) {
        if (colouring[i] == -1) continue;
        path P = path(i);
        last[colouring[i]].push_back(col_path(P, colouring));
        id_by_colour[colouring[i]] = colouring[i];
    }
    
    for (int l = 1; l * 2 <= n; l ++) {
        vector<vector<col_path> > next;
        int N = 0;
        id_by_colour.clear();
        
        vector<pair<path, path> > bad;
        // check all colours and extend
        FOR(i, SIZE(last)) {
            if (SIZE(last[i]) < 2) {
                continue;
            }
            
            vector<vector<int> > by_start(n);
            FOR(j, SIZE(last[i])) by_start[last[i][j].P.V[0]].push_back(j);
            int cnt = 0;
            for (col_path &cP : last[i]) {
                cnt ++;
                if (cnt < 100) {
                    // filter out paths that have no disjoint paths with the same colour sign
                    // 100 is a constant here to avoid quadratic runtime here
                    int disj = 0;
                    for (col_path &oP : last[i]) {
                        if ((oP.in() & cP.in()).none()) {
                            disj ++;
                            break;
                        }
                    }
                    if (!disj) continue; // no point in extending
                }
                
                
                for (int u : hrany[cP.last()]) {
                    for (int it : by_start[u]) {
                        if ((last[i][it].in() & cP.in()).none()) {
                            // found match
                            bad.push_back({cP.P, last[i][it].P});
                            DBG cout << "Found repetitive " << cP << " and " << last[i][it] << endl;
                        }
                    }
                }
                
                vector<col_path> exts = cP.extend(hrany, colouring);
                for (col_path &colP : exts) {
                    if (!id_by_colour.count(colP.col_type)) {
                        id_by_colour[colP.col_type] = N;
                        N ++;
                        next.push_back(vector<col_path>());
                    }
                    next[id_by_colour[colP.col_type]].push_back(colP);
                }
            }
        }
        
        if (!bad.empty()) {
            int it = gen_mt() % SIZE(bad);
            return path(bad[it]);
        }
        last = next;
        
    }
    
    
    return path();
}

int find_last(int v, bitset<Nmax> seen, vector<int> &C, vector<vector<int> > &hrany) {
    seen[v] = 1;
    for (int w : hrany[v]) {
        if ((!seen[w]) && C[w] == -1) return find_last(w, seen, C, hrany);
    }
    return v;
}

// LIM = maximum number of steps; increase to test whether more EC iterations might help.
pair<bool, vector<int> > find_col_EC(int n, int q, vector<vector<int> > &hrany, int LIM = 20000) {
    vector<int> C(n, -1);
    int mame = 0;
    int steps = 0;
    while (mame < n && steps < LIM) {
        steps ++;
        DBG cout << "Step = " << steps << " currently set = " << mame << " colours = " << C << endl;
        int from = gen_mt() % n;
        while (C[from] != -1) from = gen_mt() % n;
        bitset<Nmax> seen(0);
        from = find_last(from, seen, C, hrany);
        
        vector<int> available(q, true);
        for (int w : hrany[from]) {
            if (C[w] != -1) available[C[w]] = false;
        }
        
        C[from] = gen_mt() % q;
        while (!available[C[from]]) C[from] = gen_mt() % q;
        path P = find_shortest(n, q, hrany, C);
        mame ++;
        if (P.L) {
            for (int w : P.V) C[w] = -1;
            mame -= P.L;
        }
    }
    return {mame == n, C};
}



int main() {
    cin.sync_with_stdio(false);
    cout.sync_with_stdio(false);
    int n = get<int>();
    int m = get<int>();
    int target = get<int>();
    
    vector<vector<int> > hrany(n);
    FOR(i, m) {
        int a = get<int>();
        int b = get<int>();
        hrany[a].push_back(b);
        hrany[b].push_back(a);
    }
    
    auto res = find_col_EC(n, target, hrany);
    while (!res.first) {
        target ++;
        res = find_col_EC(n, target, hrany);
    }
    cout << target << "\n" << res.second;
    
}
