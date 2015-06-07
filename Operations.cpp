/*
 *      Author: Zofia Abramowska
 *		File: Operations.h
 */

#include "Operations.h"
#include "Common.h"
#include "Parameters.h"
#include <queue>
#include <vector>
#include <iostream>

typedef cv::Mat_<cv::Vec3b> Matrix;

void dilate(cv::Mat &I) {
	Matrix _I = I;
	cv::Mat R = I.clone();
	Matrix _R = R;
	for_pixel(I,
		[&_I, &I, &_R] (int row, int col) {
			for_neighbour(row, col,
				[&_R, &_I, &I, row, col] (int x, int y) {
					Point point = normalize(I, x ,y);
					if (gray(_R(point.x, point.y)) == 0) {
						_I(row, col) = {0, 0, 0};
						return false;
					}
					return true;
				}
			);
			return true;

	}
	);
}
void erose(cv::Mat &I) {
	Matrix _I = I;
	cv::Mat R = I.clone();
	Matrix _R = R;
	for_pixel(I,
		[&_I, &I, &_R] (int row, int col) {
			for_neighbour(row, col,
				[&_R, &_I, &I, row, col] (int x, int y) {
					Point point = normalize(I, x ,y);
					if (gray(_R(point.x, point.y)) == 255) {
						_I(row, col) = {255, 255, 255};
						return false;
					}
					return true;
				}
			);
			return true;
	}
	);
}

void close(cv::Mat &I, int strength) {
	for (int i = strength; i != 0; i--) {
		dilate(I);
	}
	for (int i = strength; i != 0; i--) {
		erose(I);
	}
}



std::vector<cv::Vec3b> getMask(const cv::Mat_<cv::Vec3b> & _I, int p_i, int p_j, int N) {
	std::vector<cv::Vec3b> mask;
	for (int i = p_i - N / 2; i <= p_i + N / 2; i++)
		for (int j = p_j - N / 2; j <= p_j + N / 2; j++) {
			if (std::abs(p_j - j) + std::abs(p_i-i) * 2/3 <= N / 2)
				mask.push_back(_I(i, j));
			if (static_cast<long int>(mask.size()) > N * N) {
				std::cout << "Overflow!" << std::endl;
				exit(17);
			}

		}
	if (mask.size() == 0) std::cout << "Underflow!" << std::endl;
	return mask;
}

cv::Mat filterMedian(cv::Mat& I) {
	CV_Assert(I.depth() != sizeof(uchar));
	cv::Mat  res(I.rows, I.cols, CV_8UC3);
	unsigned int N = 3;
	unsigned int R = N/2;

	cv::Mat_<cv::Vec3b> _I = I;
	cv::Mat_<cv::Vec3b> _R = res;
	for (unsigned int i = N / 2; i < I.rows - N / 2; i++) {
		for (unsigned int j = N / 2; j < I.cols - N / 2; j++) {
			auto mask = getMask(_I, i, j, N);
			std::sort(mask.begin(), mask.end(), [](const cv::Vec3b &p, const cv::Vec3b &r) {
				return gray(p) < gray(r);
			});
			auto rank_pix = mask[R >= mask.size() ? mask.size() - 1 : R];
			_R(i, j) = rank_pix;
		}
	}
	return res;
}

