#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "json.hpp"  // Biblioteca JSON para C++

using json = nlohmann::json;

class RapGenerator {
   private:
    std::map<std::string, std::string> var_map;
    int current_reg = 0;
    int label_counter = 0;
    std::vector<std::string> code;

    std::string get_reg(const std::string& var) {
        if (var_map.find(var) == var_map.end()) {
            var_map[var] = "%r" + std::to_string(current_reg);
            current_reg++;
        }
        return var_map[var];
    }

    std::string new_label() {
        label_counter++;
        return "L" + std::to_string(label_counter);
    }

    std::string handle_value(const json& node) {
        if (!node.contains("instruction")) {
            return "0";
        }

        std::string instr = node["instruction"];
        if (instr == "CONSTANT") {
            if (node["value"].contains("instruction")) {
                std::string value = node["value"]["instruction"];
                if (value == "TRUE") return "1";
                if (value == "FALSE") return "0";
                if (isdigit(value[0])) return value;
                return "\"" + value + "\"";
            }
            return "0";
        } else if (instr == "VARIABLE") {
            return get_reg(node["value"]["instruction"]);
        }
        return "0";
    }

    std::string generate_expression(const json& node) {
        static const std::map<std::string, std::string> instr_map = {
            {"INTEGER_ADDITION", "add"},
            {"INTEGER_SUBTRACTION", "sub"},
            {"INTEGER_MULTIPLICATION", "mult"},
            {"INTEGER_DIVISION", "div"},
            {"INTEGER_MODULUS", "mod"},
            {"GREATER_THAN", "greater"},
            {"GREATER_THAN_EQUAL", "greatereq"},
            {"LESS_THAN", "less"},
            {"LESS_THAN_EQUAL", "lesseq"},
            {"EQUAL", "equal"},
            {"NOT_EQUAL", "diff"},
            {"AND", "and"},
            {"OR", "or"}};

        if (!node.contains("instruction")) return "0";
        std::string instr = node["instruction"];

        if (instr_map.find(instr) != instr_map.end()) {
            std::string op = instr_map.at(instr);
            std::string left = generate_expression(node["value"]);
            std::string right = generate_expression(node["next"]);
            std::string result_reg = "%r" + std::to_string(current_reg);
            code.push_back(op + " " + result_reg + ", " + left + ", " + right);
            current_reg++;
            return result_reg;
        } else if (instr == "NEGATION") {
            std::string value = generate_expression(node["value"]);
            std::string result_reg = "%r" + std::to_string(current_reg);
            code.push_back("not " + result_reg + ", " + value);
            current_reg++;
            return result_reg;
        } else if (instr == "VARIABLE") {
            return get_reg(node["value"]["instruction"]);
        } else if (instr == "CONSTANT") {
            return handle_value(node);
        } else if (instr == "ARRAY") {
            std::string array_reg = get_reg(node["name"]);
            json current = node["value"];
            int index = 0;
            while (!current.is_null()) {
                std::string value = handle_value(current["value"]);
                code.push_back("store " + value + ", " + std::to_string(index) + "(" + array_reg + ")");
                current = current.value("next", json());
                index++;
            }
            return array_reg;
        }
        return "0";
    }

   public:
    void generate(const json& node) {
        if (node.is_null()) return;

        if (!node.contains("instruction")) return;
        std::string instr = node["instruction"];

        if (instr == "VARIABLE_DECLARATION") {  // Corrigido typo
            std::string var_name = node["value"]["instruction"];
            json value_node = node["value"]["value"]["value"];
            std::string value = handle_value(value_node);
            std::string var_reg = get_reg(var_name);
            code.push_back("mov " + var_reg + ", " + value);
        } else if (instr == "ASSIGN") {
            std::string var_name = node["value"]["instruction"];
            json value_node = node["value"]["value"];
            if (value_node.contains("instruction") && value_node["instruction"] == "ARRAY") {
                std::string array_reg = get_reg(var_name);
                json current = value_node["value"];
                int index = 0;
                while (!current.is_null()) {
                    std::string value = handle_value(current["value"]);
                    code.push_back("store " + value + ", " + std::to_string(index) + "(" + array_reg + ")");
                    current = current.value("next", json());
                    index++;
                }
            } else {
                std::string value = generate_expression(value_node);
                std::string var_reg = get_reg(var_name);
                code.push_back("mov " + var_reg + ", " + value);
            }
        } else if (instr == "IF_ELSE_CONDITION") {
            std::string else_label = new_label();
            std::string end_label = new_label();

            json cond_node = node["value"]["value"]["value"];
            std::string cond = generate_expression(cond_node);
            code.push_back("jf " + cond + ", " + else_label);

            generate(node["value"]["next"]);
            code.push_back("jump " + end_label);

            code.push_back("label " + else_label);
            if (node.contains("next")) {
                generate(node["next"]);
            }

            code.push_back("label " + end_label);
        } else if (instr == "FOR_LOOP") {
            std::string start_label = new_label();
            std::string end_label = new_label();

            // Inicialização
            json init_node = node["value"]["value"];
            std::string var_name = init_node["value"]["value"]["instruction"];
            std::string var_reg = get_reg(var_name);
            std::string init_val = handle_value(init_node["value"]["value"].value("value", json()));
            code.push_back("mov " + var_reg + ", " + init_val);

            // Condição
            code.push_back("label " + start_label);
            std::string limit = handle_value(node["value"]["value"]["next"]);
            std::string cond_reg = "%r" + std::to_string(current_reg);
            code.push_back("less " + cond_reg + ", " + var_reg + ", " + limit);
            current_reg++;
            code.push_back("jf " + cond_reg + ", " + end_label);

            // Corpo do loop
            generate(node["value"]["next"]);

            // Incremento
            code.push_back("add " + var_reg + ", " + var_reg + ", 1");
            code.push_back("jump " + start_label);

            // Fim do loop
            code.push_back("label " + end_label);
        } else if (instr == "PRINT_STATEMENT") {
            json target = node["value"];
            if (target["instruction"] == "VARIABLE") {
                std::string var_reg = get_reg(target["value"]["instruction"]);
                code.push_back("printv " + var_reg);
            } else {
                std::string value = handle_value(target["value"]);
                code.push_back("printf " + value);
            }
        }

        if (node.contains("next")) {
            generate(node["next"]);
        }
    }

    const std::vector<std::string>& getCode() const {
        return code;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " input.json output.rap\n";
        return 1;
    }

    RapGenerator generator;

    try {
        std::ifstream input(argv[1]);
        json ast;
        input >> ast;

        generator.generate(ast);

        std::ofstream output(argv[2]);
        for (const auto& line : generator.getCode()) {
            output << line << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}