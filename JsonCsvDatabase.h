#pragma once

/*
 * File: JsonCsvDatabase.h
 * Author: Maximilian Giesen
 *
 * License: MIT License
 * GitHub: https://github.com/mgiesen/SimpleJsonCsvDatabase
 */

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

// External libraries
#include <json/json.h>

class JsonCsvDatabase
{
    std::map<std::string, int> m_database_blueprint = {};
    ; // Defines the database structure
    std::vector<std::string> m_ordered_database_column_names = {};
    ; // Stores sorted column names

    std::string m_database_csv_filepath = ""; // Path to the CSV file

    char m_delimiter = ';';            // Delimiter used in the CSV file
    int m_empty_value_placeholder = 0; // Placeholder for empty values

    bool m_debug_mode = false; // Debug mode (default: off)

    // Converts an iterable object to a JSON array
    template <typename iterable>
    Json::Value iterable2json(iterable const &object)
    {
        Json::Value json_array = Json::arrayValue;
        for (auto &&element : object)
        {
            json_array.append(element);
        }
        return json_array;
    }

    // Prints a debug message if debug mode is enabled
    void print_debug_message(std::string message)
    {
        if (m_debug_mode)
        {
            std::cout << "[JsonCsvDatabase] " + message << std::endl;
        }
    }

