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
	cv::Mat getBgrImage() const {
		return m_bgrImage.clone();
	}
	cv::Mat getObjImage() const {
		return m_objImage.clone();
	}
	cv::Mat getImage() const {
		return m_image.clone();
	}
	/*
	 * Enhance red and gray colours
	 */
	void enhanceColours();
	/*
	 * Find possible background and logo objects
	 */
	void segmentate();
	/*
	 * Get all found potential background objects
	 */
	const std::vector<Boundary>& getBackgroundObjects() const {
		return m_bgrObjects;
	}
	/*
	 * Get all potential background objects
	 */
	const std::vector<Boundary>& getObjects() const {
		return m_objects;
	}
private:
	/*
	 * Find next red (in m_image) not yet visited (not marked in backgrounds image) point
	 */
	Point findNextRed(Point start);
	/*
	 * Mark background point (marked in backgrounds image)
	 */
	void addBackgroundPoint(Point x, unsigned char gray);
	/*
	 * Add point neighbors to queue
	 */
	void addNeighbors(std::queue<Point> &points, Point x);
	/*
	 * Find all potential background objects
	 */
	void processBackground();
	/*
	 * Find all potential logo objects
	 */
	void processObjects();
	/*
	 * Find potential logo object in background boundary rectangle
	 */
	void findObject(const Boundary &b);
	/*
	 * Fill line of a potential object (marked in objects image)
	 */
	void fillObject(int row, int start_col, int end_col, unsigned char gray);
	/*
	 * Find all red pixels connected to start point (on enhanced image)
	 * and marked them in gray (on backgrounds image)
	 */
	void BFS(Point start, unsigned char gray);

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

