// SaleDataLoader.h
#ifndef CUSTOMER_CREATOR_H
#define CUSTOMER_CREATOR_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

// For adding a record in spreadsheet form
void CreateCustomerRow(Session* session, 
                   int customer_id, 
                   std::string name, 
                   std::string phone_number,  
                   std::string email,
                   std::string address);
                   
// For adding a new customer at the Point of Sale. returns customer_id of newly generated customer via the insert result (post auto-increment)
int MakeNewCustomer(Session* session,
                   std::string name, 
                   std::string phone_number,  
                   std::string email,
                   std::string address);

#endif // CUSTOMER_CREATOR_H