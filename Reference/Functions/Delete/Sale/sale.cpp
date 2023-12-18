// sale.cpp
#include "sale.h"

void DeleteSaleRecord(mysqlx::Session* session, int saleId) {
    try {
        mysqlx::Schema schema = session->getSchema("carInventory");
        mysqlx::Table salesTable = schema.getTable("sale");

        salesTable.remove()
                  .where("sale_id = :saleId")
                  .bind("saleId", saleId)
                  .execute();
    } catch (const std::exception& e) {
        // Handle exceptions (e.g., log the error or show a message)
    }
}
