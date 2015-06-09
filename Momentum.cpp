#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

#include "Momentum.h"

std::map<std::string, std::function<double(cv::Mat&, const Boundary &b, const Point&)>> momentumMap = {
		{"M01", countM1},
		{"M02", countM2},
		{"M03", countM3},
		{"M04", countM4},
		{"M05", countM5},
		{"M06", countM6},
		{"M07", countM7},
		{"M08", countM8},
		{"M09", countM9},
		{"M10", countM10}
};


long Momentum::countMomentum(int p, int q) {
	cv::Mat_<cv::Vec3b> _I = m_image;
	Boundary &b = m_boundary;
	long momentum = 0;
	for (int i = b.sw.x; i < b.ne.x; i++) {
		for (int j = b.sw.y; j < b.ne.y; j++) {
			//std::cout << j << std::endl;
			//std::cout << (int)_I(i,j)[0] << " : " << b.gray << " : " << (_I(j, i)[0] == b.gray ? 1 : 0) << std::endl;

			// i and j are switched!!!
			momentum += std::pow(i - b.sw.x, p) * std::pow(j - b.sw.y, q) * (_I(j, i)[0] == b.gray ? 1 : 0);
		}
	}
	return momentum;
}

Point Momentum::countCG() {
	Point cg;
	cg.x = countMomentum(1, 0) / m00;
	cg.y = countMomentum(0, 1) / m00;
	return cg;
}

double Momentum::countMpq(int p, int q) {
	cv::Mat_<cv::Vec3b> _I = m_image;
	Point &cg = m_cg;
	Boundary &b = m_boundary;
	long momentum = 0;
	for (int i = b.sw.x; i < b.ne.x; i++) {
		for (int j = b.sw.y; j < b.ne.y; j++) {
			momentum += std::pow((i - b.sw.x) - cg.x, p) * std::pow((j - b.sw.y) - cg.y, q) * (_I(j, i)[0] < 255 ? 1 : 0);
		}
	}
	return momentum;
}

double Momentum::countMM1() {
	return (M20 + M02) / std::pow(m00,2);
}
double Momentum::countMM2() {
	return (std::pow(M20 - M02, 2) + 4 * std::pow(M11, 2)) /
			std::pow(m00, 4);
}

double Momentum::countMM3() {
	return (std::pow(M30 - 3 * M12, 2) +
		std::pow(3 * M21 - M03, 2)) /
		std::pow(m00, 5);
}
double Momentum::countMM4() {
	return (std::pow(M30 + M12, 2) + std::pow(M21 + M03, 2)) / std::pow(m00,5);
}
double Momentum::countMM5() {
	return ((M30 - 3 * M12) * (M30 + M12) * (std::pow(M30 + M12, 2) - 3 * std::pow(M21 + M03, 2)) +
			(3 * M21 - M03) * (M21 + M03) * (3 * std::pow(M30 + M12,2) - std::pow(M21 + M03, 2))) / std::pow(m00, 10);
}
double Momentum::countMM6() {
	return ((M20 - M02) * (std::pow(M30 + M12, 2) - std::pow(M21 + M03, 2)) + 4 * M11 *
			(M30 + M12) * (M21 + M03)) / std::pow(m00, 7);
}
double Momentum::countMM7() {
	return (M20 * M02 - std::pow(M11, 2)) / std::pow(m00, 4);
}

double Momentum::countMM8()  {
	return  (M30 * M12 + M21 * M03 - std::pow(M12, 2) - std::pow(M21, 2)) / std::pow(m00, 5);
}
double Momentum::countMM9() {
	return (M20 * (M21 * M03 - std::pow(M12, 2)) + M02 * (M03 * M12 - std::pow(M21, 2)) - M11 * (M30 * M03 - M21 * M12)) / std::pow(m00, 7);
}
double Momentum::countMM10() {
	return (std::pow(M30 * M03 - M12 * M21, 2) - 4 * (M30 * M12 - std::pow(M21,2)) * (M03 * M21 - M12)) / std::pow(m00, 10);
}

double countW1(long space) {
	return 2 * std::sqrt(space / pi);
}

double countW3( long space, long circuit) {
	return (circuit / (2 * std::sqrt(pi * space))) - 1;
}

long countMomentum(cv::Mat &I, const Boundary &b, int p, int q) {
	cv::Mat_<cv::Vec3b> _I = I;
	long momentum = 0;
	for (int i = b.sw.x; i < b.ne.x; i++) {
		for (int j = b.sw.y; j < b.ne.y; j++) {
			//std::cout << j << std::endl;
			momentum += std::pow(i - b.sw.x, p) * std::pow(j - b.sw.y, q) * (_I(j, i)[0] == b.gray ? 1 : 0);
		}
	}
	return momentum;
}

Point countCG(cv::Mat &I, const Boundary &b) {
	Point cg;
	long m00 = countMomentum(I, b, 0, 0);
	cg.x = countMomentum(I, b, 1, 0) / m00;
	cg.y = countMomentum(I, b, 0, 1) / m00;
	return cg;
}

double countMpq(cv::Mat &I, const Boundary &b, const Point &cg, int p, int q) {
	cv::Mat_<cv::Vec3b> _I = I;
	long momentum = 0;
	for (int i = b.sw.x; i < b.ne.x; i++) {
		for (int j = b.sw.y; j < b.ne.y; j++) {
			momentum += std::pow((i - b.sw.x) - cg.x, p) * std::pow((j - b.sw.y) - cg.y, q) * (_I(j, i)[0] < 255 ? 1 : 0);
		}
	}
	return momentum;
}

