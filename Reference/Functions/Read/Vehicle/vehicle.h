// SaleDataLoader.h
#ifndef VEHICLE_DATA_LOADER_H
#define VEHICLE_DATA_LOADER_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

RowResult LoadVehicleData(wxPanel* mainPanel, Session* session, wxGrid*& grid);
RowResult LoadVehicleData(Session* session);


#endif // VEHICLE_DATA_LOADER_H