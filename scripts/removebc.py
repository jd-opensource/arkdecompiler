import argparse
import os
import shutil

def remove_bc_data(input_file):
    temp_file = input_file + '.tmp'
    
    with open(input_file, 'r') as infile, open(temp_file, 'w') as outfile:
        for line in infile:
            bc_index = line.find('bc:')
            if bc_index != -1:
                line = line[:bc_index].rstrip() + '\n'
            outfile.write(line)
    
    shutil.move(temp_file, input_file)

def main():
    parser = argparse.ArgumentParser(description='Remove data after "bc:" in each line of a file.')
    parser.add_argument('-f', '--file', type=str, help='Input file name', default='logs/func_main_0:(any,any,any).ir')

    args = parser.parse_args()
    
    input_filename = args.file

    if not os.path.exists(input_filename):
        print(f"Error: The file '{input_filename}' does not exist.")
        return

    remove_bc_data(input_filename)
    print(f"Processing complete. Changes applied to '{input_filename}'.")

if __name__ == '__main__':
    main()
