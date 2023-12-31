
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/listbox.h> 
#include <wx/colour.h>
#include <wx/combobox.h>
#include <wx/font.h>
#include <wx/button.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/timer.h>
#include <wx/event.h>
#include <wx/thread.h> 
#include "XML/TaskListDocument.h"

#include "Networking/ai_api_python_process.h"

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using std::cout;
using std::endl;

// Define a new application type

class MyApp : public wxApp {
public:
    virtual bool OnInit();
    
};

// Implement MyApp & MyFrame
wxIMPLEMENT_APP(MyApp);

// DEF
class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    wxString basePath = wxFileName(exePath).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + "test";
    void ProcessGptText(std::string gptText);


private:
    // Add pointers to the grid and panel as member variables
    wxListCtrl* subTasksListCtrl;
    wxScrolledWindow* scrollArea;
    wxGridSizer* gridSizer;
    wxComboBox *searchBar;
    int currentSelectedTileIndex = -1;
    TaskListDocument taskListDoc;

    // Document Data Structures to be managed by the view
    std::vector<TileData> tileDataVector;  // Vector to store data for each tile
    std::map<int, TileTimer*> tileTimers;
    std::vector<wxWindow*> tiles;
    std::vector<wxTextCtrl*> tileLabels; // Store button references

    // Tile Stuff
    void CreateNewTile();
    const wxSize tilesize; // Define a fixed size for the tiles   

    wxBitmap normalBitmap; // Bitmap for normal tile state
    wxBitmap selectedBitmap; // Bitmap for selected tile state

    // Handlers for events
    void OnKey(wxKeyEvent& event);
    void OnTileClicked(wxMouseEvent& event);
    void OnTileDoubleClick(wxMouseEvent& event);
    void OnTileLabelEdit(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    // Helper function to load sub-tasks
    void LoadSubTasks(int tileNumber);

    void ClearTiles();
    void DeleteTile(int tileIndex);
    void SelectTile(int tileIndex);

    // Timer
    void StartTimerForTile(int tileIndex);
    std::map<int, wxStaticText*> timerLabels;

    // SubTasks
    void AddNewSubTask();
    void OnSubTaskEdit(wxListEvent& event);
    void OnSubTaskEditCallback(int itemIndex, const wxString& newLabel);
    int GetVisibleRowIndex();

    std::map<int, wxDateTime> lastKeyPressTime;

    // Loaders
    void LoadBitmaps();
    void CreateTilesFromData();

    // Document Save/Load
    void SaveData(const wxString& baseFilePath);
    void LoadData(const wxString& baseFilePath);

    // AI Integration
    std::map<std::string, std::vector<std::string>> task_map;
    void LoadDataFromTaskMap(std::map<std::string, std::vector<std::string>> task_map);

    // Search/GPT Bar
    void OnSearchBarTextEntered(wxCommandEvent& event);

     void OnClose(wxCloseEvent& event);
    // Declare the event table for wxWidgets to use
    wxDECLARE_EVENT_TABLE();
};

// Event table to connect events to handlers
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_LEFT_DOWN(MyFrame::OnTileClicked)
wxEND_EVENT_TABLE()

// ...
class GptProcessingThread : public wxThread {
public:
    GptProcessingThread(const std::string& textToProcess, MyFrame* frame)
        : m_textToProcess(textToProcess), m_frame(frame) {}

    virtual ExitCode Entry() {
        // Place your time-consuming processing code here
        // For example, call your ProcessGptText function
        m_frame->ProcessGptText(m_textToProcess);
        return NULL;
    }

private:
    std::string m_textToProcess;
    MyFrame* m_frame;
};

void MyFrame::LoadSubTasks(int tileNumber) {
    subTasksListCtrl->DeleteAllItems();

    // Check if the tileNumber is valid and present in tileDataVector
    if (tileDataVector.size() > tileNumber) {
        // Get the sub-tasks for the specified tile
        const std::vector<std::string>& subTasks = tileDataVector[tileNumber].subTasks;

        // Insert each sub-task into the list control with numbering
        for (size_t i = 0; i < subTasks.size(); ++i) {
            wxString itemLabel = wxString::Format("%zu. %s", i + 1, subTasks[i]);
            subTasksListCtrl->InsertItem(i, itemLabel);
        }
    }
}

