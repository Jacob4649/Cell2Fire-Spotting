// Class inclusions
#include "SpottingFBP.h"
#include "CellsFBP.h" 

// Include libraries
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

std::vector<int> SpottingFBP(
    const std::unordered_map<int, CellsFBP>& Cells_Obj, 
    const std::vector<std::vector<int>>& coordCells, 										
    const std::unordered_set<int> & AvailSet, 
    double WSD, 
    double WSC, 
    const SpottingParams& spottingParams,
    bool verbose
) {
    // Early exit first.
    if (spottingParams.spot_angle * spottingParams.spot_0_prob * spottingParams.spot_10_time * WSC == 0) {
        return vector<int>();
    }

    // TODO data structures should be passed by reference   
    // TODO
    std::unordered_map<int, std::unordered_map<int, int>> Angles = std::unordered_map<int, std::unordered_map<int, int>>();
    std::unordered_map<int, std::unordered_map<int, int>> Distances = std::unordered_map<int, std::unordered_map<int, int>>();
    std::unordered_map<int, std::unordered_map<int, double>> SpotProb = std::unordered_map<int, std::unordered_map<int, double>>();

    double WTolerance = spottingParams.spot_angle / 2.0;
    std::cout << "debug AvailSet size=" << AvailSet.size() << std::endl;

    double WA = WSD - WTolerance;
    double WB = WSD + WTolerance;

    double WWindowA = WA;
    double WWindowB = WB;

    if (WA < 0)
        WWindowA += 360;

    if (WB > 360)
        WWindowB -= 360;

    if (verbose) {
        std::cout<< "Wind Direction:" << WSD << std::endl;
        std::cout << "Wind Speed:" << WSC << std::endl;
        std::cout << "Spotting Wind Windows: [" << WA << ", " << WB << "]" << std::endl;
    }

    // TODO: maybe true debugging
    /*
	std::cout << "debug obj keys size " << Cells_Obj.size();
    std::cout << "debug AvailSet size " << AvailSet.size();
    std::cout << "debug CoordCells size " << CoordCells.size();
	*/

    for (auto & _c1 : Cells_Obj) {
        int c1 = 0;
        Angles[c1] = std::unordered_map<int, int>();
        Distances[c1] = std::unordered_map<int, int>();
        for (auto & c2 : AvailSet){
         
            if (c1 == c2) {
                // CP: None = -1
                Angles[c1][c2] = -1;
                Distances[c1][c2] = -1;
            } else {
                int a = coordCells[c1][0] - coordCells[c2 - 1][0];
                int b = coordCells[c1][1] - coordCells[c2 - 1][1];

                if (a == 0) {
                    if (b >= 0) 
                        Angles[c1][c2] = 270;
                    else{
                        Angles[c1][c2] = 90;
                    Distances[c1][c2] = std::abs(b);
					}
                }
                if (b == 0) {
                    if (a >= 0) 
                        Angles[c1][c2] = 180;
                    else{
                        Angles[c1][c2] = 0;
                    Distances[c1][c2] = std::abs(a);
					}
                }
            
				if (a != 0 && b != 0) {
					double radToDeg = 180 / M_PI;
					double angle = std::atan(b * 1.0 / a) * radToDeg;
					double distance = std::sqrt(a * a + b * b);

					if (a > 0)
						angle += 180;
					if (a < 0 && b > 0){
						angle += 360;
						Angles[c1][c2] = angle;
						Distances[c1][c2] = distance;
					}
				}
			}
        }
    }
    if (verbose) {
        std::cout<< "Angles size: " << Angles.size() << std::endl;
        std::cout<< "Distances size: " << Distances.size() << std::endl;
    }

    // TODO: fix these later
    int cellsize = Cells_Obj.begin()->second.perimeter / 4.0;
    double beta = spottingParams.spot_0_prob;
    double tilde_d = spottingParams.spot_10_time * WSC / cellsize;
    double alpha = std::log(.1) / tilde_d + std::log(beta) / tilde_d;

    for (auto & _c1 : Cells_Obj) {
        int c1 = _c1.first;
        SpotProb[c1] = std::unordered_map<int, double>();
        for (auto & c2 : AvailSet) {
            // TODO: can't be None
            if (Angles[c1][c2]  == -1) {
                SpotProb[c1][c2] = 0;
            } else {
                SpotProb[c1][c2] = beta * std::exp(-1 * Distances[c1][c2] * alpha);
                double WA_t = WA;
                double WB_t = WB;
                // TODO the logic here is a shitshow
                if (WA_t >= 0 && WB_t >= 0 && WA_t < 360 && WTolerance < 180) {
                    if (WB_t >= 360)
                        WB_t -= 360;
                    if (!(Angles[c1][c2] <= WB_t && Angles[c1][c2] >= WA_t)) 
                        SpotProb[c1][c2] = 0;
                }

                WA_t = WA;
                WB_t = WB;
                
                if (WA_t < 0 && WB_t >= 0 && WB_t < 360 && WTolerance < 180) {
                    if (!(Angles[c1][c2] <= WB_t || Angles[c1][c2] >= WA_t + 360)) {
                        SpotProb[c1][c2] = 0;
                    }
                }
            }
        }
    }

    if (verbose) {
        std::cout << "Probabilities size: " << SpotProb.size() << std::endl;
    }

	
	// TODO: CHECK THIS FINAL STEP!!!!!! NOT COMPILING DUE TO C2 out of scope
	// MODIFYING!!!!  got rid of  msg_list out of scope, got rid of c2 problem, BUT CHECK!!!!!!!!!!
	// COMPILER THROWS ERROR: 
	
	/*
	In file included from SpottingFBP.h:4:0,
                 from SpottingFBP.cpp:2:
CellsFBP.h:57:9: note: candidate: CellsFBP::CellsFBP(int, double, std::vector<int>, double, int, std::__cxx11::string, double, double, int, std::unordered_map<std::__cxx11::basic_string<char>, std::vector<int> >&, std::vector<double>, int, bool)
         CellsFBP(int _id, double _area, std::vector<int> _coord, double _age,
	
	*/
    std::vector<int> msg_list = vector<int>();
	
    for (auto & _c1 : SpotProb) {
        int c1 = _c1.first;
        for (auto & _c2 : SpotProb[c1]) {
			int c2 = _c2.first;
            // Lowkey maybe a static generator would be better than std::rand,
            // but that's something for later.
            //
            // This should just be a static_cast on std::rand, not multiply by 1.0
            double random = 1.0 * std::rand() / RAND_MAX; 
            // This looks wrong to me. If we are generating a random value
            // and comparing it to the probability of spotting, 
            // wouldn't we want to spot if the random value is LESS than the probability,
            // not more?
            // 
            // If we have a 30% spotting probability between c1 and c2, there is a 30%
            // chance of producing a random value less than 0.3. So wouldn't we want
            // to spot in these cases?
            //
            // I made the change but for posterity, this is what was here before
            // random > 0 && SpotProb[c1][c2] < random
            if (random < SpotProb[c1][c2]) {
                if (verbose) {
                    std::cout << "Spotting msg from " << c1 + 1 << " to " << c2; 
                }
                msg_list.push_back(c2);
            }
        }
    }

    return msg_list;
}
