#pragma once

#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>

// Helper function to convert a std::time_t to a YYYY-MM-DD formatted string
std::string timeToString(std::time_t time);
std::string GetTodaysDate();