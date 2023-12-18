INSERT INTO Employee (Employee_ID, Name, Make_Responsible, Phone_Number, Email, Address) VALUES
(1, 'John Doe', 'Honda', '707-775-0190', 'john.doe@email.com', '234 Main St'),
(2, 'Jane Smith', 'Toyota', '510-424-2127', 'jane.smith@email.com', '4 21st St'),
(3, 'Emily Johnson', 'Ford', '707-555-0102', 'emily.johnson@email.com', '424 Sunset Blvd.');

INSERT INTO Customer (Customer_ID, Name, Phone_Number, Email, Address) VALUES
(101, 'Alice Brown', '707-555-0200', 'alice.brown@email.com', '5678 Park Ave'),
(102, 'Bob Green', '415-542-0422', 'bob.green@email.com', '305 Pine Street'),
(103, 'Charlie Black', '415-125-322', 'charlie.black@email.com', '56 Elm Road');

INSERT INTO Vehicle (VIN, Manufacturer, Model_Name, Model_Year, Price) VALUES
('1HGBH41JXMN109186', 'Honda', 'Civic', '2021-01-01', 20000.00),
('JH4KA7660MC000000', 'Toyota', 'Corolla', '2022-01-01', 18000.00),
('2HGES267XNH590480', 'Ford', 'Focus', '2023-01-01', 22000.00);

INSERT INTO Sale (Sale_ID, Emp_ID, Cust_ID, VIN, Price, Sale_Date) VALUES
(1001, 1, 101, '1HGBH41JXMN109186', 20000.00, '2023-03-01'),
(1002, 2, 102, 'JH4KA7660MC000000', 18000.00, '2023-03-02'),
(1003, 3, 103, '2HGES267XNH590480', 22000.00, '2023-03-03');
(3, 103, '2HGES267XNH590480', 22000.00, '2023-03-03');


INSERT INTO Inventory (Inventory_ID, Quantity_Per_Vehicle, Model_Info)
VALUES
    (1, '5', 'Honda Standard model with basic features'),
    (2, '3', 'Toyota Upgraded model with leather seats'),
    (3, '7', 'Ford Luxury model with advanced technology');

INSERT INTO Options (VIN, Engine, Transmission, Drive_Train, Color)
VALUES
    ('1HGBH41JXMN109186', '2.0L 4-cylinder', 'Automatic', 'Front-Wheel Drive', 'Silver'),
    ('JH4KA7660MC000000', '1.8L 4-cylinder', 'Automatic', 'Front-Wheel Drive', 'Blue'),
    ('2HGES267XNH590480', '2.5L 4-cylinder', 'Automatic', 'Front-Wheel Drive', 'Red');
