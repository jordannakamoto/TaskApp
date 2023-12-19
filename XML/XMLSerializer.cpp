#include <wx/xml/xml.h>
#include <map>
#include <vector>


class XmlSerializer {
public:
    wxXmlDocument SerializeTileDataMap(const std::map<int, TileData>& tileDataMap) {
        wxXmlDocument doc;
        wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Tiles");

        for (const auto& pair : tileDataMap) {
            wxXmlNode* tileNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Tile");
            tileNode->AddAttribute("id", wxString::Format("%d", pair.first));
            tileNode->AddAttribute("label", pair.second.GetLabel());
            tileNode->AddAttribute("timerElapsed", wxString::Format("%d", pair.second.GetTimerElapsed()));

            for (const auto& subtask : pair.second.GetSubTasks()) {
                wxXmlNode* subtaskNode = new wxXmlNode(wxXML_ELEMENT_NODE, "SubTask");
                subtaskNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", subtask));
                tileNode->AddChild(subtaskNode);
            }

            root->AddChild(tileNode);
        }

        doc.SetRoot(root);
        return doc;
    }

    std::map<int, TileData> DeserializeTileDataMap(const wxXmlDocument& doc) {
        std::map<int, TileData> tileDataMap;
        wxXmlNode* root = doc.GetRoot();

        for (wxXmlNode* tileNode = root->GetChildren(); tileNode; tileNode = tileNode->GetNext()) {
            if (tileNode->GetName() == "Tile") {
                int id = wxAtoi(tileNode->GetAttribute("id", "0"));
                TileData tileData;
                tileData.SetLabel(tileNode->GetAttribute("label", ""));
                tileData.SetTimerElapsed(wxAtoi(tileNode->GetAttribute("timerElapsed", "0")));

                for (wxXmlNode* subtaskNode = tileNode->GetChildren(); subtaskNode; subtaskNode = subtaskNode->GetNext()) {
                    if (subtaskNode->GetName() == "SubTask") {
                        tileData.AddSubTask(subtaskNode->GetNodeContent());
                    }
                }

                tileDataMap[id] = tileData;
            }
        }

        return tileDataMap;
    };

};
