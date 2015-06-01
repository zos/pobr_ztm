/*
 *      Author: Zofia Abramowska
 *		File: Operations.h
 */

#pragma once

#include "opencv2/core/core.hpp"

void dilate(cv::Mat &I);
void erose(cv::Mat &I);

void close(cv::Mat &I, int strength);



