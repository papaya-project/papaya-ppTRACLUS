#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <abycore/aby/abyparty.h>
#include <abycore/circuit/share.h>
#include <abycore/circuit/booleancircuits.h>
#include <abycore/circuit/arithmeticcircuits.h>
#include <math.h>

#include <abycore/sharing/sharing.h>
#include <cassert>
#include <iomanip>
#include <iostream>
#include "pptraclus.h"
using namespace std;

uint32_t MAX_CLUSTERS = UINT32_MAX;

VerificationLineSegment::VerificationLineSegment() = default;

share* BuildEuclidDistanceCircuit(share *s_x1, share *s_x2, share *s_y1,
    share *s_y2, BooleanCircuit *bc) {

  share* check_sel,
      *check_sel_inv;

  share *res_x;
  share *res_y;
  share *out;
  share *t_a;
  share *t_b;


  /** Following code performs (x2-x1)*(x2-x1) */
  check_sel = bc->PutGTGate(s_x1, s_x2);
  check_sel_inv = bc->PutINVGate(check_sel);
  t_a = bc->PutMUXGate(s_x1, s_x2, check_sel);
  t_b = bc->PutMUXGate(s_x1, s_x2, check_sel_inv);
  res_x = bc->PutSUBGate(t_a, t_b);
  res_x->set_max_bitlength(uint32_t(64));
  res_x = bc->PutMULGate(res_x, res_x);
 
 
  /** Following code performs (y2-y1)*(y2-y1) */
  check_sel = bc->PutGTGate(s_y1, s_y2);
  check_sel_inv = bc->PutINVGate(check_sel);
  t_a = bc->PutMUXGate(s_y1, s_y2, check_sel);
  t_b = bc->PutMUXGate(s_y1, s_y2, check_sel_inv);
  res_y = bc->PutSUBGate(t_a, t_b);
  res_y->set_max_bitlength(uint32_t(64));
  res_y = bc->PutMULGate(res_y, res_y);
  
  /** Following code performs out = res_y + res_x*/
  out = bc->PutADDGate(res_x, res_y);

  return out;
}

share* buildmatrix(share *s_x1_b,share *s_x1_e,
  share *s_y1_b,share *s_y1_e,BooleanCircuit *bc ,uint32_t nvals,share* s_threshold,uint32_t i){

  //std::vector<share* > ss_out (nvals);
  share *s_out;

  //for (uint32_t i=0;i<nvals;i++){
  share* ss_x2_b= bc-> PutRepeaterGate(i+1,bc->PutSplitterGate(bc->PutCombineAtPosGate(s_x1_b,i)));
  share* ss_x2_e= bc-> PutRepeaterGate(i+1,bc->PutSplitterGate(bc->PutCombineAtPosGate(s_x1_e,i)));
  share* ss_y2_b= bc-> PutRepeaterGate(i+1,bc->PutSplitterGate(bc->PutCombineAtPosGate(s_y1_b,i)));
  share* ss_y2_e= bc-> PutRepeaterGate(i+1,bc->PutSplitterGate(bc->PutCombineAtPosGate(s_y1_e,i)));
  s_out = BuildSquaredEuclidDistanceCircuit(s_x1_b, s_x1_e, ss_x2_b, ss_x2_e,
    s_y1_b, s_y1_e, ss_y2_b, ss_y2_e, (BooleanCircuit*) bc);
  //bc->PutPrintValueGate(s_out,"s_out");
  //bc->PutPrintValueGate(s_out,"s_out");
  s_out = bc->PutGTGate(s_threshold,s_out);
  //bc->PutPrintValueGate(s_out,"s_out");
  
  //ss_out[i]=s_out;

//}

  
  return s_out;
}

