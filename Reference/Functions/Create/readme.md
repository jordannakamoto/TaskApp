mysqlx::Schema.mysqlx::Table.mysqlx::TableInsert

define a table insert

then call the values method on that insert to populate its values

...well first we have to define the row that is the collection of values

mysqlx::Row.set(0, ) :: repeat for each column and its argument. i.e. 1, empId 2,price etc.

is what has been gathered from what the user entered into the UI which was passed into the function call to CreateSaleRow (the function defined in the Create file)

assigning a value to a row column by its index is rather vague but this is the method that I found on the API Docs.

anyway then we run insert.values(row)
