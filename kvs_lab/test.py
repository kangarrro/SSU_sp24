import struct

def parse_snapshot(file_path, max_level=16):
    with open(file_path, "rb") as f:
        # 1. Header 읽기
        header_id = struct.unpack("Q", f.read(8))[0]  # 8바이트 (포인터/ID)
        level = struct.unpack("i", f.read(4))[0]      # 4바이트 (int)
        items = struct.unpack("i", f.read(4))[0]      # 4바이트 (int)
        print(f"Header ID: {header_id}, Level: {level}, Items: {items}")

        # 2. .node 섹션 읽기
        nodes = []
        for _ in range(items):
            node_id = struct.unpack("I", f.read(4))[0]           # ID (4바이트)
            key_size = struct.unpack("I", f.read(4))[0]          # Key 크기 (4바이트)
            value_size = struct.unpack("I", f.read(4))[0]        # Value 크기 (4바이트)
            key_offset = struct.unpack("Q", f.read(8))[0]        # Key Offset (더미)
            value_offset = struct.unpack("Q", f.read(8))[0]      # Value Offset (더미)
            forward_ids = [
                struct.unpack("Q", f.read(8))[0] for _ in range(max_level)
            ]  # Forward ID 배열 (8바이트)

            nodes.append({
                "id": node_id,
                "key_size": key_size,
                "value_size": value_size,
                "forward_ids": forward_ids
            })

        # 3. .data 섹션 읽기
        for node in nodes:
            key = f.read(node["key_size"]).decode("utf-8")
            value = f.read(node["value_size"]).decode("utf-8")
            node["key"] = key
            node["value"] = value

        # 출력
        for node in nodes:
            print(f"Node ID: {node['id']}")
            print(f"  Key: {node['key']} (Size: {node['key_size']})")
            print(f"  Value: {node['value']} (Size: {node['value_size']})")
            print(f"  Forward IDs: {node['forward_ids']}")

# 바이너리 파일 읽기
parse_snapshot("kvs.img")
