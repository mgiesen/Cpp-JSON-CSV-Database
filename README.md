# SimpleJsonCsvDatabase

## Overview

This C++ library simplifies the storage and management of numerical data, such as sensor measurements, by utilizing a straightforward CSV format. By doing so, it ensures that data remains easily transferable, compatible with external visualization tools, and accessible during program runtime by other applications.

### Key Features:

- **Efficiency**: The library offers exceptional speed, and the process of adding a new row remains independent of the size of the CSV file, ensuring efficient performance.

- **Dynamic Data Interaction**: Data can be both read and modified at runtime, enabling dynamic data interaction.

- **Flexible Column Handling**: The library allows for the addition, repositioning, or deletion of columns, providing flexibility in data management.

- **Segmented Data Sharing**: It enables data retrieval from the database on a row-by-row basis, allowing it to be organized and maintained within a JSON object. This functionality simplifies segmented data sharing, for instance, through a REST interface.

## Prerequisites

Before you begin, ensure you have met the following requirements:

- C++ development environment.
- [JsonCpp](https://github.com/open-source-parsers/jsoncpp) library for JSON support. I recommend using [vcpkg](https://github.com/microsoft/vcpkg) for easy installation.

## Getting Started

To get started, follow these steps:

1. Clone the repository:

```bash
git clone https://github.com/yourusername/SimpleJsonCsvDatabase.git
```

2. Include the `JsonCsvDatabase.h` header file in your C++ project.

3. Refer to the example code for usage.

# Example

```cpp
#include <iostream>

#include "JsonCsvDatabase.h"

void demonstrate_database_functions()
{
    // Initialize the JsonCsvDatabase with database file path, empty value placeholder, delimiter, and debug mode.
    JsonCsvDatabase database("./database.csv", 0, ';', true);

    // Define a new database row using a map.
    std::map<std::string, int> new_database_row = {
        {"Column A", 1},
        {"Column B", 2},
        {"Column C", 3},
        {"Column D", 4},
        {"Column E", 5},
    };

    // Add the new database row to the database, and check if it was successful.
    if (database.add_data_row(new_database_row) != EXIT_SUCCESS)
    {
        std::cout << "New database row could not be created" << std::endl;
        return;
    }

    // Query the database and store the result in a Json::Value object, then check if the query was successful.
    Json::Value json_output;
    if (database.load_data_rows(json_output) != EXIT_SUCCESS)
    {
        std::cout << "Database query failed" << std::endl;
        return;
    }
    else
    {
        // Print the JSON output if the query was successful.
        std::cout << json_output << std::endl;
    }
}

int main()
{
    demonstrate_database_functions();
    std::getchar();
}

```

# **How can I contribute?** 💁🏼

- **Report Issues:** If you encounter any issues or bugs, please open an issue on GitHub.

- **Enhancements:** Feel free to propose new features or improvements by creating a pull request.

- **Documentation:** Help improve the documentation by fixing typos, clarifying instructions, or adding examples. Documentation enhancements can greatly benefit other users.

- **Tell your friends:** If you find the project useful, consider sharing it with others in your community or network.
