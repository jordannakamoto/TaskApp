#include "TileData.h"

TileData::TileData() : timerElapsed(0) {}

TileData::TileData(const std::string& lbl, int timeElapsed) 
    : label(lbl), timerElapsed(timeElapsed) {}

void TileData::addSubTask(const std::string& subTask) {
    subTasks.push_back(subTask);
}
