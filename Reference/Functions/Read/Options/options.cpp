// OptionsDataLoader.cpp
#include "options.h"
#include <wx/wx.h>
#include "../../Helpers/helpers.h"

void LoadOptionsData(wxPanel* mainPanel, Session* session, wxGrid*& grid) {
    if (grid != nullptr) {
        grid->Destroy();
    }

    try {
        // Connect to the schema containing the "Options" table
        Schema schema = session->getSchema("carInventory");
        Table optionsTable = schema.getTable("Options");

        RowResult rows = optionsTable.select("VIN","Engine", "Transmission", "Drive_Train", "Color")
            .execute();

        grid = new wxGrid(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(680, 400));

        int numRows = rows.count();
        int numCols = 5;

        grid->CreateGrid(numRows, numCols);

        grid->SetColLabelValue(0, "VIN");
        grid->SetColLabelValue(1, "Engine");
        grid->SetColLabelValue(2, "Transmission");
        grid->SetColLabelValue(3, "Drive Train");
        grid->SetColLabelValue(4, "Color");

        int rowIdx = 0;
        for (Row row : rows) {
            grid->SetCellValue(rowIdx, 0, row[0].get<std::string>());
            grid->SetCellValue(rowIdx, 1, row[1].get<std::string>());
            grid->SetCellValue(rowIdx, 2, row[2].get<std::string>());
            grid->SetCellValue(rowIdx, 3, row[3].get<std::string>());
            grid->SetCellValue(rowIdx, 4, row[4].get<std::string>());

            rowIdx++;
        }

        for (int row = 0; row < grid->GetNumberRows(); ++row) {
            for (int col = 0; col < numCols; col++) {
                grid->SetReadOnly(row, col, true);
            }
        }

        wxSizer* sizer = mainPanel->GetSizer();
        if (sizer != nullptr) {
            sizer->Add(grid, 1, wxEXPAND | wxALL, 5);
        }

        mainPanel->Layout();
    }
    catch (const mysqlx::Error& err) {
        wxMessageBox(wxString(err.what()), "Error", wxOK | wxICON_ERROR);
    }
    catch (std::exception& ex) {
        wxMessageBox(wxString(ex.what()), "Exception", wxOK | wxICON_ERROR);
    }
    catch (const char* ex) {
        wxMessageBox(wxString(ex), "Exception", wxOK | wxICON_ERROR);
    }
}

RowResult LoadOptionsData(Session* session) {
    RowResult rows;
    try {
        // Connect to the schema containing the "Options" table
        Schema schema = session->getSchema("carInventory");
        Table optionsTable = schema.getTable("Options");

        rows = optionsTable.select("VIN","Engine", "Transmission", "Drive_Train", "Color")
            .execute();
    }
    catch (const mysqlx::Error& err) {
        wxMessageBox(wxString(err.what()), "Error", wxOK | wxICON_ERROR);
    }
    catch (std::exception& ex) {
        wxMessageBox(wxString(ex.what()), "Exception", wxOK | wxICON_ERROR);
    }
    catch (const char* ex) {
        wxMessageBox(wxString(ex), "Exception", wxOK | wxICON_ERROR);
    }
}