void MyFrame::OnKey(wxKeyEvent& event) {
    int keyCode = event.GetKeyCode();
    int visibleRow = GetVisibleRowIndex();
    wxDateTime now = wxDateTime::UNow();


    // Handle Enter key press
    if (keyCode == WXK_RETURN || keyCode == WXK_NUMPAD_ENTER) {
        if (currentSelectedTileIndex != -1 && currentSelectedTileIndex < tileLabels.size()) {
            wxTextCtrl* labelCtrl = tileLabels[currentSelectedTileIndex];
            if (labelCtrl) {
                labelCtrl->SetFocus();
                labelCtrl->SelectAll(); // Optional: Select all text for immediate editing
            }
        }
        else{
            event.Skip();
        }
    }
    // Handle Control+Shift+T key press
    else if (event.GetModifiers() == (wxMOD_CONTROL | wxMOD_SHIFT) && event.GetKeyCode() == 'T') {
        AddNewSubTask();
    }
    // Handle number keys (1-5)
    else if (keyCode >= '1' && keyCode <= '4') {
        int tileIndex = (visibleRow * 4) + (keyCode - '1');
        SelectTile(tileIndex);
        // Check if this is a double press
        if (lastKeyPressTime.count(keyCode) > 0 && 
            (now - lastKeyPressTime[keyCode]).GetMilliseconds() < 300) { // 500 ms for double press
            int tileIndex = (visibleRow * 4) + (keyCode - '1');
            StartTimerForTile(tileIndex);
            lastKeyPressTime.erase(keyCode);
        }
        else
            lastKeyPressTime[keyCode] = now;
    }
    else if (event.GetModifiers() == wxMOD_CMD && event.GetKeyCode() == 'T') {
        CreateNewTile();
    }
    else {
        event.Skip(); // Allow other handlers to process the event
    }
}

// INIT

bool MyApp::OnInit() {
    wxInitAllImageHandlers();
    MyFrame *frame = new MyFrame("MiniTask");
    frame->SetInitialSize(wxSize(520, 680)); // Set the default screen size
    frame->Show(true);
    return true;
}


// CONSTRUCTOR
MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title), tilesize(100, 100) {
    // Load things from file system
    LoadBitmaps();

    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);

    // Load current document (project)
    
    // Create a panel to put all our widgets on
    wxPanel *panel = new wxPanel(this, wxID_ANY);

    // Search/GPT Bar
    wxComboBox *searchBar = new wxComboBox(panel, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, 0, NULL,  wxTE_PROCESS_ENTER);
    searchBar->Append("Project 1");
    searchBar->Bind(wxEVT_COMMAND_TEXT_ENTER, &MyFrame::OnSearchBarTextEntered, this);

    // Set the initial size of the scroll area
    wxSize initialScrollAreaSize(520, 240); // Replace with your desired size
    scrollArea = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, initialScrollAreaSize, wxVSCROLL);
    scrollArea->SetScrollRate(0, 10); // Vertical scroll rate
    scrollArea->SetBackgroundColour(wxColour("#edf3f9"));

    // Sizer for scroll area content
    gridSizer = new wxGridSizer(4, 4, 0); // 5 columns, 5px vertical and horizontal gaps


    // CREATE TILES:
    LoadData(basePath);
    CreateTilesFromData();

    // Set the grid sizer for the scroll area
    scrollArea->SetSizer(gridSizer);
    scrollArea->Layout();

    // Initialize the grid to display sub-tasks
    // Initialize the list control to display sub-tasks
    subTasksListCtrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_HEADER);
    subTasksListCtrl->InsertColumn(0, "Sub-Tasks"); // Add a single column
    // Set the width of the first column to 350 pixels
    subTasksListCtrl->SetColumnWidth(0, 350);
    subTasksListCtrl->Show(true);
    subTasksListCtrl->Bind(wxEVT_LIST_END_LABEL_EDIT, &MyFrame::OnSubTaskEdit, this);


    // Main Sizer for the frame
    // Add a padding border or margin around the panel
    int borderSize = 15;
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(searchBar, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(scrollArea, 0, wxEXPAND | wxALL, 10); // Remove the proportion argument to fix the height
    mainSizer->Add(subTasksListCtrl, 1, wxEXPAND | wxALL, 10);

    panel->SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);
    this->Bind(wxEVT_CHAR_HOOK, &MyFrame::OnKey, this);
    Centre();
}

