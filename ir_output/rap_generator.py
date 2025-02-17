import json
import sys

class RapGenerator:
    def __init__(self):
        self.var_map = {}  # Mapeia variáveis para registradores
        self.current_reg = 0  # Contador de registradores
        self.label_counter = 0  # Contador de rótulos
        self.code = []  # Lista de instruções RAP geradas

    def get_reg(self, var):
        """Retorna o registrador associado a uma variável."""
        if var not in self.var_map:
            self.var_map[var] = f"%r{self.current_reg}"
            self.current_reg += 1
        return self.var_map[var]

    def new_label(self):
        """Gera um novo rótulo único."""
        self.label_counter += 1
        return f"L{self.label_counter}"

    def handle_value(self, node):
        """Traduz um valor (constante ou variável) para RAP."""
        if "instruction" not in node:
            return "0"

        if node["instruction"] == "CONSTANT":
            value = node["value"].get("instruction", "")
            if value.isdigit():
                return value
            elif value in ["TRUE", "FALSE"]:
                return "1" if value == "TRUE" else "0"
            else:
                return f'"{value}"'
        elif node["instruction"] == "VARIABLE":
            return self.get_reg(node["value"]["instruction"])
        return "0"

    def generate_expression(self, node):
        """Gera código RAP para uma expressão."""
        instr_map = {
            "INTEGER_ADDITION": "add",
            "INTEGER_SUBTRACTION": "sub",
            "INTEGER_MULTIPLICATION": "mult",
            "INTEGER_DIVISION": "div",
            "INTEGER_MODULUS": "mod",
            "GREATER_THAN": "greater",
            "GREATER_THAN_EQUAL": "greatereq",
            "LESS_THAN": "less",
            "LESS_THAN_EQUAL": "lesseq",
            "EQUAL": "equal",
            "NOT_EQUAL": "diff",
            "AND": "and",
            "OR": "or"
        }

        instr = node.get("instruction")
        if instr in instr_map:
            op = instr_map[instr]
            left = self.generate_expression(node["value"])
            right = self.generate_expression(node["next"])
            result_reg = f"%r{self.current_reg}"
            self.code.append(f"{op} {result_reg}, {left}, {right}")
            self.current_reg += 1
            return result_reg

        elif instr == "NEGATION":
            value = self.generate_expression(node["value"])
            result_reg = f"%r{self.current_reg}"
            self.code.append(f"not {result_reg}, {value}")
            self.current_reg += 1
            return result_reg

        elif instr == "VARIABLE":
            return self.get_reg(node["value"]["instruction"])

        elif instr == "CONSTANT":
            return self.handle_value(node)

        elif instr == "ARRAY":
            array_reg = self.get_reg(node["name"])
            current = node["value"]
            index = 0
            while current:
                value = self.handle_value(current["value"])
                self.code.append(f"store {value}, {index}({array_reg})")
                current = current.get("next")
                index += 1
            return array_reg

        return "0"

    def generate(self, node):
        """Gera código RAP a partir de um nó da AST."""
        if not node:
            return

        instr = node.get("instruction", "")

        if instr == "VARIABLE_DECLARATION":
            var_name = node["value"]["instruction"]
            value_node = node["value"]["value"]["value"]
            value = self.handle_value(value_node)
            var_reg = self.get_reg(var_name)
            self.code.append(f"mov {var_reg}, {value}")

        elif instr == "ASSIGN":
            var_name = node["value"]["instruction"]
            value_node = node["value"]["value"]
            if value_node.get("instruction") == "ARRAY":
                array_reg = self.get_reg(var_name)
                current = value_node["value"]
                index = 0
                while current:
                    value = self.handle_value(current["value"])
                    self.code.append(f"store {value}, {index}({array_reg})")
                    current = current.get("next")
                    index += 1
            else:
                value = self.generate_expression(value_node)
                var_reg = self.get_reg(var_name)
                self.code.append(f"mov {var_reg}, {value}")

        elif instr == "IF_ELSE_CONDITION":
            else_label = self.new_label()
            end_label = self.new_label()

            cond = self.generate_expression(node["value"]["value"]["value"])
            self.code.append(f"jf {cond}, {else_label}")

            self.generate(node["value"]["next"])
            self.code.append(f"jump {end_label}")

            self.code.append(f"label {else_label}")
            if "next" in node:
                self.generate(node["next"])

            self.code.append(f"label {end_label}")

        elif instr == "FOR_LOOP":
            start_label = self.new_label()
            end_label = self.new_label()

            # Inicialização
            init_node = node["value"]["value"]
            var_name = init_node["value"]["value"]["instruction"]
            var_reg = self.get_reg(var_name)
            init_val = self.handle_value(init_node["value"]["value"].get("value", {}))
            self.code.append(f"mov {var_reg}, {init_val}")

            # Condição
            self.code.append(f"label {start_label}")
            limit = self.handle_value(node["value"]["value"]["next"])
            cond_reg = f"%r{self.current_reg}"
            self.code.append(f"less {cond_reg}, {var_reg}, {limit}")
            self.current_reg += 1
            self.code.append(f"jf {cond_reg}, {end_label}")

            # Corpo do loop
            self.generate(node["value"]["next"])

            # Incremento
            self.code.append(f"add {var_reg}, {var_reg}, 1")
            self.code.append(f"jump {start_label}")

            # Fim do loop
            self.code.append(f"label {end_label}")

        elif instr == "PRINT_STATEMENT":
            target = node["value"]
            if target["instruction"] == "VARIABLE":
                var_reg = self.get_reg(target["value"]["instruction"])
                self.code.append(f"printv {var_reg}")
            else:
                value = self.handle_value(target["value"])
                self.code.append(f'printf {value}')

        if "next" in node:
            self.generate(node["next"])

def main():
    if len(sys.argv) != 3:
        print("Uso: python conversor.py input.json output.rap")
        sys.exit(1)

    generator = RapGenerator()

    with open(sys.argv[1]) as f:
        ast = json.load(f)

    generator.generate(ast)

    with open(sys.argv[2], "w") as f:
        f.write("\n".join(generator.code))

if __name__ == "__main__":
    main()