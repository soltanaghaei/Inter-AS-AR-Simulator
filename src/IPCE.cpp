/* 
 * File:   IPCE.cpp
 * Author: leo
 * revised: Elahe
 * Created on June 27, 2014, 12:22 AM
 */

#include "IPCE.h"
#include <vector>
#include <string>
#include "intradijkstra.h"

IPCE::IPCE(){}

IPCE::IPCE(int reservationWindowSize, int lead_time, int numNode) {
    windowSizeTimeslot = reservationWindowSize;
    ARleadtime = lead_time;
    num_nodes=numNode;
}


//construct a new table which keeps available bandwidth of each link in different time slotes
void IPCE::readTopology(vector<Intra_Link> intra_links) {
    //for each entry of intra_links table
    for (int i = 0; i < intra_links.size(); i++) {
        int node1 = intra_links[i].start_node;
        int node2 = intra_links[i].end_node;
        //create an index based on start and end nodes
        int index = node1 * 1000 + node2;
        linkAvailableBandwithTable tmp;
        tmp.setBandandWeight(intra_links[i].bandwidth, intra_links[i].weight);
        
        tmp.constructResTable(windowSizeTimeslot, ARleadtime);
        
        intraASLinksAR[index] = tmp;
    }
}

bool IPCE::findPathMulShortestPossibleAndReserv(int source_node, int dest_node, double capacity, int duration, vector<int> ARvec) {
    
    intradijkstra G;
    //cout << "find path in IPCE " << endl;
    pathvector.clear();
    int turn=0;
    selectedOptionIndex=-1;
    pathLength=-1;
    for (int i = 0; i < ARvec.size(); i++) {
        
        G.read(source_node, dest_node, num_nodes, intraASLinksAR, ARvec[i], ARvec[i] + duration, capacity);
 
        
        G.calculateDistance();
        

        //check if a path is found
        if (G.flag == 0)
            //no path between these two nodes
            continue;
        else {
            
            G.output();
            turn++;
            if(turn==1)
            {
                pathvector=G.pathvector;
                selectedOptionIndex=i;
                pathLength=G.pathvector.size()-1;
            }
            else if(pathvector.size()> G.pathvector.size())
            {   pathvector=G.pathvector;
                selectedOptionIndex=i;
                pathLength=G.pathvector.size();
            }
            
        }
    }
    if(selectedOptionIndex!=-1)
    {
        reserveCall(pathvector, ARvec[selectedOptionIndex], ARvec[selectedOptionIndex] + duration, capacity);
        return true;
    }else
        return false;

}

bool IPCE::findPathShortestEarliestAndReserv(int source_node, int dest_node, double capacity, int duration, vector<int> ARvec) {
    
    intradijkstra G;
    //cout << "find path in IPCE " << endl;
    pathvector.clear();
    int turn=0;
    selectedOptionIndex=-1;
    pathLength=-1;
    //For The first scenario to find the first possible path
    for (int i = 0; i < ARvec.size(); i++) {
        //cout <<source_node << " " << dest_node << " " << num_nodes << " " << ARvec[i] << " " << ARvec[i] + duration << " " << capacity <<endl;
        G.readForShortest(source_node, dest_node, num_nodes, intraASLinksAR, ARvec[i], ARvec[i] + duration, capacity);
 
        
        G.calculateDistance();
        
        
                
        //check if a path is found
        if (G.flag == 0)
            //no path between these two nodes
            continue;
        else {
            G.output();
            if(G.checkAvailability(intraASLinksAR, ARvec[i], ARvec[i] + duration, capacity)==1)
                G.flag==0;
            else
            {
                
                selectedOptionIndex=i;
                pathLength=G.pathvector.size()-1;
                pathvector=G.pathvector;
                reserveCall(G.pathvector, ARvec[i], ARvec[i] + duration, capacity);
                return true;
            }
        }
    }
    return false;
}


bool IPCE::findPathPossibleShortEarliestAndReserv(int source_node, int dest_node, double capacity, int duration, vector<int> ARvec) {
    
    intradijkstra G;
    //cout << "find path in IPCE " << endl;
    pathvector.clear();
    int turn=0;
    selectedOptionIndex=-1;
    pathLength=-1;
    //For The first scenario to find the first possible path
    for (int i = 0; i < ARvec.size(); i++) {
        //cout <<source_node << " " << dest_node << " " << num_nodes << " " << ARvec[i] << " " << ARvec[i] + duration << " " << capacity <<endl;
        G.read(source_node, dest_node, num_nodes, intraASLinksAR, ARvec[i], ARvec[i] + duration, capacity);
 
        
        G.calculateDistance();
                
        //check if a path is found
        if (G.flag == 0)
            //no path between these two nodes
            continue;
        else {
            G.output();
                            
            selectedOptionIndex=i;
            pathLength=G.pathvector.size()-1;
            pathvector=G.pathvector;
            reserveCall(G.pathvector, ARvec[i], ARvec[i] + duration, capacity);
            return true;
            
        }
    }
    return false;
}


//TODO: Capacity for the transmission rate of a link bps
//TODO: "Rate" for rate requested for the call bps


//TODO: check it to work correctly
bool IPCE::reserveCall(vector<int> pathVector, int start_time, int end_time, double capacity) {
    
    for (int i = 0; i < pathVector.size() - 1; i++) {

        for (int j = start_time; j <= end_time; j++) {
            /*if(pathVector[i+1]==1000)
            {cout << "pathV " <<pathVector.size() << " " << i <<endl;
            
            cout << "ver1: " <<pathVector[i]<<endl;
            cout << "ver2: " << pathVector[i+1]<<endl;
            }
             */
            int vertex1 = pathVector[i];
            int vertex2 = pathVector[i+1];
            
            intraASLinksAR[vertex1 * 1000 + vertex2].availableBandwidthTable[j] -= capacity;
            intraASLinksAR[vertex2 * 1000 + vertex1].availableBandwidthTable[j] -= capacity;
        }
    }

}




