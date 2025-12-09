import argparse
import json
from pathlib import Path
import sys

# --- Configuration ---
# Default directory for categorized outputs
DEFAULT_CATEGORY_DIR = "status_outputs"
# Name of the global result file located within the default directory
DEFAULT_GLOBAL_RESULT_FILE_NAME = "res.json" 

def count_members_in_json_file(filepath):
    """
    Reads a JSON file and counts the number of members (list items or dict keys).
    """
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            data = json.load(f)
            if isinstance(data, list) or isinstance(data, dict):
                count = len(data)
                return count, "valid"
            else:
                return 1, type(data).__name__

    except (FileNotFoundError, json.JSONDecodeError, Exception) as e:
        # print(f"Error reading {filepath}: {e}") # Keep quiet for cleaner output
        return 0, "N/A"

def main():
    parser = argparse.ArgumentParser(description="Read JSON files in a directory and count members/items, calculating percentages.")
    
    # Directory argument, optional with default value
    parser.add_argument("-D", "--directory", type=str, default=DEFAULT_CATEGORY_DIR,
                        help=f"The directory containing all JSON files (including res.json) (default: '{DEFAULT_CATEGORY_DIR}').")
    
    # Global file name argument for overriding the default name
    parser.add_argument("-G", "--global_file_name", type=str, default=DEFAULT_GLOBAL_RESULT_FILE_NAME,
                        help=f"The name of the global JSON file (must be inside the directory) (default: '{DEFAULT_GLOBAL_RESULT_FILE_NAME}').")
    
    args = parser.parse_args()

    input_dir = Path(args.directory)
    global_file_path = input_dir / args.global_file_name # Construct full path

    if not input_dir.is_dir():
        print(f"Error: '{args.directory}' is not a valid directory.")
        sys.exit(1)
    if not global_file_path.is_file():
        print(f"Error: Global result file not found at '{global_file_path}'. Please ensure '{args.global_file_name}' exists within the '{args.directory}' directory.")
        sys.exit(1)

    # 1. Get the total count
    total_count, _ = count_members_in_json_file(global_file_path)
    
    if total_count == 0:
        print("Error: Global result file is empty, cannot calculate proportions.")
        sys.exit(1)

    print("-" * 45)
    print(f"Total file count (from {global_file_path.name}): {total_count}")
    print(f"Scanning directory: {input_dir.resolve()}")
    print("-" * 45)

    # 2. Scan categorized JSON files and count members
    # Find all JSON files in the directory
    json_files = sorted(list(input_dir.glob('*.json'))) 
    
    if not json_files:
        print("No categorized JSON files found in the specified directory.")
        return

    processed_count = 0

    print("File Name             | Count | Proportion")
    print("-" * 45)
    for file_path in json_files:
        # Exclude the global file itself from the categorized list
        if file_path.name == args.global_file_name:
            continue

        count, data_type = count_members_in_json_file(file_path)
        if count > 0:
            percentage = (count / total_count) * 100
            # Use formatted output to align columns
            print(f"{file_path.name:<20} | {count:^4} | {percentage:8.2f}%")
            processed_count += count
        
    print("-" * 45)
    print(f"Processed category total: {processed_count} (Should ideally equal total count {total_count})")
    if processed_count != total_count:
         print(f"Warning: Total counts do not match, there might be uncategorized records or JSON parsing errors.")


if __name__ == "__main__":
    main()
