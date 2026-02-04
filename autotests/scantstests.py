import argparse
import os
import shutil
import subprocess
import json
import sys
import signal
import copy
import ipdb
from pathlib import Path
from itertools import chain

env_vars = copy.deepcopy(os.environ)
env_vars['LD_LIBRARY_PATH'] = '../out/arkcompiler/runtime_core:../out/thirdparty/zlib'

def load_skip_list(skip_file_path):
    if not skip_file_path or not os.path.exists(skip_file_path):
        return set()
    try:
        with open(skip_file_path, 'r', encoding='utf-8') as f:
            return {line.strip() for line in f if line.strip()}
    except Exception as e:
        print(f"Warning: Could not read skip file {skip_file_path}: {e}")
        return set()

def execute_cmd(cmd):
    status_detail = ""
    return_code = None

    try:
        result = subprocess.run(
            cmd,
            check=False,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
            start_new_session=True,
            timeout=10,
            env=env_vars,
            #shell=True
        )

        return_code = result.returncode

        if result.returncode == 0:
            print(f"✅ Success (Exit Code 0)")
            status_detail = "Success"
        elif result.returncode == 1:
            print(f"❌ Warning: failure (std::exit(EXIT_FAILURE), Exit Code 1)!")
            status_detail = "Error (EXIT_FAILURE)"
        elif result.returncode < 0:
            # Negative return codes indicate termination by signal on Linux
            signal_value = abs(result.returncode)
            signal_name = signal.Signals(signal_value).name if 1 <= signal_value <= 64 else f"Signal {signal_value}"
            
            if signal_value == signal.SIGSEGV.value:
                 print(f"❌ Severe Error: Segmentation Fault ({signal_name})!")
                 status_detail = "Segmentation Fault"
            else:
                 print(f"❌ Warning: terminated by signal ({signal_name})!")
                 status_detail = f"Error (Signal: {signal_name})"
        else:
            # Other non-zero return codes
            print(f"❌ Warning: exited abnormally (Exit Code {result.returncode})!")
            status_detail = f"Error (Exit Code {result.returncode})"

    except subprocess.TimeoutExpired as e:
        print(f"❌ Timeout Error: Process timed out.")
        status_detail = "Timeout Expired"
        return_code = -1
    except FileNotFoundError:
        print(f"Error: Executable not found. Please check the path configuration.")
        status_detail = "Error (Executable Not Found)"
        return_code = -2
    except Exception as e:
        print(f"An error occurred during execution : {e}")
        status_detail = f"Error (Execution Exception: {str(e)})"
        return_code = -3
    finally:
        os.system("pkill -f -9 xabc")
    
    res = {
        "status": status_detail,
        "return_code": return_code
    }
    return res

def load_results(results_path):
    if os.path.exists(results_path):
        try:
            with open(results_path, 'r', encoding='utf-8') as f:
                results_list = json.load(f)
                analyzed_paths = {item.get("file") for item in results_list if item.get("file")}
                return results_list, analyzed_paths
        except json.JSONDecodeError:
            print(f"Warning: Could not parse {results_path}, creating new results.")
    return [], set()

def analysis_onefile(analysis_file):
    # Status Codes Explanation:
    # ret=1: File copy failed. Precondition error, subsequent flow not started.
    # ret=2: Compilation failed. Both attempts (with/without --module) failed to generate a valid .abc file.
    # ret=3: Decompilation failed. At least one compilation succeeded (produced .abc), but all generated .abc files failed the decompilation check.
    # ret=0: Success. Found a mode (module or script) that passed the full link from compilation to decompilation.

    # --- Summary of All Cases ---
    # | First Compile Result (--module) | First Decompile Result | Second Compile Result (No Args) | Second Decompile Result | ret | Status Explanation            |
    # |---------------------------------|------------------------|---------------------------------|-------------------------|-----|-------------------------------|
    # | Fail                            | N/A                    | Fail                            | N/A                     | 2   | Both Compiles Failed          |
    # | Fail                            | N/A                    | Success                         | Fail                    | 3   | Compile OK, Decompile Fail    |
    # | Fail                            | N/A                    | Success                         | Success                 | 0   | Script Mode Success           |
    # | Success                         | Fail                   | Fail                            | N/A                     | 3   | Compile OK, Decompile Fail    |
    # | Success                         | Fail                   | Success                         | Fail                    | 3   | Compile OK, Decompile Fail    |
    # | Success                         | Fail                   | Success                         | Success                 | 0   | Script Mode Success           |
    # | Success                         | Success                | N/A                             | N/A                     | 0   | Module Mode Success (Early Exit)|
    # | N/A                             | N/A                    | N/A                             | N/A                     | 1   | Copy Failed (Precondition Error)|

    es2abc_path = "../../out/x64.release/arkcompiler/ets_frontend/es2abc"
    decompile_exe = "../out/arkcompiler/common/xabc"
    res = {"file": analysis_file}

    if execute_cmd(["cp", analysis_file, "demo.ts"])["return_code"] != 0:
        res["status"] = 1
        return res

    plans = [
        [es2abc_path, "--module", "--dump-assembly", "demo.ts", "--output", "demo.abc"],
        [es2abc_path, "--dump-assembly", "demo.ts", "--output", "demo.abc"]
    ]

    has_any_compile_success = False

    for cmd in plans:
        if os.path.exists("demo.abc"):
            os.remove("demo.abc")

        compile_res = execute_cmd(cmd)
        if compile_res["return_code"] == 0:
            has_any_compile_success = True

            decompile_res = execute_cmd([decompile_exe])
            if decompile_res["return_code"] == 0:
                res["status"] = 0
                return res
        else:
            continue

    if has_any_compile_success:
        res["status"] = 3
    else:
        res["status"] = 2

    return res

