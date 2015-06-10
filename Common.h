/*
 *      Author: Zofia Abramowska
 *		File: Common.h
 */

#pragma once
#include "opencv2/core/core.hpp"
#include <iostream>
#include <set>

const double pi = 3.14;

struct Point {
	long x;
	long y;
	bool operator!=(const Point &other) const {
		return other.x != x || other.y != y;
	}
	bool operator==(const Point &other) const {
		return other.x == x && other.y == y;
	}
};

struct Boundary {
	/*
	 * Points of rectangle
	 */
	Point sw, ne;
	/*
	 * Gray level of object
	 */
	int gray;
};

/*
 * For each pixel call f() operation, until it returns false
 */
template<typename F>
void for_pixel(cv::Mat &I, F f)  {
	 for (int row = 0; row < I.rows; row++) {
		for (int column = 0; column < I.cols; column++) {
			f(row, column);
		}
	}
}

/*
 * For each neighbor of point call f() operation, until it returns false
 */
template<typename F>
void for_neighbour(int row, int col, F f) {
	std::vector<int> rows = {row - 1, row, row + 1};
	std::vector<int> cols = {col - 1, col, col + 1};
	for(auto r : rows) {
		for(auto c : cols)
			if (r != row || c != col)
				if(!f(r, c))
					return;
	}
}

/*
 * For each element of mask and image call f() operation, until it returns false
 */
template<typename F>
void for_mask(int row, int col, std::vector<std::vector<int>> mask, F f) {
	std::vector<int> rows, cols;
	for (size_t i = 1; i <= mask.size() / 2; i++) {
		cols.push_back(col - i);
	}
	cols.push_back(col);
	for (size_t i = 1; i <= mask.size() / 2; i++) {
		cols.push_back(col + i);
	}
	for (size_t i = 1; i <= mask[0].size() / 2; i++) {
		rows.push_back(row - i);
	}
	rows.push_back(row);
	for (size_t i = 1; i <= mask[0].size() / 2; i++) {
		rows.push_back(row + i);
	}
	for(size_t i = 0; i < rows.size(); ++i) {
		auto r = rows[i];
		for(size_t j = 0; j < cols.size(); ++j) {
			auto c = cols[j];
			if(!f(r, c, mask[i][j]))
				return;
		}
	}
}

/*
 * Normalize point in terms of maximum and minimum values of x and y coordinates
 */
Point normalize(cv::Mat &I, int row, int col);

/*
 * Count gray value of image pixel
 */
int gray(const cv::Vec3b &pix);

/*
 * Operator for human readable representation of structures
 */
std::ostream& operator<<(std::ostream &os, const Point &p);
std::ostream& operator<<(std::ostream &os, const Boundary &b);
std::ostream& operator<<(std::ostream &os, const std::set<int> &s);

/*
 * Find coordinates of rectangle bounding object of given gray level
 */
Boundary countBoundary(cv::Mat &I, short gray_level = 0);

/*
 * Fill point of image with given colour
 */
void fillPoint(cv::Mat &res, const Point &p, const cv::Vec3b &colour);
/*
 * Fill rectangle border in image
 */
void fillBoundary(cv::Mat& res, const Boundary &boundary, int percentage = 0);

/*
 * Count very simple weight distance between two vectors of values
 * Vectors must be the same length
 */
double dist(const std::vector<double> &pattern, const std::vector<double> &object);

/*
 * Count Mahalanobis distance
 */
double countDist(const std::vector<double> &values);
