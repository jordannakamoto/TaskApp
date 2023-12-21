#include "XMLSerializer.h"

wxXmlDocument XMLSerializer::SerializeTileDataMap(const std::map<int, TileData>& tileDataMap) {
    wxXmlDocument doc;
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Tiles");

    for (const auto& pair : tileDataMap) {
        wxXmlNode* tileNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Tile");
        tileNode->AddAttribute("id", wxString::Format("%d", pair.first));
        tileNode->AddAttribute("label", pair.second.label); // Directly access the label
        wxLongLong seconds = pair.second.timerElapsed;
        tileNode->AddAttribute("timerElapsed", wxString::Format("%lld", seconds.GetValue()));

        for (const auto& subtask : pair.second.subTasks) {
            wxXmlNode* subtaskNode = new wxXmlNode(wxXML_ELEMENT_NODE, "SubTask");
            subtaskNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", subtask));
            tileNode->AddChild(subtaskNode);
        }

        root->AddChild(tileNode);
    }

    doc.SetRoot(root);
    return doc;
}

std::map<int, TileData> XMLSerializer::DeserializeTileDataMap(const wxXmlDocument& doc) {
    std::map<int, TileData> tileDataMap;
    wxXmlNode* root = doc.GetRoot();
    if (!root) {
        // Log error or handle the missing root node
        return tileDataMap;
    }

    for (wxXmlNode* tileNode = root->GetChildren(); tileNode; tileNode = tileNode->GetNext()) {
        if (tileNode->GetName() == "Tile") {
            int id = wxAtoi(tileNode->GetAttribute("id", "0"));
            TileData tileData;
            tileData.label = tileNode->GetAttribute("label", "").ToStdString();
            
            wxString timerElapsedStr = tileNode->GetAttribute("timerElapsed", "0");
            long long elapsedSeconds;
            timerElapsedStr.ToLongLong(&elapsedSeconds);
            tileData.timerElapsed = elapsedSeconds;
            
            // Process SubTasks node
            for (wxXmlNode* subtaskNode = tileNode->GetChildren(); subtaskNode; subtaskNode = subtaskNode->GetNext()) {

                        if (subtaskNode->GetName() == "SubTask") {
                            // Assuming the subtask text is stored in a child text node
                            if (subtaskNode->GetChildren() && subtaskNode->GetChildren()->GetType() == wxXML_TEXT_NODE) {
                                wxString subtaskText = subtaskNode->GetChildren()->GetContent();
                                tileData.subTasks.push_back(subtaskText.ToStdString());
                            }
                        }
            }

            tileDataMap[id] = tileData;
        }
    }

    return tileDataMap;
}

void XMLSerializer::CompressXml(const wxXmlDocument &doc, wxOutputStream &outStream)
    {
        wxZipOutputStream zip(outStream);

        zip.PutNextEntry("paintdocument.xml");
        doc.Save(zip);

        zip.CloseEntry();

        zip.Close();
    }

    void XMLSerializer::CompressXml(const wxXmlDocument &doc, const wxString &zipFile)
    {
        auto outStream = wxFileOutputStream(zipFile);

        CompressXml(doc, outStream);
        outStream.Close();
    }

    wxXmlDocument XMLSerializer::DecompressXml(wxInputStream &in)
    {
        wxXmlDocument doc;
        wxZipInputStream zipIn(in);
        std::unique_ptr<wxZipEntry> entry(zipIn.GetNextEntry());

        while (entry)
        {
            wxString entryName = entry->GetName();

            if (entryName == "paintdocument.xml" && zipIn.CanRead())
            {
                doc.Load(zipIn);
                zipIn.CloseEntry();

                break;
            }

            zipIn.CloseEntry();
            entry.reset(zipIn.GetNextEntry());
        }

        return doc;
    }

    wxXmlDocument XMLSerializer::DecompressXml(const wxString &in)
    {
        wxFileSystem fs;
        std::unique_ptr<wxFSFile> zip(fs.OpenFile(in + "#zip:paintdocument.xml"));

        wxXmlDocument doc;

        if (zip)
        {
            wxInputStream *in = zip->GetStream();

            if (in)
            {
                doc.Load(*in);
            }
        }

        return doc;
    }
