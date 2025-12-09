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

env_vars = copy.deepcopy(os.environ)
env_vars['LD_LIBRARY_PATH'] = '../out/arkcompiler/runtime_core:../out/thirdparty/zlib'

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

def save_results(results_list, results_path):
    with open(results_path, 'w', encoding='utf-8') as f:
        json.dump(results_list, f, ensure_ascii=False, indent=4)

def analysis_onefile(analysis_file):
    #cpcmd = f"cp {analysis_file} demo.ts"
    #compilecmd = "../../out/x64.release/arkcompiler/ets_frontend/es2abc --module --dump-assembly demo.ts --output demo.abc"
    #decompilecmd = "LD_LIBRARY_PATH=../out/arkcompiler/runtime_core:../out/thirdparty/zlib ../out/arkcompiler/common/xabc"

    cpcmd = ["cp", analysis_file, "demo.ts"]
    compilecmd = ["../../out/x64.release/arkcompiler/ets_frontend/es2abc", "--module", "--dump-assembly", "demo.ts", "--output", "demo.abc"]
    decompilecmd = ["../out/arkcompiler/common/xabc"]

    res = {}

    res["file"] = analysis_file
    
    cp_res = execute_cmd(cpcmd)
    if cp_res["return_code"] != 0:
        res["status"] = -1
        return res
    
    compile_res = execute_cmd(compilecmd)
    if compile_res["return_code"] != 0:
        res["status"] = -2
        return res
    
    decompile_res = execute_cmd(decompilecmd)
    if decompile_res["return_code"] != 0:
        res["status"] = -3
        return res
    
    res["status"] = 0

    return res

def analysis_files(root_dir, results_file):
    ts_files = list(root_dir.rglob('*.ts'))
    total_files = len(ts_files)
    
    results_list, analyzed_paths = load_results(results_file)
    print(f"Loaded {len(analyzed_paths)} historical analysis records.")

    for i, ts_file in enumerate(ts_files):
        if ts_file not in analyzed_paths:
            print(f"\n[{i+1}/{total_files}] ", end="")
            print(f"analysis: {ts_file}")
            res = analysis_onefile(str(ts_file.resolve()))
            print(res)
            results_list.append(res)
        else:
            analyzed_paths.add(ts_file)

    save_results(results_list, results_file) 

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Recursively analyze TS files using the abc tool.")
    parser.add_argument("-i", "--input_dir", type=str, default="../autotests/third_party_typescript/tests",
                        help=f"Root directory to scan for TS files (default: '../autotests/third_party_typescript/tests')")
    
    parser.add_argument("-d", "--record_file", type=str, default="res.json",
                        help=f"JSON file path for analyzed file records (default: res.json)")
    args = parser.parse_args()
    root_dir = Path(args.input_dir)
    if not root_dir.is_dir():
        print(f"Error: '{args.input_dir}' is not a valid directory.")
        sys.exit(1)

    analysis_files(root_dir, args.record_file)

    #test_file = "../autotests/third_party_typescript/tests/cases/conformance/emitter/es2015/asyncGenerators/emitter.asyncGenerators.classMethods.es2015.ts"
    #analysis_onefile(test_file)


    