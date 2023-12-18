// sale.h
#ifndef SALE_DELETE_H
#define SALE_DELETE_H

#include <string>
#include <mysqlx/xdevapi.h>

void DeleteSaleRecord(mysqlx::Session* session, int saleId);

#endif // SALE_SALE_DELETE_H
