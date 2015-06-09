#pragma once

#include "opencv2/core/core.hpp"
#include <vector>
#include "Common.h"
#include <map>
#include <functional>

extern std::map<std::string, std::function<double(cv::Mat&, const Boundary &b, const Point&)>> momentumMap;

std::vector<cv::Vec3b> getMask(const cv::Mat_<cv::Vec3b> & _I, int p_i, int p_j, int N);

double countW1(long space);
double countW3( long space, long circuit);

long countMomentum(cv::Mat &I, const Boundary &b, int p, int q);
Point countCG(cv::Mat &I, const Boundary &b);
double countMpq(cv::Mat &I, const Boundary &b, const Point &cg, int p, int q);

double countM1(cv::Mat &I, const Boundary &b, const Point &cg);
double countM2(cv::Mat &I, const Boundary &b, const Point &cg);
double countM3(cv::Mat &I, const Boundary &b, const Point &cg);
double countM4(cv::Mat &I, const Boundary &b, const Point &cg);
double countM5(cv::Mat &I, const Boundary &b, const Point &cg);
double countM6(cv::Mat &I, const Boundary &b, const Point &cg);
double countM7(cv::Mat &I, const Boundary &b, const Point &cg);
double countM8(cv::Mat &I, const Boundary &b, const Point &cg);
double countM9(cv::Mat &I, const Boundary &b, const Point &cg);
double countM10(cv::Mat &I, const Boundary &b, const Point &cg);

class Momentum {
public:
	Momentum(cv::Mat &I, const Boundary &b) : m_image(I), m_boundary(b) {
		m00 = countMomentum(0,0);
		m_cg = countCG();
		M03 = countMpq(0, 3);
		M30 = countMpq(3, 0);
		M02 = countMpq(0, 2);
		M20 = countMpq(2, 0);
		M12 = countMpq(1, 2);
		M21 = countMpq(2, 1);
		M11 = countMpq(1, 1);
		m_momentums["M01"] = &Momentum::countMM1;
		m_momentums["M02"] = &Momentum::countMM2;
		m_momentums["M03"] = &Momentum::countMM3;
		m_momentums["M04"] = &Momentum::countMM4;
		m_momentums["M05"] = &Momentum::countMM5;
		m_momentums["M06"] = &Momentum::countMM6;
		m_momentums["M07"] = &Momentum::countMM7;
		m_momentums["M08"] = &Momentum::countMM8;
		m_momentums["M09"] = &Momentum::countMM9;
		m_momentums["M10"] = &Momentum::countMM10;
	}
	double countMM1();
	double countMM2();
	double countMM3();
	double countMM4();
	double countMM5();
	double countMM6();
	double countMM7();
	double countMM8();
	double countMM9();
	double countMM10();

	double getm00() {
		return m00;
	}

	Point getCG() {
		return m_cg;
	}

	std::map<std::string, double(Momentum::*)(void)> getMap() {
		return m_momentums;
	}
	std::vector<double> countMomentums() {
		std::vector<double> momVector;
		for(auto &momentum : m_momentums) {
			auto func = momentum.second;
			momVector.push_back((this->*func)());
		}
		return momVector;
	}

private:
	cv::Mat m_image;
	Boundary m_boundary;
	long countMomentum(int p, int q);
	double countMpq(int p, int q);
	Point countCG();


	std::map<std::string, double(Momentum::*)(void)> m_momentums;
	Point m_cg;
	double m00;
	double M03, M30, M12, M21, M02, M20, M11;
};
