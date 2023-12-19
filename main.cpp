/*
// Main:
// Duties:
// --------------------
// I.  Draws UI Canvas and accepts User Mouse/Keyboard Input
//
// UI Features:
// Datatype Menu, Db Records Grid, Context Menu
// --------------------
// II. Communicates with MYSQL Database
//
// Db API Features:
// Creates connection to local MYSQL instance.
// Links database commands SQL <-> CPP Interface to Program User Interface
// Create, Read, Update, Delete
// --------------------
*/


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


#include <map>
#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::endl;
// Define a new application type
class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

// tileTimer class
class TileTimer : public wxTimer {
    wxStaticText* timerLabel;
    wxDateTime startTime;
    wxTimeSpan pausedTime;
    wxTimeSpan timeElapsed;

public:
    TileTimer(wxStaticText* label) : timerLabel(label), startTime(wxDateTime::Now()) {}

    void Notify() override {
        timeElapsed = wxDateTime::Now() - startTime + pausedTime;
        // Update the label only if more than 1 minute has elapsed
        timerLabel->SetLabel(wxString::Format("%dm", timeElapsed.GetMinutes()));
        
    }

    void StartTimer() {
        timerLabel->SetForegroundColour(*wxBLUE); // Set the text color to blue
        if (!this->IsRunning()) {
            if (pausedTime == wxTimeSpan(0)) {
                // Timer is starting for the first time
                timerLabel->SetLabel(wxString::Format("%dm", timeElapsed.GetMinutes()));
                startTime = wxDateTime::Now();
            } else {
                // Resuming the timer
                startTime = wxDateTime::Now() - pausedTime;
                pausedTime = wxTimeSpan(0);
            }
            this->Start(1000); // Update every second
        } else {
            // Timer is already running, so pause it
            PauseTimer();
        }
    }

    void PauseTimer() {
        if (this->IsRunning()) {
            this->Stop();
            pausedTime += wxDateTime::Now() - startTime;
            timerLabel->SetForegroundColour(*wxLIGHT_GREY); // Set the text color to blue
        }
    }
};

class TileData {
public:
    std::string label;                       
    std::vector<std::string> subTasks;       

    TileData() {}  // Default constructor
    TileData(const std::string& lbl) : label(lbl) {}

    void addSubTask(const std::string& subTask) {
        subTasks.push_back(subTask);
    }
};
// Implement MyApp & MyFrame
wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

private:
    // Add pointers to the grid and panel as member variables
    wxListCtrl* subTasksListCtrl;
    wxScrolledWindow* scrollArea;
    int currentSelectedTileIndex = -1;

    // Document Data Structures to be managed by the view
    std::map<int, TileData> tileDataMap;  // Map to store data for each tile, keyed by tile index
    std::vector<wxTextCtrl*> tileLabels; // Store button references
    std::map<int, TileTimer*> tileTimers;

    // View Items
    std::vector<int> bitmapIDs;

    wxBitmap normalBitmap; // Bitmap for normal state
    wxBitmap selectedBitmap; // Bitmap for selected state

    // Handlers for events
    void OnExit(wxCommandEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnTileClicked(wxMouseEvent& event);
    void OnTileDoubleClick(wxMouseEvent& event);

    // Helper function to load sub-tasks
    void LoadSubTasks(int tileNumber);
    void SelectTile(int tileIndex);

    // Timer
    void StartTimerForTile(int tileIndex);
    std::map<int, wxStaticText*> timerLabels;

    int GetVisibleRowIndex();

    std::map<int, wxDateTime> lastKeyPressTime;

    // Loaders
    void LoadBitmaps();

    // Declare the event table for wxWidgets to use
    wxDECLARE_EVENT_TABLE();
};

// Event table to connect events to handlers
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_LEFT_DOWN(MyFrame::OnTileClicked)
wxEND_EVENT_TABLE()

// ...


void MyFrame::LoadSubTasks(int tileNumber) {
    // Clear the existing list control
    subTasksListCtrl->DeleteAllItems();

    // Placeholder logic for adding sub-tasks
    // Replace this with your actual sub-task loading logic
    for (int i = 0; i < 5; ++i) { // Assuming a maximum of 5 sub-tasks for simplicity
        subTasksListCtrl->InsertItem(i, wxString::Format("Sub-task %d for Tile %d", i + 1, tileNumber));
    }

    // Adjust the column width to fit the new items
    subTasksListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
}


void MyFrame::OnKey(wxKeyEvent& event) {
    int keyCode = event.GetKeyCode();
    int visibleRow = GetVisibleRowIndex();
    wxDateTime now = wxDateTime::UNow();


    // Handle number keys (1-5)
    if (keyCode >= '1' && keyCode <= '4') {
        int tileIndex = (visibleRow * 4) + (keyCode - '1');
        SelectTile(tileIndex);
        // Check if this is a double press
        if (lastKeyPressTime.count(keyCode) > 0 && 
            (now - lastKeyPressTime[keyCode]).GetMilliseconds() < 500) { // 500 ms for double press
            int tileIndex = (visibleRow * 4) + (keyCode - '1');
            StartTimerForTile(tileIndex);
            lastKeyPressTime.erase(keyCode);
        }

        lastKeyPressTime[keyCode] = now;
    }
    else {
        event.Skip(); // Allow other handlers to process the event
    }
    event.Skip();
}