void MyFrame::SelectTile(int tileIndex) {
    // Ensure the tile index is within range
    if (tileIndex < 0 || tileIndex >= tileLabels.size()) {
        return; // Invalid tile index
    }

    // Reset the style of the previously selected button
    if (currentSelectedTileIndex != -1) {
            wxStaticBitmap* associatedBitmap = dynamic_cast<wxStaticBitmap*>(wxWindow::FindWindowById(currentSelectedTileIndex));
            if (associatedBitmap) {
                // Change the bitmap
                associatedBitmap->SetBitmap(normalBitmap);
                associatedBitmap->Refresh();
            }
    }

    // Update the bitmap of the new selected tile
    wxStaticBitmap* newSelectedBitmap = dynamic_cast<wxStaticBitmap*>(wxWindow::FindWindowById(tileIndex));
    if (newSelectedBitmap) {
        newSelectedBitmap->SetBitmap(selectedBitmap);
        newSelectedBitmap->Refresh();
    }
    // scrollArea->Scroll((tileIndex) * 6, -1); // Set scroll based on tile position
    currentSelectedTileIndex = tileIndex;
    // Load sub-tasks for the selected tile
    LoadSubTasks(tileIndex); // tileIndex + 1 because IDs start from 1
}


int MyFrame::GetVisibleRowIndex() {
    int x, y;
    scrollArea->GetViewStart(&x, &y); // Get the current scroll position

    int rowHeight = 10; // Assuming each tile (and hence each row) is 100 pixels high
    return y / rowHeight; // Calculate the index of the visible row
}


void MyFrame::OnTileClicked(wxMouseEvent& event) {
    int tileIndex = event.GetId(); // Subtract 1 because IDs start from 1
    SelectTile(tileIndex);
}

void MyFrame::OnTileDoubleClick(wxMouseEvent& event) {
    int tileIndex = event.GetId();
    StartTimerForTile(tileIndex);
}

void MyFrame::StartTimerForTile(int tileIndex) {
    // Start or restart the timer
    tileTimers[tileIndex]->StartTimer();
}

void MyFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

// MyFrame Loaders

void MyFrame::LoadBitmaps() {
        wxString baseAssetsPath = wxFileName(exePath).GetPath() + wxFileName::GetPathSeparator() + "Assets" + wxFileName::GetPathSeparator();
        wxString normalImagePath = baseAssetsPath + "tile3.png";
        wxString selectedImagePath = baseAssetsPath + "tile3_selected.png";

        normalBitmap = wxBitmap(normalImagePath, wxBITMAP_TYPE_PNG);
        selectedBitmap = wxBitmap(selectedImagePath, wxBITMAP_TYPE_PNG);
    }

// Saving/Loading data
void MyFrame::SaveData(const wxString& baseFilePath) {
    // wxString path = wxSaveFileSelector("task data", "xml");
    wxString savePath = baseFilePath + ".xml";
    taskListDoc.tileDataVector = tileDataVector;
        std::ofstream out(savePath.ToStdString());
        taskListDoc.SaveObject(out);
    
}

void MyFrame::LoadData(const wxString& baseFilePath) {
    wxString loadPath = baseFilePath + ".xml";
    if (!loadPath.IsEmpty()) {
        std::ifstream in(loadPath.ToStdString());
        taskListDoc.LoadObject(in);
        tileDataVector = taskListDoc.tileDataVector;
    }
}

void MyFrame::OnClose(wxCloseEvent& event) {
    SaveData(basePath); // Save the data
    event.Skip(); // Proceed with the default close behavior
}
// ...

// CREATE Tile(Task) Creation