share* BuildSquaredEuclidDistanceCircuit(share *s_x1_b,share *s_x1_e,share *s_x2_b,share *s_x2_e,
  share *s_y1_b,share *s_y1_e,share *s_y2_b,share *s_y2_e, BooleanCircuit *bc) {

  //share* out;
  share *out;



  //ED1 = (x1_b - x2_b)^2 + (y1_b - y2_b)^2
  out = BuildEuclidDistanceCircuit(s_x1_b, s_x2_b, s_y1_b, s_y2_b, (BooleanCircuit*) bc);

  //ED2 = ED1 + (x1_b - x2_e)^2 + (y1_b - y2_e)^2
  out = bc->PutADDGate(out,BuildEuclidDistanceCircuit(s_x1_b, s_x2_e,
     s_y1_b,s_y2_e, (BooleanCircuit*) bc));

  //ED3 = ED2 + (x1_e - x2_b)^2 + (y1_e - y2_b)^2
  out = bc->PutADDGate(out,BuildEuclidDistanceCircuit(s_x1_e, s_x2_b,
     s_y1_e,s_y2_b, (BooleanCircuit*) bc));

  //ED4 = ED3 + (x1_e - x2_e)^2 + (y1_e - y2_e)^2 
  out = bc->PutADDGate(out,BuildEuclidDistanceCircuit(s_x1_e, s_x2_e,
     s_y1_e,s_y2_e, (BooleanCircuit*) bc));
  //out = ED1 + ED2 + ED3 + ED4

  
  return out;
}


