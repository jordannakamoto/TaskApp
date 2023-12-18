// Include the necessary headers
#include "customer.h"
#include <mysqlx/xdevapi.h>
#include <iostream>

#include <mysqlx/xdevapi.h>
#include <iostream>
#include <string>

void CreateCustomerRow(mysqlx::Session* session, 
                       int customer_id,
                       std::string name, 
                       std::string phone_number,  
                       std::string email,
                       std::string address) {
    try {
        mysqlx::Schema schema = session->getSchema("carInventory");
        mysqlx::Table customerTable = schema.getTable("Customer");

        // Create a TableInsert object for the table
        mysqlx::TableInsert insert = customerTable.insert("Name", "Phone_Number", "Email", "Address");

        // Insert values directly without creating a Row object
        insert.values(name, phone_number, email, address);

        // Perform the insert operation
        mysqlx::Result result = insert.execute();

        // Check if the insert was successful
        if (result.getAffectedItemsCount() == 0) {
            std::cerr << "Error: No rows were inserted." << std::endl;
        } else {
            std::cout << "Successfully inserted a new customer." << std::endl;
        }
    } catch (const std::exception& e) {
        // Handle any exceptions thrown by the insert operation
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int MakeNewCustomer(mysqlx::Session* session, 
                    const std::string name, 
                    const std::string phone_number,  
                    const std::string email,
                    const std::string address) {
    try {
        mysqlx::Schema schema = session->getSchema("carInventory");
        mysqlx::Table customerTable = schema.getTable("Customer");

        // Create a TableInsert object for the table
        mysqlx::TableInsert insert = customerTable.insert("Name", "Phone_Number", "Email", "Address");

        // Insert values directly
        insert.values(name, phone_number, email, address);

        // Perform the insert operation
        mysqlx::Result result = insert.execute();

        // Check if the insert was successful
        if (result.getAffectedItemsCount() == 0) {
            std::cerr << "Error: No rows were inserted." << std::endl;
            return -1; // Return an error indicator, such as -1
        } else {
            std::cout << "Successfully inserted a new customer." << std::endl;
            // Retrieve and return the auto-generated customer_id
            return static_cast<int>(result.getAutoIncrementValue());
        }
    } catch (const std::exception& e) {
        // Handle any exceptions thrown by the insert operation
        std::cerr << "Error: " << e.what() << std::endl;
        return -1; // Return an error indicator in case of exception
    }
}

