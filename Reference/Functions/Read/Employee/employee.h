// SalesDataLoader.h
#ifndef EMP_DATA_LOADER_H
#define EMP_DATA_LOADER_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

void LoadEmployeeData(wxPanel* mainPanel, Session* session, wxGrid*& grid);

#endif // EMP_DATA_LOADER