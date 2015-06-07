/*
 *      Author: Zofia Abramowska
 *		File: Segmentation.h
 */

#pragma once

#include <opencv2/core/core.hpp>
#include <vector>
#include <queue>

#include "Common.h"

class Segmentation {
public:
	Segmentation(cv::Mat &I);
	cv::Mat getResult() {
		return m_bgrImage;
	}
	cv::Mat getObjImage() {
		return m_objImage;
	}
	cv::Mat getImage() {
		return m_image;
	}
	void enhanceColours();
	void segmentate();
	std::vector<Boundary>& getBackgroundObjects() {
		return m_bgrObjects;
	}
	std::vector<Boundary>& getObjects() {
		return m_objects;
	}
private:
	Point findNextRed(Point start);
	void addPoint(Point x, unsigned char gray);
	void addNeighbors(std::queue<Point> &points, Point x);
	void processBackground();
	void processObjects();
	void findObject(const Boundary &b);
	void fillObject(int row, int start_col, int end_col, unsigned char gray);
	void BFS(Point begin, unsigned char gray);

	cv::Mat m_image;
	cv::Mat_<cv::Vec3b> m_imageMatrix;
	cv::Mat m_enhanceImage;
	cv::Mat_<cv::Vec3b> m_enhanceImageMatrix;
	cv::Mat m_bgrImage;
	cv::Mat_<cv::Vec3b> m_bgrImageMatrix;
	cv::Mat m_objImage;
	cv::Mat_<cv::Vec3b> m_objImageMatrix;
	std::vector<Boundary> m_bgrObjects;
	std::vector<Boundary> m_objects;
};

