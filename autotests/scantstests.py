import argparse
import os
import shutil
import subprocess
import json
import sys
import signal
from pathlib import Path

# --- Default Configuration ---
DEFAULT_INPUT_DIR = "."
DEFAULT_RECORD_FILE = "analyzed_records.json"
DEFAULT_LOCAL_COPY_DIR = "temp_ts_files"
DEFAULT_ABC_PATH = "./abc_tool"

def load_records(record_path):
    """Load records of analyzed files."""
    if os.path.exists(record_path):
        try:
            # Records are now stored as a dictionary {file_path: status_details}
            with open(record_path, 'r', encoding='utf-8') as f:
                return json.load(f)
        except json.JSONDecodeError:
            print(f"Warning: Could not parse {record_path}, creating a new record file.")
    return {}

def save_records(records, record_path):
    """Save records of analyzed files."""
    with open(record_path, 'w', encoding='utf-8') as f:
        # Use pretty printing for readability
        json.dump(records, f, ensure_ascii=False, indent=4)

def analyze_file(ts_file_path, records, abc_path, temp_copy_dir):
    """
    Analyze a single TS file. Records the outcome (success/failure) in the records dict.
    Returns True if analysis was attempted, False if skipped (already analyzed).
    """
    # Use the absolute path as the unique ID/key
    file_id = str(ts_file_path.resolve())

    if file_id in records:
        print(f"Skipping already analyzed file: {ts_file_path.name}")
        return False

    if not os.path.exists(temp_copy_dir):
        os.makedirs(temp_copy_dir)
        
    local_copy_path = Path(temp_copy_dir) / ts_file_path.name
    try:
        shutil.copy(ts_file_path, local_copy_path)
        print(f"Analyzing file: {ts_file_path.name}")
    except IOError as e:
        print(f"Could not copy file {ts_file_path}: {e}")
        # Record the file with a specific status even if copy fails
        records[file_id] = {"status": f"Error (Copy Failed: {str(e)})", "return_code": -99}
        return True # Attempted analysis failed early

    # Execute abc tool
    status_detail = ""
    return_code = None

    try:
        result = subprocess.run(
            [abc_path, str(local_copy_path)],
            check=False,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        return_code = result.returncode

        if result.returncode == 0:
            print(f"✅ Success (Exit Code 0)")
            status_detail = "Success"
        elif result.returncode == 1:
            print(f"❌ Warning: File '{ts_file_path.name}' caused failure (std::exit(EXIT_FAILURE), Exit Code 1)!")
            status_detail = "Error (EXIT_FAILURE)"
        elif result.returncode < 0:
            # Negative return codes indicate termination by signal on Linux
            signal_value = abs(result.returncode)
            signal_name = signal.Signals(signal_value).name if 1 <= signal_value <= 64 else f"Signal {signal_value}"
            
            if signal_value == signal.SIGSEGV.value:
                 print(f"❌ Severe Error: File '{ts_file_path.name}' caused Segmentation Fault ({signal_name})!")
                 status_detail = "Segmentation Fault"
            else:
                 print(f"❌ Warning: File '{ts_file_path.name}' terminated by signal ({signal_name})!")
                 status_detail = f"Error (Signal: {signal_name})"
        else:
            # Other non-zero return codes
            print(f"❌ Warning: File '{ts_file_path.name}' exited abnormally (Exit Code {result.returncode})!")
            status_detail = f"Error (Exit Code {result.returncode})"

    except FileNotFoundError:
        print(f"Error: Executable '{abc_path}' not found. Please check the path configuration.")
        status_detail = "Error (Executable Not Found)"
        return_code = -1
    except Exception as e:
        print(f"An error occurred during execution of '{abc_path}': {e}")
        status_detail = f"Error (Execution Exception: {str(e)})"
        return_code = -2
    
    # Record the result regardless of success or failure
    records[file_id] = {
        "status": status_detail,
        "return_code": return_code
    }
    return True # Analysis attempt finished


def main():
    parser = argparse.ArgumentParser(description="Recursively analyze TS files using the abc tool.")
    
    # Input Directory Argument (Optional, with default)
    parser.add_argument("-i", "--input_dir", type=str, default=DEFAULT_INPUT_DIR,
                        help=f"Root directory to scan for TS files (default: '{DEFAULT_INPUT_DIR}')")
    
    # Abc Tool Path Argument (Optional, with default)
    parser.add_argument("-a", "--abc_path", type=str, default=DEFAULT_ABC_PATH,
                        help=f"Path to the abc executable (default: '{DEFAULT_ABC_PATH}')")
    
    # Record File Path Argument (Optional, with default)
    parser.add_argument("-d", "--record_file", type=str, default=DEFAULT_RECORD_FILE,
                        help=f"JSON file path for analyzed file records (default: '{DEFAULT_RECORD_FILE}')")

    # Temporary Copy Directory Argument (Optional, with default)
    parser.add_argument("-t", "--temp_dir", type=str, default=DEFAULT_LOCAL_COPY_DIR,
                        help=f"Directory for temporary file copies (default: '{DEFAULT_LOCAL_COPY_DIR}')")

    args = parser.parse_args()

    root_dir = Path(args.input_dir)
    if not root_dir.is_dir():
        print(f"Error: '{args.input_dir}' is not a valid directory.")
        sys.exit(1)

    print("-" * 30)
    print(f"Scan Directory:      {root_dir.resolve()}")
    print(f"ABC Tool Path:       {args.abc_path}")
    print(f"Record File Path:    {args.record_file}")
    print(f"Temp File Directory: {args.temp_dir}")
    print("-" * 30)

    # Load existing records (dictionary format)
    records = load_records(args.record_file)
    print(f"Loaded {len(records)} historical analysis records.")
    
    ts_files = list(root_dir.rglob('*.ts'))
    total_files = len(ts_files)
    new_files_analyzed_count = 0

    print(f"Found {total_files} .ts files in directory '{root_dir}'.")

    for i, ts_file in enumerate(ts_files):
        print(f"\n[{i+1}/{total_files}] ", end="")
        # Pass all configuration parameters to the analyze function
        if analyze_file(ts_file, records, args.abc_path, args.temp_dir):
            new_files_analyzed_count += 1
            
    # Save the updated dictionary of records
    save_records(records, args.record_file) 
    print(f"\nAnalysis complete. Total new files analyzed: {new_files_analyzed_count}. Records updated in {args.record_file}.")
    # Optional: Clean up the temporary directory here if desired shutil.rmtree(args.temp_dir)

if __name__ == "__main__":
    main()
