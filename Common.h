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
	bool operator!=(const Point &other) const {
		return other.x != x || other.y != y;
	}
	bool operator==(const Point &other) const {
		return other.x == x && other.y == y;
	}
};

struct Boundary {
	Point sw, ne;
	int gray;
};

template<typename F>
void for_pixel(cv::Mat &I, F f)  {
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
				if(!f(r, c))
					return;
	}
}

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

Point normalize(cv::Mat &I, int row, int col);

int gray(const cv::Vec3b &pix);
std::ostream& operator<<(std::ostream &os, const Point &p);
std::ostream& operator<<(std::ostream &os, const Boundary &b);
std::ostream& operator<<(std::ostream &os, const std::set<int> &s);

Boundary countBoundary(cv::Mat &I, short gray_level = 0);
Point boundaryCenter(const Boundary &b);

void fillPoint(cv::Mat &res, const Point &p, const cv::Vec3b &colour);
void fillBoundary(cv::Mat& res, const Boundary &boundary, int percentage = 0);

double dist(const std::vector<double> &pattern, const std::vector<double> &object);
