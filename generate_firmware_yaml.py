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

def get_packages(host, api_variant, firmware, branch, factory=False):
    # Define packages in exact order with their conditions
    is_esp32 = 'esp32' in host.lower()
    has_psram = host in [
        'esp32-s2',
        'esp32-s3',
        'esp32-s3-quad',
        'doorman-s3',
        'doorman-s3-rev2',
        'doorman-s3-rev2-audio'
    ]

    packages_config = [
        ('host', f'packages/host/{host}.yaml', True),
        
        ('external_components', 'packages/common/external-components.yaml', branch != 'local'),
        ('external_components', 'packages/common/external-components.local.yaml', branch == 'local'),

        # RGB Status LED
        # Doorman S3 rev 2.x.x SK6812B
        ('rgb_status_led', 'packages/common/rgb-status-led.sk6812b.yaml', host == 'doorman-s3-rev2'),
        # Doorman S3 rev 1.x.x / ESP32-S3 / ESP32-S3 WS2812B
        ('rgb_status_led', 'packages/common/rgb-status-led.ws2812b.yaml', host != 'doorman-s3-rev2'),
        ('rgb_status_led_effects', 'packages/common/rgb-status-led.effects.yaml', True),

        ('base', 'packages/common/base.yaml', True),

        ('webserver_dev', 'packages/common/web-server.dev.yaml', branch == 'dev' or branch == 'local'),
        
        # Doorman S3 rev 2.x.x extension boards
        ('audio', 'packages/audio/es8311.yaml', host == 'doorman-s3-rev2-audio'),

        # Improv BLE (not compatible with Nuki Bridge firmware)
        ('bluedroid_ble', 'packages/common/bluedroid-ble.yaml', is_esp32 and firmware != 'nuki-bridge'),
        
        # OTA
        ('ota_update_esphome', 'packages/ota/esphome.yaml', True),
        # HTTP OTA for factory config
        ('ota_update_http', 'packages/ota/http.yaml', factory),
        
        # API integration
        ('api', 'packages/api/homeassistant.yaml', api_variant == 'ha'),
        ('api', 'packages/api/mqtt.yaml', api_variant == 'mqtt'),
        ('api', 'packages/api/homekit.yaml', api_variant == 'homekit'),
        ('api', 'packages/api/custom.yaml', api_variant == 'custom'),

        # Debug utilities
        ('debug_utilities', 'packages/debug/debug-utilities.yaml', branch == 'dev' or branch == 'local'),
        ('debug_component', 'packages/debug/debug-component.yaml', branch == 'dev' or branch == 'local'),
        ('debug_component_psram', 'packages/debug/debug-component.psram.yaml', (branch == 'dev' or branch == 'local') and has_psram),
        
        # Experimental Features
        # Configo component for serial interface
        # ('configo', 'packages/experimental/configo.yaml', branch == 'local'),
        # Virtual devices
        ('virtual_devices', 'packages/bus_devices/virtual-devices.yaml', branch == 'local'),

        ('pattern_events', 'packages/pattern_events/pattern-events.yaml', True),
        ('ring_to_open', 'packages/ring_to_open/ring-to-open.yaml', True),
        ('ring_to_open_homekit', 'packages/ring_to_open/ring-to-open.homekit.yaml', api_variant == 'homekit'),
        ('ring_to_open_nuki', 'packages/ring_to_open/ring-to-open-nuki.yaml', firmware == 'nuki-bridge'),
        ('indoor_station_settings', 'packages/bus_devices/indoor-station-settings.yaml', True),
        ('addon_nuki_bridge', 'packages/nuki/nuki-bridge.yaml', firmware == 'nuki-bridge'),
        ('interactive_setup', 'packages/bus_devices/interactive-setup.yaml', True),

        # Add outdoor station for local tests
        ('outdoor_station', 'packages/bus_devices/outdoor-station.yaml', branch == 'local'),
    ]
    
    return [(name, path) for name, path, condition in packages_config if condition]

def generate_yaml_content(host, api_variant, firmware, branch, factory=False):
    
    # TODO: Extract version from central file
    version = "2026.3.0"

    if api_variant == "ha":
        api_variant_desc = "Home Assistant"
    elif api_variant == "mqtt":
        api_variant_desc = "MQTT"
    elif api_variant == "homekit":
        api_variant_desc = "HomeKit"
    else:
        api_variant_desc = "Custom"
    
    if branch == "local":
        device_name = "doorman-s3-local-dev"
        device_friendly_name = "Doorman S3 (Development)"
    else:
        device_name = "doorman-s3"
        device_friendly_name = "Doorman S3"

    content = [
        f'# Doorman {"Nuki Bridge" if firmware == "nuki-bridge" else "Standard"} Firmware ({api_variant_desc})',
        f'# Base Board {host.upper()}',
        '',
        '# You can change a few options here.',
        'substitutions:',
        f'  name: "{device_name}"',
        f'  friendly_name: "{device_friendly_name}"',
        f'  branch: "{branch}"',
        f'  firmware_type: "{firmware}"',
        f'  api_variant: "{api_variant}"',
        f'  host_platform: "{host}"',
        '',
        'esphome:',
        '  name: "${name}"',
        '  friendly_name: "${friendly_name}"',
        '  name_add_mac_suffix: false',
        '  project:',
        f'    name: AzonInc.Doorman {"Nuki-Bridge" if firmware == "nuki-bridge" else "Standard"}',
        f'    version: {version}',
        '',
        'wifi:',
        '  id: wifi_id',
        '  ap:',
        '    ssid: "${friendly_name} Setup"',
        '    password: "Op3n-Sesame!"',
        '',
        'captive_portal:',
        '',
        'improv_serial:',
        '',
    ]

    if factory and api_variant == "ha":
        content += [
            'dashboard_import:',
            f'  package_import_url: github://azoninc/doorman/firmware/configurations/{host}.{api_variant}.{firmware}.{branch}.yaml@{branch}',
            '  import_full_config: true',
            '',
        ]


    packages = get_packages(host, api_variant, firmware, branch, factory)
    
    content += [
        'packages:',
    ]
    
    if branch == 'local':
        for name, path in packages:
            content.append(f'  {name}: !include ../{path}')
    else:
        content += [
            '  remote_package_files:',
            '    url: https://github.com/azoninc/doorman',
            f'    ref: {branch}',
            '    refresh: 0s',
            '    files:',
        ]
        for name, path in packages:
            content.append(f'      - path: {path}')
    
    return '\n'.join(content)

