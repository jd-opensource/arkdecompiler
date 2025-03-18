import re
import argparse
from collections import OrderedDict
import ipdb

def process_intrinsic_cases(input_file, output_file):
    with open(input_file, 'r') as file:
        content = file.read()

    # Regular expression to match case statements
    case_pattern = re.compile(r'case\s+BytecodeInstruction::Opcode::(\w+):')

    # Find all matches and store them in an OrderedDict to maintain order and ensure uniqueness
    matches = list(OrderedDict.fromkeys(case_pattern.findall(content)))

    # Separate matches into regular and deprecated
    regular_matches = [match for match in matches if not match.startswith('DEPRECATED')]
    deprecated_matches = [match for match in matches if match.startswith('DEPRECATED')]

    # Combine lists with regular matches first
    sorted_matches = regular_matches + deprecated_matches

    with open(output_file, 'w') as file:
        for match in sorted_matches:
            file.write(match + '\n')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Process compiler intrinsic case statements with default file paths',
        formatter_class=argparse.RawTextHelpFormatter)
    
    parser.add_argument('input', 
                        nargs='?', 
                        default='../out/gen/arkcompiler/runtime_core/compiler/generated/inst_builder_gen.cpp',
                        help='Input file path (default: %(default)s)')
    
    parser.add_argument('output', 
                        nargs='?', 
                        default='intrinsic_list.txt',
                        help='Output file path (default: %(default)s)')
    
    args = parser.parse_args()
    process_intrinsic_cases(args.input, args.output)

