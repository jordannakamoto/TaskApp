#include "customer.h"

void DeleteCustomerRecord(mysqlx::Session* session, int custId) {
    try {
        mysqlx::Schema schema = session->getSchema("carInventory");
        mysqlx::Table salesTable = schema.getTable("customer");

        salesTable.remove()
                  .where("customer_id = :custId")
                  .bind("custId", custId)
                  .execute();
    } catch (const std::exception& e) {
        // Handle exceptions (e.g., log the error or show a message)
    }
}
