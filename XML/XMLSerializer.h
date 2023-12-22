#ifndef XMLSERIALIZER_H
#define XMLSERIALIZER_H

#include <wx/xml/xml.h>
#include <wx/fs_zip.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <map>
#include <vector>


#include <memory>

#include "../DataModels/TileData/TileData.h"
#include "../DataModels/TileTimer/TileTimer.h"

class XMLSerializer {
public:
    wxXmlDocument SerializeTileDataVector(const std::vector<TileData>& tileDataVector);
    std::vector<TileData> DeserializeTileDataVector(const wxXmlDocument& doc);

    void CompressXml(const wxXmlDocument &doc, wxOutputStream &outStream);
    void CompressXml(const wxXmlDocument &doc, const wxString &zipFile);
    wxXmlDocument DecompressXml(wxInputStream &in);
    wxXmlDocument DecompressXml(const wxString &in);
};

#endif // XMLSERIALIZER_H