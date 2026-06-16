import re

# Steuer-Schlüssel, die wir ignorieren wollen
IGNORED_KEYS = {
    'if', 'then', 'else', 'while', 'repeat', 'lambda',
    'delay', 'wait_until', 'script', 'for', 'on_press',
    'on_release', 'on_click', 'on_double_click'
}

def extract_entries(yaml_path):
    with open(yaml_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    results = []
    current_section = None
    current_platform = None
    last_description = None
    last_virtual_platform = None
    last_options = None
    in_platform_block = False
    indent_stack = []

    for i, line in enumerate(lines):
        # Neue Sektion (z. B. "lock:")
        section_match = re.match(r'^(\w+):\s*$', line)
        if section_match:
            current_section = section_match.group(1)
            continue

        # Kommentare lesen
        desc_match = re.match(r'^\s*#\s*@description:\s*(.+)', line, re.IGNORECASE)
        if desc_match:
            last_description = desc_match.group(1).strip()
            continue

        virt_platform_match = re.match(r'^\s*#\s*@platform:\s*(\w+)', line, re.IGNORECASE)
        if virt_platform_match:
            last_virtual_platform = virt_platform_match.group(1).strip()
            continue

        options_match = re.match(r'^\s*#\s*@options:\s*(.+)', line, re.IGNORECASE)
        if options_match:
            # Split by comma and strip whitespace
            last_options = [opt.strip() for opt in options_match.group(1).split(',')]
            continue

        # - platform: xyz
        platform_match = re.match(r'^(\s*)-\s*platform:\s*(\w+)', line)
        if platform_match:
            indent = len(platform_match.group(1))
            current_platform = platform_match.group(2)
            in_platform_block = True
            indent_stack = [indent]
            continue

        # Subkomponente erkennen (z. B. is_connected:)
        sub_match = re.match(r'^(\s*)(\w+):\s*$', line)
        if in_platform_block and sub_match:
            indent = len(sub_match.group(1))
            key = sub_match.group(2)

            # Ignoriere typische Kontrollblöcke
            if key in IGNORED_KEYS:
                continue

            if indent_stack and indent > indent_stack[-1]:
                name = None
                for j in range(i + 1, len(lines)):
                    name_match = re.match(r'^\s*name:\s*["\']?(.*?)["\']?\s*$', lines[j])
                    if name_match:
                        name = name_match.group(1)
                        break
                    if re.match(r'^\s*\w+:\s*$', lines[j]):
                        break

                if name:
                    results.append({
                        'section': current_section,
                        'platform': last_virtual_platform or f"{current_platform} → {key}",
                        'name': name,
                        'description': last_description,
                        'options': last_options
                    })
                    last_description = None
                    last_virtual_platform = None
                    last_options = None
            continue

        # Top-level name im aktuellen platform-Block
        name_match = re.match(r'^\s*name:\s*["\']?(.*?)["\']?\s*$', line)
        if name_match and current_platform:
            name = name_match.group(1)
            results.append({
                'section': current_section,
                'platform': current_platform,
                'name': name,
                'description': last_description,
                'options': last_options
            })
            last_description = None
            last_virtual_platform = None
            last_options = None

    return results


# Hauptprogramm
if __name__ == '__main__':
    import sys

    yaml_path = sys.argv[1] if len(sys.argv) > 1 else 'test.yaml'

    try:
        entries = extract_entries(yaml_path)
    except FileNotFoundError:
        print(f"❌ Datei nicht gefunden: {yaml_path}")
        sys.exit(1)

    print(f"📄 Ergebnisse aus: {yaml_path}\n")
    for entry in entries:
        print(f"Section     : {entry['section']}")
        print(f"Platform    : {entry['platform']}")
        print(f"Name        : {entry['name']}")
        print(f"Description : {entry['description']}")
        print(f"Options     : {', '.join(entry['options']) if entry['options'] else '—'}")
        print("---")
