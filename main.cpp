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

#include <mysqlx/xdevapi.h>

#include "./Functions/Read/Employee/employee.h"

#include "./Functions/Create/Customer/customer.h"
#include "./Functions/Read/Customer/customer.h"
#include "./Functions/Delete/Customer/customer.h"

#include "./Functions/Read/Vehicle/vehicle.h"
#include "./Functions/Read/Options/options.h"
#include "./Functions/Read/Inventory/inventory.h"

#include "./Functions/Create/Sale/sale.h"
#include "./Functions/Read/Sale/sale.h"
#include "./Functions/Update/Sale/sale.h"
#include "./Functions/Delete/Sale/sale.h"
#include "./Functions/Helpers/helpers.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using namespace mysqlx;

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size); // Define UI Canvas
    void ConnectToDatabase();
    void CreateNewView();  // Draw UI Canvas for the first time

    // Function Defs Interaction Events
    void OnMenuSelect(wxCommandEvent& event);
    void OnSubmitEdit(wxCommandEvent& event);
    void UpdateDatabaseForRow(int row);

    void OnSubmitCreate(wxCommandEvent& event);
    void OnAddRow(wxCommandEvent& event);
    void OnDeleteRow(wxCommandEvent& event);
    void OnRightClick(wxGridEvent& event);
    void OnContextMenuClose(wxMenuEvent& event);
    void OnEditRow(wxCommandEvent& event);
    void CreateDatabaseRow(int row);

    void OnMakeSale(wxCommandEvent& event);

    // Sales Panel Events
    void PostMakeSale(wxCommandEvent& event);
    void CancelMakeSale(wxCommandEvent& event);



    // UI Helpers
    void PositionAddRowButton(wxButton* button);
    void PositionMakeSaleButton(wxButton* button);
    void OnResize(wxSizeEvent& event);

    // Sales form values --- should probably be in their own class
    wxTextCtrl* customerNameTextCtrl;
    wxTextCtrl* priceTextCtrl;
    wxTextCtrl* phoneTextCtrl;
    wxTextCtrl* emailTextCtrl;
    wxTextCtrl* addressTextCtrl;
    wxComboBox* makeModelYearDropdown;
    std::vector<std::string> fetchedVehiclesVIN;

    // Color Customization
    wxColour customHighlightColor;

private:
    Session *session;     // Use a pointer for session ... 
    wxPanel *mainPanel;   // we will be passing these pointer variables to 
    wxListBox* menuList;  // the local mysql db through imported function calls
    wxGrid* grid;
    wxPanel* salesFormPanel;
    wxString selectedOption;
    wxButton* addRowButton;
    wxButton* makeSaleButton;

    // Register ContextMenu (right click) Items
    const int ID_EditRow = wxNewId(); 
    const int ID_DeleteRow = wxNewId();

    int currentRow = -1;  // Add a member variable to store the current selected or interacting row
    int currentCol = -1;

    // Memory for row colors and rows that are currently selected for edit
    wxColour originalColor;
    std::vector<int> editedRows;
    
};

wxIMPLEMENT_APP(MyApp); // start up wxWidgets

// 1. Main UI Canvas Frame Constructor - wxWidgets 
bool MyApp::OnInit() {
    std::cout<< "\n\n:::::::::Car Dealership App Loading:::::::::\n\n" << std::endl;
    MyFrame *frame = new MyFrame("Car Dealership App", wxDefaultPosition, wxSize(840, 450));
    frame->Show(true);
    std::cout<< "\n\n:::::::::Welcome to Car Dealership App:::::::::\n\n" << std::endl;
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    

    mainPanel = new wxPanel(this, wxID_ANY);

    vbox->Add( 0, 60);
    vbox->Add(mainPanel, 1, wxEXPAND);
    Bind(wxEVT_SIZE, &MyFrame::OnResize, this);


    SetSizer(vbox);
    mainPanel->Hide(); // Initially hide the main panel
    std::cout<< "GUI Window Loaded" << std::endl;

    ConnectToDatabase(); // Connect to the database and create the view
    std::cout<< "Connected to Database (async)" << std::endl;

    // Set colors
    customHighlightColor.SetRGB(0xF3FCF9); // Hexadecimal RGB
}

