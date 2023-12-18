// sale.h
#ifndef CUSTOMER_DELETE_H
#define CUSTOMER_DELETE_H

#include <string>
#include <mysqlx/xdevapi.h>

void DeleteCustomerRecord(mysqlx::Session* session, int custId);

#endif // CUSTOMER_DELETE_H
