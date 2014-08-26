/*

Created by: Zhe Song
Created at: Aug 2014

*/
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include "../../src/CallGenerator.h"
#include "../../src/randgen.h"
#include <limits>
#include <set>
#include <map>
#include <queue>
#include <list>
#include <string>
#include <vector>

// Input variable that need to be changed if locations/names are // modified.

using namespace std;


int main(int argc, char** argv)
{
if(argc != 8)
{
	cout<<"usage: " << argv[0] << " AS_num " << " call_arrival_rate " << " number_of_call_generated " << " output_file_name "<< " AR_whole_time "<<" lead_time" <<" single_slot_time "<<endl;
	return 0;
}
else
{
	int ASnum = atoi(argv[1]); 
	double arrival_rate = atof(argv[2]);
	int number_of_call_generated = atoi(argv[3]);
	string output_file_name = argv[4];
	int AR_whole_time = atoi(argv[5]);
	int lead_time = atoi(argv[6]);
	int single_slot_time = atoi(argv[7]);
	CallGenerator testcallGen(ASnum);
	//please change the following three lines after you make changes to readNodeVector(string dir) function
	testcallGen.readNodeVector("../../src/common/nodenum");
    testcallGen.readCommonFile("../../src/common/common_parameter");
    testcallGen.readMatrix("../../src/common/src_dst_prob_matrix");
    double arrival_time = expon(arrival_rate);
    testcallGen.generateCall(arrival_time,AR_whole_time,lead_time,single_slot_time,1);
    ofstream ofs(output_file_name.c_str());
    if(ofs.is_open())
    {
    	ofs << arrival_time << " ";
    	for(int i = 0; i < testcallGen.ARvec.size(); i++)
    		ofs << testcallGen.ARvec[i] << " ";
    	ofs << testcallGen.Duration << " " << testcallGen.Capacity << " " << ASnum << " " << testcallGen.source_node << " " << testcallGen.dest_AS << " " << testcallGen.dest_node<<endl; 
    	for(int j = 2; j <= number_of_call_generated; j++)
    	{
    		arrival_time += expon(arrival_rate);
    		testcallGen.generateCall(arrival_time,AR_whole_time,lead_time,single_slot_time,1);
    		ofs << arrival_time << " ";
    		for(int i = 0; i < testcallGen.ARvec.size(); i++)
    			ofs << testcallGen.ARvec[i] << " ";
    		ofs << testcallGen.Duration << " " << testcallGen.Capacity << " " << ASnum << " " << testcallGen.source_node << " " << testcallGen.dest_AS << " " << testcallGen.dest_node<<endl; 
    	}
    }
    else cout << "Unable to open file"<<endl;
}
return 1;
}