// 2. Database Connection
// MYSQLCPPCONNECTOR
void MyFrame::ConnectToDatabase() {

    int mode = 1;

    if(mode == 1){
    wxString username = wxGetTextFromUser("Enter your MySQL username:", "MySQL Username", "root");
    wxString password = wxGetPasswordFromUser("Enter your MySQL password:", "MySQL Password");

    try {
        // Construct the connection string with the provided username and password
        wxString connectionString = "mysqlx://" + username + ":" + password + "@127.0.0.1";
        session = new Session(connectionString.ToStdString());

        cout << "Connection successful!" << endl;
        mainPanel->Show(); // Show the main panel after successful connection
        CreateNewView();   // Create and show the new view
    }
    catch (const mysqlx::Error &err) {
        wxMessageBox(wxString(err.what()), "Error", wxOK | wxICON_ERROR);
    }
    catch (std::exception &ex) {
        wxMessageBox(wxString(ex.what()), "Exception", wxOK | wxICON_ERROR);
    }
    catch (const char *ex) {
        wxMessageBox(wxString(ex), "Exception", wxOK | wxICON_ERROR);
    }
    }
    else{
        wxString connectionString = "mysqlx://root:benedict232@127.0.0.1";
        session = new Session(connectionString.ToStdString());
        mainPanel->Show(); // Show the main panel after successful connection
        CreateNewView();   // Create and show the new view
    }
}

// 3. First Paint of UI and its elements
// UI Render 1 
void MyFrame::CreateNewView() {
    wxInitAllImageHandlers();

    // Clear the main panel to prepare for the new view
    mainPanel->DestroyChildren();  // Clear any existing children

    // Use a vertical sizer to layout elements vertically
    wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);

    // Create the branded header
    wxPanel* headerPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(200, 60));


    // Load and display the image
    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    wxString imagePath = wxFileName(exePath).GetPath() + wxFileName::GetPathSeparator() + "Assets" + wxFileName::GetPathSeparator() + "logo2.png";

    wxStaticBitmap* logoStaticBitmap = new wxStaticBitmap(headerPanel, wxID_ANY, wxBitmap(imagePath, wxBITMAP_TYPE_PNG), wxDefaultPosition, wxSize(200, 60));

    // Create a sizer for the header panel
    wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);
    headerSizer->Add(logoStaticBitmap, 0, wxALL, 5);


    // Create a horizontal sizer for the menu and grid
    wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);

    // Create and add the menu list to the horizontal sizer
    menuList = new wxListBox(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(150, -1));
    const int ID_FIRST_MENU_ITEM = wxNewId(); // Unique ID to programatically highlight the Sales button without user Input
    menuList->Append("Sales");
    menuList->Append("Employees");
    menuList->Append("Customers");
    menuList->Append("Vehicles");
    menuList->Append(" - Options");
    menuList->Append("Inventory");
    menuList->Bind(wxEVT_LISTBOX, &MyFrame::OnMenuSelect, this);
    hSizer->Add(menuList, 0, wxEXPAND | wxALL, 5);

    // Create a wxGrid for displaying the data
    grid = new wxGrid(mainPanel, wxID_ANY);
    // (Configure grid columns and rows as before...)

    hSizer->Add(grid, 1, wxEXPAND | wxALL, 5);

    // Add sales form

    mainPanel->SetSizer(hSizer);  // Set the horizontal sizer for the main panel
    mainPanel->Layout();  // Layout the main panel

    headerPanel->Show();
    mainPanel->Show();    // Show the main panel
    this->Layout();       // Update the layout of the frame
    LoadSaleData(mainPanel, session, grid);
    menuList->SetSelection(0);
    grid->AutoSizeColumns();
    grid->Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &MyFrame::OnRightClick, this);
    selectedOption = "Sales";
    std::cout<< "Loaded Sales Table" << std::endl;

    // Create the "Add Row" button
    addRowButton = new wxButton(mainPanel, wxID_ANY, "Add New Row", wxDefaultPosition, wxDefaultSize);
    makeSaleButton = new wxButton(mainPanel, wxID_ANY, "Make Sale", wxDefaultPosition, wxDefaultSize);


    addRowButton->Bind(wxEVT_BUTTON, &MyFrame::OnAddRow, this);
    makeSaleButton->Bind(wxEVT_BUTTON, &MyFrame::OnMakeSale, this);


    // Position the button in the bottom-right corner of the frame
    PositionAddRowButton(addRowButton);
    PositionMakeSaleButton(makeSaleButton);
    std::cout<< "Positioned Final UI Elements" << std::endl;
}


