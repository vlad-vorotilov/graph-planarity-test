// Author: Vladislav Vorotilov
// This code can be used for educational purposes only

#pragma once
#include <vector>
using namespace std;


class Graph {
public:
    Graph( int numberOfVertexes ) : adjacencyList( numberOfVertexes ), edgesCount(0) {}

    const size_t Size() const { return adjacencyList.size(); }
    const size_t EdgesCount() const { return edgesCount; }
    const std::vector<int>& GetAdjacentVertexes( size_t v ) const { return adjacencyList.at(v); }
    void AddEdge( int from, int to );
    void AddOrientedEdge( int from, int to );

private:
    std::vector<std::vector<int>> adjacencyList;
    size_t edgesCount;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inlines

inline void Graph::AddEdge(int from, int to)
{
    adjacencyList.at(from).push_back(to);
    adjacencyList.at(to).push_back(from);
    edgesCount += 1;
}

inline void Graph::AddOrientedEdge(int from, int to)
{
    adjacencyList.at( from ).push_back(to);
    edgesCount += 1;
}