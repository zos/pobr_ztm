#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Common.h"
#include "Momentum.h"
#include "Operations.h"
#include "Segmentation.h"
#include "Log.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>


const std::string root_dir = "/home/zosia/studia/POBR/projekt/";
const std::string foty_dir = root_dir + "foty/";
const std::string ztm = root_dir + "logoztm.jpg";
const std::string cola = root_dir + "logocola.jpg";
const std::string pizza = root_dir + "logopizza.png";
const std::string pizzamale = root_dir + "logopizzamale.png";
const std::string zwierciadlo = root_dir + "logozwierciadlo.jpg";
const std::string zwierciadlomale = root_dir + "logozwierciadlomale.jpg";

const std::string res_file = root_dir + "res2.csv";

struct Pattern {
	std::string file;
	int strength;
};

std::vector<Pattern> patterns = {{ztm, 4}};
std::vector<Pattern> antipatterns = {{cola, 4}, {pizza, 6}, {pizzamale, 5}, {zwierciadlo, 6}, {zwierciadlomale, 6}};

std::map<std::string, std::vector<double>> results;

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

	auto boundary = Boundary{{0,0}, {I.cols - 1, I.rows -1} ,0};
	std::cout << "Boundary: " << boundary << std::endl;
	//Counting momentums
	Momentum mom(res, boundary);
	for (auto momentum : mom.getMap()) {
		auto func = momentum.second;
		auto mx = (mom.*func)();
		results[name].push_back(mx);
		LOG_YELLOW(momentum.first << " : " << mx);
	}

	return res;
}

void saveResults() {
	std::ofstream f(res_file, std::ofstream::out | std::ofstream::trunc);
	if (!f.is_open()) {
		LOG_RED("Couldn't open save file");
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
		LOG_RED("Writing failed");
	}
}

/*
 * Find momentum which has the greatest value of smallest difference between pattern and antipatterns
 */
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
	std::cout << "End " << pattern.file << std::endl;
}

int main(int argc, char **argv) {
    for(const auto &pattern : patterns) {
    	processPattern(pattern);
    }
    for(const auto &anti : antipatterns) {
    	processPattern(anti);
    }
	std::string name = patterns[0].file.substr(root_dir.length(), patterns[0].file.size());
    findBestMomentum(name);
    saveResults();
    //cv::waitKey(-1);
    return 0;
}
