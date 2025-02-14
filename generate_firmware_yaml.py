import os
from itertools import product

def get_host_architectures():
    """Get host architectures from firmware/hosts directory"""
    hosts_dir = os.path.join('firmware', 'hosts')
    if not os.path.exists(hosts_dir):
        raise FileNotFoundError(f"Hosts directory not found: {hosts_dir}")
    
    hosts = []
    for file in os.listdir(hosts_dir):
        if file.endswith('.yaml'):
            # Remove .yaml extension
            host = os.path.splitext(file)[0]
            hosts.append(host)
    
    if not hosts:
        raise ValueError("No host configuration files found in firmware/hosts directory")
    
    return sorted(hosts)  # Sort for consistent order

def get_packages(host, api_variant, firmware, branch):
    # Define packages in exact order with their conditions
    is_esp32 = 'esp32' in host.lower()
    has_psram = host in ['esp32-s2', 'esp32-s3', 'esp32-s3-quad']
    has_status_led = is_esp32

    packages_config = [
        ('host', f'!include ../hosts/{host}.yaml', True),
        
        ('rgb_status_led', '!include ../components/rgb-status-led.yaml', has_status_led),
        ('rgb_status_led', '!include ../components/rgb-status-led.dummy.yaml', not has_status_led),
        
        ('base', '!include ../base.yaml', True),
        ('bluedroid_ble', '!include ../components/bluedroid-ble.yaml', is_esp32 and firmware != 'nuki-bridge'),
        
        ('ota_update', '!include ../components/ota-update.yaml', api_variant == 'ha'),
        ('ota_update_default_dev', '!include ../components/ota-update.dev.yaml', api_variant == 'ha' and branch == 'dev-rmt3'),
        ('dashboard_import', '!include ../components/dashboard-import.yaml', api_variant == 'ha'),
        
        ('api', '!include ../components/api.homeassistant.yaml', api_variant == 'ha'),
        ('api', '!include ../components/api.mqtt.yaml', api_variant == 'mqtt'),
        ('api', '!include ../components/api.custom.yaml', api_variant == 'custom'),

        ('debug_utilities', '!include ../components/debug-utilities.yaml', branch == 'dev-rmt3'),
        ('debug_utilities_arduino', '!include ../components/debug-utilities.arduino.yaml', not is_esp32),

        ('debug_component', '!include ../components/debug-component.yaml', branch == 'dev-rmt3'),
        ('debug_component_psram', '!include ../components/debug-component.psram.yaml', branch == 'dev-rmt3' and has_psram),

        ('pattern_events', '!include ../components/pattern-events.yaml', True),
        ('ring_to_open', '!include ../components/ring-to-open.yaml', True),
        ('intercom_settings', '!include ../components/intercom-settings.yaml', True),
        ('addon_nuki_bridge', '!include ../components/nuki-bridge.yaml', firmware == 'nuki-bridge'),
        ('interactive_setup', '!include ../components/interactive-setup.yaml', True),
        
        ('arduino_removals', '!include ../components/arduino.yaml', not is_esp32),
    ]
    
    return [(name, path) for name, path, condition in packages_config if condition]

def generate_yaml_content(host, api_variant, firmware, branch):
    content = [
        'substitutions:',
        f'  branch: "{branch}"',
        f'  firmware_type: "{firmware}"',
        f'  api_variant: "{api_variant}"',
        f'  host_platform: "{host}"',
        '',
        'packages:'
    ]
    
    packages = get_packages(host, api_variant, firmware, branch)
    for name, path in packages:
        content.append(f'  {name}: {path}')
    
    return '\n'.join(content)

def generate_example_yaml(host, api_variant, firmware, branch):

    filename = f'github://azoninc/doorman/firmware/configurations/{host}.{api_variant}.{firmware}.{branch}.yaml@{branch}'
    
    api_variant_desc = "Home Assistant" if api_variant == "ha" else "MQTT" if api_variant == "mqtt" else "Custom"

    content = [
        f'# Doorman {"Nuki Bridge" if firmware == "nuki-bridge" else "Stock"} Firmware ({api_variant_desc})',
        f'# Base Board {host.upper()}',
        '',
        '# You can change a few options here.',
        'substitutions:',
        '  name: "doorman-s3"',
        '  friendly_name: "Doorman S3"',
        '  # log_level: "ERROR"',
        '  # led_pin: "GPIO1"',
        '  # rgb_led_pin: "GPIO2"',
        '  # relay_pin: "GPIO42"',
        '  # external_button_pin: "GPIO41"',
        '  # adc_input_pin: "GPIO10"',
        '',
        '# Import Doorman Firmware Config',
        'packages:',
        f'  AzonInc.Doorman{"-Nuki-Bridge" if firmware == "nuki-bridge" else ""}: ' +
        f'{filename}',
        '',
        'wifi:',
        '  ssid: !secret wifi_ssid',
        '  password: !secret wifi_password'
    ]

    if api_variant == 'mqtt':
        content.extend([
            '',
            'mqtt:',
            '  broker: "10.10.0.2"',
            '  username: ""',
            '  password: ""'
        ])
    
    return '\n'.join(content)


# Configuration options
HOST_ARCHITECTURES = get_host_architectures()

API_VARIANTS = ['ha', 'mqtt', 'custom']
FIRMWARES = ['stock', 'nuki-bridge']
BRANCHES = ['master', 'dev-rmt3']


def main():
    os.makedirs('firmware/configurations', exist_ok=True)
    os.makedirs('firmware/examples', exist_ok=True)
    
    # Filter out invalid combinations before generating files
    combinations = [
        (host, api_variant, firmware, branch)
        for host, api_variant, firmware, branch in product(HOST_ARCHITECTURES, API_VARIANTS, FIRMWARES, BRANCHES)
        if not (firmware == 'nuki-bridge' and 'esp8266' in host.lower())
    ]

    for host, api_variant, firmware, branch in combinations:
        filename = f'{host}.{api_variant}.{firmware}.{branch}.yaml'
        example_filename = f'{host}.{api_variant}.{firmware}.{branch}.example.yaml'
        
        config_content = generate_yaml_content(host, api_variant, firmware, branch)
        example_content = generate_example_yaml(host, api_variant, firmware, branch)

        # Write config YAML
        config_filepath = os.path.join('firmware', 'configurations', filename)
        with open(config_filepath, 'w', encoding='utf-8') as f:
            f.write(config_content)
        
        # Write example YAML
        example_filepath = os.path.join('firmware', 'examples', example_filename)
        with open(example_filepath, 'w', encoding='utf-8') as f:
            f.write(example_content)
        
        print(f'Generated: {filename}')

if __name__ == '__main__':
    main()