// Author: Vladislav Vorotilov
// This code can be used for educational purposes only
// Article about algorithm can be found here:
//  http://algo.uni-konstanz.de/publications/b-lrpt-sub.pdf

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <memory>
#include <algorithm>
#include "Graph.h"

class LeftRightPlanarityTest {
public:
    LeftRightPlanarityTest( const Graph& graph );

    bool IsPlanar();

private:

    struct Edge : public std::pair<int, int> {
        Edge() : std::pair<int,int>(-1, -1) {}
        Edge( int from, int to ) : std::pair<int,int>(from, to ) {}
        bool IsEmpty() const { return first == -1 && second == -1; }
        void MakeEmpty() { first = -1; second = -1;}
    };
    struct EdgeHash {
        std::size_t operator() (const Edge& e ) const
        {
            std::hash<int> intHash;
            return ( intHash(e.first) << 4 ) + intHash(e.second);
        }
    };
    template <typename T>
    using EdgeDict = std::unordered_map<Edge, T, EdgeHash>;

    struct Interval : public std::pair<Edge, Edge> {
        Interval() = default;
        Interval( Edge low , Edge high) : std::pair<Edge, Edge>( low, high ) {}
        Edge& low() { return first; }
        const Edge& low() const { return first; }
        Edge& high() { return second; }
        const Edge& high() const { return second; }
        bool IsEmpty() const { return first.IsEmpty() && second.IsEmpty(); }
    };

    struct ConflictPair : public std::pair<Interval, Interval> {
        ConflictPair() = default;
        ConflictPair( Interval L, Interval R ) : std::pair<Interval, Interval>( L, R ) {}
        Interval& L() { return first; }
        const Interval& L() const { return first; }
        Interval& R() { return second; }
        const Interval& R() const { return second;}
        bool IsEmpty() const { return first.IsEmpty() && second.IsEmpty(); }
    };

    class not_planar_exception : public std::exception {

    };

    const Graph& graph;
    // doDfs1


    // Orientation
    std::unordered_set<Edge, EdgeHash> orientation;
    // Distance to the root in DFS tree
    std::vector<int> height;
    // Parent vertex for dfs orientation
    std::vector<int> parent;
    // height of lowest return point
    EdgeDict<int> lowPoint;
    // height of next-to-lowest return point (tree edges only)
    EdgeDict<int> lowPoint2;
    // nesting depth
    EdgeDict<int> nesting_depth;
    //
    EdgeDict<ConflictPair> stack_bottom;
    std::stack<ConflictPair> S;
    EdgeDict<Edge> ref;
    EdgeDict<int> side;
    EdgeDict<Edge> lowpt_edge;

    static const int Inf;// = 5000000;

    void prepareDfs1();
    void doDfs1(int v);

    bool isReached(int v) const;
    void dfs1(int v);

    void prepareDfs2();
    void dfs2(int v);

    void addEdgeConstraint( const Edge& e );
    bool conflicting( const Interval& I, const Edge& b);
    void trimBackEdgesEndingAtParent(int u);
    int lowest( const ConflictPair& P );

    // dfs2
    std::unique_ptr<Graph> reorderedGraph;

};
