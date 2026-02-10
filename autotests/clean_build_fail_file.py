import json
import os
from pathlib import Path

def clean_files_from_json(json_file_path, root_search_dir="."):
    if not os.path.exists(json_file_path):
        print(f"Error: JSON file not found at {json_file_path}")
        return

    with open(json_file_path, 'r', encoding='utf-8') as f:
        try:
            data = json.load(f)
        except json.JSONDecodeError:
            print(f"Error: {json_file_path} is not a valid JSON file")
            return

    print(f"Successfully loaded {len(data)} records, starting cleanup...")

    success_count = 0
    fail_count = 0
    skip_count = 0

    for item in data:
        relative_path = item.get("file")
        if not relative_path:
            continue
        
        target_file = Path(root_search_dir) / relative_path

        if target_file.exists() and target_file.is_file():
            try:
                target_file.unlink()  # Execute deletion
                print(f"[Deleted] {relative_path}")
                success_count += 1
            except Exception as e:
                print(f"[Failed] {relative_path} : {e}")
                fail_count += 1
        else:
            # print(f"[Skipped] File does not exist: {relative_path}")
            skip_count += 1

    print("\n" + "="*30)
    print(f"Cleanup Summary:")
    print(f" - Successfully deleted: {success_count}")
    print(f" - Failed to delete: {fail_count}")
    print(f" - Files already missing: {skip_count}")
    print("="*30)

if __name__ == "__main__":
    clean_files_from_json("status_outputs/2.json")
