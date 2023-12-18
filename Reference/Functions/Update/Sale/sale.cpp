#include "sale.h"
#include <mysqlx/xdevapi.h>
#include <iostream>

void UpdateSaleRow(Session* session, 
                   int saleId, 
                   int empId, 
                   int custId, 
                   const std::string& vin, 
                   double price, 
                   const std::string& saleDateString) {
    try {
        Schema schema = session->getSchema("carInventory");
        Table salesTable = schema.getTable("sale");

        // Perform the update
        Result result = salesTable.update()
            .set("emp_id", empId)
            .set("cust_id", custId)
            .set("vin", vin)
            .set("price", price)
            .set("sale_date", expr("STR_TO_DATE('" + saleDateString + "', '%Y-%m-%d')"))
            .where("sale_id = :saleId")
            .bind("saleId", saleId)
            .execute();

        // Check if the update was successful
        if (result.getAffectedItemsCount() == 0) {
            std::cerr << "Error: No rows were updated." << std::endl;
        } else {
            std::cout << "Successfully updated the row." << std::endl;
        }
    } catch (const std::exception& e) {
        // Handle any exceptions thrown by the update operation
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
