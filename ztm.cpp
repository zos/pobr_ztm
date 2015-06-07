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


const std::string root_dir = "/home/zosia/studia/POBR/projekt/";
const std::string foty_dir = root_dir + "foty/";
const std::string ztm = root_dir + "logoztm.jpg";
const std::string cola = root_dir + "logocola.jpg";
const std::string pizza = root_dir + "logopizza.png";
const std::string pizzamale = root_dir + "logopizzamale.png";
const std::string zwierciadlo = root_dir + "logozwierciadlo.jpg";
const std::string zwierciadlomale = root_dir + "logozwierciadlomale.jpg";

const std::string img1 = foty_dir + "IMG_7845.png";
const std::string img2 = foty_dir + "IMG_7775.png";
const std::string img3 = foty_dir + "IMG_7778.png";
const std::string img4 = foty_dir + "IMG_7774.png";
const std::string res_file = root_dir + "res.csv";

struct Pattern {
	std::string file;
	int strength;
};

std::vector<Pattern> patterns = {{ztm, 4}};
std::vector<Pattern> antipatterns = {{cola, 4}, {pizza, 6}, {pizzamale, 5}, {zwierciadlo, 6}, {zwierciadlomale, 6}};

std::vector<std::string> images = {img3};

std::map<std::string, std::vector<double>> results;

bool checkBounder(const cv::Mat_<cv::Vec3b> &_I, long i, long j) {
	if (_I(i, j)[0] != 0)
		return false;
	if (_I(i - 1, j)[0] == 0 && _I(i + 1, j)[0] == 0 && _I(i, j - 1)[0] == 0 && _I(i, j + 1)[0] == 0)
		return false;
	return true;
}

void fillCG(cv::Mat &res, const Point &cg) {
	fillPoint(res, cg, cv::Vec3b{0,0,0});
}

cv::Mat processOne(cv::Mat &I, const std::string &name, int strength) {
	cv::Mat_<cv::Vec3b> _I = I;
	cv::Mat res = I.clone();
	cv::Mat_<cv::Vec3b> _R = res;

	for_pixel(I,
		[&_I, &_R](int row, int col) {
			if (gray(_I(row,col)) > 200) {
				_R(row, col) = {0, 0, 0};
			} else {
				_R(row, col) = {255, 255, 255};
			}
	});

	close(res, strength);
	auto boundary = countBoundary(res);
	std::cout << "Boundary: " << boundary << std::endl;
	Momentum mom(res, boundary);
//	auto m00 = countMomentum(res, boundary, 0, 0);
	//auto m00_2 = mom.getm00();

	//auto cg = countCG(res, boundary);
	//auto cg_2 = mom.getCG();

	for (auto momentum : mom.getMap()) {
		auto func = momentum.second;
		auto mx = (mom.*func)();
		results[name].push_back(mx);
		std::cout << momentum.first << " : " << mx  << std::endl;
	}
	// TODO make tests!!!
	/*
	int i = 0;
	for (auto momentum : momentumMap) {
		std::cout << "Counting " << momentum.first << std::endl;
		auto mx = momentum.second(res, boundary, cg);
		if (mx != results[name][i])
			std::cout << "M" << i + 1 << "DIFFERS!!!" << std::endl;
		i++;

	}*/

	return res;
}

void saveResults() {
	std::ofstream f(res_file, std::ofstream::out | std::ofstream::trunc);
	if (!f.is_open()) {
		std::cout << "Couldn't open save file" << std::endl;
		return;
	}

	for (auto &mom : momentumMap) {
		f << "," << mom.first;
	}
	f << std::endl;
	for (auto &res : results) {
		f << res.first;
		for (auto &val : res.second) {
			f << "," << std::setprecision(12) << std::fixed << val;
		}
		f << std::endl;
	}
	if (!f) {
		std::cout << "Writing failed" << std::endl;
	}
}

void findBestMomentum(const std::string &name) {
	std::vector<double> diffs;
	double min_diff = std::numeric_limits<double>::max();
	std::cout << "Results: " << results.begin()->second.size() << std::endl;
	for (unsigned int i = 0; i < results.begin()->second.size(); i++) {
		double value, diff;
		value = results[name][i];
		min_diff = std::numeric_limits<double>::max();
		for(auto &res : results) {
			if(res.first != name) {
				diff = std::abs(res.second[i] - value) / std::abs(value) * 100;
				///std::cout << "Diff between " << name << " and " << res.first << " on M" << i + 1 << " : " << diff << "%"  << std::endl;
				if (diff < min_diff) {
					min_diff = diff;
				}
			}
		}
		diffs.push_back(min_diff);
	}
	double max_diff = diffs[0];
	unsigned int max = 0;
	for (unsigned int i = 1; i < diffs.size(); i++) {
		std::cout << "M" << i + 1 << " : " << diffs[i] << "%" << std::endl;
		//std::cout << diffs[i] << "-" << max_diff << std::endl;
		if (diffs[i] > max_diff) {
			max_diff = diffs[i];
			max = i;
		}
	}

	std::cout << "Best momentum is : " << "M" << max + 1 << std::endl;
}

void processPattern(const Pattern &pattern) {
	std::cout << "Start " << pattern.file << "..." << std::endl;
	cv::Mat image = cv::imread(pattern.file);
	std::cout << "Channels: " << image.channels() << std::endl;
	std::string name = pattern.file.substr(root_dir.length(), pattern.file.size());
	cv::Mat show = processOne(image, name, pattern.strength);

	//cv::imshow(file, image);
	//cv::imshow("Result",show);
	//cv::imwrite("Result.dib", show);
	std::cout << "End " << pattern.file << std::endl;
}

void processImage(const std::string &image_file) {
	std::cout << "Start " << image_file << "..." << std::endl;
	cv::Mat I = cv::imread(image_file);
	cv::Mat res = I.clone();

	cv::imshow("real", I);
	cv::waitKey(-1);
	Segmentation segmentator(res);

	segmentator.enhanceColours();
	cv::imshow("enhance colors", segmentator.getImage());
	cv::waitKey(-1);
	segmentator.segmentate();
	auto &objects = segmentator.getBackgroundObjects();
	auto segmentated = segmentator.getResult();
	cv::imwrite(root_dir + "Result.png", segmentated);
	for(auto &object : objects) {
		//std::cout << "Boundary : " << object << std::endl;
		fillBoundary(segmentated, object);
	}
	cv::imshow("segmentate-borders", segmentated);
	cv::waitKey(-1);
	//cv::imwrite(root_dir + "Result.png", segmentated);

	auto &objects2 = segmentator.getObjects();
	auto objImage = segmentator.getObjImage();
	for(auto &object : objects2) {
		fillBoundary(objImage, object);
	}
	cv::imshow("segmentate-objects", objImage);
	cv::waitKey(-1);

}

int main(int, char *[]) {
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
    for(const auto &image : images) {
    	processImage(image);
    }
    //cv::waitKey(-1);
    return 0;
}
