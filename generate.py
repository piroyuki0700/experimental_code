import re

def generate_c_code(input_file, output_file):
    with open(input_file, 'r') as f:
        lines = f.readlines()

    # メンバーリストを解析
    members = [line.strip() for line in lines]

    # enum struct_index を生成
    enum_members = {}
    for member in members:
        enum_name = member
        value = 0;
        if '.' in member:
            enum_name = member.split('.')[0]
            value = 1;
        if enum_name not in enum_members:
            enum_members[enum_name] = value

    enum_code = "enum pointer_index {\n"
    for member in enum_members:
        enum_code += f"  pointer_index_{member},\n"
    enum_code += "  pointer_index_max\n};\n\n"

    # vinput_update_record を生成
    record_code = "struct vinput_update_record update_record[] = {\n"
    for member in members:
        if '.' in member:
            struct_name = member.split('.')[0]
            enum_name = struct_name
        else:
            enum_name = member

        record_code += f'  {{"{member}", (void *)&{member}, sizeof({member}), {{0}}, false, pointer_index_{enum_name}}},\n'
    record_code += "};\n\n"

    # vinput_replace_record を生成
    struct_record_code = "struct vinput_replace_record replace_record[pointer_index_max] = {\n"
    for member, need_size in enum_members.items():
        size = "0"
        if need_size == 1:
            size = f'sizeof({member})'
        struct_record_code += f'  {{&{member}, (void **)&updated_input.{member}, {size}, false}},\n'
    struct_record_code += "};\n"

    # 全体のコードを結合
    c_code = f"{enum_code}\n{record_code}{struct_record_code}"

    with open(output_file, 'w') as f:
        f.write(c_code)

if __name__ == "__main__":
    generate_c_code("python_input.txt", "python_output.c")
