#include "helpers.h"

std::string timeToString(std::time_t time) {
    std::tm *tm = std::localtime(&time); // Convert to local time
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm);
    return std::string(buffer);
}

std::string GetTodaysDate() {
    // Get current time as a time_point
    auto now = std::chrono::system_clock::now();
    
    // Convert time_point to time_t
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // Convert time_t to tm (structure holding a calendar date and time)
    std::tm now_tm = *std::localtime(&now_time_t);

    // Use stringstream to format the date
    std::stringstream ss;
    ss << std::put_time(&now_tm, "%Y-%m-%d");

    return ss.str();
}