// SaleDataLoader.h
#ifndef SALE_UPDATER_H
#define SALE_UPDATER_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

void UpdateSaleRow(Session* session, 
                   int saleId, 
                   int empId, 
                   int custId, 
                   const std::string& vin, 
                   double price, 
                   const std::string& saleDateString);

#endif // SALE_UPDATER_H