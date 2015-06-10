/*
 *      Author: Zofia Abramowska
 *		File: ztm.cpp
 */

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Common.h"
#include "Log.h"
#include "Momentum.h"
#include "Operations.h"
#include "Parameters.h"
#include "Segmentation.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>

/*
 * Constant values for sample pictures
 */
const std::string root_dir = "/home/zosia/studia/POBR/projekt/";
const std::string foty_dir = root_dir + "foty/";

const std::string img1 = foty_dir + "dwa1.png";
const std::string img2 = foty_dir + "dwa2.png";
const std::string img3 = foty_dir + "obrot1.png";
const std::string img4 = foty_dir + "obrot2.png";
const std::string img5 = foty_dir + "proste1.png";
const std::string img6 = foty_dir + "proste2.png";
const std::string img7 = foty_dir + "proste3.png";
const std::string img8 = foty_dir + "schowane.png";
const std::string img9 = foty_dir + "obrot3.png";

/*
 * These are default images if program is run without -f argument
 */
std::vector<std::string> images = {img1, img2, img3, img4, img5, img6, img7, img8, img9};

/*
 * Calculated pattern momentums values
 */
const std::vector<double> patternNoClose = {
	0.463518449716,0.108229379040,0.004737206912,0.002028581125,
	0.000005972209,0.000583888733,0.026654993547,-0.000338578223,
	-0.000009956709,0.000000030174
};

/*
 * Print human readable vector of values
 */
std::ostream& operator<<(std::ostream &os, const std::vector<double> &vec) {
	os << "{";
	for(auto el : vec) {
		os << el << ", ";
	}
	os << "}";
	return os;
}

/*
 * Get best objects in terms of fulfillment of condition of max distance to pattern
 */
std::vector<Boundary> getBestObjects(cv::Mat &image, const std::vector<Boundary> &objects) {
	std::vector<Boundary> best_objects;
	LOG_YELLOW("Pattern: " << std::setprecision(12) << std::fixed << patternNoClose);
	for (auto &object : objects) {
		Momentum momentum(image, object);
		auto values = momentum.countMomentums();

		double distance = dist(patternNoClose, values);
		//double distance = countDist(values);
		LOG_YELLOW("Pattern dist: " <<  distance);
		if (distance < AlgorithmParameters::MAX_DIST) {
			best_objects.push_back(object);
		}
		if (distance < 2 * AlgorithmParameters::MAX_DIST) {
			std::cout << std::setprecision(12) << std::fixed << values << std::endl;
		}
	}
	return best_objects;
}

/*
 * Show intermediate step - show potential background objects
 */
void showBackgrounds(const std::string &name, const Segmentation &segmentator, bool save = false) {
	const auto &backgrounds = segmentator.getBackgroundObjects();
	auto backgroundsImg = segmentator.getBgrImage();
	for (auto &background : backgrounds) {
		fillBoundary(backgroundsImg, background);
	}
	cv::imshow(name, backgroundsImg);
	cv::waitKey(-1);
	if (save)
		cv::imwrite(root_dir + "backgrounds-" + name, backgroundsImg);
}
/*
 * Show intermediate step - show potential logo objects
 */
void showObjects(const std::string &name, const Segmentation &segmentator, bool save = false) {
	const auto &objects = segmentator.getObjects();
	auto objImage = segmentator.getObjImage();

	for (auto &object : objects) {
		fillBoundary(objImage, object);
	}
	cv::imshow(name, objImage);
	cv::waitKey(-1);
	if (save)
		cv::imwrite(root_dir + "objects-" + name, objImage);
}

/*
 * Show intermediate step - show red and white color enhancement
 */
void showEnhanceColours(const std::string &name, const Segmentation &segmentator, bool save = false) {
	auto enhanceImg = segmentator.getImage();

	cv::imshow(name, enhanceImg);
	cv::waitKey(-1);
	if (save)
		cv::imwrite(root_dir + "enhanced-" + name, enhanceImg);
}

/*
 * Process given image
 */
void processImage(const std::string &image_file) {
	std::cout << "Start " << image_file << "..." << std::endl;
	cv::Mat I = cv::imread(image_file);
	auto pos = image_file.find_last_of('/');
	std::string name = std::string(image_file, pos + 1);
	std::cout << name << std::endl;
	Segmentation segmentator(I);
	segmentator.enhanceColours();
	segmentator.segmentate();
	showEnhanceColours(name, segmentator, true);
	showBackgrounds(name, segmentator, true);
	showObjects(name, segmentator, true);

	auto &objects = segmentator.getObjects();
	auto objImage = segmentator.getObjImage();

	if (!objects.empty()) {
		auto possibleObjects = getBestObjects(objImage, objects);
		if (possibleObjects.empty()) {
			LOG_RED( "No objects found!");
			cv::imshow(name, I);
			cv::waitKey(-1);
			return;
		}
		LOG_GREEN("Found " << possibleObjects.size() << " objects!");
		for (auto &possibleObject : possibleObjects)
			fillBoundary(I, possibleObject, 100);

		cv::imshow(name, I);
		cv::waitKey(-1);
		cv::imwrite(root_dir + "result-" + name, I);
	} else {
		LOG_RED( "No possible object found!");
	}
}

/*
 * Parse program arguments
 */
bool  parseArguments(int argc, char **argv, std::string &file) {
	const struct option longopts[] = {
			{"file", required_argument, 0, 'f'},
			{"help", no_argument, 0, 'h'}
	};
	opterr = 1;
	int arg;
	while((arg = getopt_long(argc, argv, "f:h", longopts, NULL)) != -1) {
		switch(arg) {
		case 'f':
			file = optarg;
			LOG_YELLOW("Image chosen: " << file);
			break;
		case 'h':
			LOG_YELLOW("Usage: --filename[-f] path_to_file");
			return false;
		}
	}
	return true;
}

int main(int argc, char **argv) {
	std::string user_file;
	if (!parseArguments(argc, argv, user_file))
		return 1;
	// If user file is specified, replace sample files with it
	if (!user_file.empty()) {
		images.clear();
		images.push_back(user_file);
	}
    for(const auto &image : images) {
    	processImage(image);
    }
    return 0;
}
