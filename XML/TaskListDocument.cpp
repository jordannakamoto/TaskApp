#include "TaskListDocument.h"
#include "streamutils.h"


wxIMPLEMENT_DYNAMIC_CLASS(TaskListDocument, wxDocument);

std::ostream &TaskListDocument::SaveObject(std::ostream &stream)
{
    // Assuming you have a method to serialize tileDataVector into a wxXmlDocument
    wxXmlDocument doc = serializer.SerializeTileDataVector(tileDataVector);

    // Wrap the std::ostream
    auto wrapper = OStreamWrapper(stream);
    serializer.CompressXml(doc,wrapper);
    return stream;

    
}

std::istream &TaskListDocument::LoadObject(std::istream &stream)
{
    // // Wrap the std::istream
    auto wrapper=  IStreamWrapper(stream);
    wxXmlDocument doc = serializer.DecompressXml(wrapper);


    // Deserialize the XML document back into tileDataVector
    tileDataVector = serializer.DeserializeTileDataVector(doc);

    // Handle wxWidgets stream state issue if necessary
    stream.clear();

    return stream;
}