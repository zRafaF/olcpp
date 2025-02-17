// src/semantic/generator.cpp
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "code.h"
#include "gen_for.h"
#include "gen_print_string.h"
#include "json.h"

using namespace std;

// Helper function to parse JSON into a vector of Code objects
vector<unique_ptr<Code>> parse_code_array(json_array_s* array);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    ifstream input_file(argv[1]);
    if (!input_file) {
        cerr << "Error: Could not open input file.\n";
        return 1;
    }

    stringstream buffer;
    buffer << input_file.rdbuf();
    string input = buffer.str();

    json_value_s* root = json_parse(input.c_str(), input.size());
    json_object_s* object = json_value_as_object(root);

    vector<unique_ptr<Code>> code_objects;

    // Iterate over JSON objects
    for (json_object_element_s* element = object->start; element; element = element->next) {
        string key = element->name->string;
        json_value_s* value = element->value;

        if (key == "GenFor") {
            // Parse GenFor
            json_object_s* for_obj = json_value_as_object(value);
            // Extract condition and code array
            // (You'll need to implement GenCondition parsing)
            vector<unique_ptr<Code>> code = parse_code_array(json_value_as_array(for_obj->start->next->value));
            code_objects.push_back(make_unique<GenFor>(nullptr, move(code)));  // Replace nullptr with parsed condition
        } else if (key == "GenPrintString") {
            // Parse GenPrintString
            string message = json_value_as_string(value)->string;
            code_objects.push_back(make_unique<GenPrintString>(message));
        }
    }

    // Generate output
    ofstream output_file(argv[2]);
    if (!output_file) {
        cerr << "Error: Could not open output file.\n";
        return 1;
    }

    for (const auto& code : code_objects) {
        code->generate(output_file);
    }

    cout << "Assembly code generated successfully and written to " << argv[2] << endl;

    return 0;
}

// Helper function to parse a JSON array into a vector of Code objects
vector<unique_ptr<Code>> parse_code_array(json_array_s* array) {
    vector<unique_ptr<Code>> code_objects;
    for (json_array_element_s* element = array->start; element; element = element->next) {
        json_object_s* obj = json_value_as_object(element->value);
        // Recursively parse nested code objects
        // (You'll need to implement this based on your JSON structure)
    }
    return code_objects;
}