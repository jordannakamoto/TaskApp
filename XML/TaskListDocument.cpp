#include "TaskListDocument.h"
#include "streamutils.h"


wxIMPLEMENT_DYNAMIC_CLASS(TaskListDocument, wxDocument);

std::ostream &TaskListDocument::SaveObject(std::ostream &stream)
{
    // Assuming you have a method to serialize tileDataMap into a wxXmlDocument
    wxXmlDocument doc = serializer.SerializeTileDataMap(tileDataMap);

    // Wrap the std::ostream
    auto wrapper = OStreamWrapper(stream);
    doc.Save(wrapper);
    return stream;

    
}

std::istream &TaskListDocument::LoadObject(std::istream &stream)
{
    // // Wrap the std::istream
    // auto wrapper=  IStreamWrapper(stream);
    // wxXmlDocument doc = serializer.DecompressXml(wrapper);

    auto doc = IStreamWrapper(stream);

    // Deserialize the XML document back into tileDataMap
    tileDataMap = serializer.DeserializeTileDataMap(doc);

    // Handle wxWidgets stream state issue if necessary
    stream.clear();

    return stream;
}