    // Reads the existing database structure from a CSV file
    int get_existing_blueprint_from_csv()
    {
        std::ifstream file(m_database_csv_filepath);
        std::string first_line;

        if (!file)
        {
            // No CSV file found, creating a new database
            print_debug_message("Warning: No CSV file found. Creating a new database.");
            return EXIT_SUCCESS;
        }
        else if (file.is_open())
        {
            // Read the first line to extract column names
            std::getline(file, first_line);

            std::string column_name;
            std::stringstream ss(first_line);

            int column_index = 0;

            while (std::getline(ss, column_name, m_delimiter))
            {
                // Check if a column name is not a string
                if (atoi(column_name.c_str()) != 0)
                {
                    print_debug_message("Error: At least one column name in the first line is not a string.");
                    return EXIT_FAILURE;
                }

                // Store the column name and its index in the blueprint
                m_database_blueprint[column_name] = column_index;

                column_index++;
            }

            file.close();
        }
        else
        {
            print_debug_message("Error: Database file could not be opened.");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    // Creates CSV headings based on the blueprint
    void create_csv_headings_from_blueprint()
    {
        std::remove((m_database_csv_filepath + ".tmp").c_str());

        std::ifstream old_file(m_database_csv_filepath);
        std::ofstream new_file(m_database_csv_filepath + ".tmp");

        if (!new_file.is_open())
        {
            print_debug_message("Warning: Unhandled exception. Unable to create a temporary database.");
            return;
        }

        if ((!old_file || old_file.is_open()))
        {
            // Write the header line to the .tmp file
            for (int i = 0; i < m_ordered_database_column_names.size(); i++)
            {
                new_file << m_ordered_database_column_names[i];

                if (i != m_ordered_database_column_names.size() - 1)
                {
                    new_file << m_delimiter;
                }
            }

            new_file << "\n";

            // Copy the data section to the .tmp file
            bool ignore_next_line = true;
            std::string line;
            while (std::getline(old_file, line))
            {
                // Ignore the first line
                if (ignore_next_line)
                {
                    ignore_next_line = false;
                    continue;
                }

                if (line != "")
                {
                    new_file << line << "\n";
                }
            }

            old_file.close();
            new_file.close();

            // Replace the original file with the new file
            std::remove((m_database_csv_filepath).c_str());
            std::rename((m_database_csv_filepath + ".tmp").c_str(), m_database_csv_filepath.c_str());
        }
        else
        {
            print_debug_message("Warning: Unhandled exception. Unable to open the existing database.");
        }
    }

    // Sorts the database blueprint based on column indices
    void sort_database_blueprint()
    {
        std::vector<std::pair<std::string, int>> pairs(m_database_blueprint.begin(), m_database_blueprint.end());

        std::sort(pairs.begin(), pairs.end(),
                  [](const std::pair<std::string, int> &a, const std::pair<std::string, int> &b)
                  {
                      return a.second < b.second;
                  });

        m_ordered_database_column_names.clear();

        for (auto it = pairs.begin(); it != pairs.end(); ++it)
        {
            const auto &key = it->first;
            const auto &value = it->second;
            m_ordered_database_column_names.push_back(key);
        }
    }

    // Adds missing columns to the database blueprint
    void add_missing_columns_to_database_blueprint(std::map<std::string, int> &program_database_blueprint)
    {
        bool blueprint_inconsistent_with_csv = false;

        for (auto it = program_database_blueprint.begin(); it != program_database_blueprint.end(); ++it)
        {
            if (m_database_blueprint.count(it->first) == 0)
            {
                // Add the missing column to the blueprint
                m_database_blueprint[it->first] = m_database_blueprint.size();

                blueprint_inconsistent_with_csv = true;
            }
        }

        sort_database_blueprint();

        if (blueprint_inconsistent_with_csv)
        {
            create_csv_headings_from_blueprint();
        }
    }

public:
    JsonCsvDatabase(const std::string &database_filepath, int empty_value_placeholder = 0, char delimiter = ';', bool debug_mode = false)
        : m_database_csv_filepath(database_filepath),
          m_delimiter(delimiter),
          m_empty_value_placeholder(empty_value_placeholder),
          m_debug_mode(debug_mode)
    {
    }

    // Stores a new row in the database
    int add_data_row(std::map<std::string, int> &new_data_row)
    {
        m_database_blueprint.clear();

        // #1 Add existing file columns to the database blueprint
        if (get_existing_blueprint_from_csv() == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }

        // #2 Add missing column names from the program-specific program_database_column_names
        add_missing_columns_to_database_blueprint(new_data_row);

        std::ofstream file(m_database_csv_filepath, std::ios::app);

        // #3 Write the new row to the database
        for (const auto &column_name : m_ordered_database_column_names)
        {
            if (new_data_row.count(column_name) == 0)
            {
                file << "" << m_delimiter;
            }
            else
            {
                file << new_data_row[column_name] << m_delimiter;
            }
        }

        file << "\n";
        file.close();

        return EXIT_SUCCESS;
    }

    // Retrieves data from the database and returns it in CSV format
    int load_data_rows(Json::Value &data, int first_row = 0, int last_row = 0, std::string alternative_database_path = "")
    {
        std::string path_to_database = m_database_csv_filepath;

        if (alternative_database_path != "")
        {
            path_to_database = alternative_database_path;
        }

        std::ifstream csv_file(path_to_database);

        if (!csv_file.is_open())
        {
            print_debug_message("Error: Database could not be opened. Path: " + m_database_csv_filepath);
            return EXIT_FAILURE;
        }

        std::vector<std::string> column_names;
        std::vector<std::vector<int>> column_values;

        int row_index = 0;

        first_row = first_row + 1;
        last_row = last_row + 1;

        std::string line;
        while (std::getline(csv_file, line))
        {
            std::stringstream ss(line);
            std::string column;

            row_index++;

            // Extract column names from the first line
            if (row_index == 1)
            {
                while (std::getline(ss, column, m_delimiter))
                {
                    column_names.push_back(column);
                    column_values.push_back({});
                }
                continue;
            }
            // Skip rows if less than the first_row
            else if (row_index < first_row)
            {
                continue;
            }
            // Stop querying if the row exceeds last_row
            else if (row_index > last_row && last_row != 1)
            {
                break;
            }

            // #2 Extract row values
            for (int i = 0; i < column_names.size(); i++)
            {
                std::getline(ss, column, m_delimiter);

                if (column.empty())
                {
                    column_values[i].push_back(m_empty_value_placeholder);
                }
                else
                {
                    column_values[i].push_back(std::stoi(column));
                }

                column.clear();
            }
        }

        data = Json::objectValue;

        for (int i = 0; i < column_names.size(); i++)
        {
            data[column_names[i]] = iterable2json(column_values[i]);
        }

        csv_file.close();

        return EXIT_SUCCESS;
    }
};
