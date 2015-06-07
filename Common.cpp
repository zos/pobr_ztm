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
	long max_vert = 0, min_vert = I.rows, max_horiz = 0, min_horiz = I.cols;
	for (long i = 0; i < I.rows; i++) {
		for (long j = 0; j < I.cols; j++) {
			if (gray(_I(i, j)) == gray_level) {
				if (i > max_vert)
					max_vert = i;
				if (i < min_vert)
					min_vert = i;
				if (j > max_horiz)
					max_horiz = j;
				if (j < min_horiz)
					min_horiz = j;
			}
		}
	}

	return Boundary{ { min_horiz - 1, min_vert - 1}, { max_horiz + 1, max_vert + 1}, gray_level };
}

void fillPoint(cv::Mat &res, const Point &p, const cv::Vec3b &colour) {
	cv::Mat_<cv::Vec3b> _R = res;
	_R(p.y, p.x) = colour;
}

void fillBoundary(cv::Mat& res, const Boundary &boundary, int percentage) {
	cv::Mat_<cv::Vec3b> _R = res;
	unsigned char blue = 255 * (100 - percentage)/100;
	unsigned char green = 255 * (percentage)/100;
	cv::Vec3b colour = {blue, 0, green};

	for (long i = boundary.sw.x; i < boundary.ne.x; i++) {
		_R(i, boundary.sw.y) = colour;
		_R(i, boundary.ne.y) = colour;
	}
	for (long i = boundary.sw.y; i < boundary.ne.y; i++) {
		_R(boundary.sw.x, i) = colour;
		_R(boundary.ne.x, i) = colour;
	}
}

Point boundaryCenter(const Boundary &b) {
	return {(b.sw.x + b.ne.x)/2, (b.sw.y + b.ne.y)/2};
}
