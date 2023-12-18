// SalesDataLoader.h
#ifndef CUST_DATA_LOADER_H
#define CUST_DATA_LOADER_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

void LoadCustomerData(wxPanel* mainPanel, Session* session, wxGrid*& grid);

#endif // CUST_DATA_LOADER