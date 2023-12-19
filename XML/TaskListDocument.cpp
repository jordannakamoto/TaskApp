#include <wx/xml/xml.h>
#include <wx/fs_zip.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <map>
#include <vector>

class TaskListDocument : public wxDocument{
    public:
    std::ostream &SaveToXML(std::ostream &stream) override;
    std::istream &LoadFromXML (std::istream &stream) override;

    std::map<int, TileData> tileDataMap;  // Map to store data for each tile, keyed by tile index
    std::vector<wxButton*> tileButtons; // Store button references
    std::map<int, TileTimer*> tileTimers; // Store timer references

    XMLSerializer serializer;

    wxDECLARE_DYNAMCIC_CLASS(TaskListDocument);

}

std::ostream &TaskListDocument::SaveObject(std::ostream &stream)
{
    // Assuming you have a method to serialize tileDataMap into a wxXmlDocument
    wxXmlDocument doc = serializer.SerializeTileDataMap(tileDataMap);

    // Wrap the std::ostream
    wxOutputStreamWrapper wrapper(stream);
    serializer.CompressXml(doc, wrapper);
    return stream;
}

std::istream &TaskListDocument::LoadObject(std::istream &stream)
{
    // Wrap the std::istream
    wxInputStreamWrapper wrapper(stream);
    wxXmlDocument doc = serializer.DecompressXml(wrapper);

    // Deserialize the XML document back into tileDataMap
    tileDataMap = serializer.DeserializeTileDataMap(doc);

    // Handle wxWidgets stream state issue if necessary
    stream.clear();

    return stream;
}