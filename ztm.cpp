#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Common.h"
#include "Momentum.h"
#include "Operations.h"
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

const std::string img1 = foty_dir + "IMG_7845.png";
const std::string img2 = foty_dir + "IMG_7775.png";
const std::string img3 = foty_dir + "IMG_7778.png";
const std::string img4 = foty_dir + "IMG_7774.png";


struct Pattern {
	std::string file;
	int strength;
};

std::vector<std::string> images = {img3};

const std::vector<double> patternMomentums = {
 0.451335272085,0.102219487872,0.004930019743,0.002324325516,
 2524886116409.473632812500,0.000668413490,0.025371009989,
 -0.000325711778,-0.000007742543,0.000000031851
};

const std::vector<double> patternNoClose = {
0.463518449716,0.108229379040,0.004737206912,0.002028581125,
2032325693867.869873046875,0.000583888733,0.026654993547,
-0.000338578223,-0.000009956709,0.000000030174
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
	for(auto &object : objects) {
		Momentum momentum(image, object);
		auto values = momentum.countMomentums();
		std::cout << object << " - " << std::setprecision(12) << std::fixed << values << std::endl;
		double distance = dist(patternNoClose, values);
		std::cout << "Pattern diff: " <<  distance << std::endl;
		if (distance < min_dist) {
			min_dist = distance;
			best_obj = object;
		}
	}
	return best_obj;
}

void processImage(const std::string &image_file) {
	std::cout << "Start " << image_file << "..." << std::endl;
	cv::Mat I = cv::imread(image_file);

	//cv::imshow("real", I);
	//cv::waitKey(-1);
	Segmentation segmentator(I);

	segmentator.enhanceColours();
	//cv::imshow("enhance colors", segmentator.getImage());
	//cv::waitKey(-1);
	segmentator.segmentate();
	//auto &backgrounds = segmentator.getBackgroundObjects();
	//auto backgroundsImg = segmentator.getResult();
	//cv::imwrite(root_dir + "Backgrounds.png", backgroundsImg);

	//cv::imshow("segmentate-borders", backgroundsImg);
	//cv::waitKey(-1);
	//cv::imwrite(root_dir + "Result.png", segmentated);

	auto &objects = segmentator.getObjects();
	auto objImage = segmentator.getObjImage();
	//cv::imwrite(root_dir + "Objects.png", objImage);

	auto possibleObject = getBestObject(objImage, objects);
	fillBoundary(I, possibleObject, 100);

	cv::imshow("result", I);
	cv::waitKey(-1);
}

void parseArguments(int argc, char **argv, std::string &file) {
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
			break;
		}
	}

}

int main(int argc, char **argv) {
    /*for(const auto &pattern : patterns) {
    	processPattern(pattern);
    }
    for(const auto &anti : antipatterns) {
    	processPattern(anti);
    }
	std::string name = patterns[0].file.substr(root_dir.length(), patterns[0].file.size());
    findBestMomentum(name);
    saveResults();
    */
	std::string user_file;
	parseArguments(argc, argv, user_file);
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
