// Include the necessary headers
#include "sale.h"
#include <mysqlx/xdevapi.h>
#include <iostream>

void CreateSaleRow(mysqlx::Session* session, 
                   int saleId, 
                   int empId, 
                   int custId, 
                   const std::string& vin, 
                   double price, 
                   const std::string& saleDateString) {
    try {
        mysqlx::Schema schema = session->getSchema("carInventory");
        mysqlx::Table salesTable = schema.getTable("sale");

        // Create a TableInsert object for the table
        mysqlx::TableInsert insert = salesTable.insert();

        // Create a Row object and set its values using the 'set' method for each column
        mysqlx::Row row;
        row.set(0, saleId);
        row.set(1, empId);
        row.set(2, custId);
        row.set(3, vin);
        row.set(4, price);
        row.set(5, mysqlx::expr("STR_TO_DATE('" + saleDateString + "', '%Y-%m-%d')"));

        // Add the Row object to the insert operation
        insert.values(row);

        // Perform the insert operation
        mysqlx::Result result = insert.execute();
        std::cout << saleDateString << std::endl;

        // Check if the insert was successful
        if (result.getAffectedItemsCount() == 0) {
            std::cerr << "Error: No rows were inserted." << std::endl;
        } else {
            std::cout << "Successfully inserted a new row." << std::endl;
        }
    } catch (const std::exception& e) {
        // Handle any exceptions thrown by the insert operation
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void MakeNewSale(mysqlx::Session* session,
                   int empId,
                   int custId,
                   std::string vin, 
                   double price, 
                   std::string saleDateString) {
    try {
        mysqlx::Schema schema = session->getSchema("carInventory");
        mysqlx::Table salesTable = schema.getTable("sale");

        // Create a TableInsert object for the table
        mysqlx::TableInsert insert = salesTable.insert("emp_id","cust_id", "vin", "price", "sale_date");

        // Create a Row object and set its values using the 'set' method for each column
        
        insert.values(empId, custId, vin, price, saleDateString);

        // Perform the insert operation
        mysqlx::Result result = insert.execute();

        // Check if the insert was successful
        if (result.getAffectedItemsCount() == 0) {
            std::cerr << "Error: No rows were inserted." << std::endl;
        } else {
            std::cout << "Successfully inserted a new sale." << std::endl;
        }
    } catch (const std::exception& e) {
        // Handle any exceptions thrown by the insert operation
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
