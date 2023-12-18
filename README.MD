# Car Dealership App - 355 Databases Project

![GitHub contributors](https://img.shields.io/github/stars/jordannakamoto/355CarDealership?style=social)

Simple app with a native Desktop UI that demonstrates a cpp program that interacts with a local mysql database.

![Screenshot](https://i.ibb.co/hd2ds9z/Screenshot-2023-12-05-at-10-56-19-PM.png)

## Table of Contents
- [Car Dealership App - 355 Databases Project](#car-dealership-app---355-databases-project)
  - [Table of Contents](#table-of-contents)
  - [Getting Started / Installation](#getting-started--installation)
  - [Running the App](#running-the-app)
  - [Development](#development)
  - [Authors](#authors)
      - [Jordan Nakamoto](#jordan-nakamoto)
  - [License](#license)
  - [Acknowledgments](#acknowledgments)

## Getting Started / Installation

If you haven't installed mysql locally, please do so. Additionally, I am using mycli as my command line in iTerm2 to interact with mysql from a Mac Terminal.

Clone GitHub project and install dependency libraries on your computer.

For mysql-cpp data interface:
1. Setup Oracle mysqlcppconnector. On Mac you can just download and run an installer.
https://github.com/mysql/mysql-connector-cpp

2. Connect the CPP linker to the mysqlcppconn library locations in the Makefile. Current setup is for M1 Macbook Air filesystem
   - two source directories must be linked by their path from download location to the compiler
   - using lmysqlcppconn8 - version 8 of the connector
   - we utilize the library through #include <mysqlx/xdevapi.h>
   - xDevAPI Docs: https://dev.mysql.com/doc/dev/connector-cpp/latest/group__devapi.html

3. Download http://wxwidgets.org, the GUI library.
   - on MacOSX this is most easily done through Homebrew Package Manager
   - once again, this library has been linked in the Makefile for building on M1 Mac filesystem as documented here:
   - https://wiki.wxwidgets.org/Wx-Config
   - tutorial: https://www.youtube.com/watch?v=kPB5Y6ef9dw&list=PL0qQTroQZs5suCJzJYndeBQjLZ4T6q_H-

4. Once the program is can make and compile, set up your source data by copying and pasting the contents of the SQLCode folder into your mysql command line.

Note: Makefile is also using clang c++ compiler which is default on M1 Mac.

```
	355CARDEALERSHIP
	├── Assets     - png logo image
	└── Functions  - CRUD mysql functions
        └── SQLCode    - Plain .sql files to copy and paste into mysql cli in order to setup initial data
        app            - compiled program
        main.cpp       - main program, setup and draw UI, connect to mysql
        Makefile       - for compiling and building
```

## Running the App

* Commands from VSCode integrated terminal:
  ```
    make
    ./app
  ```

## Development

Intended basic functionality is implemented for the sale data type. For other data types, only read is implemented so the project can be extended.

TODO:
- move authentication to a settings panel
- make a panel for creating a new sale
- better UI for information based on its application value. I.E. make the employee name more accessible throughout the UX than their ID or Address.


## Authors

#### Jordan Nakamoto
* [GitHub]

## License

Open source software [licensed as MIT][license].

## Acknowledgments

Professor Vysala - Sonoma State University

* Team Members
  
[//]: # (HyperLinks)

[GitHub]: https://github.com/jordannakamoto

[license]: https://github.com/madhur-taneja/README-template/blob/master/LICENSE.md
