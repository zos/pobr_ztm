#pragma once

#include <iostream>
#include <string>

#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define RESET_COLOR "\033[0m"

#ifdef DEBUG
#define LOG(msg) std::cout << msg << std::endl;
#else
#define LOG(msg)
#endif

#define LOG_GREEN(msg) std::cout << GREEN_COLOR << msg << RESET_COLOR << std::endl;
#define LOG_RED(msg) std::cout << RED_COLOR << msg << RESET_COLOR << std::endl;
#define LOG_YELLOW(msg) std::cout << YELLOW_COLOR << msg << RESET_COLOR << std::endl;
