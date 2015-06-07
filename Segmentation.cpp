/*
 *      Author: Zofia Abramowska
 *		File: Segmentation.cpp
 */

#include "Segmentation.h"
#include "Parameters.h"
#include <cmath>


static bool is_red(const cv::Vec3b &point) {
	return point[2] == 255 && point[1] == 0 && point[0] == 0;
}

Segmentation::Segmentation(cv::Mat &I) {
	m_image = I;
	m_imageMatrix = I;
	m_bgrImage = cv::Mat(I.rows, I.cols, CV_8UC3);
	m_bgrImage = cv::Scalar(255, 255, 255);
	m_enhanceImage = m_image.clone();
	m_enhanceImageMatrix = m_enhanceImage;
	m_objImage = m_bgrImage.clone();
	m_objImageMatrix = m_objImage;
	m_bgrImageMatrix = m_bgrImage;
}

static bool is_gray(cv::Vec3b &point) {
	const int max_diff = 10;
	int i_gray = gray(point);
	int r_gray = std::abs(point[2] - i_gray);
	int g_gray = std::abs(point[1] - i_gray);
	int b_gray = std::abs(point[0] - i_gray);

	return r_gray <= max_diff && g_gray <= max_diff && b_gray <= max_diff;
}

void Segmentation::enhanceColours() {
	for_pixel(m_image, [&](int row, int col) {
		auto &_I = m_imageMatrix;
		//if (_I(row, col)[2] > 2 * _I(row, col)[1] /*&& _I(row, col)[2] > 2 * _I(row,col)[0]*/) {
		//	_I(row, col) = {0, 0, 255};
		//}
		if (_I(row, col)[2] > 1.5 * _I(row, col)[1] /*&& _I(row, col)[2] > 2 * _I(row,col)[0]*/) {
			_I(row, col) = {0, 0, 255};
		}

		if (is_gray(_I(row, col))) {
			_I(row, col) = {255, 255, 255};
		}
	});
}

Point Segmentation::findNextRed(Point start) {
	Point x = {-1, -1};
	for (int row = start.x; row < m_image.rows; ++row) {
		for(int col = start.y; col < m_image.cols; ++col)
			if (is_red(m_imageMatrix(row, col)) && gray(m_bgrImageMatrix(row, col)) == 255) {
				x = Point{row, col};
				return x;
			}
		start.y = 0;
	}
	return x;
}

static void updateBoundary(Point &ne, Point &sw, const Point &x) {
	if (x.x < sw.x)
		sw.x = x.x;
	if (x.x > ne.x)
		ne.x = x.x;
	if (x.y < sw.y)
		sw.y = x.y;
	if (x.y > ne.y)
		ne.y = x.y;
}

void Segmentation::addPoint(Point x, unsigned char gray) {

	m_bgrImageMatrix(x.x, x.y) = {gray, gray, gray};
	//std::cout << "Adding point " << x << " : " << m_imageMatrix(x.x, x.y) << " - " << m_resultMatrix(x.x, x.y) << std::endl;
}

//std::ostream& operator<<(std::ostream &os, const cv::Vec3b &point) {
//	os << "{" << (int)point[0] << ", " << (int)point[1] << ", " << (int)point[2] << "}";
//	return os;
//}

void Segmentation::addNeighbors(std::queue<Point> &points, Point x) {
	//std::cout << "Getting neighbors for : " << x << std::endl;
	std::vector<Point> neighs = {{x.x - 1, x.y}, {x.x + 1, x.y}, {x.x, x.y - 1}, {x.x, x.y + 1}};
	cv::Vec3b startPoint = m_bgrImageMatrix(x.x, x.y);
	//Already visited start point
	if (startPoint[0] != 255)
		return;
	for(Point &p : neighs) {
		cv::Vec3b resPoint = m_bgrImageMatrix(p.x, p.y);
		cv::Vec3b imagePoint = m_imageMatrix(p.x, p.y);
		//std::cout << "Checking " << p << ":" <<  imagePoint  << " - " << resPoint << std::endl;
		//Not yet visited neighbor
		if (resPoint[0] == 255 && resPoint[1] == 255 && resPoint[2] == 255) {
			//std::cout << "Processing" << std::endl;
			points.push(p);
		}
	}
}

void Segmentation::BFS(Point x, unsigned char gray) {
	std::queue<Point> points;
	points.push(x);

	Point sw = {m_image.rows, m_image.cols};
	Point ne = {0, 0};
	long int pixels = 0;
	while(!points.empty()) {
		Point y = points.front();
		points.pop();
		if (!is_red(m_imageMatrix(y.x, y.y))) {
			continue;
		}
		//std::cout << "Point for " << (int)gray << " " << y << std::endl;
		pixels++;
		updateBoundary(ne, sw, y);
		addNeighbors(points, y);
		addPoint(y, gray);
		//if (pixels == 50)
		//	exit(1);
	}
	if (pixels < AlgorithmParameters::MIN_SPACE || sw == ne)
		return;

	m_bgrObjects.push_back({sw, ne ,gray});
}

void Segmentation::processBackground() {
	Point x = findNextRed(Point{0, 0});

	unsigned char gray = 0;
	while(x.x != -1) {
		//std::cout << "Red point: " << x << std::endl;
		BFS(x, gray++);
		Point y;
		if (x.y + 1 >= m_image.cols) {
			y.y = 0;
			y.x = x.x + 1;
		} else {
			y.x = x.x;
			y.y = x.y + 1;
		}
		x = findNextRed(y);
	}
}

enum State {
	BEGIN,
	IN_BGR,
	IN_WHITE
};

void Segmentation::fillObject(int row, int start_col, int end_col, unsigned char gray) {
	for (int i = start_col; i < end_col; ++i) {
		m_objImageMatrix(i, row) = {gray, gray, gray};
	}
}

void Segmentation::findObject(const Boundary &b) {
	static unsigned char obj_gray = 0;
	Point sw = {m_image.rows, m_image.cols};
	Point ne = {0, 0};
	int pixels = 0;
	for (int row = b.sw.y + 1; row < b.ne.y; ++row) {
		int start_col, end_col;
		State state = BEGIN;
		for (int col = b.sw.x + 1; col < b.ne.x; ++col) {
			std::cout << Point{col, row} << gray(m_bgrImageMatrix(col, row)) << std::endl;
			switch(state) {
			case BEGIN:
				if(gray(m_bgrImageMatrix(col, row)) == b.gray) {
					state = IN_BGR;
				}
				break;
			case IN_BGR:
				if(gray(m_bgrImageMatrix(col, row)) == 255) {
					state = IN_WHITE;
					start_col = col;
				}
				break;
			case IN_WHITE:
				if(gray(m_bgrImageMatrix(col, row)) == b.gray) {
					end_col = col;
					fillObject(row, start_col, end_col, obj_gray);
					pixels += end_col - start_col;
					std::cout << "Updating points " << Point{start_col, row} << ", " << Point{end_col - 1, row} << std::endl;
					updateBoundary(ne, sw, Point{start_col, row});
					updateBoundary(ne, sw, Point{end_col - 1, row});
					state = IN_BGR;
				}
				break;
			}
		}
	}
	if (pixels < 20 || pixels > 1000)
		return;
	m_objects.push_back(Boundary{sw, ne, obj_gray});
	std::cout << "Object " << Boundary{sw, ne, obj_gray} << std::endl;
}

void Segmentation::processObjects() {
	for(auto &bgr : m_bgrObjects) {
		findObject(bgr);
	}
}

void Segmentation::segmentate() {
	processBackground();
	processObjects();
}