// 4. Left Sidebar Menu and its Options
// Action: Menu Select
void MyFrame::OnMenuSelect(wxCommandEvent& event) {
    selectedOption = menuList->GetStringSelection();

    // Change the background color of the selected table to highlight it
    if (selectedOption == "Sales") {
        LoadSaleData(mainPanel, session, grid);  // Load Sales data into the grid
    } else if (selectedOption == "Employees") {
        LoadEmployeeData(mainPanel, session, grid);  // Load Sales data into the grid
    } else if (selectedOption == "Customers") {
        LoadCustomerData(mainPanel, session, grid);
    } else if (selectedOption == "Vehicles") {
        LoadVehicleData(mainPanel, session, grid);
    } else if (selectedOption == " - Options") {
        LoadOptionsData(mainPanel, session, grid);
    }
    else if (selectedOption == "Inventory") {
        LoadInventoryData(mainPanel, session, grid);
    }
    grid->AutoSizeColumns();
    grid->Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &MyFrame::OnRightClick, this);
    // Create the "Add Row" button
    addRowButton->Destroy();
    makeSaleButton->Destroy();
    addRowButton = new wxButton(mainPanel, wxID_ANY, "Add New Row", wxDefaultPosition, wxDefaultSize);
    makeSaleButton = new wxButton(mainPanel, wxID_ANY, "Make Sale", wxDefaultPosition, wxDefaultSize);

    addRowButton->Bind(wxEVT_BUTTON, &MyFrame::OnAddRow, this);
    makeSaleButton->Bind(wxEVT_BUTTON, &MyFrame::OnMakeSale, this);

    // Position the button in the bottom-right corner of the frame
    PositionAddRowButton(addRowButton);
    PositionMakeSaleButton(makeSaleButton);
}

// 5. ContextMenu Event Handler
void MyFrame::OnRightClick(wxGridEvent& event) {
    wxPoint position = wxGetMousePosition();
    position = ScreenToClient(position);

    currentRow = event.GetRow();  // Store the current row
    currentCol = event.GetCol(); // Store the current cell
    // Enable editing for the selected row
    for (int col = 0; col < grid->GetNumberCols(); ++col) {
        grid->SetCellBackgroundColour(currentRow, col, customHighlightColor);
        grid->Refresh();
    }
    wxMenu menu;
    menu.Append(ID_EditRow, "Edit Row");
    menu.Append(ID_DeleteRow, "Delete Row"); // Add this line
    

    // Bind event for menu item
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnEditRow, this, ID_EditRow);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnDeleteRow, this, ID_DeleteRow); // Add this line
    Bind(wxEVT_MENU_CLOSE, &MyFrame::OnContextMenuClose, this);  // Bind the menu close event

    PopupMenu(&menu, position);
}

// 5a1. ContextMenu Event 1 - EDIT
void MyFrame::OnEditRow(wxCommandEvent& event) {


    originalColor = grid->GetCellBackgroundColour(currentRow, 0);


    // Enable editing for the selected row
    for (int col = 0; col < grid->GetNumberCols(); ++col) {
        grid->SetReadOnly(currentRow, col, false);
        grid->SetCellBackgroundColour(currentRow, col, customHighlightColor);
    }

    if (currentRow >= 0 && currentRow < grid->GetNumberRows() &&
        currentCol >= 0 && currentCol < grid->GetNumberCols()) {
        
        // Set the grid cursor to the specified cell
        grid->SetGridCursor(currentRow, currentCol);

        // Enable editing on this cell
        grid->EnableCellEditControl();
    }

    editedRows.push_back(currentRow); // Update lastEditedRow
    

    grid->ForceRefresh(); // Refresh the grid to update the color immediately

    // Create the "Submit" button
    wxButton* submitButton = new wxButton(grid, wxID_ANY, "Submit", wxDefaultPosition, wxDefaultSize);
    submitButton->SetClientData(new int(currentRow)); // Store the row index

    // Position the button at the end of the current row
    wxRect rect = grid->CellToRect(currentRow, grid->GetNumberCols() - 1);
    int buttonX = rect.GetX() + rect.GetWidth() + 85; // Adjust X position
    int buttonY = rect.GetY() + 30; // Adjust Y position
    submitButton->SetSize(buttonX, buttonY, submitButton->GetSize().GetWidth(), rect.GetHeight());

    // Bind the event handler for the button
    submitButton->Bind(wxEVT_BUTTON, &MyFrame::OnSubmitEdit, this);
}

