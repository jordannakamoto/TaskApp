-- Create the Employee (Emp) table
CREATE TABLE Employee (
    Employee_ID INT PRIMARY KEY,
    Name VARCHAR(255),
    Make_Responsible VARCHAR(255),
    Phone_Number VARCHAR(255),
    Email VARCHAR(255),
    Address VARCHAR(255)
);

-- Create the Customer (Cust) table
CREATE TABLE Customer (
    Customer_ID INT AUTO_INCREMENT PRIMARY KEY,
    Name VARCHAR(255),
    Phone_Number VARCHAR(20),
    Email VARCHAR(255),
    Address VARCHAR(255)
);
-- Create the Vehicle (Vehicle) table

CREATE TABLE Vehicle (
    VIN VARCHAR(17) PRIMARY KEY,
    Manufacturer VARCHAR(255),
    Model_Name VARCHAR(255),
    Model_Year DATE,
    Price DECIMAL(10, 2)
);


-- Create the Sale table
CREATE TABLE Sale (
    Sale_ID INT AUTO_INCREMENT PRIMARY KEY,
    Emp_ID INT,
    Cust_ID INT,
    VIN VARCHAR(17),
    Price DECIMAL(10, 2),
    Sale_Date DATE,
    FOREIGN KEY (Emp_ID) REFERENCES Employee(Employee_ID),
    FOREIGN KEY (Cust_ID) REFERENCES Customer(Customer_ID),
    FOREIGN KEY (VIN) REFERENCES Vehicle(VIN)
);

CREATE TABLE Inventory (
    Inventory_ID INT PRIMARY KEY,
    Quantity_Per_Vehicle CHAR(10),
    Model_Info CHAR(255)
);

-- Option type is called Options because option is a key word in mysql
CREATE TABLE Options (
    VIN VARCHAR(17) PRIMARY KEY,
    Engine CHAR(255),
    Transmission CHAR(255),
    Drive_Train CHAR(255),
    Color CHAR(255),
    FOREIGN KEY (VIN) REFERENCES Vehicle(VIN)
);