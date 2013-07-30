/************************************************
 Merges several tmp files into a single tmp file.
 by Dominik Schultes, April 2005

 see README.html for some notes on the usage
*************************************************/


#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>


using namespace std;


typedef unsigned int NodeID;
typedef unsigned int EdgeID;
typedef double EdgeWeight;
typedef pair<double, double> Pos;

// write nodes and edges separately to two different files ? 
const bool separate = false;


int main(int argc, char *argv[])    
{
    if ((argc != 4) && (argc != 2)) {
	cerr << "Wrong number of arguments." << endl;
	exit(-1);
    }

    vector<string> fileIn;
    string fileOut;
    if (argc == 4) {
	// Case 1: Merging two files, given as first and second argument
	fileIn.push_back(argv[1]);
	fileIn.push_back(argv[2]);
	fileOut = argv[3];
    }
    else {
	// Case 2: Merging several files, given as space-separated list (read from standard in)
	fileOut = argv[1];
	string filename;
	while ( cin >> filename ) {
	    fileIn.push_back( filename );
	}
    }
    
    int inputs = fileIn.size(); // number of input files

    ifstream in;
    vector< ifstream::pos_type > mark(inputs); // positions in the input files

    // open output file(s)
    ofstream out( fileOut.c_str() );
    if (! out) {
	cerr << "Output file cannot be opened." << endl;
	exit(-1);
    }

    ofstream out2;
    if (separate) {
	string fileOut2 = fileOut + "2";
	out2.open( fileOut2.c_str() );
	if (! out2) {
	    cerr << "Output file 2 cannot be opened." << endl;
	    exit(-1);
	}
	out2 << setprecision(20);
    }

    
    vector<NodeID> nIn(inputs); // number of nodes in each input
    NodeID nInTotal = 0;        // total number of nodes in the inputs
    NodeID n = 0;               // total number of distinct nodes

    vector<EdgeID> mIn(inputs); // number of edges in each input
    EdgeID m = 0;               // total number of edges in the inputs

    map< Pos, NodeID > nodes;   // at each position, there can be at most one node
    vector< Pos > positions;    // contains the positions of all nodes

    // For each input, the index of a node is its rank in the list of nodes, i.e.,
    // the first node in the file has index 0, the second one index 1 and so on.
    // The original id of a node is its id in the input.
    // A node is assigned a new id which is unique in the output. 
    vector< vector< NodeID > > mappingOrder(inputs);   // for each input and each node index, stores the new node id
    vector< map< double, NodeID > > mappingID(inputs); // for each input and each original node id, stores the new node id
    vector< vector< bool > > newNode(inputs); // for each input and each node index, TRUE iff this node is encountered for the first time
    double v, x, y; // original node id, node position (longitude, latitude)

    
    // READ NODES    
    
    for (int i = 0; i < inputs; i++) { // Read all input files, one after the other
	in.open( fileIn[i].c_str() );
	if (! in) {
	    cerr << "Input file " << i << " cannot be opened." << endl;
	    exit(-1);
	}
	
	in >> nIn[i]; // read number of nodes
	
	for (NodeID u = 0; u < nIn[i]; u++) { // process all nodes
	    in >> v >> x >> y; // read original node id and position
	    
	    Pos p(x, y);
	    if (nodes.count(p) == 0) { // node encountered for the first time ?
		nodes[p] = n++; // new id
		positions.push_back( p ); // store position
		newNode[i].push_back( true );  // set 'new' flag
	    }
	    else {
		newNode[i].push_back( false ); // set 'old' flag
	    }
	    mappingOrder[i].push_back( nodes[p] ); // map index -> new node id
	    mappingID[i][v] = nodes[p];            // map original node id -> new node id
	}
	cout << "File " << i << " (" << fileIn[i] << "): " << nIn[i] << " nodes read." << endl;
	nInTotal += nIn[i]; // sum up number of nodes in the inputs

	in >> mIn[i]; // read number of edges
	m += mIn[i];  // sum up number of edges in the inputs
	mark[i] = in.tellg(); // mark position in the input file (in order to read the edges later)
	in.close();
    }

    cout << nInTotal << " nodes read from " << inputs << " files." << endl;


    // WRITE NODES

    out << setprecision(20) << n << endl; // write number of distinct nodes
    for (int i = 0; i < inputs; i++) {
	for (NodeID u = 0; u < nIn[i]; u++) {
	    if ( ! newNode[i][u] ) continue; // skip 'old' nodes
	    NodeID newNodeID = mappingOrder[i][u]; // get new node id
	    out << newNodeID << " " << positions[newNodeID].first << " " << positions[newNodeID].second << endl; // write node
	}
    }
    cout << n << " nodes written." << endl;
    cout << "[" << (nInTotal - n) << " overlapping nodes]" << endl;


    // READ AND WRITE EDGES
    
    NodeID s, t; // source and target nodes
    double k1, k2, k3; // keys

    if (separate) out2 << m << endl; else out << m << endl; // write number of edges
    for (int i = 0; i < inputs; i++) {
	in.open( fileIn[i].c_str() );
	in.seekg( mark[i] ); // retrieve position in the file where the list of edges starts
	for (EdgeID e = 0; e < mIn[i]; e++) {
	    in >> s >> t >> k1 >> k2 >> k3; // read edge

	    // write edge (map old node ids to new node ids)
	    if (separate) {
		out2 << mappingID[i][s] << " " << mappingID[i][t] << endl
		     << k1 << " " << k2 << " " << k3 << endl;
	    }
	    else {
		out << mappingID[i][s] << " " << mappingID[i][t] << endl
		    << k1 << " " << k2 << " " << k3 << endl;
	    }
	}
	cout << "File " << i << ": " << mIn[i] << " edges read." << endl;
	in.close();
    }
    cout << m << " edges written." << endl << endl;
}
