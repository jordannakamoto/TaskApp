#ifndef OPTIONSDATALOADER_H
#define OPTIONSDATALOADER_H

#include <mysqlx/xdevapi.h>
#include <wx/wx.h>
#include <wx/grid.h>

using namespace mysqlx;

void LoadOptionsData(wxPanel* mainPanel, mysqlx::Session* session, wxGrid*& grid);
RowResult LoadOptionsData(Session* session);



#endif // OPTIONSDATALOADER_H