def generate_example_yaml(host, api_variant, firmware, branch):

    if branch == "local":
        filename = f'!include ../configurations/{host}.{api_variant}.{firmware}.{branch}.yaml'
    else:
        filename = f'github://azoninc/doorman/firmware/configurations/{host}.{api_variant}.{firmware}.{branch}.yaml@{branch}'
    
    if api_variant == "ha":
        api_variant_desc = "Home Assistant"
    elif api_variant == "mqtt":
        api_variant_desc = "MQTT"
    elif api_variant == "homekit":
        api_variant_desc = "HomeKit"
    else:
        api_variant_desc = "Custom"
    
    if branch == "local":
        device_name = "doorman-s3-local-dev"
        device_friendly_name = "Doorman S3 (Development)"
    else:
        device_name = "doorman-s3"
        device_friendly_name = "Doorman S3"

    content = [
        f'# Doorman {"Nuki Bridge" if firmware == "nuki-bridge" else "Standard"} Firmware ({api_variant_desc})',
        f'# Base Board {host.upper()}',
        '',
        '# You can change a few options here.',
        'substitutions:',
        f'  name: "{device_name}"',
        f'  friendly_name: "{device_friendly_name}"',
        '  # led_pin: "GPIO1"',
        '  # rgb_led_pin: "GPIO2"',
        '  # relay_pin: "GPIO42"',
        '  # external_button_pin: "GPIO41"',
        '  # adc_input_pin: "GPIO10"',
        '',
        '# Import Doorman Firmware Config',
        'packages:',
        f'  AzonInc.Doorman-{"Nuki-Bridge" if firmware == "nuki-bridge" else "Standard"}: ' +
        f'{filename}',
        '',
        'wifi:',
        '  ssid: !secret wifi_ssid',
        '  password: !secret wifi_password'
    ]

    #if api_variant == 'mqtt':
    #    content.extend([
    #        '',
    #        'mqtt:',
    #        '  broker: "10.10.0.2"',
    #        '  username: ""',
    #        '  password: ""'
    #    ])
    
    return '\n'.join(content)


# Configuration options
HOST_ARCHITECTURES = get_host_architectures()

API_VARIANTS = ['ha', 'mqtt', 'homekit', 'custom']
FIRMWARES = ['standard', 'nuki-bridge']
BRANCHES = ['master', 'dev', 'local']
FACTORY_BUILDS = [False, True]

def main():
    os.makedirs('firmware/configurations', exist_ok=True)
    os.makedirs('firmware/examples', exist_ok=True)
    
    # Filter out invalid combinations before generating files
    combinations = [
        (host, api_variant, firmware, branch, factory)
        for host, api_variant, firmware, branch, factory in product(HOST_ARCHITECTURES, API_VARIANTS, FIRMWARES, BRANCHES, FACTORY_BUILDS)
        if not (firmware == 'nuki-bridge' and 'mqtt' in api_variant)
        and not (factory and branch == 'local')  # CI builds make no sense for local
    ]

    for host, api_variant, firmware, branch, factory in combinations:
        factory_suffix = '.factory' if factory else ''
        filename = f'{host}.{api_variant}.{firmware}.{branch}{factory_suffix}.yaml'

        config_content = generate_yaml_content(host, api_variant, firmware, branch, factory)

        # Write config YAML
        config_filepath = os.path.join('firmware', 'configurations', filename)
        with open(config_filepath, 'w', encoding='utf-8') as f:
            f.write(config_content)

        print(f'Generated: {filename}')

        # Example YAMLs only for non-CI builds
        if not factory:
            example_filename = f'{host}.{api_variant}.{firmware}.{branch}.example.yaml'
            example_content = generate_example_yaml(host, api_variant, firmware, branch)
 
            example_filepath = os.path.join('firmware', 'examples', example_filename)
            with open(example_filepath, 'w', encoding='utf-8') as f:
                f.write(example_content)
 
            print(f'Generated: {example_filename}')

if __name__ == '__main__':
    main()