#ifndef READARGS
#define READARGS

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>

using namespace std;

// Let's consider defining a strongly-typed struct for our
// spotting params
/**
 * The parameters that define the behaviour of the spotting function.
 */
struct SpottingParams {

	/**
	 * I wish I understood enough about fire and spotting to
	 * write the comment describing what this parameter is. From what I can
	 * see it's something like the angle of the window around the wind direction
	 * in which spotting can occur?
	 * 
	 * I think this is measured in degrees.
	 */
	double spot_angle;

	/**
	 * This is the probability of spotting at the center of the window. I kind of expected
	 * that there would be some kind of falloff in the spotting probability as we got
	 * farther from the center of the window, but this is not the case at all.
	 * Spotting probability is completely constant with respect to angle from
	 * what I can see in the code. Rather it seems to be a function of the distance.
	 * 
	 * I suppose the 0 in spot_0_prob really means that this is the probability of
	 * spotting at 0 distance (which is consistent with how it is used in the code).
	 */
	double spot_0_prob;

	/**
	 * This seems to be like the time it takes to spot or something. It is combined with
	 * the wind speed to influence the probability of spotting but I have a few questions.
	 *
	 * 1. If we increase the spotting time, the probability of spotting seems to decrease in the
	 *    codebase. This makes sense, as it seems to be increasing the expected time by decreasing the
	 *    likelihood of spotting, but this is a bit problematic when considering that currently,
	 *    if a cell does not send any messages in a time step, it is transitioned to a burned state.
	 *    As a result, if we increase the spotting time, the cell may just move to its burned state
	 *    before it has the chance to spot. Not an issue, just something worth considering. See the
	 *    comment on line 500 of CellsFBP.cpp for more discussion on this.
	 *
	 *    After further reflection (as is reflected in the long comment mentioned before),
	 *    I think this is probably fine, and I just wasn't understanding the mechanics
	 *    of cells burning out.
	 *
	 * 2. I guess I'm also curious what the 10 stands for. It would also be cool to know
	 *    if there are supposed to be units on the time.
	 */
	double spot_10_time;

	SpottingParams(
		double spot_angle,
		double spot_0_prob,
		double spot_10_time
	) : spot_angle(spot_angle), spot_0_prob(spot_0_prob), spot_10_time(spot_10_time) {}

	SpottingParams() : SpottingParams(0, 0, 0) {}

};

// Unnecessary typedef.
/*
*   Args structure
*/
typedef struct{ 
	
	string InFolder, OutFolder, WeatherOpt, HarvestPlan;
	
	bool OutMessages, Trajectories, NoOutput, verbose, Ignitions, OutputGrids, FinalGrid, PromTuned, Stats, BBOTuning;

	float ROSCV, ROSThreshold, HFIThreshold, HFactor, FFactor, BFactor, EFactor, FirePeriodLen;

	int MinutesPerWP, MaxFirePeriods, TotalYears, TotalSims, NWeatherFiles, IgnitionRadius, seed;

	unordered_set<int>  HCells, BCells;

	// Following the same naming convention I did in CellsFBP.cpp.
	SpottingParams spotting_params;
} arguments;


char* getCmdOption(char ** begin, char ** end, const string & option);

bool cmdOptionExists(char** begin, char** end, const string& option);

void parseArgs(int argc, char * argv[], arguments * args_ptr);

void printArgs(arguments args);


#endif