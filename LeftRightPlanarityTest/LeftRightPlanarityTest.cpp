// Author: Vladislav Vorotilov
// This code can be used for educational purposes only
// Article about algorithm can be found here:
//  http://algo.uni-konstanz.de/publications/b-lrpt-sub.pdf

#include "LeftRightPlanarityTest.h"
#include <cassert>


LeftRightPlanarityTest::LeftRightPlanarityTest(const Graph& _graph) :
	graph( _graph )
{
}

bool LeftRightPlanarityTest::IsPlanar()
{
	const size_t vertexesCount = graph.Size();
	const size_t edgesCount = graph.EdgesCount();
	// too dense check
	if( vertexesCount > 2 && edgesCount > ( 3 * vertexesCount - 6 ) ) {
		return false;
	}

	// Orientation
	std::vector<int> roots;
	prepareDfs1();
	for( int v = 0; v < vertexesCount; ++v ) {
		if( !isReached(v) ) {
			roots.push_back( v );
			dfs1(v);
		}
	}
	//testing
	try {
		//sort adjacency lists according to non-decreasing nesting depth
		prepareDfs2();
		for( int s : roots ) {
			dfs2(s);
		}
	} catch ( not_planar_exception& e ) {
		return false;
	}

	return true;
}

const int LeftRightPlanarityTest::Inf = 50000;

void LeftRightPlanarityTest::prepareDfs1()
{
	const size_t size = graph.Size();
	height.assign( size, Inf );
	parent.assign( size, -1 );
}

void LeftRightPlanarityTest::dfs1(int v)
{
	assert( height[v] = Inf );
	height[v] = 0;
	doDfs1(v);
}

void LeftRightPlanarityTest::doDfs1(int v)
{
	Edge u_v( parent[v], v);
	for( const int w: graph.GetAdjacentVertexes(v) ) {
		if( orientation.find(Edge(w, v) ) != orientation.end() ) {
			continue;
		}
		Edge v_w(v, w);
		orientation.insert(v_w);
		if( height[w] == Inf ) { // tree edge
			parent[w] = v;
			height[w] = height[v] + 1;
			doDfs1(w);
		} else { // back edge
			lowPoint[v_w] = height[w];
		}
		// determine nesting depth
		if( lowPoint.find(v_w) != lowPoint.end() ) {
			nesting_depth[v_w] = 2 * lowPoint[v_w];
			auto lp2FindResult = lowPoint2.find(v_w);
			if (lp2FindResult != lowPoint2.end() && lp2FindResult->second < height[v]) { // chordal
				nesting_depth[v_w] += 1;
			}
		}
		// update lowpoints of u_v
		if( u_v.first != -1 ) { // not root
			if( lowPoint.find(u_v) == lowPoint.end() ) {
				lowPoint[u_v] = lowPoint[v_w];
				if( lowPoint2.find(v_w) != lowPoint2.end() ) {
					lowPoint2[u_v] = lowPoint2[v_w];
				}
			} else {
				if (lowPoint[v_w] < lowPoint[u_v]) {
					lowPoint2[u_v] = lowPoint2.find(v_w) != lowPoint2.end() ?
									 std::min(lowPoint[u_v], lowPoint2[v_w]) : lowPoint[u_v];
					lowPoint[u_v] = lowPoint[v_w];
				} else if (lowPoint[v_w] > lowPoint[u_v]) {
					lowPoint2[u_v] = lowPoint2.find(u_v) != lowPoint2.end() ?
									 std::min(lowPoint2[u_v], lowPoint[v_w]) : lowPoint[v_w];
				} else {
					if( lowPoint2.find(u_v) != lowPoint2.end() ) {
						if( lowPoint2.find(v_w) != lowPoint2.end() ) {
							lowPoint2[u_v] = std::min(lowPoint2[u_v], lowPoint2[v_w]);
						}
					} else if ( lowPoint2.find( v_w ) != lowPoint2.end() ) {
						lowPoint2[u_v] = lowPoint2[v_w];
					}
				}
			}
		}
	}
}

bool LeftRightPlanarityTest::isReached(int v) const { return height[v] != Inf; }

void LeftRightPlanarityTest::prepareDfs2() {
	reorderedGraph.reset( new Graph( graph.Size() ) );
	vector<vector<pair<int, int>>> newEdges(graph.Size());
	for( const auto& v : nesting_depth ) {
		const int from = v.first.first;
		const int to = v.first.second;
		const int order = v.second;
		newEdges[from].push_back( make_pair( order, to ) );
	}
	for( int i = 0; i < newEdges.size(); ++i ) {
		sort(newEdges[i].begin(), newEdges[i].end(), less<std::pair<int,int>>());
		for( const auto& v : newEdges[i] ) {
			reorderedGraph->AddOrientedEdge( i, v.second );
		}
	}
	orientation.clear();
}

