/*
 *      Author: Zofia Abramowska
 *		File: Parameters.h
 */


#pragma once

/*
 * Algorithm parameters - specify conditions under which segmentation is run
 */
namespace AlgorithmParameters {

/*
 * How much more red must be in pixel than other color for pixel to get enhanced
 */
const double MIN_RED_FACTOR = 2.0;

/*
 * Background object space constraints
 */
const int MIN_BGR_SPACE = 10;
const int MAX_BGR_SPACE = 50000;

/*
 * Logo object space contraints
 */
const int MIN_OBJ_SPACE = 100;
const int MAX_OBJ_SPACE = 1500;

/*
 * Maximum difference of momentum values vectors between object and pattern
 */
const double MAX_DIST = 4.08;
}