def save_results(results_list, output_dir):
    results_path = Path(output_dir) / "res.json"
    with open(results_path, 'w', encoding='utf-8') as f:
        json.dump(results_list, f, ensure_ascii=False, indent=4)

    grouped_results = {}
    for result_entry in results_list:
        status_code = result_entry.get("status")
        if status_code is not None:
            if status_code not in grouped_results:
                grouped_results[status_code] = []
            grouped_results[status_code].append(result_entry)
        else:
            # Handle entries that somehow lack a status code
            if "unknown_status" not in grouped_results:
                grouped_results["unknown_status"] = []
            grouped_results["unknown_status"].append(result_entry)

    print(f"Distributing results into files based on status code:")
    for status_code, records in grouped_results.items():
        # Ensure the filename is valid (e.g., status 0 -> 0.json, status -11 -> negative_11.json)
        filename = f"{status_code}.json"
        if isinstance(status_code, int) and status_code < 0:
             filename = f"negative_{abs(status_code)}.json"

        filepath = Path(output_dir) / filename
        
        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(records, f, ensure_ascii=False, indent=4)
            
        print(f"  - Status {status_code}: Saved {len(records)} records to {filepath}")

def analysis_files(root_dir, output_dir, skip_list):
    #ts_files = list(chain(root_dir.rglob('*.js'), root_dir.rglob('*.ts')))
    ts_files = list(root_dir.rglob('*.ts'))
    total_files = len(ts_files)
    results_path = Path(output_dir) / "res.json"

    results_list, analyzed_paths = load_results(str(results_path.resolve()))
    print(f"Loaded {len(analyzed_paths)} historical analysis records.")

    for i, ts_file in enumerate(ts_files):
        file_str = str(ts_file)
        if file_str in analyzed_paths:
            continue

        if file_str in skip_list:
            print(f"Skipping (listed in skip file): {file_str}")
            continue

        print(f"\n[{i+1}/{len(ts_files)}] Analyzing: {file_str}")
        res = analysis_onefile(file_str)
        results_list.append(res)

        if (i + 1) % 1000 == 0:
            save_results(results_list, output_dir)

    save_results(results_list, output_dir)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Recursively analyze TS files using the abc tool.")
    parser.add_argument("-i", "--input_dir", type=str, default="../autotests/third_party_typescript/tests",
                        help=f"Root directory to scan for TS files (default: '../autotests/third_party_typescript/tests')")
    
    parser.add_argument("-o", "--output_dir", type=str, default="status_outputs",
                        help=f"Directory to save status-specific JSON files (default: 'status_outputs')")
    
    parser.add_argument("-s", "--skip-file", type=str, default="blacklist.txt",
                        help="Path to a file containing a list of files to skip (one per line)")

    parser.add_argument("-d", "--record_file", type=str, default="res.json",
                        help=f"JSON file path for analyzed file records (default: res.json)")

    args = parser.parse_args()
    root_dir = Path(args.input_dir)
    if not root_dir.is_dir():
        print(f"Error: '{args.input_dir}' is not a valid directory.")
        sys.exit(1)

    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)
        print(f"\nCreated output directory: {args.output_dir}")

    skip_list = load_skip_list(args.skip_file)
    analysis_files(root_dir, args.output_dir, skip_list)

    #test_file = "../autotests/third_party_typescript/tests/cases/conformance/emitter/es2015/asyncGenerators/emitter.asyncGenerators.classMethods.es2015.ts"
    #analysis_onefile(test_file)


    
