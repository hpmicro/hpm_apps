#!/usr/bin/env python3
"""
make_packed_fs.py - 将前端构建产物打包为 packed_fs.c

用法:
  python tools/make_packed_fs.py web_app/dist web_server/src/packed_fs.c

功能:
  - 递归扫描输入目录中的所有文件
  - 用 gzip 压缩每个文件
  - 生成 C 字节数组和 packed_files[] 表
  - 输出到 packed_fs.c（可被 Mongoose 的 mg_fs_packed 使用）
"""
import sys
import os
import gzip
import time
from io import BytesIO
import argparse

def gz_compress_with_mtime(data, mtime=None):
    """使用 gzip 压缩数据，保留 mtime"""
    buf = BytesIO()
    mtime_val = int(mtime if mtime is not None else time.time())
    with gzip.GzipFile(fileobj=buf, mode='wb', mtime=mtime_val) as f:
        f.write(data)
    return buf.getvalue()

def to_c_array(name, data, wrap=12):
    """将字节数据转为 C 数组格式"""
    lines = [f"static const unsigned char {name}[] = {{"]
    for i in range(0, len(data), wrap):
        chunk = data[i:i+wrap]
        lines.append("  " + ", ".join(str(b) for b in chunk) + ",")
    lines.append("};")
    return "\n".join(lines)

def c_string_literal(s):
    """生成 C 字符串字面量，处理转义"""
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'

def main():
    parser = argparse.ArgumentParser(description='打包前端文件到 packed_fs.c')
    parser.add_argument('indir', help='输入目录（例如 web_app/dist）')
    parser.add_argument('outc', help='输出 C 文件（例如 web_server/src/packed_fs.c）')
    parser.add_argument('--prefix', default='/web_root', 
                        help='packed_files 中的路径前缀（默认 /web_root）')
    args = parser.parse_args()

    indir = os.path.abspath(args.indir)
    outc = os.path.abspath(args.outc)
    prefix = args.prefix.rstrip('/')

    if not os.path.isdir(indir):
        print(f"错误：输入目录不存在: {indir}")
        sys.exit(1)

    # 收集所有文件
    files = []
    for root, dirs, filenames in os.walk(indir):
        dirs.sort()
        filenames.sort()
        for fn in filenames:
            full = os.path.join(root, fn)
            rel = os.path.relpath(full, indir).replace(os.path.sep, '/')
            files.append((rel, full))

    files.sort()
    
    if not files:
        print(f"警告：输入目录为空: {indir}")
        sys.exit(1)

    print(f"找到 {len(files)} 个文件，开始打包...")

    arrays = []
    table_entries = []

    for idx, (rel, full) in enumerate(files, start=1):
        with open(full, 'rb') as f:
            raw = f.read()
        
        # gzip 压缩
        gz = gz_compress_with_mtime(raw, mtime=int(os.path.getmtime(full)))
        # 添加尾部 0 字节（匹配仓库约定）
        gz_with_null = gz + b'\x00'
        
        varname = f"v{idx}"
        arrays.append((varname, gz_with_null))
        
        # 目标路径：prefix + "/" + rel + ".gz"
        target_name = prefix + '/' + rel
        if not target_name.endswith('.gz'):
            target_name = target_name + '.gz'
        
        table_entries.append((target_name, varname, len(gz_with_null), int(os.path.getmtime(full))))
        print(f"  [{idx}/{len(files)}] {rel} -> {target_name} ({len(raw)} -> {len(gz)} 字节)")

    # 生成 C 文件
    print(f"写入 {outc} ...")
    with open(outc, 'w', newline='\n', encoding='utf-8') as f:
        # 头部
        f.write('#include <stddef.h>\n')
        f.write('#include <string.h>\n')
        f.write('#include <time.h>\n')
        f.write('#include <stdio.h>\n\n')
        f.write('#if defined(__cplusplus)\nextern "C" {\n#endif\n')
        f.write('const char *mg_unlist(size_t no);\n')
        f.write('const char *mg_unpack(const char *, size_t *, time_t *);\n')
        f.write('#if defined(__cplusplus)\n}\n#endif\n\n')
        
        # 字节数组
        for name, data in arrays:
            f.write(to_c_array(name, data))
            f.write("\n\n")
        
        # packed_files 表
        f.write("static const struct packed_file {\n")
        f.write("  const char *name;\n")
        f.write("  const unsigned char *data;\n")
        f.write("  size_t size;\n")
        f.write("  time_t mtime;\n")
        f.write("} packed_files[] = {\n")
        for name, varname, size, mtime in table_entries:
            f.write(f'  {{{c_string_literal(name)}, {varname}, sizeof({varname}), {mtime}}},\n')
        f.write("  {NULL, NULL, 0, 0}\n")
        f.write("};\n\n")
        
        # 辅助函数
        f.write("static int scmp(const char *a, const char *b) {\n")
        f.write("  while (*a && (*a == *b)) a++, b++;\n")
        f.write("  return *(const unsigned char *) a - *(const unsigned char *) b;\n")
        f.write("}\n")
        f.write("const char *mg_unlist(size_t no) {\n")
        f.write("  return packed_files[no].name;\n")
        f.write("}\n")
        f.write("const char *mg_unpack(const char *name, size_t *size, time_t *mtime) {\n")
        f.write("  const struct packed_file *p;\n")
        f.write("  for (p = packed_files; p->name != NULL; p++) {\n")
        f.write("    if (scmp(p->name, name) != 0) continue;\n")
        f.write("    if (size != NULL) *size = p->size - 1;\n")
        f.write("    if (mtime != NULL) *mtime = p->mtime;\n")
        f.write("    return (const char *) p->data;\n")
        f.write("  }\n")
        f.write("  return NULL;\n")
        f.write("}\n")

    print(f"✓ 成功生成 {outc}")
    print(f"  - {len(arrays)} 个文件")
    print(f"  - 总大小: {sum(len(data) for _, data in arrays)} 字节")

if __name__ == '__main__':
    main()