void MyFrame::CreateNewTile() {
    int newTileIndex = tileDataVector.size();  // Determine the new tile's index

    // Create a panel for the new tile
    wxWindow *tile = new wxWindow(scrollArea, wxID_ANY, wxDefaultPosition, tilesize);
    wxBoxSizer *tileSizer = new wxBoxSizer(wxVERTICAL);

    // Create a static bitmap for the tile
    wxStaticBitmap *tileImage = new wxStaticBitmap(tile, newTileIndex, normalBitmap);
    tileImage->Bind(wxEVT_LEFT_DOWN, &MyFrame::OnTileClicked, this);
    tileImage->Bind(wxEVT_LEFT_DCLICK, &MyFrame::OnTileDoubleClick, this);
    
    // Create a text control for the tile label
    wxTextCtrl* tileLabelCtrl = new wxTextCtrl(tile, wxID_ANY, wxString::Format("New Task %d", newTileIndex + 1),
                                               wxDefaultPosition, wxSize(85, 60),
                                               wxTE_PROCESS_ENTER | wxTE_MULTILINE | wxBORDER_NONE);
    // Attach event bindings

    // Add to data structures
    tileDataVector.push_back(TileData(tileLabelCtrl->GetValue().ToStdString(), 0.0));
    tileLabels.push_back(tileLabelCtrl);
    tileLabelCtrl->Bind(wxEVT_TEXT, &MyFrame::OnTileLabelEdit, this, newTileIndex);
    TileData& tileData = tileDataVector[newTileIndex];

    // Create and store the timer label
    wxStaticText* timerLabel = new wxStaticText(tile, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    tileSizer->Add(timerLabel, 0, wxALIGN_CENTER | wxALL, 5);
    timerLabels[newTileIndex] = timerLabel;
    tileTimers[newTileIndex] = new TileTimer(timerLabels[newTileIndex], tileData);

    // Add the tile label to the sizer and layout
    tileSizer->Add(tileLabelCtrl, 0, wxALIGN_CENTER | wxALL, 5);
    tile->SetSizer(tileSizer);
    tile->Layout();

    if (gridSizer) {
        gridSizer->Add(tile, 1, wxALIGN_CENTER | wxALL, 5);
        scrollArea->Layout();
        scrollArea->FitInside();  // Ensure the scroll area fits the new content
    }
}

void MyFrame::CreateTilesFromData() {
    int i = 0; // Initialize the index

    for (TileData& tileData : tileDataVector) {
        wxBoxSizer *tileSizer = new wxBoxSizer(wxVERTICAL);

        wxWindow *tile = new wxWindow(scrollArea, wxID_ANY, wxDefaultPosition, tilesize);
        tiles.push_back(tile);

        wxStaticBitmap *tileImage = new wxStaticBitmap(tile, i, normalBitmap);
        tileImage->Bind(wxEVT_LEFT_DOWN, &MyFrame::OnTileClicked, this);
        tileImage->Bind(wxEVT_LEFT_DCLICK, &MyFrame::OnTileDoubleClick, this);

        wxTextCtrl* tileLabelCtrl = new wxTextCtrl(tile, i + 101, wxString::FromUTF8(tileData.label),
                                                   wxDefaultPosition, wxSize(85, 60),
                                                   wxTE_PROCESS_ENTER | wxTE_MULTILINE | wxBORDER_NONE);

        // Attach event bindings
        tileLabels.push_back(tileLabelCtrl); // Store the text control reference
        tileLabelCtrl->Bind(wxEVT_TEXT, &MyFrame::OnTileLabelEdit, this, i + 101);

        // Create a label for the timer
        wxTimeSpan timeElapsed = wxTimeSpan::Seconds(tileData.timerElapsed);
        wxStaticText* timerLabel;
        if (timeElapsed > 0) {
            timerLabel = new wxStaticText(tile, wxID_ANY, wxString::Format("%dm", timeElapsed.GetMinutes()),
                wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        }
        else {
            timerLabel = new wxStaticText(tile, wxID_ANY, wxString(""),
                wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        }
        tileSizer->Add(timerLabel, 0, wxALIGN_CENTER | wxALL, 5);

        timerLabels[i] = timerLabel;
        tileTimers[i] = new TileTimer(timerLabels[i], tileData, timeElapsed);
        timerLabel->SetForegroundColour(*wxLIGHT_GREY);

        tileSizer->Add(tileLabelCtrl, 0, wxALIGN_CENTER | wxALL, 5);

        tile->SetSizer(tileSizer);
        tile->Layout();
        gridSizer->Add(tile, 1, wxALIGN_CENTER | wxALL, 5);

        i++; // Increment the index
    }

    gridSizer->Layout();
}

// UPDATE - EditTile

void MyFrame::OnTileLabelEdit(wxCommandEvent& event) {
    wxTextCtrl* labelCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    int tileIndex = event.GetId() -101;
    // std::cout << tileIndex << std::endl;
    // std::cout << tileDataVector[0].label << std::endl;
    if (tileIndex > -1) {
        wxString newLabel = labelCtrl->GetValue();
        // std::cout << newLabel.ToStdString() << std::endl;
        tileDataVector[tileIndex].label = newLabel;
    }
}   

// SUBTASKS
void MyFrame::AddNewSubTask() {
    int newSubTaskIndex = subTasksListCtrl->GetItemCount();
    subTasksListCtrl->InsertItem(newSubTaskIndex, "New Sub-task");

    tileDataVector[currentSelectedTileIndex].subTasks.push_back("");
    // Set focus to the new subtask for editing
    subTasksListCtrl->EditLabel(newSubTaskIndex);

    // TODO: Need to add in input pausing so we don't change tiles during input state
}

void MyFrame::OnSubTaskEdit(wxListEvent& event) {
    int itemIndex = event.GetIndex(); // The index of the item in the list
    wxString newLabel = event.GetText(); // The new text for the subtask

    // Update the subtask in data model
    if (currentSelectedTileIndex != -1 && currentSelectedTileIndex < tileDataVector.size()) {
        // Assuming each tile's subtasks are stored in a vector of strings
        if (itemIndex >= 0) {
            tileDataVector[currentSelectedTileIndex].subTasks[itemIndex] = newLabel.ToStdString();
            // Update the list control item to show the number and the new label
            wxTheApp->CallAfter([this, itemIndex, newLabel]() {
                this->OnSubTaskEditCallback(itemIndex, newLabel);
            });
        }
    }
    // Debug
    // std::cout << tileDataVector[currentSelectedTileIndex].label << std::endl;
    // std::cout << itemIndex << std::endl;
    // std::cout << tileDataVector[currentSelectedTileIndex].subTasks[itemIndex] << std::endl;
}

void MyFrame::OnSubTaskEditCallback(int itemIndex, const wxString& newLabel) {
    wxString updatedLabel = wxString::Format("%d. %s", itemIndex + 1, newLabel);
    subTasksListCtrl->SetItemText(itemIndex, updatedLabel);
}

void MyFrame::LoadDataFromTaskMap(std::map<std::string, std::vector<std::string>> task_map){
    int tileIndex = 0;  // Initial index for tiles
    tileDataVector.clear();

    for (const auto& [task, subTasks] : task_map) {
        TileData tile(task, 0);  // Assuming timerElapsed is initialized as 0

        for (const auto& subTask : subTasks) {
            tile.addSubTask(subTask);
        }

        tileDataVector[tileIndex++] = tile;  // Assign to tileDataVector and increment index
    }
}

// searchBar
// Event handler for searchBar text enter
void MyFrame::OnSearchBarTextEntered(wxCommandEvent& event) {
    cout << "combobox enter pressed" << endl;

    wxString searchText = event.GetString();

    // Check if the entered text starts with "gpt:"
    if (searchText.StartsWith("gpt:")) {
        // Extract the text following "gpt:"
        wxString gptText = searchText.AfterFirst(':').Trim();

        // Create and start a separate thread for processing
        GptProcessingThread* thread = new GptProcessingThread(gptText.ToStdString(), this);
        if (thread->Create() == wxTHREAD_NO_ERROR) {
            thread->Run();
        }
    }
}

// Function to process the extracted "gpt:" text
void MyFrame::ProcessGptText(std::string gptText) {
    AI_API_Python_Process ai_process;
    task_map = ai_process.test_openai_video_parser(gptText);
    LoadDataFromTaskMap(task_map);
    CreateTilesFromData();
}

void MyFrame::ClearTiles(){
    tiles.clear();
}

void MyFrame::DeleteTile(int tileIndex){
    tiles.erase(tiles.begin()+tileIndex);
}