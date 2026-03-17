import os
from itertools import product

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.dirname(SCRIPT_DIR)
FIRMWARE_DIR = os.path.join(ROOT_DIR, 'firmware')

def get_host_architectures():
    """Get host architectures from firmware/packages/host directory"""
    hosts_dir = os.path.join(FIRMWARE_DIR, 'packages', 'host')
    if not os.path.exists(hosts_dir):
        raise FileNotFoundError(f"Hosts directory not found: {hosts_dir}")
    
    hosts = []
    for file in os.listdir(hosts_dir):
        if file.endswith('.yaml'):
            # Remove .yaml extension
            host = os.path.splitext(file)[0]
            hosts.append(host)
    
    if not hosts:
        raise ValueError("No host configuration files found in firmware/packages/host directory")
    
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
        # Setup Hardware
        ('host', f'host/{host}.yaml', True),
        # RGB Status LED
        # Doorman S3 rev 2.x.x SK6812B
        ('rgb_status_led', 'common/rgb-status-led.sk6812b.yaml', host == 'doorman-s3-rev2'),
        # Doorman S3 rev 1.x.x / ESP32-S3 / ESP32-S3 WS2812B
        ('rgb_status_led', 'common/rgb-status-led.ws2812b.yaml', host != 'doorman-s3-rev2'),
        ('rgb_status_led_effects', 'common/rgb-status-led.effects.yaml', True),

        # Setup Software
        ('external_components', 'common/external-components.yaml', branch != 'local'),
        ('external_components', 'common/external-components.local.yaml', branch == 'local'),
        ('base', 'common/base.yaml', True),
        ('wifi', 'common/wifi.yaml', True),
        # ES8311 Audio (requires Doorman S3 rev 2.x.x with Audio Extension Board)
        ('audio', 'audio/es8311.yaml', host == 'doorman-s3-rev2-audio'),
        # Improv BLE (not compatible with Nuki Bridge firmware)
        ('bluedroid_ble', 'common/bluedroid-ble.yaml', is_esp32 and firmware != 'nuki-bridge'),
        # OTA
        ('ota_update_esphome', 'ota/esphome.yaml', True),
        # HTTP OTA for factory config
        ('ota_update_http', 'ota/http.yaml', factory),
        # API integration
        ('api', 'api/homeassistant.yaml', api_variant == 'ha'),
        ('api', 'api/mqtt.yaml', api_variant == 'mqtt'),
        ('api', 'api/homekit.yaml', api_variant == 'homekit'),
        ('api', 'api/custom.yaml', api_variant == 'custom'),
        # Debug utilities
        ('debug_utilities', 'debug/debug-utilities.yaml', branch == 'dev' or branch == 'local'),
        ('debug_component', 'debug/debug-component.yaml', branch == 'dev' or branch == 'local'),
        ('debug_component_psram', 'debug/debug-component.psram.yaml', (branch == 'dev' or branch == 'local') and has_psram),
        # Other features
        ('pattern_events', 'pattern_events/pattern-events.yaml', True),
        ('ring_to_open', 'ring_to_open/ring-to-open.yaml', True),
        ('ring_to_open_homekit', 'ring_to_open/ring-to-open.homekit.yaml', api_variant == 'homekit'),
        ('ring_to_open_nuki', 'ring_to_open/ring-to-open-nuki.yaml', firmware == 'nuki-bridge'),
        ('indoor_station_settings', 'bus_devices/indoor-station-settings.yaml', True),
        ('addon_nuki_bridge', 'nuki/nuki-bridge.yaml', firmware == 'nuki-bridge'),
        ('interactive_setup', 'bus_devices/interactive-setup.yaml', True),
        # Add outdoor station for local tests
        ('outdoor_station', 'bus_devices/outdoor-station.yaml', branch == 'local'),
        
        # Experimental Features
        # Virtual devices
        ('virtual_devices', 'bus_devices/virtual-devices.yaml', branch == 'local'),
        # Configo component for serial interface
        # ('configo', 'serial_interface/configo.yaml', branch == 'local'),
    ]
    
    return [(name, path) for name, path, condition in packages_config if condition]

def generate_yaml_content(host, api_variant, firmware, branch, factory=False):
    api_variant_desc = {
        "ha": "Home Assistant",
        "mqtt": "MQTT",
        "homekit": "HomeKit",
    }.get(api_variant, "Custom")
    
    if branch == "local":
        device_name = "doorman-s3-local-dev"
        device_friendly_name = "Doorman S3 (Development)"
    else:
        device_name = "doorman-s3"
        device_friendly_name = "Doorman S3"

    content = [
        f'# Doorman {"Nuki Bridge" if firmware == "nuki-bridge" else "Standard"} Firmware ({api_variant_desc})',
        f'# Base Board {host.upper()}',
        *([ 
            '',
            '# ESPHome Dashboard import and remote logging are not',
            '# supported in this firmware, as they require the API.',
            '# Available only via the Home Assistant API integration.',
        ] if api_variant == "custom" or api_variant == "homekit" else []),
        *([ 
            '',
            '# ESPHome Dashboard import is not supported in this',
            '# firmware, as it requires the API.',
            '# Available only via the Home Assistant API integration.',
        ] if api_variant is "mqtt" else []),
        '',
        '# You can change a few options here.',
        'substitutions:',
        f'  name: "{device_name}"',
        f'  friendly_name: "{device_friendly_name}"',
        f'  ap_password: "Op3n-Sesame!"',
        '  # DO NOT CHANGE - INTERNALLY USED',
        f'  branch: "{branch}"',
        f'  firmware_type: "{firmware}"',
        f'  api_variant: "{api_variant}"',
        f'  host_platform: "{host}"',
        '',
        'esphome:',
        '  name: "${name}"',
        '  friendly_name: "${friendly_name}"',
    ]

    if factory:
        # Only Home Assistant variant supports dashboard import
        # The others dont use the API
        if api_variant == "ha":
            content += [
                '',
                'dashboard_import:',
                f'  package_import_url: github://azoninc/doorman/firmware/configurations/{host}.{api_variant}.{firmware}.{branch}.yaml@{branch}',
                '  import_full_config: true',
            ]
    else:
        content += [
            '',
            'wifi:',
            '  ssid: !secret wifi_ssid',
            '  password: !secret wifi_password',
        ]

    packages = get_packages(host, api_variant, firmware, branch, factory)
    
    content += [
        '',
        'packages:'
    ]
    
    if branch == 'local' or factory:
        for name, path in packages:
            content.append(f'  {name}: !include ../packages/{path}')
    else:
        content += [
            '  remote_package_files:',
            '    url: https://github.com/azoninc/doorman',
            f'    ref: {branch}',
            '    refresh: 0s',
            '    files:',
        ]
        for name, path in packages:
            content.append(f'      - path: firmware/packages/{path}')
    
    return '\n'.join(content)


# Configuration options
HOST_ARCHITECTURES = get_host_architectures()
API_VARIANTS = ['ha', 'mqtt', 'homekit', 'custom']
FIRMWARES = ['standard', 'nuki-bridge']
BRANCHES = ['master', 'dev', 'local']
FACTORY_BUILDS = [False, True]

def main():
    os.makedirs(os.path.join(FIRMWARE_DIR, 'configurations'), exist_ok=True)
    
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
        config_filepath = os.path.join(FIRMWARE_DIR, 'configurations', filename)
        with open(config_filepath, 'w', encoding='utf-8') as f:
            f.write(config_content)

        print(f'Generated: {filename}')

if __name__ == '__main__':
    main()