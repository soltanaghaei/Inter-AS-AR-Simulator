/* 
 * File:   intradijkstra.cpp
 * Author: leo
 * 
 * Created on March 24, 2013, 2:42 AM
 */

#include "intradijkstra.h"
#include<iostream>
//#include "p.h"
#include "GraphElements.h"
#define INFINITY 999

/*
 * Description:  This class does an intra-domain shortest path algorithm with available bandwidth constrains 
 */



/*
 * Input: starting: the starting node number; ending: the ending node number; edgedata: the reference of a map of EdgeTable, please see GraphElements.h to understand the data structure of edgedata object, 
 * generally speaking this is a data structure for each edge in the intra-domain topology with advance reservation window integrated; start: the starting time slot; end: the ending time slot; capacity: the requested capacity(Gbps)
 * 
 * Job description: This function reads in the edge data structure and fills a 2D matrix. The row(from) and column(to) number of the matrix correspond to the two endpoints of an edge
 * The value of each element in the matrix equals to the available bandwidth and for those pairs which are not directly connected, the value equals to INFINITY. If the available bandwidth
 * on certain edges(links) is less than capacity, then the value will also be set to INFINITY.
 * The matrix is used for computing a valid shortest path by using Dijkstra algorithm
 */
void intradijkstra::read(int starting, int ending, int vnum, map<int, EdgeTable>& edgedata, int start, int end, double capacity) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            adjMatrix[i][j] = INFINITY;
        }
    }
    numOfVertices = vnum;
    for (std::map<int, EdgeTable>::iterator iter = edgedata.begin(); iter != edgedata.end(); ++iter) {
        int from_node = iter->first / 1000;
        int to_node = iter->first % 1000;
        w = 10;
        for (int k = start; k <= end; k++) {
            if (iter->second.reservationTable[k] < capacity) {
                w = INFINITY;
                break;
            }
        }
        adjMatrix[from_node-1][to_node-1] = w;
    }
    source = starting-1;
    dest = ending-1;
}


/*
 * This is another version of the above read()function. Here the input parameter capacity is replaced by capacity_rate. So the function, instead of using a fixed capacity value to determine the connectivity of edges, becomes
 * disconnecting those edges(links) whose available bandwidth is less than capacity_rate of their full capacity
 */
void intradijkstra::readR(int starting, int ending, int vnum, map<int, EdgeTable>& edgedata, int start, int end, double capacity_rate) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            adjMatrix[i][j] = INFINITY;
        }
    }
    numOfVertices = vnum;
    for (std::map<int, EdgeTable>::iterator iter = edgedata.begin(); iter != edgedata.end(); ++iter) {
        int from_node = iter->first / 1000;
        int to_node = iter->first % 1000;
        w = 10;
        for (int k = start; k <= end; k++) {
            if (iter->second.reservationTable[k] < capacity_rate*iter->second.bandwidth) {
                w = INFINITY;
                break;
            }
        }
        adjMatrix[from_node-1][to_node-1] = w;
    }
    source = starting-1;
    dest = ending-1;
}
/*
void intradijkstra::recreate(int start, map<int, EdgeTable>& edgedata, int allcount, int end, int capacity) {
    ecount = allcount;
    theallcount = allcount;
    for (i = 0; i < MAX; ++i) {
        deletable[i] = true;
    }
    for (int j = 0; j < allcount; ++j) {
        for (int k = start; k <= end; k++) {
            if (reservwindow[j][k] < capacity)
                deletable[j] = false;
            ecount--;
        }
    }

}
*/
/*
 * initialize several vectors used by the algorithm
 */
void intradijkstra::initialize() {
    for (int i = 0; i < numOfVertices; i++) {
        mark[i] = false;
        predecessor[i] = -1;
        distance[i] = INFINITY;
    }
    distance[source] = 0;
}

/*
 * called by other inner functions
 */

int intradijkstra::getClosestUnmarkedNode() {
    int minDistance = INFINITY;
    int closestUnmarkedNode;
    for (int i = 0; i < numOfVertices; i++) {
        if ((!mark[i]) && (minDistance >= distance[i])) {
            minDistance = distance[i];
            closestUnmarkedNode = i;
        }
    }
    return closestUnmarkedNode;
}

/*
 * compute the shortest path
 */
void intradijkstra::calculateDistance() {
    //cout<<"do cal!"<<endl;
  //      cout<<"pred:";
        
  //  for(int i = 0; i < MAX; i++)
  //  {
  //      cout<< predecessor[i]<<" ";
  //  }
    initialize();
    int minDistance = INFINITY;
    int closestUnmarkedNode;
    int count = 0;
    while (count < numOfVertices) {
        closestUnmarkedNode = getClosestUnmarkedNode();
        mark[closestUnmarkedNode] = true;
        for (int i = 0; i < numOfVertices; i++) {
            if ((!mark[i]) && (adjMatrix[closestUnmarkedNode][i] > 0)) {
                if (distance[i] > distance[closestUnmarkedNode] + adjMatrix[closestUnmarkedNode][i]) {
                    distance[i] = distance[closestUnmarkedNode] + adjMatrix[closestUnmarkedNode][i];
                    predecessor[i] = closestUnmarkedNode;
                }
            }
        }
        count++;
    }
  //  cout<<"pred:";
  //  for(int i = 0; i < MAX; i++)
  //  {
   //     cout<< predecessor[i]<<" ";
   // }
         
}

/*
 * called by output()
 */
void intradijkstra::printPath(int node) {
    //cout<<node<<"..";
    if (node == source) {
         //cout<<node<<"..";
        pathvector.push_back(node);
        flag = 1;
    } else if (predecessor[node] == -1) {
        cout << "No path from " << source+1 << "to" << node+1 << endl;
        flag = 0;
    } else {

        printPath(predecessor[node]);
        //cout<<node<<"..";
        pathvector.push_back(node);
    }
}

/*
 * output the shortest path. If no valid path is found, then print 'no path' information(by printPaht() function)
 */
void intradijkstra::output() {
    pathvector.clear();
/*
    for (int i = 0; i < numOfVertices; i++) {
        pathvector.clear();
        if (i == source) {
            //cout<<source<<".."<<source;
           // pathvector.push_back(source);
            pathvector.push_back(source);
        } else
            printPath(i);
        //for (int j = 0; j < pathvector.size(); j++)
        //    cout << pathvector[j] << "--";
       // cout << "->" << distance[i] << endl;
    }
*/
    printPath(dest);
    for (int j = 0; j < pathvector.size(); j++)
        pathvector[j]+=1;
    for (int j = 0; j < pathvector.size(); j++)
        cout << pathvector[j] << "--";
    cout << "->" << distance[dest] << endl;
}


