// SaleDataLoader.h
#ifndef SALE_DATA_LOADER_H
#define SALE_DATA_LOADER_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

void LoadSaleData(wxPanel* mainPanel, Session* session, wxGrid*& grid);

#endif // SALE_DATA_LOADER_H