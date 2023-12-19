#ifndef XMLSERIALIZER_H
#define XMLSERIALIZER_H

#include <wx/xml/xml.h>
#include <wx/fs_zip.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <map>

#include <memory>

#include "TileData.h" // Assuming this is the correct header file for TileData
#include "../DataModels/TileTimer/TileTimer.h"

class XMLSerializer {
public:
    wxXmlDocument SerializeTileDataMap(const std::map<int, TileData>& tileDataMap);
    std::map<int, TileData> DeserializeTileDataMap(const wxXmlDocument& doc);

    void CompressXml(const wxXmlDocument &doc, wxOutputStream &outStream);
    void CompressXml(const wxXmlDocument &doc, const wxString &zipFile);
    wxXmlDocument DecompressXml(wxInputStream &in);
    wxXmlDocument DecompressXml(const wxString &in);
};

#endif // XMLSERIALIZER_H