// SaleDataLoader.h
#ifndef SALE_CREATOR_H
#define SALE_CREATOR_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

void CreateSaleRow(Session* session, 
                   int saleId, 
                   int empId, 
                   int custId, 
                   const std::string& vin, 
                   double price, 
                   const std::string& saleDateString);
                   
void MakeNewSale(Session* session,
                   int empId,
                   int custId,
                   std::string vin, 
                   double price, 
                   std::string saleDateString);

#endif // SALE_CREATOR_H