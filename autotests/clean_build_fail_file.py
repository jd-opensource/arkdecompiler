import json
import os
from pathlib import Path

def clean_files_from_json(json_file_path, root_search_dir="."):
    if not os.path.exists(json_file_path):
        print(f"错误: 找不到 JSON 文件 {json_file_path}")
        return

    with open(json_file_path, 'r', encoding='utf-8') as f:
        try:
            data = json.load(f)
        except json.JSONDecodeError:
            print(f"错误: {json_file_path} 格式不正确，无法解析")
            return

    print(f"成功加载 {len(data)} 条记录，开始清理...")

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
                target_file.unlink()  # 执行删除
                print(f"[已删除] {relative_path}")
                success_count += 1
            except Exception as e:
                print(f"[删除失败] {relative_path} : {e}")
                fail_count += 1
        else:
            # print(f"[跳过] 文件不存在: {relative_path}")
            skip_count += 1

    print("\n" + "="*30)
    print(f"清理完成统计:")
    print(f" - 成功删除: {success_count}")
    print(f" - 删除失败: {fail_count}")
    print(f" - 文件原本就不存在: {skip_count}")
    print("="*30)

if __name__ == "__main__":
    clean_files_from_json("status_outputs/2.json")