// 5a2. Submit Action/Button OnEdit - EDIT
void MyFrame::OnSubmitEdit(wxCommandEvent& event) {
    if (grid->IsCellEditControlEnabled()) {
        // Save the current value in the edit control
        grid->SaveEditControlValue();

        // End the editing session
        grid->DisableCellEditControl();
    }
    wxButton* button = dynamic_cast<wxButton*>(event.GetEventObject());
    int row = *static_cast<int*>(button->GetClientData());
    
    // Update the database with the new values from the edited row
    UpdateDatabaseForRow(row);

    // Revert the row color to its original and set the row to read-only
    for (int col = 0; col < grid->GetNumberCols(); ++col) {
        grid->SetReadOnly(row, col, true);
        grid->SetCellBackgroundColour(row, col, originalColor);
    }

    // Refresh the grid
    grid->ForceRefresh();

    // Hide or destroy the submit button
    button->Destroy();
}

// 5b. ContextMenu Event 2 - DELETE
void MyFrame::OnDeleteRow(wxCommandEvent& event) {
    if(selectedOption == "Sales"){
        int saleId = wxAtoi(grid->GetCellValue(currentRow, 0));
        DeleteSaleRecord(session, saleId);
        grid->DeleteRows(currentRow, 1); // Remove the row from the grid
    }
    else if(selectedOption == "Customers"){
        int custId = wxAtoi(grid->GetCellValue(currentRow, 0));
        DeleteCustomerRecord(session, custId);
        grid->DeleteRows(currentRow, 1); // Remove the row from the grid
    }
}

// 5c. ContextMenu OnClose
void MyFrame::OnContextMenuClose(wxMenuEvent& event) {
    // Deselect the row
    for (int col = 0; col < grid->GetNumberCols(); ++col) {
        grid->SetCellBackgroundColour(currentRow, col, originalColor);
        grid->Refresh();
    }

    // Unbind the event to avoid it affecting other menus
    Unbind(wxEVT_MENU_CLOSE, &MyFrame::OnContextMenuClose, this);
}

// 6.
// UPDATE
// (Called by 5a2. - EDIT on SUBMIT)
void MyFrame::UpdateDatabaseForRow(int row) {

    if(selectedOption == "Sales"){

    // Extract values from the grid
    int saleId = wxAtoi(grid->GetCellValue(row, 0));
    int empId = wxAtoi(grid->GetCellValue(row, 1));
    int custId = wxAtoi(grid->GetCellValue(row, 2));
    std::string vin = grid->GetCellValue(row, 3).ToStdString();
    double price = wxAtof(grid->GetCellValue(row, 4));
    std::string saleDateString = grid->GetCellValue(row, 5).ToStdString();

    // Call the update function
    UpdateSaleRow(session, saleId, empId, custId, vin, price, saleDateString);
    }
    // else if(selectedOption == "Customers"){

    // // Extract values from the grid
    // int saleId = wxAtoi(grid->GetCellValue(row, 0));
    // int empId = wxAtoi(grid->GetCellValue(row, 1));
    // int custId = wxAtoi(grid->GetCellValue(row, 2));
    // std::string vin = grid->GetCellValue(row, 3).ToStdString();
    // double price = wxAtof(grid->GetCellValue(row, 4));
    // std::string saleDateString = grid->GetCellValue(row, 5).ToStdString();

    // // Call the update function
    // UpdateCustomerRow(session, saleId, empId, custId, vin, price, saleDateString);
    // }
}


