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


struct Pattern {
	std::string file;
	int strength;
};

std::vector<std::string> images = {img1, img2, img3, img4, img5, img6, img7, img8, img9};

const std::vector<double> patternNoClose = {
	0.463518449716,0.108229379040,0.004737206912,0.002028581125,
	0.000005972209,0.000583888733,0.026654993547,-0.000338578223,
	-0.000009956709,0.000000030174
};

std::ostream& operator<<(std::ostream &os, const std::vector<double> &vec) {
	os << "{";
	for(auto el : vec) {
		os << el << ", ";
	}
	os << "}";
	return os;
}

Boundary getBestObject(cv::Mat &image, const std::vector<Boundary> &objects) {
	Boundary best_obj;
	double min_dist = std::numeric_limits<double>::max();
	for (auto &object : objects) {
		Momentum momentum(image, object);
		auto values = momentum.countMomentums();
		std::cout << object << " - " << std::setprecision(12) << std::fixed << values << std::endl;
		double distance = dist(patternNoClose, values);
		std::cout << "Pattern dist: " <<  distance << std::endl;
		if (distance < min_dist) {
			min_dist = distance;
			best_obj = object;
		}
	}
	return best_obj;
}

std::vector<Boundary> getBestObjects(cv::Mat &image, const std::vector<Boundary> &objects) {
	std::vector<Boundary> best_objects;
	std::cout << "Pattern: " << std::setprecision(12) << std::fixed << patternNoClose << std::endl;
	for (auto &object : objects) {
		Momentum momentum(image, object);
		auto values = momentum.countMomentums();

		double distance = dist(patternNoClose, values);
		std::cout << "Pattern dist: " <<  distance << std::endl;
		if (distance < AlgorithmParameters::MAX_DIST) {
			best_objects.push_back(object);
		}
		if (distance < 2 * AlgorithmParameters::MAX_DIST) {
			std::cout << std::setprecision(12) << std::fixed << values << std::endl;
		}
	}
	return best_objects;
}

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

void showEnhanceColours(const std::string &name, const Segmentation &segmentator, bool save = false) {
	auto enhanceImg = segmentator.getImage();

	cv::imshow(name, enhanceImg);
	cv::waitKey(-1);
	if (save)
		cv::imwrite(root_dir + "enhanced-" + name, enhanceImg);
}

void processImage(const std::string &image_file) {
	std::cout << "Start " << image_file << "..." << std::endl;
	cv::Mat I = cv::imread(image_file);
	auto pos = image_file.find_last_of('/');
	std::string name = std::string(image_file, pos);
	std::cout << name << std::endl;
	Segmentation segmentator(I);
	segmentator.enhanceColours();
	segmentator.segmentate();
	//showEnhanceColours(name, segmentator);
	showBackgrounds(name, segmentator, true);
	showObjects(name, segmentator);

	auto &objects = segmentator.getObjects();
	auto objImage = segmentator.getObjImage();

	if (!objects.empty()) {
		//auto possibleObject = getBestObject(objImage, objects);
		auto possibleObjects = getBestObjects(objImage, objects);
		if (possibleObjects.empty()) {
			LOG_RED( "No objects found!");
			return;
		}
		LOG_GREEN("Found " << possibleObjects.size() << " objects!");
		for (auto &possibleObject : possibleObjects)
			fillBoundary(I, possibleObject, 100);

		cv::imshow(name, I);
		cv::waitKey(-1);
	} else {
		LOG_RED( "No possible object found!");
	}
}

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
			std::cout << "Image chosen: " << file << std::endl;
			break;
		case 'h':
			std::cout << "Usage: --filename[-f] path_to_file" << std::endl;
			return false;
		}
	}
	return true;
}

int main(int argc, char **argv) {
	std::string user_file;
	if (!parseArguments(argc, argv, user_file))
		return 1;
	if (!user_file.empty()) {
		images.clear();
		images.push_back(user_file);
	}
    for(const auto &image : images) {
    	processImage(image);
    }
    //cv::waitKey(-1);
    return 0;
}
