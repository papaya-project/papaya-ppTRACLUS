/**
 File client.cpp
 Authors: Zied Becha (becha@eurecom.fr) 
 Description: Client-side ABY party for PP-Trajectory Clustering
**/

#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include "ABY/src/abycore/aby/abyparty.h"

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <float.h>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <bits/stdc++.h> 
using namespace std;

#include "common/pptraclus.h"

//Function used to extract points from the data set 
//Dataset have 4 points in each line delimited by ","
//Output can be used this way 
//line_segments[line][point]
std::vector<std::vector<uint64_t>> readLineSegments(string path, int begin, int end){
  std::vector<std::vector<uint64_t>> line_segments;
  int counter = 0;
  std::string line;
  ifstream infile (path);
  // int comma_counter = 0
  while (std::getline(infile, line))
  {
      // cout << line << endl;
      if (counter < begin){
        counter++;
        continue;
      }
      std::vector<uint64_t> coordinates;
      std::istringstream iss(line);
      std::string delimiter = ",";

      size_t pos = 0;
      std::string token;
      while ((pos = line.find(delimiter)) != std::string::npos) {
          token = line.substr(0, pos);
          
          coordinates.push_back(stoull(token));
          
          line.erase(0, pos + delimiter.length());
      }
      pos = line.find("\n");
      line = line.substr(0, pos);
      coordinates.push_back(stoull(line));
    
      line_segments.push_back(coordinates);
      counter ++;
      if (counter >= end)
          break;
  }
  return line_segments;
}


void read_test_options(int32_t* argcp, char*** argvp,
   string* address, uint16_t* port ,string* datasetpath,uint32_t* minLns, uint64_t* threshold
   ,uint32_t* end, uint32_t* mode,uint32_t* itera) {

  uint32_t int_port = 0;
  double int_threshold;
  parsing_ctx options[] =
  {
  {(void*) mode, T_NUM, "w", "Mode (0/1/2)", true, false },
  {(void*) minLns, T_NUM, "m", "minLns", false, false },
  {(void*) &int_threshold, T_DOUBLE, "t", "epsilon", false, false },
  {(void*) datasetpath, T_STR, "i", "Dataset path", false, false },
  {(void*) end, T_NUM, "e", "Dataset end", true, false },
  {(void*) address, T_STR, "a", "IP-address, default: localhost", false, false },
  {(void*) &int_port, T_NUM, "p", "Port, default: 7766", false, false },
  {(void*) itera, T_NUM, "l", "Iterations (1/2/3/4)", false, false }

  };

  if (!parse_options(argcp, argvp, options,
    sizeof(options) / sizeof(parsing_ctx))) {
    print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
  cout << "Exiting" << endl;  
  exit(0);

  }


  if (int_port != 0) {
    assert(int_port < 1 << (sizeof(uint16_t) * 8));
    *port = (uint16_t) int_port;
  }

  if (int_threshold != 0) {
    assert(int_threshold < 1 << (sizeof(uint64_t) * 8));
    *threshold = (uint64_t) int_threshold;
  }

}

int mode1(string path, int begin, int end, const string& address, uint16_t port, seclvl seclvl, uint32_t nthreads,
  e_mt_gen_alg mt_alg,uint32_t nvals, e_sharing sharing,uint32_t itera){

  std::vector<std::vector<uint64_t>> lineSegments = readLineSegments(path,begin, end);

  string line;
  uint64_t threshold;
  ifstream tmyfile ("threshold.csv");
  if (tmyfile.is_open())
  {
    while ( getline (tmyfile,line))
    {
      threshold=stoull(line);
    }
    tmyfile.close();
  }

  else cout << "Unable to open file"; 


 
  ifstream mmyfile ("minLns.csv");
  uint32_t minLns;
  if (mmyfile.is_open())
  {
    while ( getline (mmyfile,line) )
    {
      minLns=stoull(line);
    }
    mmyfile.close();
  }

  else cout << "Unable to open file"; 


  construct(CLIENT, address, port, seclvl, nthreads,mt_alg, lineSegments,
   end, sharing, minLns, threshold,itera);

  return 0;
}

int mode2(int end, const string& address, uint16_t port, seclvl seclvl, uint32_t nthreads,
  e_mt_gen_alg mt_alg,uint32_t nvals, e_sharing sharing){


   uint32_t* cluster = new uint32_t[end];
  fstream newfile;
  newfile.open("cluster.csv",ios::in); //open a file to perform read operation using file object
   int i =0;
   if (newfile.is_open()){   //checking whether the file is open
      string tp;
      while(getline(newfile, tp)){ //read data from file object and put it into string.
         cluster[i]=stoul(tp);
         i++;
      }
     newfile.close(); //close the file object.

}

  recoverCluster(CLIENT, address, port, seclvl, nthreads,mt_alg, cluster,end,sharing);

  return 0;
}



int main(int argc, char** argv) {
  
  // Setup parameters
  string address = "127.0.0.1" ;
  uint16_t port = 6677;
  seclvl seclvl = get_sec_lvl (128) ;
  e_sharing sharing = S_BOOL ;
  uint32_t bitlen = 32;
  uint32_t nthreads = 1;
  string datasetpath = "";
  uint32_t begin = 0;
  uint32_t end = 1000;
  uint32_t mode=0;
  uint32_t minLns = 0;
  uint64_t threshold=100;
  uint32_t itera = 2;
  e_mt_gen_alg mt_alg = MT_OT;

  read_test_options(&argc, &argv, &address,
    &port,&datasetpath,&minLns, &threshold, &end, &mode, &itera);

  

  minLns=minLns-1;

  switch (mode) {
        case 0: {
          std::vector<std::vector<uint64_t>> lineSegments = readLineSegments(datasetpath,begin, end);
          pptraclus(CLIENT, address, port, seclvl, nthreads,mt_alg, lineSegments,end,sharing,minLns,threshold);
          break;
        }
        case 1: {
          mode1(datasetpath,begin,end, address, port, seclvl, nthreads, mt_alg, end, sharing,itera);
          break;}
        case 2: {
          mode2(end, address, port, seclvl, nthreads, mt_alg,end, sharing);
          break;}

    }

  
    


  }