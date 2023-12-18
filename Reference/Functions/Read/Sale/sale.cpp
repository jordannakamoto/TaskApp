// SaleDataLoader.cpp
#include "sale.h"
#include <wx/wx.h>
#include "../../Helpers/helpers.h"


void LoadSaleData(wxPanel* mainPanel, Session* session, wxGrid*& grid) {
    if (grid != nullptr) {
        grid->Destroy();
    }

    try {
        // Connect to the schema containing the "sale" table
        Schema schema = session->getSchema("carInventory");
        Table salesTable = schema.getTable("sale");

        RowResult rows = salesTable.select("sale_id", "emp_id", "cust_id", "vin", "price", "UNIX_TIMESTAMP(sale_date) AS saleTimestamp").execute();

        grid = new wxGrid(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(680, 400));

        
        int numRows = rows.count();
        int numCols = 6;

        grid->CreateGrid(numRows, numCols);

        grid->SetColLabelValue(0, "SaleID");
        grid->SetColLabelValue(1, "EmpID");
        grid->SetColLabelValue(2, "CustID");
        grid->SetColLabelValue(3, "VIN");
        grid->SetColLabelValue(4, "Price");
        grid->SetColLabelValue(5, "SaleDate");

        int rowIdx = 0;
        for (Row row : rows) {
            grid->SetCellValue(rowIdx, 0, wxString::Format("%llu", row[0].get<uint64_t>()));
            grid->SetCellValue(rowIdx, 1, wxString::Format("%llu", row[1].get<uint64_t>()));
            grid->SetCellValue(rowIdx, 2, wxString::Format("%llu", row[2].get<uint64_t>()));
            grid->SetCellValue(rowIdx, 3, row[3].get<std::string>());
            grid->SetCellValue(rowIdx, 4, wxString::Format("%.2f", row[4].get<double>()));
            std::time_t saleDate = static_cast<std::time_t>(row[5].get<uint64_t>());
            std::string dateString = timeToString(saleDate);
            grid->SetCellValue(rowIdx, 5, wxString(dateString)); // Set the formatted date string in the grid

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