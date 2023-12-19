#ifndef TASKLISTDOCUMENT_H
#define TASKLISTDOCUMENT_H

#include <wx/docview.h>
#include <wx/stdstream.h>
#include "TileData.h"
#include "../DataModels/TileTimer/TileTimer.h"
#include "XMLSerializer.h"


#include <map>
#include <iostream>

class TaskListDocument : public wxDocument{
    public:
    std::ostream &SaveObject(std::ostream &stream) override;
    std::istream &LoadObject (std::istream &stream) override;

    std::map<int, TileData> tileDataMap;  // Map to store data for each tile, keyed by tile index
    std::map<int, TileTimer*> tileTimer; // Store timer references

    XMLSerializer serializer;

    wxDECLARE_DYNAMIC_CLASS(TaskListDocument);

};

#endif // TASKLISTDOCUMENT