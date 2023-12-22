#ifndef TASKLISTDOCUMENT_H
#define TASKLISTDOCUMENT_H

#include <wx/docview.h>
#include <wx/stdstream.h>
#include "../DataModels/TileData/TileData.h"
#include "../DataModels/TileTimer/TileTimer.h"
#include "XMLSerializer.h"


#include <map>
#include <vector>
#include <iostream>

class TaskListDocument : public wxDocument{
    public:
    std::ostream &SaveObject(std::ostream &stream) override;
    std::istream &LoadObject (std::istream &stream) override;

    std::vector<TileData> tileDataVector;  // Vector to store data for each tile
    std::map<int, TileTimer*> tileTimer; // Store timer references

    XMLSerializer serializer;

    wxDECLARE_DYNAMIC_CLASS(TaskListDocument);

};

#endif // TASKLISTDOCUMENT