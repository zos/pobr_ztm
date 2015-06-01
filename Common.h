/*
 *      Author: Zofia Abramowska
 *		File: common.h
 */

#pragma once
#include "opencv2/core/core.hpp"
#include <iostream>
#include <set>

const double pi = 3.14;

struct Point {
	long x;
	long y;
	bool operator!=(const Point &other) {
		return other.x != x || other.y != y;
	}
};

struct Boundary {
	Point sw, ne;
	int gray;
};

template<typename F>
void for_pixel(cv::Mat & I, F f)  {
	 for (int row = 0; row < I.rows; row++) {
		for (int column = 0; column < I.cols; column++) {
			f(row, column);
		}
	}
}

template<typename F>
void for_neighbour(int row, int col, F f) {
	std::vector<int> rows = {row - 1, row, row + 1};
	std::vector<int> cols = {col - 1, col, col + 1};
	for(auto r : rows) {
		for(auto c : cols)
			if (r != row || c != col)
				f(r, c);
	}
}

Point normalize(cv::Mat &I, int row, int col);

int gray(const cv::Vec3b &pix);
std::ostream& operator<<(std::ostream &os, const Point &p);
std::ostream& operator<<(std::ostream &os, const Boundary &b);
std::ostream& operator<<(std::ostream &os, const std::set<int> &s);

Boundary countBoundary(cv::Mat &I, short gray_level = 0);
Point boundaryCenter(const Boundary &b);

void fillPoint(cv::Mat &res, const Point &p, const cv::Vec3b &colour);
void fillBoundary(cv::Mat& res, const Boundary &boundary);


