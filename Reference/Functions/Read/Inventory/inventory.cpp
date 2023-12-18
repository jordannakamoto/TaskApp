#include "inventory.h"
#include <wx/wx.h>
#include "../../Helpers/helpers.h"

void LoadInventoryData(wxPanel* mainPanel, mysqlx::Session* session, wxGrid*& grid) {
    if (grid != nullptr) {
        grid->Destroy();
    }

    try {
        // Connect to the schema containing the "Inventory" table
        mysqlx::Schema schema = session->getSchema("carInventory");
        mysqlx::Table inventoryTable = schema.getTable("Inventory");

        mysqlx::RowResult rows = inventoryTable.select("Inventory_ID", "Quantity_Per_Vehicle", "Model_Info")
            .execute();

        grid = new wxGrid(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(680, 400));

        int numRows = rows.count();
        int numCols = 3;

        grid->CreateGrid(numRows, numCols);

        grid->SetColLabelValue(0, "Inventory ID");
        grid->SetColLabelValue(1, "Quantity Per Vehicle");
        grid->SetColLabelValue(2, "Model Info");

        int rowIdx = 0;
        for (mysqlx::Row row : rows) {
            grid->SetCellValue(rowIdx, 0, wxString::Format("%llu", row[0].get<uint64_t>()));
            grid->SetCellValue(rowIdx, 1, wxString::FromUTF8(row[1].get<std::string>().c_str()));
            grid->SetCellValue(rowIdx, 2, wxString::FromUTF8(row[2].get<std::string>().c_str()));

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
