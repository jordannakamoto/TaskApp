#ifndef TILEDATA_H
#define TILEDATA_H

#include <vector>
#include <string>

class TileData {
public:
    std::string label; // Label of the tile
    std::vector<std::string> subTasks; // List of sub-tasks
    int timerElapsed; // Time elapsed

    TileData();
    TileData(const std::string& lbl, int timeElapsed);

    void addSubTask(const std::string& subTask);
};

#endif // TILEDATA_H
