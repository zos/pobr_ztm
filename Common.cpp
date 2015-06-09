/*
 *      Author: Zofia Abramowska
 *		File: common.h
 */
#include "Common.h"

Point normalize(cv::Mat &I, int row, int col) {
	if (row < 0) {
		row = 0;
	} else if (row >= I.rows) {
		row = I.rows - 1;
	}

	if (col < 0) {
		col = 0;
	} else if (col >= I.cols) {
		col = I.cols - 1;
	}
	return {row, col};
}

int gray(const cv::Vec3b &pix) {
	//std::cout << (int)pix[0] << "," << (int)pix[1] << "," << (int)pix[2] << std::endl;
	int gray = (pix[0] + pix[1] + pix[2]) / 3;
	//std::cout << " gray: " << gray << std::endl;
	return gray;
}

std::ostream& operator<<(std::ostream &os, const Point &p) {
	os << "(" << p.x << ", " << p.y << ")";
	return os;
}

std::ostream& operator<<(std::ostream &os, const Boundary &b) {
	os << b.sw << " - " << b.ne << " [" << b.gray << "]";
	return os;
}

std::ostream& operator<<(std::ostream &os, const std::set<int> &s) {
	os << "{";
	for (auto i : s) {
		os << i << ", ";
	}
	os << "}";
	return os;
}

Boundary countBoundary(cv::Mat &I, short gray_level) {
	cv::Mat_<cv::Vec3b> _I = I;
	long max_row = 0, min_row = I.rows, max_col = 0, min_col = I.cols;
	for (long row = 0; row < I.rows; row++) {
		for (long col = 0; col < I.cols; col++) {
			if (gray(_I(row, col)) == gray_level) {
				if (row > max_row)
					max_row = row;
				if (row < min_row)
					min_row = row;
				if (col > max_col)
					max_col = col;
				if (col < min_col)
					min_col = col;
			}
		}
	}

	return Boundary{ { min_col - 1, min_row - 1}, { max_col + 1, max_row + 1}, gray_level };
}

void fillPoint(cv::Mat &res, const Point &p, const cv::Vec3b &colour) {
	cv::Mat_<cv::Vec3b> _R = res;
	_R(p.y, p.x) = colour;
}

void fillBoundary(cv::Mat& res, const Boundary &boundary, int percentage) {
	cv::Mat_<cv::Vec3b> _R = res;
	unsigned char blue = 255 * (100 - percentage)/100;
	unsigned char green = 255 * (percentage)/100;
	cv::Vec3b colour = {blue, green, 0};

	for (long col = boundary.sw.x; col < boundary.ne.x; col++) {
		_R(boundary.sw.y, col) = colour;
		_R(boundary.ne.y, col) = colour;
	}
	for (long row = boundary.sw.y; row < boundary.ne.y; row++) {
		_R(row, boundary.sw.x) = colour;
		_R(row, boundary.ne.x) = colour;
	}
}

Point boundaryCenter(const Boundary &b) {
	return {(b.sw.x + b.ne.x)/2, (b.sw.y + b.ne.y)/2};
}

double dist(const std::vector<double> &pattern, const std::vector<double> &object) {
	double diff = 0;
	for(size_t i = 0; i < pattern.size(); i++) {
		diff += std::abs(object[i] - pattern[i])/std::abs(pattern[i]);
	}
	return std::sqrt(diff);
}
