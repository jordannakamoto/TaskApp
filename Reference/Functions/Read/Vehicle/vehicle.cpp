// SaleDataLoader.cpp
// TODO - Nested Object Options not implemented
#include "vehicle.h"
#include <wx/wx.h>
#include "../../Helpers/helpers.h"


RowResult LoadVehicleData(wxPanel* mainPanel, Session* session, wxGrid*& grid) {
    RowResult rows;
    
    if (grid != nullptr) {
        grid->Destroy();
    }

    try {
        // Connect to the schema containing the "sale" table
        Schema schema = session->getSchema("carInventory");
        Table salesTable = schema.getTable("vehicle");

        rows = salesTable.select("vin", "manufacturer", "model_name", "UNIX_TIMESTAMP(model_year) AS yearTimestamp", "price").execute();

        grid = new wxGrid(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(680, 400));
        
        int numRows = rows.count();
        int numCols = 5;

        grid->CreateGrid(numRows, numCols);

        grid->SetColLabelValue(0, "VIN");
        grid->SetColLabelValue(1, "Manufacturer");
        grid->SetColLabelValue(2, "Model");
        grid->SetColLabelValue(3, "Year");
        //grid->SetColLabelValue(4, "Options");
        grid->SetColLabelValue(4, "Price");

        int rowIdx = 0;
        for (Row row : rows) {
            grid->SetCellValue(rowIdx, 0, row[0].get<std::string>());
            grid->SetCellValue(rowIdx, 1, row[1].get<std::string>());
            grid->SetCellValue(rowIdx, 2, row[2].get<std::string>());
            std::time_t yearDate = static_cast<std::time_t>(row[3].get<uint64_t>());
            std::string dateString = timeToString(yearDate);
            dateString = dateString.substr(0,4);
            grid->SetCellValue(rowIdx, 3, wxString(dateString)); // Set the formatted date string in the grid
            grid->SetCellValue(rowIdx, 4, wxString::Format("%.2f", row[4].get<double>()));
            rowIdx++;
        }

        for (int row = 0; row < grid->GetNumberRows(); ++row) {
            for( int col = 0; col < numCols; col++){
                grid->SetReadOnly(row, col, true);
            }
        }

         // sizer is just the section that the grid gets added to. it contains both the menu and the grid in a parent container

        wxSizer* sizer = mainPanel->GetSizer();
        if (sizer != nullptr) {
            sizer->Add(grid, 1, wxEXPAND | wxALL, 5);
        }

        mainPanel->Layout();

        return rows;
    }
    catch (const mysqlx::Error &err) {
        wxMessageBox(wxString(err.what()), "Error", wxOK | wxICON_ERROR);
        return rows;
    }
    catch (std::exception &ex) {
        wxMessageBox(wxString(ex.what()), "Exception", wxOK | wxICON_ERROR);
        return rows;
    }
    catch (const char *ex) {
        wxMessageBox(wxString(ex), "Exception", wxOK | wxICON_ERROR);
        return rows;
    }
}

RowResult LoadVehicleData(Session* session) {
    RowResult rows;

    try {
        // Connect to the schema containing the "sale" table
        Schema schema = session->getSchema("carInventory");
        Table salesTable = schema.getTable("vehicle");

        rows = salesTable.select("vin", "manufacturer", "model_name", "UNIX_TIMESTAMP(model_year) AS yearTimestamp", "price").execute();

        return rows;
    }
    catch (const mysqlx::Error &err) {
        wxMessageBox(wxString(err.what()), "Error", wxOK | wxICON_ERROR);
        return rows;
    }
    catch (std::exception &ex) {
        wxMessageBox(wxString(ex.what()), "Exception", wxOK | wxICON_ERROR);
        return rows;
    }
    catch (const char *ex) {
        wxMessageBox(wxString(ex), "Exception", wxOK | wxICON_ERROR);
        return rows;
    }
}