// 7.
// CREATE
void MyFrame::OnAddRow(wxCommandEvent& event) {
    // Append a new row to the grid
    grid->AppendRows(1);
    int newRow = grid->GetNumberRows() - 1;

    currentRow = newRow;

    // ------
    // Create the "Submit" button for the new row
    //
    wxButton* submitButton = new wxButton(grid, wxID_ANY, "Submit", wxDefaultPosition, wxDefaultSize);
    submitButton->SetClientData(new int(currentRow)); // Store the row index

    // Position the button at the end of the new row
    wxRect rect = grid->CellToRect(currentRow, grid->GetNumberCols() - 1);
    int buttonX = rect.GetX() + rect.GetWidth() + 85; // Adjust X position
    int buttonY = rect.GetY() + 30; // Adjust Y position
    submitButton->SetSize(buttonX, buttonY, submitButton->GetSize().GetWidth(), rect.GetHeight());

    // Bind the event handler for the button
    submitButton->Bind(wxEVT_BUTTON, &MyFrame::OnSubmitCreate, this);
    // ------

}

// 7a. Submit Action/Button OnSubmitCreate - CREATE
void MyFrame::OnSubmitCreate(wxCommandEvent& event) {
    if (grid->IsCellEditControlEnabled()) {
        // Save the current value in the edit control
        grid->SaveEditControlValue();

        // End the editing session
        grid->DisableCellEditControl();
    }
    wxButton* button = dynamic_cast<wxButton*>(event.GetEventObject());
    int row = *static_cast<int*>(button->GetClientData());
    
    // Update the database with the new values from the edited row
    CreateDatabaseRow(row);

    // Revert the row color to its original and set the row to read-only
    for (int col = 0; col < grid->GetNumberCols(); ++col) {
        grid->SetReadOnly(row, col, true);
    }

    // Refresh the grid
    grid->ForceRefresh();

    // Hide or destroy the submit button
    button->Destroy();
}

// 7b.
// CREATE Database UI Spreadsheet Row
// (Called by 7a. - CREATE on CREATE)
void MyFrame::CreateDatabaseRow(int row) {

    if(selectedOption == "Sales"){
        // Extract values from the grid
        int saleId = wxAtoi(grid->GetCellValue(row, 0));
        int empId = wxAtoi(grid->GetCellValue(row, 1));
        int custId = wxAtoi(grid->GetCellValue(row, 2));
        std::string vin = grid->GetCellValue(row, 3).ToStdString();
        double price = wxAtof(grid->GetCellValue(row, 4));
        std::string saleDateString = grid->GetCellValue(row, 5).ToStdString();

        CreateSaleRow(session, saleId, empId, custId, vin, price, saleDateString);
    }
    else if (selectedOption == "Customers"){
        // Extract values from the grid
        int customer_id = wxAtoi(grid->GetCellValue(row, 0));
        std::string name = grid->GetCellValue(row, 1).ToStdString();
        std::string phone_number = grid->GetCellValue(row, 2).ToStdString();
        std::string email = grid->GetCellValue(row, 3).ToStdString();
        std::string address = grid->GetCellValue(row, 4).ToStdString();

        CreateCustomerRow(session, customer_id, name, phone_number, email, address);
        cout << address << endl;
    }
}




// 8. Draw "Add New Row" Button to Canvas and Reposition on App Window Resize
// OnResize() can be extended to manage other elements as well...
void MyFrame::PositionAddRowButton(wxButton* button) {
    if (!button) return;

    // Get the size of the main panel and the button
    wxSize panelSize = mainPanel->GetSize();
    wxSize buttonSize = button->GetSize();

    // Calculate the position for the button
    int xPos = panelSize.GetWidth() - buttonSize.GetWidth() - 20; // 20 is a margin
    int yPos = panelSize.GetHeight() - buttonSize.GetHeight() - 20;

    // Set the position of the button
    button->SetPosition(wxPoint(xPos, yPos));
}

void MyFrame::OnResize(wxSizeEvent& event) {
    // Reposition the add row button
    PositionAddRowButton(addRowButton);
    PositionMakeSaleButton(makeSaleButton);
    event.Skip(); // Ensure the event is still processed by default handlers
}