vector<VerificationLineSegment> BuildClusterUsingYao(vector<uint32_t*> ps_out, uint32_t nvals, share* minLns
  , BooleanCircuit *yc,BooleanCircuit *bc, uint32_t itera, ABYParty* party)
 {


  uint32_t* tmp_output= new uint32_t[nvals];

  vector<share*> s_out(nvals);
  for (uint32_t i=0;i<nvals;i++){
    for(uint32_t k=0;k<nvals;k++){
      if (i>k){
        tmp_output[k]=ps_out[i][k];
      }else{
        tmp_output[k]=ps_out[k][i];
      }

    }
    s_out[i]=bc->PutSharedSIMDINGate(nvals,tmp_output,8);
  }
  delete [] tmp_output;

  std::vector<VerificationLineSegment> verificationLineSegments;
  share* s_cluster;
  share* one =  bc->PutCONSGate(1,1);
  share* zero = bc->PutSUBGate(bc->PutCONSGate(1,1),bc->PutCONSGate(1,1));
  share* s_max = yc->PutCONSGate(MAX_CLUSTERS,32);
  share* oney = yc->PutB2YGate(one);
  share* zeroy = yc->PutB2YGate(zero);

  share* n_oney = yc->PutRepeaterGate(nvals,oney);
  share* n_zeroy = yc->PutRepeaterGate(nvals,zeroy);

  /*vector<uint64_t> clusterId (nvals);
  vector<uint64_t> isNoise (nvals);
  vector<uint64_t> notProcessed (nvals);*/

  uint32_t* clusterId = new uint32_t[nvals];
  uint8_t* isNoise = new uint8_t[nvals];
  uint8_t* notProcessed = new uint8_t[nvals];
  uint8_t* validNeighborhoodSize = new uint8_t[nvals];
  uint32_t currentCluster;

  vector<uint64_t*> output_out(nvals);
  vector<uint32_t> out_bitlen_out(nvals);
  vector<uint32_t> out_nvals_out(nvals);

  for(uint32_t t=0;t<nvals;t++){
    
        verificationLineSegments.emplace_back();
        verificationLineSegments[t].clusterId=s_max;
        verificationLineSegments[t].isNoise = zeroy;
        verificationLineSegments[t].isNoise->set_bitlength(1);
        verificationLineSegments[t].notProcessed = oney;
        verificationLineSegments[t].neighborhoodsize = zero;
           
    }
  

   for(uint32_t u=0;u<nvals;u++){
    for(uint32_t t=0;t<nvals;t++){
      if(u!=t){
      verificationLineSegments[u].neighborhoodsize = 
           bc->PutADDGate(verificationLineSegments[u].neighborhoodsize,
           bc->PutCombineAtPosGate(s_out[u],t));
           }

    
    }
//bc->PutPrintValueGate(verificationLineSegments[u].neighborhoodsize,"neighborhoodsize");
    //check if neighborhoodsize > minLns
    share* t_a = bc->PutMUXGate(one, zero, bc->PutGTGate(verificationLineSegments[u].neighborhoodsize,minLns));
    t_a = yc->PutB2YGate(t_a);
    t_a->set_bitlength(1);
    verificationLineSegments[u].validNeighborhoodSize = t_a;
    s_out[u]=yc->PutB2YGate(s_out[u]);

    //yc->PutPrintValueGate(s_out[u],"s_out");
    }


    uint8_t val [nvals];
    for (uint32_t k=0;k<nvals;k++){
      val[k]=1;
    }
  share* hum;

  //starting with clusterID=1
  share* currentClusterId = oney;
    for(uint32_t i=0;i<nvals;i++) {


        VerificationLineSegment &elem = verificationLineSegments[i];
        
        share* tmp_notProcessed=elem.notProcessed;
        
        share* test1 =yc->PutMUXGate(yc->PutMUXGate(oney,zeroy,elem.validNeighborhoodSize),zeroy,tmp_notProcessed);
        
        //elem.clusterId=currentClusterId if elem.validNeighborhoodSize=1 and elem.notProcessed=1
        //yc->PutPrintValueGate(currentClusterId,"----------currentClusterId");
        //yc->PutPrintValueGate(elem.clusterId,"----------elem.clusterId");
        //yc->PutPrintValueGate(elem.validNeighborhoodSize,"----------elem.validNeighborhoodSize");
        elem.clusterId=yc->PutMUXGate(currentClusterId,elem.clusterId,test1);

        //elem.notProcessed=0 if elem.validNeighborhoodSize=1 and elem.notProcessed=1
        elem.notProcessed=yc->PutMUXGate(zeroy,tmp_notProcessed,test1);
        
        //This is used later for the SIMD operation
        val[i]=0;
        if(CLIENT){
        hum = yc->PutSIMDINGate(nvals,val,1,CLIENT);
      }else{
        hum = yc->PutSIMDINGate(nvals,val,1,SERVER);
        }
        
        //for each i we will iterate itera times all the other line segments
        for (uint32_t iteration=0;iteration<itera;iteration++){

        //Iterating all the line segments
        for (uint32_t j=0;j<nvals;j++) {
          if(j!=i){

            VerificationLineSegment *elem2 = &verificationLineSegments[j];

            share* s_isNoise2=(*elem2).isNoise;
            share* s_distelem=yc->PutCombineAtPosGate(s_out[i],j);
            share* test2 = 
                  
                  yc->PutMUXGate(
                  yc->PutMUXGate(
                  yc->PutMUXGate(oney,zeroy,s_isNoise2)
                  ,zeroy,s_distelem)
                  ,zeroy,test1);
                
                //(*elem2).clusterId=currentClusterId if elem.validNeighborhoodSize=1 and elem.notProcessed=1
                // and i and j are neighbors and (*elem2).isNoise=1
                (*elem2).clusterId = yc->PutMUXGate(currentClusterId,(*elem2).clusterId,test2);
                 
                //(*elem2).isNoise=0 if elem.validNeighborhoodSize=1 and elem.notProcessed=1
                // and i and j are neighbors and (*elem2).isNoise=1            
                (*elem2).isNoise =  yc->PutMUXGate(zeroy,(*elem2).isNoise,test2);
            
                share* notProcessed2 = (*elem2).notProcessed;
            
            share* test3 = 
                  
                  yc->PutMUXGate(
                  yc->PutMUXGate(
                  yc->PutMUXGate(oney,zeroy,notProcessed2)
                  ,zeroy,s_distelem)
                  ,zeroy,test1);
              
                
                //(*elem2).clusterId=currentClusterId if elem.validNeighborhoodSize=1 and elem.notProcessed=1
                // and i and j are neighbors and (*elem2).notProcessed=1
                (*elem2).clusterId =  yc->PutMUXGate(currentClusterId,(*elem2).clusterId,test3);


                //(*elem2).notProcessed=0 if elem.validNeighborhoodSize=1 and elem.notProcessed=1
                // and i and j are neighbors and (*elem2).notProcessed=1
                (*elem2).notProcessed = yc->PutMUXGate(zeroy,(*elem2).notProcessed,test3);


                  //This replaces the 3rd loop using SIMD
                  share* test = 
                    yc->PutMUXGate(
                    yc->PutMUXGate(oney,zeroy,test3)
                    ,zeroy,(*elem2).validNeighborhoodSize);

                  share* n_test = yc->PutRepeaterGate(nvals,test);
                  
                  n_test=yc->PutMUXGate(n_test,n_zeroy,hum);
                  
                  //Update resultDistancesCompToThresholds of i with all other line segments
                  //if
                  s_out[i]=yc->PutMUXGate(
                            yc->PutMUXGate(
                               n_oney,s_out[i],s_out[j])
                               ,s_out[i],n_test);
     
          
          }
  
    
        
        }
}
        //yc->PutPrintValueGate(s_out[i],"s_out_af");
        //party->Reset();
        //val[i]=1;
        //currentClusterId += 1;
        currentClusterId=bc->PutY2BGate(currentClusterId);
        share * tmpc = bc->PutADDGate(currentClusterId,one);
        currentClusterId=yc->PutB2YGate(currentClusterId);

        tmpc=yc->PutB2YGate(tmpc);

        currentClusterId=yc->PutMUXGate(tmpc,currentClusterId,test1);
        

        
  

          elem.isNoise=yc->PutMUXGate(
          yc->PutMUXGate(elem.isNoise,oney,elem.validNeighborhoodSize),elem.isNoise,tmp_notProcessed);
          elem.clusterId=yc->PutMUXGate(
          yc->PutMUXGate(elem.clusterId,zeroy,elem.validNeighborhoodSize),elem.clusterId,tmp_notProcessed);
          elem.notProcessed=yc->PutMUXGate(
          yc->PutMUXGate(elem.clusterId,zeroy,elem.validNeighborhoodSize),elem.notProcessed,tmp_notProcessed);

          if (i%3==0 && i!=nvals){
          //Reset party after every iteration
          currentClusterId = bc->PutSharedOUTGate(bc->PutY2BGate(currentClusterId));
        for (uint32_t t=0;t<nvals;t++){
          //yc->PutPrintValueGate(verificationLineSegments[t].notProcessed,"notProcessed_before");
          verificationLineSegments[t].clusterId=bc->PutSharedOUTGate(bc->PutY2BGate(verificationLineSegments[t].clusterId));
          verificationLineSegments[t].isNoise=bc->PutSharedOUTGate(bc->PutY2BGate(verificationLineSegments[t].isNoise));
          verificationLineSegments[t].notProcessed=bc->PutSharedOUTGate(bc->PutY2BGate(verificationLineSegments[t].notProcessed));
          s_out[t]=bc->PutSharedOUTGate(bc->PutY2BGate(s_out[t]));
          //if (t==0){
            verificationLineSegments[t].validNeighborhoodSize=bc->PutSharedOUTGate(bc->PutY2BGate(verificationLineSegments[t].validNeighborhoodSize));
          //}
          

        }
        
          party->ExecCircuit();

            currentCluster=currentClusterId->get_clear_value<uint32_t>();
          for (uint32_t t=0;t<nvals;t++){
    
          clusterId[t]=verificationLineSegments[t].clusterId->get_clear_value<uint32_t>();
          isNoise[t]= verificationLineSegments[t].isNoise->get_clear_value<uint8_t>();
          notProcessed[t]=verificationLineSegments[t].notProcessed->get_clear_value<uint8_t>();
          s_out[t]->get_clear_value_vec(&output_out[t], &out_bitlen_out[t], &out_nvals_out[t]);
          //if (t==0){
            validNeighborhoodSize[t]=verificationLineSegments[t].validNeighborhoodSize->get_clear_value<uint8_t>();
          //}
          

        }
         
          party->Reset();
          
          currentClusterId=yc->PutB2YGate(bc->PutSharedINGate(currentCluster,32));
          
        for (uint32_t t=0;t<nvals;t++){
          verificationLineSegments[t].clusterId=yc->PutB2YGate(bc->PutSharedINGate(clusterId[t],32));
          verificationLineSegments[t].isNoise=yc->PutB2YGate(bc->PutSharedINGate(isNoise[t],8));
          verificationLineSegments[t].notProcessed=yc->PutB2YGate(bc->PutSharedINGate(notProcessed[t],8));
          verificationLineSegments[t].validNeighborhoodSize=yc->PutB2YGate(bc->PutSharedINGate(validNeighborhoodSize[t],8));
          s_out[t]=yc->PutB2YGate(bc->PutSharedSIMDINGate(nvals,output_out[t],8));

         

        }

        one =  bc->PutCONSGate(1,1);
        zero = bc->PutSUBGate(bc->PutCONSGate(1,1),bc->PutCONSGate(1,1));
        s_max = yc->PutCONSGate(MAX_CLUSTERS,32);
        oney = yc->PutB2YGate(one);
        zeroy = yc->PutB2YGate(zero);

        n_oney = yc->PutRepeaterGate(nvals,oney);
        n_zeroy = yc->PutRepeaterGate(nvals,zeroy);

      
        }
  }
 

  return verificationLineSegments;


 }


