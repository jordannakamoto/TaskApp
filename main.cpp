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
#include <wx/grid.h>
#include <wx/listbox.h> // Add this line to include ListBox header
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

public:
    TileTimer(wxStaticText* label) : timerLabel(label), startTime(wxDateTime::Now()) {}

    void Notify() override {
        wxTimeSpan timeElapsed = wxDateTime::Now() - startTime + pausedTime;

        // Update the label only if more than 1 minute has elapsed

        timerLabel->SetLabel(wxString::Format("%dm", timeElapsed.GetMinutes()));
        timerLabel->SetForegroundColour(*wxBLUE); // Set the text color to blue
        
    }

    void StartTimer() {
        if (!this->IsRunning()) {
            if (pausedTime == wxTimeSpan(0)) {
                // Timer is starting for the first time
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
    std::map<int, TileData> tileDataMap;  // Map to store data for each tile, keyed by tile index


private:
    // Add pointers to the grid and panel as member variables
    wxGrid* subTasksGrid;
    wxScrolledWindow* scrollArea;
    wxButton* currentSelectedButton = nullptr;
    std::vector<wxButton*> tileButtons; // Store button references
    std::map<int, TileTimer*> tileTimers;

    // Handlers for events
    void OnExit(wxCommandEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnButtonClicked(wxCommandEvent& event);
    void OnTileDoubleClick(wxMouseEvent& event);

    // Helper function to load sub-tasks
    void LoadSubTasks(int tileNumber);
    void SelectTile(int tileIndex);

    // Timer
    void StartTimerForTile(int tileIndex);
    std::map<int, wxStaticText*> timerLabels;

    int GetVisibleRowIndex();

    std::map<int, wxDateTime> lastKeyPressTime;

    // Declare the event table for wxWidgets to use
    wxDECLARE_EVENT_TABLE();
};

// Event table to connect events to handlers
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(wxID_ANY, MyFrame::OnButtonClicked)
wxEND_EVENT_TABLE()

// ...

void MyFrame::OnButtonClicked(wxCommandEvent& event) {
    int tileIndex = event.GetId() - 1; // Subtract 1 because IDs start from 1
    SelectTile(tileIndex);
}



void MyFrame::LoadSubTasks(int tileNumber) {
    // Clear the existing grid
    subTasksGrid->ClearGrid();
    
    // Load sub-tasks for the selected tile
    // This is just placeholder logic, replace with actual sub-task loading
    for (int row = 0; row < subTasksGrid->GetNumberRows(); ++row) {
        subTasksGrid->SetCellValue(row, 0, wxString::Format("Sub-task %d for Tile %d", row + 1, tileNumber));
    }
     subTasksGrid->AutoSizeColumns();
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
            int tileIndex = (visibleRow * 5) + (keyCode - '1');
            StartTimerForTile(tileIndex);
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
    MyFrame *frame = new MyFrame("Task Manager");
    frame->SetInitialSize(wxSize(640, 480)); // Set the default screen size
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title) {
    // Create a panel to put all our widgets on
    wxPanel *panel = new wxPanel(this, wxID_ANY);

    // Custom dropdown combobox with buttons
    wxComboBox *customCombo = new wxComboBox(panel, wxID_ANY);
    customCombo->Append("Project 1");
    customCombo->Append("Project 2");
    customCombo->Append("Project 3");
    customCombo->Append("Project 4");


    // Set the initial size of the scroll area
    wxSize initialScrollAreaSize(640, 120); // Replace with your desired size
    scrollArea = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, initialScrollAreaSize, wxVSCROLL);
    scrollArea->SetScrollRate(0, 10); // Vertical scroll rate
    scrollArea->SetBackgroundColour(wxColour("#edf3f9"));


    // Sizer for scroll area content
    wxGridSizer* gridSizer = new wxGridSizer(4, 5, 5); // 5 columns, 5px vertical and horizontal gaps

    
    // Create buttons styled as tiles with timers
    const int numTiles = 8;
    const wxString timerText = "00:00"; // Placeholder text for the timer
    const wxSize tilesize(100, 100); // Define a fixed size for the tiles
    
    // CREATE TILES:
    for (int i = 0; i < numTiles; ++i) {
        wxPanel *tile = new wxPanel(scrollArea, wxID_ANY, wxDefaultPosition, tilesize);
        wxBoxSizer *tileSizer = new wxBoxSizer(wxVERTICAL);
        
         // Load the PNG image
        wxString exePath = wxStandardPaths::Get().GetExecutablePath();
        wxString imagePath = wxFileName(exePath).GetPath() + wxFileName::GetPathSeparator() + "Assets" + wxFileName::GetPathSeparator() + "tile3.png";

        // Create a static bitmap to display the image
        wxStaticBitmap *tileBitmapCtrl = new wxStaticBitmap(tile, wxID_ANY, wxBitmap(imagePath, wxBITMAP_TYPE_PNG));


        std::string tileLabel = wxString::Format("Build Thing%d", i + 1).ToStdString();
        tileDataMap[i] = TileData(tileLabel);
        wxButton *button = new wxButton(tile, i + 1, wxString::Format("Build Thing%d", i + 1), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        button->Bind(wxEVT_LEFT_DCLICK, &MyFrame::OnTileDoubleClick, this);
        tileButtons.push_back(button);
        // Create a label for the timer
        wxStaticText* timerLabel = new wxStaticText(tile, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        tileSizer->Add(timerLabel, 0, wxALIGN_CENTER | wxALL, 5);

        // Store the label in a map or similar structure
        timerLabels[i] = timerLabel;
        
        tileSizer->Add(button, 1, wxALIGN_CENTER | wxALL, 5);
        
        tile->SetSizer(tileSizer);
        tile->Layout();
        gridSizer->Add(tile, 1, wxALIGN_CENTER | wxALL, 5);

    }



    // Set the grid sizer for the scroll area
    scrollArea->SetSizer(gridSizer);
    scrollArea->Layout();

    // Initialize the grid to display sub-tasks
    subTasksGrid = new wxGrid(panel, wxID_ANY);
    subTasksGrid->CreateGrid(5, 1); // Example: 5 rows for sub-tasks, 1 column
    subTasksGrid->SetColLabelValue(0, "Sub-tasks"); // Set the header for the column
    subTasksGrid->AutoSizeColumns();
    subTasksGrid->SetColLabelSize(0);
    subTasksGrid->SetRowLabelSize(0);


    // Main Sizer for the frame
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(customCombo, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(scrollArea, 0, wxEXPAND | wxALL, 5); // Remove the proportion argument to fix the height
    mainSizer->Add(subTasksGrid, 1, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);
    this->Bind(wxEVT_CHAR_HOOK, &MyFrame::OnKey, this);

}

void MyFrame::SelectTile(int tileIndex) {
    // Ensure the tile index is within range
    if (tileIndex < 0 || tileIndex >= tileButtons.size()) {
        return; // Invalid tile index
    }

    // Reset the style of the previously selected button
    if (currentSelectedButton) {
        currentSelectedButton->SetBackgroundColour(wxNullColour); // Default color
        currentSelectedButton->Refresh();
    }

    // Update currentSelectedButton and highlight the new selected button
    currentSelectedButton = tileButtons[tileIndex];
    currentSelectedButton->SetBackgroundColour(*wxLIGHT_GREY); // Highlight color
    currentSelectedButton->Refresh();
    // scrollArea->Scroll((tileIndex) * 6, -1);
     // Set scroll based on tile position

    // Load sub-tasks for the selected tile
    LoadSubTasks(tileIndex + 1); // tileIndex + 1 because IDs start from 1
}


void MyFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

int MyFrame::GetVisibleRowIndex() {
    int x, y;
    scrollArea->GetViewStart(&x, &y); // Get the current scroll position

    int rowHeight = 10; // Assuming each tile (and hence each row) is 100 pixels high
    return y / rowHeight; // Calculate the index of the visible row
}

void MyFrame::OnTileDoubleClick(wxMouseEvent& event) {
    int tileIndex = event.GetId() - 1;
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