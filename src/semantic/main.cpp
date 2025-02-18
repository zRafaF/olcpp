#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "generator.h"
#include "json.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    std::ifstream input_file(argv[1]);
    if (!input_file) {
        std::cerr << "Error: Could not open input file.\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string input = buffer.str();

    json_value_s* root = json_parse(input.c_str(), strlen(input.c_str()));

    Generator raposeitorGenerator = Generator();
    raposeitorGenerator.parseInstructions(root);
    raposeitorGenerator.printInstructions();

    // std::cout << getInstruction(object) << std::endl;
    // std::cout << getInstruction(getValue(object)) << std::endl;
    // std::cout << json_value_as_string(getValue(getValue(object))->start->value)->string << std::endl;

    // CodeGenContext ctx;
    // generate_statement(input, ctx);

    std::ofstream output_file(argv[2]);
    if (!output_file) {
        std::cerr << "Error: Could not open output file.\n";
        return 1;
    }

    // output_file << ctx.code;
    std::cout << "Assembly code generated successfully and written to " << argv[2] << std::endl;

    return 0;
}