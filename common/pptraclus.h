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
using namespace std;

class VerificationLineSegment{
private:

public:
  VerificationLineSegment();
  share* clusterId{};
  //uint32_t id{};
  share* isNoise{};
  share* notProcessed{};
  share* neighborhoodsize{};
  share* validNeighborhoodSize{};
  //uint32_t checkNecessary{};
  vector<share*>  resultDistancesCompToThresholds;
};
share* buildmatrix(share *s_x1_b,share *s_x1_e,
  share *s_y1_b,share *s_y1_e,share *s_x1_b_2,share *s_x1_e_2,
  share *s_y1_b_2,share *s_y1_e_2, BooleanCircuit *bc ,uint32_t nvals,share* s_threshold);
share* BuildEuclidDistanceCircuit(share *s_x1, share *s_x2, share *s_y1,share *s_y2, BooleanCircuit *bc);

share* BuildSquaredEuclidDistanceCircuit(share *s_x1_b,share *s_x1_e,share *s_x2_b,share *s_x2_e,
	share *s_y1_b,share *s_y1_e,share *s_y2_b,share *s_y2_e, BooleanCircuit *bc);

vector<VerificationLineSegment> BuildClusterUsingYao(vector <share*> s_out, uint32_t nvals, share* minLns
  , BooleanCircuit *yc,BooleanCircuit *bc, uint32_t itera, ABYParty* party);

share* pptraclus(e_role role, const string& address, uint16_t port, seclvl seclvl, uint32_t nthreads,
	e_mt_gen_alg mt_alg, vector<vector<uint64_t>> linesegments,uint32_t nvals, e_sharing sharing
	, uint32_t minLns, uint64_t threshold);

share* construct(e_role role, const string& address, uint16_t port, seclvl seclvl, uint32_t nthreads,
  e_mt_gen_alg mt_alg, vector<vector<uint64_t>> linesegments,uint32_t nvals, e_sharing sharing
  , uint32_t minLns, uint64_t threshold,uint32_t itera);

int recoverCluster(e_role role, const string& address, uint16_t port, seclvl seclvl, uint32_t nthreads,
  e_mt_gen_alg mt_alg, uint32_t* cluster,uint32_t nvals, e_sharing sharing);