double countM1(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M20 = countMpq(I, b, cg, 2, 0);
	double M02 = countMpq(I, b, cg, 0, 2);
	double m00 = countMomentum(I, b, 0, 0);
	return (M20 + M02) / std::pow(m00,2);
}
double countM2(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M20 = countMpq(I, b, cg, 2, 0);
	double M02 = countMpq(I, b, cg, 0, 2);
	double M11 = countMpq(I, b, cg, 1, 1);
	double m00 = countMomentum(I, b, 0, 0);
	return (std::pow(M20 - M02, 2) + 4 * std::pow(M11, 2)) /
			std::pow(m00, 4);
}

double countM3(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M30 = countMpq(I, b, cg, 3, 0);
	double M03 = countMpq(I, b, cg, 0, 3);
	double M21 = countMpq(I, b, cg, 2, 1);
	double M12 = countMpq(I, b, cg, 1, 2);
	double m00 = countMomentum(I, b, 0, 0);
	return (std::pow(M30 - 3 * M12, 2) +
		std::pow(3 * M21 - M03, 2)) /
		std::pow(m00, 5);
}
double countM4(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M30 = countMpq(I, b, cg, 3, 0);
	double M03 = countMpq(I, b, cg, 0, 3);
	double M21 = countMpq(I, b, cg, 2, 1);
	double M12 = countMpq(I, b, cg, 1, 2);
	double m00 = countMomentum(I, b, 0, 0);
	return (((std::pow(M30 + M12, 2) + std::pow(M21 + M03, 2)) / std::pow(m00,5)));
}
double countM5(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M30 = countMpq(I, b, cg, 3, 0);
	double M03 = countMpq(I, b, cg, 0, 3);
	double M21 = countMpq(I, b, cg, 2, 1);
	double M12 = countMpq(I, b, cg, 1, 2);
	double m00 = countMomentum(I, b, 0, 0);
	return ((M30 - 3 * M12) * (M30 + M12) * ((std::pow(M30 + M12, 2) - 3 * std::pow(M21 - M03, 2)) +
			(3 * M21 - M03) * (M21 + M03) * (3 * std::pow(M30 + M12,2) - std::pow(M21 + M03, 2)))) / std::pow(m00, 10);
}
double countM6(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M20 = countMpq(I, b, cg, 2, 0);
	double M02 = countMpq(I, b, cg, 0, 2);
	double M11 = countMpq(I, b, cg, 1, 1);
	double M21 = countMpq(I, b, cg, 2, 1);
	double M12 = countMpq(I, b, cg, 1, 2);
	double M30 = countMpq(I, b, cg, 3, 0);
	double M03 = countMpq(I, b, cg, 0, 3);
	double m00 = countMomentum(I, b, 0, 0);
	return ((M20 - M02) * (std::pow(M30 + M12, 2) - std::pow(M21 + M03, 2)) + 4 * M11 *
			(M30 + M12) * (M21 + M03)) / std::pow(m00, 7);
}
double countM7(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M20 = countMpq(I, b, cg, 2, 0);
	double M02 = countMpq(I, b, cg, 0, 2);
	double M11 = countMpq(I, b, cg, 1, 1);
	double m00 = countMomentum(I, b, 0, 0);
	return (M20 * M02 - std::pow(M11, 2)) / std::pow(m00, 4);
}

double countM8(cv::Mat &I, const Boundary &b, const Point &cg)  {
	double M30 = countMpq(I, b, cg, 3, 0);
	double M03 = countMpq(I, b, cg, 0, 3);
	double M21 = countMpq(I, b, cg, 2, 1);
	double M12 = countMpq(I, b, cg, 1, 2);
	double m00 = countMomentum(I, b, 0, 0);
	return  (M30 * M12 + M21 * M03 - std::pow(M12, 2) - std::pow(M21, 2)) / std::pow(m00, 5);
}
double countM9(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M20 = countMpq(I, b, cg, 2, 0);
	double M02 = countMpq(I, b, cg, 0, 2);
	double M11 = countMpq(I, b, cg, 1, 1);
	double M21 = countMpq(I, b, cg, 2, 1);
	double M12 = countMpq(I, b, cg, 1, 2);
	double M30 = countMpq(I, b, cg, 3, 0);
	double M03 = countMpq(I, b, cg, 0, 3);
	double m00 = countMomentum(I, b, 0, 0);
	return (M20 * (M21 * M03 - std::pow(M12, 2)) + M02 * (M03 * M12 - std::pow(M21, 2)) - M11 * (M30 * M03 - M21 * M12)) / std::pow(m00, 7);
}
double countM10(cv::Mat &I, const Boundary &b, const Point &cg) {
	double M30 = countMpq(I, b, cg, 3, 0);
	double M03 = countMpq(I, b, cg, 0, 3);
	double M21 = countMpq(I, b, cg, 2, 1);
	double M12 = countMpq(I, b, cg, 1, 2);
	double m00 = countMomentum(I, b, 0, 0);
	return (std::pow(M30 * M03 - M12 * M21, 2) - 4 * (M30 * M12 - std::pow(M21,2)) * (M03 * M21 - M12)) / std::pow(m00, 10);
}