bool MyApp::OnInit() {
    wxInitAllImageHandlers();
    MyFrame *frame = new MyFrame("MiniTask");
    frame->SetInitialSize(wxSize(520, 480)); // Set the default screen size
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title) {
    // Load things from file system
    LoadBitmaps();
    
    // Create a panel to put all our widgets on
    wxPanel *panel = new wxPanel(this, wxID_ANY);

    // Custom dropdown combobox with buttons
    wxComboBox *customCombo = new wxComboBox(panel, wxID_ANY);
    customCombo->Append("Project 1");
    customCombo->Append("Project 2");
    customCombo->Append("Project 3");
    customCombo->Append("Project 4");


    // Set the initial size of the scroll area
    wxSize initialScrollAreaSize(520, 120); // Replace with your desired size
    scrollArea = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, initialScrollAreaSize, wxVSCROLL);
    scrollArea->SetScrollRate(0, 10); // Vertical scroll rate
    scrollArea->SetBackgroundColour(wxColour("#edf3f9"));


    // Sizer for scroll area content
    wxGridSizer* gridSizer = new wxGridSizer(4, 4, 0); // 5 columns, 5px vertical and horizontal gaps

    
    // Create buttons styled as tiles with timers
    const int numTiles = 8;
    const wxString timerText = "00:00"; // Placeholder text for the timer
    const wxSize tilesize(100, 100); // Define a fixed size for the tiles
    // CREATE TILES:
    for (int i = 0; i < numTiles; ++i) {
        wxPanel *tile = new wxPanel(scrollArea, wxID_ANY, wxDefaultPosition, tilesize);
        wxBoxSizer *tileSizer = new wxBoxSizer(wxVERTICAL);

        // Create a static bitmap to display the image
        wxStaticBitmap *tileImage = new wxStaticBitmap(tile, i, normalBitmap);
        tileImage->Bind(wxEVT_LEFT_DOWN, &MyFrame::OnTileClicked, this);
        tileImage->Bind(wxEVT_LEFT_DCLICK, &MyFrame::OnTileDoubleClick, this);

        // Create a text control for the tile label
        wxTextCtrl* tileLabelCtrl = new wxTextCtrl(tile, wxID_ANY, wxString::Format("Build Thing%d", i + 1),
                                               wxDefaultPosition, wxSize(85, 60),
                                               wxTE_PROCESS_ENTER | wxTE_MULTILINE | wxBORDER_NONE);
        // tileLabelCtrl->Bind(wxEVT_TEXT_ENTER, &MyFrame::OnTileLabelEdit, this);



        tileDataMap[i] = TileData(tileLabelCtrl->GetValue().ToStdString());
        tileLabels.push_back(tileLabelCtrl); // Store the text control reference

        // Create a label for the timer
        wxStaticText* timerLabel = new wxStaticText(tile, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        tileSizer->Add(timerLabel, 0, wxALIGN_CENTER | wxALL, 5);

        // Store the label in a map or similar structure
        timerLabels[i] = timerLabel;

        tileSizer->Add(tileLabelCtrl, 0, wxALIGN_CENTER | wxALL, 5);

        tile->SetSizer(tileSizer);
        tile->Layout();
        gridSizer->Add(tile, 1, wxALIGN_CENTER | wxALL, 5);
}

    // Set the grid sizer for the scroll area
    scrollArea->SetSizer(gridSizer);
    scrollArea->Layout();

    // Initialize the grid to display sub-tasks
    // Initialize the list control to display sub-tasks
    subTasksListCtrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_HEADER);
    subTasksListCtrl->InsertColumn(0, "Sub-tasks"); // Add a single column

    // Example: Add 5 rows for sub-tasks
    for (int i = 0; i < 5; ++i) {
        subTasksListCtrl->InsertItem(i, wxString::Format("Sub-task %d", i + 1));
    }

    subTasksListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
    subTasksListCtrl->Show(true);



    // Main Sizer for the frame
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(customCombo, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(scrollArea, 0, wxEXPAND | wxALL, 5); // Remove the proportion argument to fix the height
    mainSizer->Add(subTasksListCtrl, 1, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);
    this->Bind(wxEVT_CHAR_HOOK, &MyFrame::OnKey, this);

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
    LoadSubTasks(tileIndex + 1); // tileIndex + 1 because IDs start from 1
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
    if (tileTimers.find(tileIndex) == tileTimers.end()) {
        // If there's no timer for this tile, create one
        tileTimers[tileIndex] = new TileTimer(timerLabels[tileIndex]);
    }

    // Start or restart the timer
    tileTimers[tileIndex]->StartTimer();
}

void MyFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

// MyFrame Loaders

void MyFrame::LoadBitmaps() {
        wxString exePath = wxStandardPaths::Get().GetExecutablePath();
        wxString basePath = wxFileName(exePath).GetPath() + wxFileName::GetPathSeparator() + "Assets" + wxFileName::GetPathSeparator();
        wxString normalImagePath = basePath + "tile3.png";
        wxString selectedImagePath = basePath + "tile3_selected.png";

        normalBitmap = wxBitmap(normalImagePath, wxBITMAP_TYPE_PNG);
        selectedBitmap = wxBitmap(selectedImagePath, wxBITMAP_TYPE_PNG);
    }