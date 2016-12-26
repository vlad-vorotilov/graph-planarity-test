// Author: Vladislav Vorotilov
// This code can be used for educational purposes only

#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <cassert>
#include "Graph.h"
#include "LeftRightPlanarityTest.h"

using namespace std;

void test( const Graph& g, const string testName, const bool isPlanar )
{
	cout << "test: \"" << testName << "\"";
	if( LeftRightPlanarityTest( g ).IsPlanar() == isPlanar ) {
		cout << " passed" << endl;
	} else {
		cout << " failed" << endl;
	}
}

int main() {
	
	{ // K_3_3 not planar
		Graph g( 6 );
		g.AddEdge(0, 3);
		g.AddEdge(0, 4);
		g.AddEdge(0, 5);
		g.AddEdge(1, 3);
		g.AddEdge(1, 4);
		g.AddEdge(1, 5);
		g.AddEdge(2, 3);
		g.AddEdge(2, 4);
		g.AddEdge(2, 5);

		test(g, "K_3_3 not planar", false );
	}

	{ // K_5 not planar
		Graph g( 6 );
		g.AddEdge( 0, 1 );
		g.AddEdge( 0, 2 );
		g.AddEdge( 0, 3 );
		g.AddEdge( 0, 4 );
		g.AddEdge( 1, 2 );
		g.AddEdge( 1, 3 );
		g.AddEdge( 1, 4 );
		g.AddEdge( 2, 3 );
		g.AddEdge( 2, 4 );
		g.AddEdge( 3, 4 );

		test( g, "K_5 not planar", false );
	}

	{ // planar
		Graph g( 6 );
		g.AddEdge( 0, 1 );
		g.AddEdge( 0, 2 );
		g.AddEdge( 1, 3 );
		g.AddEdge( 2, 3 );
		g.AddEdge( 3, 4 );
		g.AddEdge( 2, 4 );
		g.AddEdge( 4, 5 );
		g.AddEdge( 5, 2 );
		g.AddEdge( 5, 0 );
		test( g, "simple planar", true );
	}

	{ // almost K_5 planar
		Graph g( 5 );
		g.AddEdge( 0, 1 );
		g.AddEdge( 0, 2 );
		g.AddEdge( 0, 3 );
		g.AddEdge( 0, 4 );
		g.AddEdge( 1, 2 );
		g.AddEdge( 1, 4 );
		g.AddEdge( 2, 0 );
		g.AddEdge( 2, 3 );
		g.AddEdge( 2, 4 );

		test( g, "almost K_5 planar", true );
	}

	return 0;
}
