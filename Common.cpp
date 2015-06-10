/*
 *      Author: Zofia Abramowska
 *		File: Common.cpp
 */
#include "Common.h"

#include <vector>
#include <cmath>

std::vector<double> pattern = {0.463518449716, 0.108229379040, 0.004737206912, 0.002028581125, 0.000005972209, 0.000583888733, 0.026654993547, -0.000338578223, -0.000009956709, 0.000000030174};

/*
 * Sample values of correct object received through segmentation of sample pictures
 */
std::vector<std::vector<double>> sampleValues = {
{0.315980622011, 0.018789846058, 0.001601152838, 0.001108742427, -0.000000216425, 0.000028299088, 0.020263476857, -0.000061551301, 0.000013156185, 0.000000215664},
{0.514985207101, 0.036133271113, 0.035457702351, 0.000817790503, 0.000004403704, 0.000107701320, 0.057269123105, -0.004329988981, -0.000258357690, 0.000000234461},
{0.335776763949, 0.050499947022, 0.001491772140, 0.000545080661, 0.000000395099, 0.000086554971, 0.015561522047, -0.000118336435, -0.000001977187, 0.000000010113},
{0.338444800000, 0.039056939922, 0.001914113147, 0.000516859354, -0.000000490382, -0.000081959943, 0.018871985681, -0.000174656724, 0.000005616102, 0.000000135710},
{0.293451053089, 0.033491826073, 0.003315900010, 0.001782305199, 0.000001222217, 0.000067937558, 0.013155423621, -0.000191699351, 0.000001740928, 0.000000372960},
{0.384965670238, 0.065305347136, 0.002764011183, 0.001458509444, 0.000002780515, 0.000333313862, 0.020723305031, -0.000163187717, -0.000000444176, 0.000000018817},
{0.407049733483, 0.086861224199, 0.002644463027, 0.001509768133, 0.000002955313, 0.000420635885, 0.019707065332, -0.000141836862, -0.000001860059, 0.000000007191},
{0.393076082481, 0.076007424841, 0.003971432475, 0.002604894909, 0.000008260895, 0.000692650991, 0.019625345444, -0.000170817196, -0.000001168439, 0.000000014366},
{0.335712118276, 0.023030430634, 0.001443360964, 0.000907698598, -0.000000921196, -0.000096417535, 0.022418048931, -0.000066957796, 0.000023167549, 0.000000261624},
{0.306698432922, 0.039166161056, 0.001171068723, 0.000347636458, 0.000000211602, 0.000063718139, 0.013724441925, -0.000102929033, -0.000011328796, -0.000000000995}};


double standardDeviation(int i) {
	double expVal1 = 0;
	double expVal2 = 0;

	for (size_t j = 0; j < sampleValues[i].size(); j++) {
		expVal2 += sampleValues[i][j];
		expVal1 += std::pow(sampleValues[i][j], 2);
	}
	expVal2 = (double)std::pow((expVal1 / (double)sampleValues[i].size()), 2);
	expVal1 /= (double)sampleValues[i].size();

	return std::sqrt(expVal1 - expVal2);
}

double countDist(const std::vector<double> &values) {
	double dist = 0;
	for (size_t i = 0; i < sampleValues.size(); i++) {
		double sig = standardDeviation(i);
		double n = std::pow(pattern[i] - values[i], 2);
		dist += (n / std::pow(sig, 2));
	}
	return std::sqrt(dist);
}


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
