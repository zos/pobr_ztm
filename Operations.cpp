/*
 *      Author: Zofia Abramowska
 *		File: Operations.h
 */

#include "Operations.h"
#include "Common.h"

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
						return;
					}
				}
			);

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
						return;
					}
				}
			);

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

