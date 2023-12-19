#include <wx/xml/xml.h>

void saveToXML(const wxString& filename) {
    wxXmlDocument doc;
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Tiles");

    for (const auto& pair : tileDataMap) {
        wxXmlNode* tileNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Tile");
        tileNode->AddAttribute("id", wxString::Format("%d", pair.first));
        tileNode->AddAttribute("label", pair.second.label);
        tileNode->AddAttribute("timerElapsed", wxString::Format("%d", pair.second.timerElapsed));

        for (const auto& subtask : pair.second.subTasks) {
            wxXmlNode* subtaskNode = new wxXmlNode(wxXML_ELEMENT_NODE, "SubTask");
            subtaskNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", subtask));
            tileNode->AddChild(subtaskNode);
        }

        root->AddChild(tileNode);
    }

    doc.SetRoot(root);
    doc.Save(filename);
}

void loadFromXML(const wxString& filename) {
    wxXmlDocument doc;
    if (!doc.Load(filename)) return; // Handle error

    wxXmlNode* root = doc.GetRoot();
    for (wxXmlNode* tileNode = root->GetChildren(); tileNode; tileNode = tileNode->GetNext()) {
        if (tileNode->GetName() == "Tile") {
            int id = wxAtoi(tileNode->GetAttribute("id", "0"));
            wxString label = tileNode->GetAttribute("label", "");
            int timerElapsed = wxAtoi(tileNode->GetAttribute("timerElapsed", "0"));

            TileData tileData(label.ToStdString());
            tileData.timerElapsed = timerElapsed;

            for (wxXmlNode* subtaskNode = tileNode->GetChildren(); subtaskNode; subtaskNode = subtaskNode->GetNext()) {
                if (subtaskNode->GetName() == "SubTask") {
                    tileData.addSubTask(subtaskNode->GetNodeContent().ToStdString());
                }
            }

            tileDataMap[id] = tileData;
        }
    }
}