// build circuits
static ArithmeticCircuit* ac;
static BooleanCircuit* bc;
static BooleanCircuit* yc;


share* pptraclus(e_role role, const string& address, uint16_t port, seclvl seclvl, uint32_t nthreads,
  e_mt_gen_alg mt_alg, vector<vector<uint64_t>> linesegments,uint32_t nvals, e_sharing sharing
  , uint32_t minLns, uint64_t threshold){


  cout << "Starting" <<endl;
  uint32_t bitlen = 64;
  ABYParty* party = new ABYParty(role, address, port, seclvl, 64, nthreads, mt_alg);


  std::vector<Sharing*>& sharings = party->GetSharings();
  ac = (ArithmeticCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();
  bc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();
  yc = (BooleanCircuit*) sharings[S_YAO]->GetCircuitBuildRoutine();

  uint64_t* x1_b = new uint64_t[nvals];
  uint64_t* x1_e = new uint64_t[nvals];
  uint64_t* y1_b = new uint64_t[nvals];
  uint64_t* y1_e = new uint64_t[nvals];


  share *s_x1_b, *s_x1_e, *s_y1_b, *s_y1_e, *s_out, *s_minLns;
  
  share *s_threshold;
  

  if (role==CLIENT){
  
  for(int i = 0; i < nvals; ++i) {
    x1_b[i] = linesegments[i][0];
    y1_b[i] = linesegments[i][1];
    x1_e[i] = linesegments[i][2];
    y1_e[i] = linesegments[i][3];
  
  }
}
  

  if (role == SERVER) {
    s_x1_b = bc->PutDummySIMDINGate(nvals, 64);
    s_y1_b = bc->PutDummySIMDINGate(nvals, 64);
    s_x1_e = bc->PutDummySIMDINGate(nvals, 64);
    s_y1_e = bc->PutDummySIMDINGate(nvals, 64);
    s_threshold = bc->PutDummyINGate(64);
    s_minLns = bc->PutDummyINGate(32);
  
  } else {
    s_x1_b = bc->PutSIMDINGate(nvals, x1_b, 64, CLIENT);
    s_y1_b = bc->PutSIMDINGate(nvals, y1_b, 64, CLIENT);
    s_x1_e = bc->PutSIMDINGate(nvals, x1_e, 64, CLIENT);
    s_y1_e = bc->PutSIMDINGate(nvals, y1_e, 64, CLIENT);
    s_threshold = bc->PutINGate(threshold, 64, CLIENT);
    s_minLns = bc->PutINGate(minLns, 32, CLIENT);
   

  }


  

  s_x1_b=bc->PutSharedOUTGate(s_x1_b);
  s_y1_b=bc->PutSharedOUTGate(s_y1_b);
  s_x1_e=bc->PutSharedOUTGate(s_x1_e);
  s_y1_e=bc->PutSharedOUTGate(s_y1_e);
  s_threshold=bc->PutSharedOUTGate(s_threshold);
  s_minLns=bc->PutSharedOUTGate(s_minLns);

  
  
  delete [] x1_b;
  delete [] y1_b;
  delete [] x1_e;
  delete [] y1_e;


  
  //Execute the circuit using the ABYParty object
  party->ExecCircuit();
  cout << "circuit 1 executed" <<endl;

  uint64_t* output1;
  uint64_t* output2;
  uint64_t* output3;
  uint64_t* output4;

  uint32_t out_bitlen1, out_nvals1;
  uint32_t out_bitlen2, out_nvals2;
  uint32_t out_bitlen3, out_nvals3;
  uint32_t out_bitlen4, out_nvals4;
  

  


  // This method only works for an output length of maximum 64 bits in general,
  // if the output length is higher you must use get_clear_value_ptr
  s_x1_b->get_clear_value_vec(&output1, &out_bitlen1, &out_nvals1);
  s_y1_b->get_clear_value_vec(&output2, &out_bitlen2, &out_nvals2);
  s_x1_e->get_clear_value_vec(&output3, &out_bitlen3, &out_nvals3);
  s_y1_e->get_clear_value_vec(&output4, &out_bitlen4, &out_nvals4);
  



  ofstream myfile;
  myfile.open ("example.csv");
  for (uint32_t i=0;i<nvals;i++){
  myfile <<output1[i]<<','<<output2[i]<<','<<output3[i]<<','<<output4[i]<<'\n';
   }
  myfile.close();


  ofstream myfile2;
  myfile2.open("threshold.csv");
  myfile2 << s_threshold->get_clear_value<uint64_t>()<<'\n';
  myfile2.close();

  ofstream myfile3;
  myfile3.open("minLns.csv");
  myfile3 << s_minLns->get_clear_value<uint32_t>()<<'\n';
  myfile3.close();

  
  delete party;

 
  cout << "Done" << endl;


  return s_out;



}

//This function is used 
share* construct(e_role role, const string& address, uint16_t port, seclvl seclvl, uint32_t nthreads,
  e_mt_gen_alg mt_alg, vector<vector<uint64_t>> linesegments,uint32_t nvals, e_sharing sharing
  , uint32_t minLns, uint64_t threshold,uint32_t itera){


  cout << "Starting" <<endl;
  cout<<nvals<<endl;

  uint32_t bitlen = 64;
  ABYParty* party = new ABYParty(role, address, port, seclvl, 64, nthreads, mt_alg);


  std::vector<Sharing*>& sharings = party->GetSharings();
  ac = (ArithmeticCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();
  bc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();
  yc = (BooleanCircuit*) sharings[S_YAO]->GetCircuitBuildRoutine();



  share *ms_x1_b, *ms_y1_b, *ms_x1_e, *ms_y1_e, *s_out, *s_minLns;
  
  share *s_threshold;
  

  



  vector<uint32_t*> ps_out(nvals);
  uint32_t* tmp_output;
  uint32_t tmp_out_bitlen, tmp_out_nvals;
  
  uint64_t* x1_b = new uint64_t[nvals];
  uint64_t* x1_e = new uint64_t[nvals];
  uint64_t* y1_b = new uint64_t[nvals];
  uint64_t* y1_e = new uint64_t[nvals];
  for(int k = 0; k < nvals; ++k) {
    x1_b[k] = linesegments[k][0];
    y1_b[k] = linesegments[k][1];
    x1_e[k] = linesegments[k][2];
    y1_e[k] = linesegments[k][3];
  
  }
  for (uint32_t i=0;i<nvals;i++){
  

  ms_x1_b=bc->PutSharedSIMDINGate(i+1,x1_b,64);
  ms_y1_b=bc->PutSharedSIMDINGate(i+1,y1_b,64);
  ms_x1_e=bc->PutSharedSIMDINGate(i+1,x1_e,64);
  ms_y1_e=bc->PutSharedSIMDINGate(i+1,y1_e,64);
  
  
  s_threshold=bc-> PutRepeaterGate(nvals,bc->PutSharedINGate(threshold,64));
  s_minLns = bc->PutSharedINGate(minLns,32);

  s_out =buildmatrix(ms_x1_b,ms_x1_e,ms_y1_b, ms_y1_e, (BooleanCircuit*) bc ,nvals,s_threshold,i);
  s_out = bc->PutSharedOUTGate(s_out);
  party->ExecCircuit();
  s_out->get_clear_value_vec(&tmp_output, &tmp_out_bitlen, &tmp_out_nvals);
  ps_out[i]=tmp_output;
  party->Reset();
  


  }

  delete [] x1_b;
  delete [] y1_b;
  delete [] x1_e;
  delete [] y1_e;

  s_minLns = bc->PutSharedINGate(minLns,32);

  vector<VerificationLineSegment> s_cluster;
  s_cluster = BuildClusterUsingYao(ps_out,nvals, s_minLns, (BooleanCircuit*) yc,(BooleanCircuit*) bc, itera, (ABYParty*) party);
  
  vector<share*> ss_cluster(nvals);
  for (uint32_t i=0;i<nvals;i++){
    s_cluster[i].clusterId = bc->PutY2BGate(s_cluster[i].clusterId);
    ss_cluster[i] = bc->PutSharedOUTGate(s_cluster[i].clusterId);
    
  }


  



  
  //Execute the circuit using the ABYParty object
  party->ExecCircuit();
  cout << "circuit 1 executed" <<endl;

  ofstream clusterfile;
  clusterfile.open ("cluster.csv");
  for (uint32_t i=0;i<nvals;i++){
    clusterfile << ss_cluster[i]->get_clear_value<uint32_t>()<<'\n';
      
  }
  clusterfile.close();


  delete party;
  

  cout << "Done" << endl;


  return s_out;



}


int recoverCluster(e_role role, const string& address, uint16_t port, seclvl seclvl, uint32_t nthreads,
  e_mt_gen_alg mt_alg, uint32_t* cluster,uint32_t nvals, e_sharing sharing){

  cout << "Starting" <<endl;
  uint32_t bitlen = 64;
  ABYParty* party = new ABYParty(role, address, port, seclvl, 64, nthreads, mt_alg);


  std::vector<Sharing*>& sharings = party->GetSharings();
  bc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();


  share* s_cluster=bc->PutSharedSIMDINGate(nvals,cluster,32);

  s_cluster = bc->PutOUTGate(s_cluster,CLIENT);

  party->ExecCircuit();
  cout << "circuit executed" <<endl;


  if (CLIENT){

  uint32_t* output;
  uint32_t out_bitlen, out_nvals;
  s_cluster->get_clear_value_vec(&output, &out_bitlen, &out_nvals);

  ofstream clusterfile;
  uint32_t max = 0;
  clusterfile.open ("data/cluster_un.csv");
  for (uint32_t i=0;i<nvals;i++){
    clusterfile << output[i]<<'\n';
    if (output[i]>max){
      max = output[i];
    }
      
  }
  clusterfile.close();

  

  std::vector<uint32_t> results(max,0);
  for (int i=0;i<nvals;i++){
    results[output[i]]+=1;
  }

  /*
  ofstream clusterfilegeneral;
  clusterfilegeneral.open ("data/clusters.csv");
  for(uint32_t j=0;j< max+1;j++){
    if (j==0){
      clusterfilegeneral << results[j] << " elements were marked as noise." <<'\n';
    }else{
      clusterfilegeneral << "Cluster " << j << " contains " << results[j] << " line segments." <<'\n';
    }
  }
  clusterfilegeneral.close();
*/

  }



  return 0;

  }