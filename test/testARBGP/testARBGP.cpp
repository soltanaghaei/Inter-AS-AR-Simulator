/*

Created by: Zhe Song
Created at: Aug 2014

*/
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include "../../src/ARBGP.h"
#include "../../src/IPCE.h"
#include "../../src/GraphElements.h"
#include <limits>
#include <set>
#include <map>
#include <queue>
#include <list>
#include <string>
#include <vector>

using namespace std;


int main(int argc, char** argv)
{
if(argc != 2)
{
	cout<<"usage: " << argv[0] << " num_of_AS " << " call_arrival_rate " << " number_of_call_generated " << " output_file_name "<< " AR_whole_time "<<" lead_time" <<" single_slot_time "<<endl;
	return 0;
}
else
{
	int num_of_AS = atoi(argv[1]);
	vector<ARBGP> ARBGP_vector;
	///////read in files that contain the delays between different ASes
    string delay_file = "DelayFile";
    ifstream def(delay_file.c_str());
    int f_AS, t_AS;
    double delay;
    def >> num_of_AS;
    //create the object that contains the delay parameters
    DelayFile Delays(num_of_AS);
    while (def >> f_AS && def >> t_AS) {
        def >> delay;
        Delays.addDelay(f_AS, t_AS, delay);

    }
    ///////
    for (int as_num = 1; as_num <= num_of_AS; as_num++) {

/*
Per-AS create an ARBGP structure and a GRAPH structure. The GRAPH has all intra-AS edges and just sticking out the inter-AS links.
*/
        ARBGP testARBGP;
        GRAPH the_graph;
        int tmpasnum, num_vertices, node_u, node_v, edge_weight, edge_band, start_AS, end_AS;
        string start_AS_vertex;
        string end_AS_vertex;
        vector<Edge> intra_edges;
        vector<Inter_AS_Links> InterASLinks_table;
        stringstream asname;
// Here is where the AS input file is being read
        asname << "../../inputdata/AS" << as_num << "/AS" << as_num;
        string asfile = asname.str();
        //cout<<"AS "<<as_num<<" is up!"<<endl;
        //create an Graph module
        //Graph thegraph("AS1");
        ifstream inf(asfile.c_str());
        inf >> tmpasnum;
        inf >> num_vertices;
        testARBGP.num_vertices = num_vertices;
        while (inf >> node_u && node_u != 0) {
            if (node_u == -1) {
                break;
            }
            inf >> node_v;
            inf >> edge_weight;
            inf >> edge_band;
            Edge tmp;
            tmp.u = node_u;
            tmp.v = node_v;
            tmp.b = edge_band;
            the_graph.intra_edges.push_back(tmp);
        }
        while (inf >> start_AS_vertex) {
            int tmpindex = start_AS_vertex.find(":");
            start_AS = atoi(start_AS_vertex.substr(0, tmpindex).c_str());
            node_u = atoi(start_AS_vertex.substr((tmpindex + 1), start_AS_vertex.length() - tmpindex - 1).c_str());
            inf >> end_AS_vertex;
            tmpindex = end_AS_vertex.find(":");
            end_AS = atoi(end_AS_vertex.substr(0, tmpindex).c_str());
            node_v = atoi(end_AS_vertex.substr((tmpindex + 1), end_AS_vertex.length() - tmpindex - 1).c_str());
            inf >> edge_weight;
            inf >> edge_band;
            Inter_AS_Links inter_link_entry;
            inter_link_entry.from_AS = start_AS;
            inter_link_entry.to_AS = end_AS;
            inter_link_entry.start_vertex = node_u;
            inter_link_entry.end_vertex = node_v;
            //if want to change the strategy of assigning bandwidth to a predefined mode:like all inter_domain links are 
            //64Gbps, please modify here
            inter_link_entry.band = edge_band;
            // inter_link_entry.theLink.constructTable(windowSize,windowExt);
            the_graph.InterASLinks_table.push_back(inter_link_entry);
        }
        for (int i = 0; i < the_graph.InterASLinks_table.size(); i++) {
            //outgoing links
            if (the_graph.InterASLinks_table[i].from_AS == as_num) {
                testARBGP.addMCNAccess(the_graph.InterASLinks_table[i].to_AS, 1, the_graph.InterASLinks_table[i].start_vertex, the_graph.InterASLinks_table[i].end_vertex);
            } else {
                testARBGP.addMCNAccess(the_graph.InterASLinks_table[i].from_AS, 2, the_graph.InterASLinks_table[i].start_vertex, the_graph.InterASLinks_table[i].end_vertex);
            }
        }
        testARBGP.getMCNList();
        //AS_num starts from 1 while the index of AS_vector start from 0
        testARBGP.getMCNProcessNumber(-1);
        ARBGP_vector.push_back(testARBGP);
    }
        //test MCN_IDs

    for (int j = 0; j < ARBGP_vector.size(); j++) {
        for (int i = 0; i < ARBGP_vector[j].MCN_IDs.size(); i++)
            cout << ARBGP_vector[j].MCN_IDs[i] << " ";
        cout << endl;
    }
}
return 1;
}
