import re
import argparse
from collections import OrderedDict
import ipdb

def process_intrinsic_cases(input_file, output_file):
    with open(input_file, 'r') as file:
        content = file.read()

    pattern = r'case\s+TokenType::(\w+):\s*return\s*"([^"]+)";'
    matches = re.findall(pattern, content)
 
    with open(output_file, 'w') as file:
        for match in matches:
            file.write(f"{match[0]}, {match[1]}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Process compiler intrinsic case statements with default file paths',
        formatter_class=argparse.RawTextHelpFormatter)
    

    #es2panda/lexer/token/token.cpp
    parser.add_argument('input', 
                        nargs='?', 
                        default='../../arkcompiler/ets_frontend/es2panda/lexer/token/token.cpp',
                        help='Input file path (default: %(default)s)')
    
    parser.add_argument('output', 
                        nargs='?', 
                        default='tokens.txt',
                        help='Output file path (default: %(default)s)')
    
    args = parser.parse_args()
    process_intrinsic_cases(args.input, args.output)

