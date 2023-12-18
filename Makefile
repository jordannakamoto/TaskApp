MYSQL_CONCPP_DIR = /usr/local/mysql-connector-c++-8.2
WX_CXXFLAGS = `wx-config --cxxflags`
WX_LIBS = `wx-config --libs`
CPPFLAGS = -I $(MYSQL_CONCPP_DIR)/include $(WX_CXXFLAGS)
CXXFLAGS = -std=c++17
LDFLAGS = -L $(MYSQL_CONCPP_DIR)/lib64 -Wl,-rpath,$(MYSQL_CONCPP_DIR)/lib64
LDLIBS = -lmysqlcppconn8 $(WX_LIBS)
CXX = clang++ -stdlib=libc++

# Define your source files
SRC = main.cpp \
 	  Functions/Helpers/helpers.cpp \
      Functions/Read/Sale/sale.cpp \
	  Functions/Read/Employee/employee.cpp \
	  Functions/Read/Vehicle/vehicle.cpp \
	  Functions/Read/Customer/customer.cpp \
	  Functions/Read/Options/options.cpp \
	  Functions/Read/Inventory/inventory.cpp \
	  Functions/Update/Sale/sale.cpp \
	  Functions/Delete/Sale/sale.cpp \
	  Functions/Delete/Customer/customer.cpp \
	  Functions/Create/Sale/sale.cpp \
	  Functions/Create/Customer/customer.cpp \
      # Add other .cpp files here

# Define object files from source files
OBJ = $(SRC:.cpp=.o)

# Define your target
app: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Generic rule for compiling .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) app
