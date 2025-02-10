import os
from itertools import product

# Configuration options
HOST_ARCHITECTURES = ['esp32-s3', 'esp32-s3-oct', 'esp32']

API_VARIANTS = ['ha', 'mqtt']
FIRMWARES = ['stock', 'nuki-bridge']
BRANCHES = ['master', 'dev']

def get_packages(host, api_variant, firmware, branch):
    # Define packages in exact order with their conditions
    packages_config = [
        ('host', f'!include ../hosts/{host}.yaml', True),
        ('base', '!include ../base.yaml', True),
        ('ota_updates', '!include ../components/ota-updates.yaml', api_variant == 'ha'),
        ('ota_updates_default_dev', '!include ../components/ota-updates-default-dev.yaml', api_variant == 'ha' and branch == 'dev'),
        ('dashboard_import', '!include ../components/dashboard-import.yaml', api_variant == 'ha'),
        ('homeassistant', '!include ../components/homeassistant.yaml', api_variant == 'ha'),
        ('mqtt', '!include ../components/mqtt.yaml', api_variant == 'mqtt'),
        ('debug_utilities', '!include ../components/debug-utilities.yaml', branch == 'dev'),
        ('pattern_events', '!include ../components/pattern-events.yaml', True),
        ('ring_to_open', '!include ../components/ring-to-open.yaml', True),
        ('intercom_settings', '!include ../components/intercom-settings.yaml', True),
        ('addon_nuki_bridge', '!include ../components/nuki-bridge.yaml', firmware == 'nuki-bridge'),
        ('interactive_setup', '!include ../components/interactive-setup.yaml', True),
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

    filename = (f'{host}.{api_variant}.{firmware}.{branch}.yaml' 
                   if branch == 'dev' 
                   else f'{host}.{api_variant}.{firmware}.yaml')
    
    content = [
        f'# Doorman {"Nuki Bridge" if firmware == "nuki-bridge" else "Stock"} Firmware ({"Home Assistant" if api_variant == "api" else "MQTT"})',
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
        '',
        '# Import Doorman Firmware Config',
        'packages:',
        f'  AzonInc.Doorman{"-Nuki-Bridge" if firmware == "nuki-bridge" else ""}: ' +
        f'github://azoninc/doorman/firmware/{filename}@{branch}',
        '',
        'wifi:',
        '  ssid: !secret wifi_ssid',
        '  password: !secret wifi_password'
    ]
    
    return '\n'.join(content)

def main():
    os.makedirs('firmware/configurations', exist_ok=True)
    os.makedirs('firmware/examples', exist_ok=True)
    
    combinations = product(HOST_ARCHITECTURES, API_VARIANTS, FIRMWARES, BRANCHES)
    for host, api_variant, firmware, branch in combinations:
        filename = (f'{host}.{api_variant}.{firmware}.{branch}.yaml' 
                   if branch == 'dev' 
                   else f'{host}.{api_variant}.{firmware}.yaml')
        
        example_filename = (f'{host}.{api_variant}.{firmware}.example.{branch}.yaml' 
                   if branch == 'dev' 
                   else f'{host}.{api_variant}.{firmware}.example.yaml')
        
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