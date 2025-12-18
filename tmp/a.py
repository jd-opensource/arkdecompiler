import json
from deepdiff import DeepDiff

# 读取JSON文件
def read_json_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        return json.load(file)

# 比较两个JSON对象
def compare_json(json1, json2):
    diff = DeepDiff(json1, json2, ignore_order=True)
    return diff

# 文件路径
file1_path = 'a.json'
file2_path = 'c.json'

# 读取文件
json1 = read_json_file(file1_path)
json2 = read_json_file(file2_path)

# 获取差异
differences = compare_json(json1, json2)

# 打印差异
print("Differences between the two JSON files:")
print(differences)
