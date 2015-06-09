/*
 *      Author: Zofia Abramowska
 *		File: Segmentation.cpp
 */

#include "Log.h"
#include "Segmentation.h"
#include "Parameters.h"
#include <cmath>


static bool is_red(const cv::Vec3b &point) {
	return point[2] == 255 && point[1] == 0 && point[0] == 0;
}

Segmentation::Segmentation(cv::Mat &I) {
	m_image = I.clone();
	m_imageMatrix = m_image;

	m_enhanceImage = I.clone();
	m_enhanceImageMatrix = m_enhanceImage;

	m_bgrImage = cv::Mat(I.rows, I.cols, CV_8UC3);
	m_bgrImage = cv::Scalar(255, 255, 255);
	m_bgrImageMatrix = m_bgrImage;

	m_objImage = m_bgrImage.clone();
	m_objImageMatrix = m_objImage;

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
		if (_I(row, col)[2] > AlgorithmParameters::MIN_RED_FACTOR * _I(row, col)[1] /*&& _I(row, col)[2] > 2 * _I(row,col)[0]*/) {
			_I(row, col) = {0, 0, 255};
		}

		if (is_gray(_I(row, col))) {
			_I(row, col) = {255, 255, 255};
		}
	});
}

Point Segmentation::findNextRed(Point start) {
	Point x = {-1, -1};
	for (int row = start.y; row < m_image.rows; ++row) {
		for(int col = start.x; col < m_image.cols; ++col)
			if (is_red(m_imageMatrix(row, col)) && gray(m_bgrImageMatrix(row, col)) == 255) {
				x = Point{col, row};
				return x;
			}
		start.x = 0;
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

	m_bgrImageMatrix(x.y, x.x) = {gray, gray, gray};
	//std::cout << "Adding point " << x << " : " << m_imageMatrix(x.x, x.y) << " - " << m_resultMatrix(x.x, x.y) << std::endl;
}

//std::ostream& operator<<(std::ostream &os, const cv::Vec3b &point) {
//	os << "{" << (int)point[0] << ", " << (int)point[1] << ", " << (int)point[2] << "}";
//	return os;
//}

void Segmentation::addNeighbors(std::queue<Point> &points, Point x) {
	//std::cout << "Getting neighbors for : " << x << std::endl;
	std::vector<Point> neighs = {{x.x - 1, x.y}, {x.x + 1, x.y}, {x.x, x.y - 1}, {x.x, x.y + 1}};
								 //{x.x - 1, x.y - 1}, {x.x-1, x.y+1}, {x.x+1, x.y-1}, {x.x+1, x.y+1}};
	cv::Vec3b startPoint = m_bgrImageMatrix(x.y, x.x);
	//Already visited start point
	if (startPoint[0] != 255)
		return;
	for(Point &p : neighs) {
		cv::Vec3b resPoint = m_bgrImageMatrix(p.y, p.x);
		//Not yet visited neighbor
		if (resPoint[0] == 255 && resPoint[1] == 255 && resPoint[2] == 255) {
			points.push(p);
		}
	}
}

void Segmentation::BFS(Point x, unsigned char gray) {
	std::queue<Point> points;
	points.push(x);

	Point sw = {m_image.cols, m_image.rows};
	Point ne = {0, 0};
	long int pixels = 0;
	while(!points.empty()) {
		Point y = points.front();
		points.pop();
		if (!is_red(m_imageMatrix(y.y, y.x))) {
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
	if (pixels < AlgorithmParameters::MIN_BGR_SPACE || sw == ne) {
		LOG_YELLOW("Ignoring too small background");
		return;
	}
	//std::cout << "Background : " << Boundary{sw, ne ,gray} << std::endl;
	m_bgrObjects.push_back(Boundary{sw, ne ,gray});
}

void Segmentation::processBackground() {
	std::cout << "Finding background objects" << std::endl;
	Point x = findNextRed(Point{0, 0});

	unsigned char gray = 0;
	while(x.x != -1) {
		//std::cout << "Red point: " << x << std::endl;
		BFS(x, gray);
		gray += 5;
		if (gray == 255)
			gray += 5;
		Point y;
		if (x.x + 1 >= m_image.cols) {
			y.x = 0;
			y.y = x.y + 1;
		} else {
			y.y = x.y;
			y.x = x.x + 1;
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
		m_objImageMatrix(row, i) = {gray, gray, gray};
	}
}

void Segmentation::findObject(const Boundary &b) {
	static unsigned char obj_gray = 0;
	Point sw = {m_image.cols, m_image.rows};
	Point ne = {0, 0};
	int pixels = 0;
	for (int row = b.sw.y + 1; row < b.ne.y; ++row) {
		int start_col, end_col;
		State state = BEGIN;
		for (int col = b.sw.x + 1; col < b.ne.x; ++col) {
			switch(state) {
			case BEGIN:
				if(gray(m_bgrImageMatrix(row, col)) != 255) {
					state = IN_BGR;
					//std::cout << Point{col, row} << " background start" << std::endl;
				}
				break;
			case IN_BGR:
				if(gray(m_bgrImageMatrix(row, col)) == 255) {
					state = IN_WHITE;
					start_col = col;
					//std::cout << Point{col, row} << " white start" << std::endl;
				}
				break;
			case IN_WHITE:
				if(gray(m_bgrImageMatrix(row, col)) != 255) {
					end_col = col;
					//std::cout << Point{col, row} << " white stop" << std::endl;
					if (gray(m_bgrImageMatrix(row, start_col - 1)) != b.gray &&
							gray(m_bgrImageMatrix(row, end_col)) != b.gray) {
						state = IN_BGR;
						continue;
					}
					fillObject(row, start_col, end_col, obj_gray);
					pixels += end_col - start_col;
					//std::cout << "Updating points " << Point{start_col, row} << ", " << Point{end_col - 1, row} << std::endl;
					updateBoundary(ne, sw, Point{start_col, row});
					updateBoundary(ne, sw, Point{end_col - 1, row});
					state = IN_BGR;
				}
				break;
			}
		}
	}
	if (pixels < AlgorithmParameters::MIN_OBJ_SPACE ||
			sw.x == ne.x || sw.y == ne.y) {
		LOG_YELLOW("Ignoring too small object");
		return;
	}

	if (pixels > AlgorithmParameters::MAX_OBJ_SPACE) {
		LOG_YELLOW("Ignoring too big object");
		return;
	}
	m_objects.push_back(Boundary{sw, ne, obj_gray});
	std::cout << "Object : " << Boundary{sw, ne, obj_gray} << std::endl;
	obj_gray+=5;
}

void Segmentation::processObjects() {
	std::cout << "Finding possible objects" << std::endl;
	for(auto &bgr : m_bgrObjects) {
		findObject(bgr);
	}
}

void Segmentation::segmentate() {
	processBackground();
	processObjects();
}



