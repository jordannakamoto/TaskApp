// SalesDataLoader.cpp
#include "customer.h"
#include <wx/wx.h>

void LoadCustomerData(wxPanel* mainPanel, Session* session, wxGrid*& grid) {
    if (grid != nullptr) {
        grid->Destroy();
    }

    try {
        // Connect to the schema containing the "sales" table
        Schema schema = session->getSchema("carInventory");
        Table salesTable = schema.getTable("customer");

        RowResult rows = salesTable.select("customer_id", "name", "phone_number", "email", "address").execute();

        grid = new wxGrid(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(680, 400));
        
        int numRows = rows.count();
        int numCols = 5;

        grid->CreateGrid(numRows, numCols);

        grid->SetColLabelValue(0, "CustID");
        grid->SetColLabelValue(1, "Name");
        grid->SetColLabelValue(2, "Phone");
        grid->SetColLabelValue(3, "Email");
        grid->SetColLabelValue(4, "Address");

        int rowIdx = 0;
        for (Row row : rows) {
            grid->SetCellValue(rowIdx, 0, wxString::Format("%llu", row[0].get<uint64_t>()));
            grid->SetCellValue(rowIdx, 1, row[1].get<std::string>());
            grid->SetCellValue(rowIdx, 2, row[2].get<std::string>());
            grid->SetCellValue(rowIdx, 3, row[3].get<std::string>());
            grid->SetCellValue(rowIdx, 4, row[4].get<std::string>());
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