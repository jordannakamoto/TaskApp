#ifndef INVENTORY_H
#define INVENTORY_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <mysqlx/xdevapi.h>


void LoadInventoryData(wxPanel* mainPanel, mysqlx::Session* session, wxGrid*& grid);


#endif // INVENTORY_H
