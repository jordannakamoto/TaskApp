#ifndef TILEDATA_H
#define TILEDATA_H

#include <vector>
#include <string>

class TileData {
public:
    std::string label; // Label of the tile
    std::vector<std::string> subTasks; // List of sub-tasks
    long long timerElapsed;
    std::vector<std::string> notes;

    TileData();
    TileData(const std::string& lbl, long long timeElapsed);

    void addSubTask(const std::string& subTask);
    void removeSubTask(const std::string& subTask);
};

#endif // TILEDATA_H