// 9d. Draw "Make Sale Button" Button to Canvas and Reposition on App Window Resize
// OnResize() can be extended to manage other elements as well...
void MyFrame::PositionMakeSaleButton(wxButton* button) {
    if (!button) return;

    // Get the size of the main panel and the button
    wxSize panelSize = mainPanel->GetSize();
    wxSize buttonSize = button->GetSize();

    // Calculate the position for the button
    int xPos = panelSize.GetWidth() - buttonSize.GetWidth() - 20; // 20 is a margin
    int yPos = panelSize.GetHeight() - 30 - buttonSize.GetHeight() - 20;

    // Set the position of the button
    button->SetPosition(wxPoint(xPos, yPos));
}

// 9. On make Sale
void MyFrame::OnMakeSale(wxCommandEvent& event) {

    salesFormPanel = new wxPanel(this, wxID_ANY);
    // Create the sales form panel
    salesFormPanel->SetBackgroundColour(*wxWHITE); // You can replace wxWHITE with any wxColour
    
    // Create Panel Title Label
    wxStaticText* title = new wxStaticText(salesFormPanel, wxID_ANY, wxT("         New Sale"));
        // Create a wxFont object
    wxFont titleFont(16,             // font size
                    wxFONTFAMILY_DEFAULT, // font family - default is okay for generic purpose
                    wxFONTSTYLE_NORMAL,   // font style
                    wxFONTWEIGHT_BOLD);   // bold font weight

    // Assuming 'title' is your wxStaticText pointer
    title->SetFont(titleFont);

    // Create a sizer for the sales form panel to arrange child widgets
    wxBoxSizer* salesFormSizerWrapper = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* salesFormSizerGroup = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* salesFormSizer1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* salesFormSizer2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* salesFormSizer3 = new wxBoxSizer(wxVERTICAL);

    ////// Read Relevant Data from Db //////
    RowResult vehicleRows = LoadVehicleData(session);


    std::string col0, col1, col2, col3, concatenatedResult;
    std::time_t yearDate ;
    std::string dateString;
    std::vector<std::string> fetchedVehicles;

    for (Row row: vehicleRows) {
        // Extract the values from columns 1, 2, and 3
        // Assuming the columns are of a type that can be converted to std::string
        col0 = row[0].get<std::string>();
        col1 = row[1].get<std::string>();
        col2 = row[2].get<std::string>();
        yearDate = static_cast<std::time_t>(row[3].get<uint64_t>());
        dateString = timeToString(yearDate);
        col3 = dateString.substr(0,4);

        // Concatenate the column values with a separator, if needed
        
        concatenatedResult = col1 + " " + col2 + " " + col3; // You can change the separator as needed
        fetchedVehicles.push_back(concatenatedResult);
        fetchedVehiclesVIN.push_back(col0);
    }


    
    ////// Create UI Elements //////
    // Create and add the text controls and the 'Post' button to the sizer

    makeModelYearDropdown = new wxComboBox(salesFormPanel, wxID_ANY);
    for (const std::string& item : fetchedVehicles) {
    makeModelYearDropdown->Append(item);
}
    salesFormSizer1->Add(makeModelYearDropdown, 0, wxEXPAND | wxALL, 5);

    priceTextCtrl = new wxTextCtrl(salesFormPanel, wxID_ANY);
    salesFormSizer1->Add(priceTextCtrl, 0, wxEXPAND | wxALL, 5);

    customerNameTextCtrl = new wxTextCtrl(salesFormPanel, wxID_ANY);
    salesFormSizer2->Add(customerNameTextCtrl, 0, wxEXPAND | wxALL, 5);

    phoneTextCtrl = new wxTextCtrl(salesFormPanel, wxID_ANY);
    salesFormSizer2->Add(phoneTextCtrl, 0, wxEXPAND | wxALL, 5);

    emailTextCtrl = new wxTextCtrl(salesFormPanel, wxID_ANY);
    salesFormSizer2->Add(emailTextCtrl, 0, wxEXPAND | wxALL, 5);

    addressTextCtrl = new wxTextCtrl(salesFormPanel, wxID_ANY);
    salesFormSizer2->Add(addressTextCtrl, 0, wxEXPAND | wxALL, 5);

    // Set placeholder text
    makeModelYearDropdown->SetHint(wxT("Vehicle"));
    customerNameTextCtrl->SetHint(wxT("Customer Name"));

    // Do the same for other wxTextCtrl elements
    priceTextCtrl->SetHint(wxT("Price"));
    phoneTextCtrl->SetHint(wxT("Phone"));
    emailTextCtrl->SetHint(wxT("Email"));
    addressTextCtrl->SetHint(wxT("Street Address"));

    wxButton* postButton = new wxButton(salesFormPanel, wxID_ANY, wxT("Post"));
    wxButton* cancelButton = new wxButton(salesFormPanel, wxID_ANY, wxT("Cancel"));
    salesFormSizer3->Add(postButton, 0, wxALIGN_LEFT | wxALL, 5);
    salesFormSizer3->Add(cancelButton, 0, wxALIGN_LEFT | wxALL, 5);

    postButton->Bind(wxEVT_BUTTON, &MyFrame::PostMakeSale, this);
    cancelButton->Bind(wxEVT_BUTTON, &MyFrame::CancelMakeSale, this);

    // Pack sizers into their display groups
    salesFormSizerGroup->Add(salesFormSizer1, 1, wxEXPAND | wxALL, 5);
    salesFormSizerGroup->Add(salesFormSizer2, 1, wxEXPAND | wxALL, 5);
    salesFormSizerGroup->Add(salesFormSizer3, 1, wxEXPAND | wxALL, 5);

    salesFormSizerWrapper->Add(title, 0, wxTOP, 20); // wxCENTER will center the text, wxALL adds a border
    salesFormSizerWrapper->Add(salesFormSizerGroup, 1,  wxEXPAND | wxLEFT | wxRIGHT, 150);
    // Set the sizer for the sales form panel
    salesFormPanel->SetSizer(salesFormSizerWrapper);


    // Create a new main sizer for the frame and add the sales form panel to it
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(0,60);
    mainSizer->Add(salesFormPanel, 1, wxEXPAND | wxALL, 5);
    

    // Set the new main sizer for the frame
    this->SetSizer(mainSizer);

    // Call Layout to update the frame with the new content
    this->Layout();

}

