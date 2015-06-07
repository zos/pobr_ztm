/*
 *      Author: Zofia Abramowska
 *		File: Operations.h
 */

#pragma once

#include "opencv2/core/core.hpp"
#include "Common.h"

#include <queue>

void dilate(cv::Mat &I);
void erose(cv::Mat &I);
void close(cv::Mat &I, int strength);

cv::Mat filterMedian(cv::Mat& I);