void LeftRightPlanarityTest::dfs2(int v)
{
	const int u = parent[v];
	Edge u_v( u, v);
	for( int i = 0; i < reorderedGraph->GetAdjacentVertexes(v).size(); ++i ) {
		const int w = reorderedGraph->GetAdjacentVertexes(v)[i];
		Edge v_w(v, w);
		if( !S.empty() ) {
			stack_bottom[v_w] = S.top();
		}
		if( parent[w] == v ) { //tree edge
			dfs2( w );
		} else { // back edge
			lowpt_edge[v_w] = v_w;
			S.push( ConflictPair(Interval(), Interval(v_w, v_w)));
		}
		// integrate new return edges
		if( lowPoint[v_w] < height[v] ) { // v_w has return edge
			if( i == 0 ) {
				lowpt_edge[u_v] = v_w;
			} else {
				addEdgeConstraint( v_w );
			}
		}
	}
	// remove back edges returning to parent
	if( u != -1 ) {
		trimBackEdgesEndingAtParent(u);
		// side of u_v is side of a highest return edge
		if( lowPoint[u_v] < height[u] && !S.empty() ) { /* e has return edge */
			Edge h_L = S.top().L().high();
			Edge h_R = S.top().R().high();
			if( h_L.first != -1 && ( h_R.first != -1 || lowPoint[h_L] > lowPoint[h_R] ) ) {
				ref[u_v] = h_L;
			} else {
				ref[u_v] = h_R;
			}
		}
	}
}

void LeftRightPlanarityTest::addEdgeConstraint(const Edge &v_w)
{
	const int v = v_w.first;
	const Edge u_v( parent[v], v);
	ConflictPair P;
	// merge return edges of e into P.R()
	do {
		ConflictPair Q = S.top(); S.pop();
		if( !Q.L().IsEmpty() ) {
			std::swap( Q.L(), Q.R() );
		}
		if( !Q.L().IsEmpty() ) {
			throw not_planar_exception();
		} else {
			if( lowPoint[Q.R().low()] > lowPoint[u_v] ) { // merge intevals
				if (P.R().IsEmpty()) { // topmost interval
					P.R().high() = Q.R().high();
				} else {
					ref[P.R().low()] = Q.R().high();
				}
				P.R().low() = Q.R().low();
			} else { // align
				ref[Q.R().low()] = lowpt_edge[u_v];
			}
		}
	} while( !S.empty() && S.top() != stack_bottom[v_w] );
	// merge conflicting return edges of e1,...,ei−1 into P.L()
	while( !S.empty() && (conflicting(S.top().L(), v_w ) || conflicting( S.top().R(), v_w ) ) ) {
		ConflictPair Q = S.top(); S.pop();
		if( conflicting( Q.R(), v_w ) ) {
			std::swap( Q.L(), Q.R() );
		}
		if( conflicting(Q.R(), v_w ) ) {
			throw not_planar_exception();
		} else { // merge interval below lowPoint(e) into P.R
			ref[P.R().low()] = Q.R().high();
			if( !Q.R().low().IsEmpty() ) {
				P.R().low() = Q.R().low();
			}
		}
		if( P.L().IsEmpty() ) { // topmost interval
			P.L().high() = Q.L().high();
		} else {
			ref[P.L().low()] = Q.L().high();
		}
		P.L().low() = Q.L().low();
	}
	if( !P.IsEmpty() ) {
		S.push( P );
	}

}

void LeftRightPlanarityTest::trimBackEdgesEndingAtParent(int u)
{
	//drop entire conflict pairs
	while( !S.empty() && lowest(S.top()) == height[u] ) {
		ConflictPair P = S.top(); S.pop();
		if( !P.L().low().IsEmpty() ) {
			side[P.L().low()] = -1;
		}
	}
	if( !S.empty() ) { // one more conflict pair to consider
		ConflictPair P = S.top(); S.pop();
		// trim left interval
		while ( !P.L().high().IsEmpty() && P.L().high().second == u ) {
			P.L().high() = ref[P.L().high()];
		}
		if( P.L().high().IsEmpty() && !P.L().low().IsEmpty() ) { // just emptied
			ref[P.L().low()] = P.R().low();
			side[P.L().low()] = -1;
			P.L().low().MakeEmpty();
		}
		// trim right interval
		S.push( P );
	}
}

bool LeftRightPlanarityTest::conflicting(const Interval &I, const Edge &b)
{
	return !I.IsEmpty() && lowPoint[I.high()] > lowPoint[b];
}

int LeftRightPlanarityTest::lowest(const LeftRightPlanarityTest::ConflictPair &P) {
	if( P.L().IsEmpty() ) {
		return lowPoint[P.R().low()];
	}
	if( P.R().IsEmpty() ) {
		return lowPoint[P.L().low()];
	}
	return std::min(lowPoint[P.L().low()], lowPoint[P.R().low()]);
}