void MyFrame::PostMakeSale(wxCommandEvent& event) {
    int makemodelIdx = 0;
    wxString makeModelYear;
    if(salesFormPanel != NULL){

        // Retrieve the selection from the combo box
       if (makeModelYearDropdown != nullptr) {
            makemodelIdx = makeModelYearDropdown->GetSelection();
            if (makemodelIdx != wxNOT_FOUND) {
                makeModelYear = makeModelYearDropdown->GetString(makemodelIdx); // Use GetString with index
                cout << makeModelYear.ToStdString() + " was Purchased" << endl;
            } else {
                cout << "No item selected in dropdown." << endl;
                return;
            }
        }else{
            cout << "nullptr" << endl;
            return;
        }
        std::string saleVIN = fetchedVehiclesVIN[makemodelIdx];
        std::string customerName, customerPhone, customerEmail, customerAddress, salePriceStr;
        double salePrice;

        if (customerNameTextCtrl != nullptr) {
             customerName =  customerNameTextCtrl->GetValue();
        }else return;
        if (phoneTextCtrl != nullptr) {
             customerPhone =  phoneTextCtrl->GetValue();
        }else return;
        if (emailTextCtrl != nullptr) {
            customerEmail =  emailTextCtrl->GetValue();
        }else return;
        if (addressTextCtrl != nullptr) {
            customerAddress =  addressTextCtrl->GetValue();
        }else return;
        if (priceTextCtrl != nullptr) {
            salePrice = wxAtof(priceTextCtrl->GetValue());
            try {
            } catch (const std::invalid_argument& e) {
                // Handle error: the string did not contain a valid double
                std::cerr << "Invalid input for price: " << e.what() << std::endl;
                return;
            } catch (const std::out_of_range& e) {
                // Handle error: the string contained a double, but it is out of range
                std::cerr << "Input for price out of range: " << e.what() << std::endl;
                return;
            }
        }
        std::string dateString = GetTodaysDate();
        

        // CREATE new customer
        int newCustomerId = MakeNewCustomer(session, customerName, customerPhone, customerEmail, customerAddress);
        // CREATE new sale
        MakeNewSale(session, 1, newCustomerId, saleVIN, salePrice, dateString);

        salesFormPanel->Destroy();
    }
}

void MyFrame::CancelMakeSale(wxCommandEvent& event) {
    if(salesFormPanel != NULL)
        salesFormPanel->Destroy();
}