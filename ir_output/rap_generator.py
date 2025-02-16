import json
import sys

class RapGenerator:
    def __init__(self):
        self.var_map = {}
        self.current_reg = 0
        self.label_counter = 0
        self.code = []
        
    def get_reg(self, var):
        if var not in self.var_map:
            self.var_map[var] = f"%r{self.current_reg}"
            self.current_reg += 1
        return self.var_map[var]
    
    def new_label(self):
        self.label_counter += 1
        return f"L{self.label_counter}"
    
    
    def handle_value(self, node):
        if node["instruction"] == "CONSTANT":
            value = node["value"]["instruction"]
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
        if node["instruction"] == "INTEGER_ADDITION":
            left = self.generate_expression(node["value"])
            right = self.generate_expression(node["next"])
            self.code.append(f"add %r{self.current_reg}, {left}, {right}")
            return f"%r{self.current_reg}"
        
        elif node["instruction"] == "VARIABLE":
            return self.get_reg(node["value"]["instruction"])
        
        elif node["instruction"] == "CONSTANT":
            return self.handle_value(node)
        
        elif node["instruction"] == "ARRAY":
            current = node["value"]
            index = 0
            while current:
                value = self.handle_value(current["value"])
                self.code.append(f"load %r{self.current_reg}, {index}({self.get_reg(node['name'])})")
                self.code.append(f"add %r{self.current_reg}, %r{self.current_reg}, {value}")
                current = current.get("next")
                index += 1
            return f"%r{self.current_reg}"        
        return "0"
    
    def generate(self, node):
        if not node:
            return
        
        instr = node.get("instruction", "")
        
        if instr == "VARIABLE_DECLARATION":
            var_name = node["value"]["instruction"]
            var_type = node["value"]["value"]["instruction"]
            value_node = node["value"]["value"]["value"]
            
            if var_type == "STRING_TYPE":
                value = self.handle_value(value_node)
                self.code.append(f'printf {value}')
            else:
                value = self.handle_value(value_node)
                self.code.append(f"mov {self.get_reg(var_name)}, {value}")
        
        elif instr == "ASSIGN":
            var_name = node["value"]["instruction"]
            value_node = node["value"]["value"]
            
            if value_node.get("instruction") == "ARRAY":
                current = value_node["value"]
                index = 0
                while current:
                    value = self.handle_value(current["value"])
                    self.code.append(f"store {value}, {index}({self.get_reg(var_name)})")
                    current = current.get("next")
                    index += 1
            else:
                value = self.generate_expression(value_node)
                self.code.append(f"mov {self.get_reg(var_name)}, {value}")
        
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
            
            init_node = node["value"]["value"]
            var_reg = self.get_reg(init_node["value"]["value"]["instruction"])
            init_val = self.handle_value(init_node["value"]["value"]["value"])
            self.code.append(f"mov {var_reg}, {init_val}")
            
            self.code.append(f"label {start_label}")
            
            limit = self.handle_value(node["value"]["value"]["next"])
            self.code.append(f"lesseq %r{self.current_reg}, {var_reg}, {limit}")
            self.code.append(f"jf %r{self.current_reg}, {end_label}")
            
            self.generate(node["value"]["next"])
            
            self.code.append(f"add {var_reg}, {var_reg}, 1")
            self.code.append(f"jump {start_label}")
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