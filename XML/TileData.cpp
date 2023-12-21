#include "TileData.h"
#include <algorithm>

TileData::TileData() : timerElapsed(0) {}

TileData::TileData(const std::string& lbl, long long timeElapsed) 
    : label(lbl), timerElapsed(timeElapsed) {}

void TileData::addSubTask(const std::string& subTask) {
    subTasks.push_back(subTask);
}

void TileData::removeSubTask(const std::string& subTask) {
   subTasks.erase(std::remove(subTasks.begin(), subTasks.end(), subTask), subTasks.end());
}
