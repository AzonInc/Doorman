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
    has_psram = host in [
        'esp32-s2',
        'esp32-s3',
        'esp32-s3-quad',
        'doorman-s3',
        'doorman-s3-rev2',
        'doorman-s3-rev2-audio'
    ]

    is_doorman_s3 = "doorman-s3" in host

    is_doorman_s3_rev_2 = host in [
        'doorman-s3-rev2',
        'doorman-s3-rev2-audio'
    ]

    packages_config = [
        # Setup Hardware
        
        ## RGB Status LED
        ('rgb_status_led', 'common/rgb-status-led.yaml', True),

        ## Solid State Relay
        ('solid_state_relay', 'common/relay.yaml', is_doorman_s3),

        ## Extension Board Connector (Doorman S3 rev 2.x.x)
        ### Basic configuration
        ('extension_board', 'extension_boards/common.yaml', is_doorman_s3_rev_2),
        ### Audio Extension Board
        #('extension_board_audio', 'extension_boards/audio.yaml', host == 'doorman-s3-rev2-audio'),
        ('extension_board_audio', 'extension_boards/audio-intercom.yaml', host == 'doorman-s3-rev2-audio'),


        # Setup Software

        ## External Components
        ('external_components', 'common/external-components.yaml', branch != 'local'),
        ('external_components', 'common/external-components.local.yaml', branch == 'local'),
        
        ## Doorman Base
        ('base', 'common/base.yaml', True),

        ## WiFi
        ('wifi', 'common/wifi.yaml', True),
        ('wifi_psram', 'common/wifi.psram.yaml', has_psram),

        ## OTA
        ### ESPHome
        ('ota_update_esphome', 'ota/esphome.yaml', True),
        ### HTTP OTA for factory config
        ('ota_update_http', 'ota/http.yaml', factory),

        ## API integration
        ('api', 'api/homeassistant.yaml', api_variant == 'ha'),
        ('api', 'api/mqtt.yaml', api_variant == 'mqtt'),
        ('api', 'api/homekit.yaml', api_variant == 'homekit'),
        ('api', 'api/custom.yaml', api_variant == 'custom'),

        ## Improv BLE (not compatible with Nuki Bridge firmware)
        ## Needs to be here in order to not block on_connect due to the 5s delay
        ## DO NOT MOVE
        ('bluedroid_ble', 'common/bluedroid-ble.yaml', firmware != 'nuki-bridge'),
        ('bluedroid_ble_psram', 'common/bluedroid-ble.psram.yaml', firmware != 'nuki-bridge' and has_psram),

        ## Debug utilities
        ('debug_utilities', 'debug/debug-utilities.yaml', branch == 'dev' or branch == 'local'),
        ('debug_component', 'debug/debug-component.yaml', branch == 'dev' or branch == 'local'),
        #('debug_component_psram', 'debug/debug-component.psram.yaml', (branch == 'dev' or branch == 'local') and has_psram),

        ## Other features
        ('pattern_events', 'pattern_events/pattern-events.yaml', True),
        ('ring_to_open', 'ring_to_open/ring-to-open.yaml', True),
        ('ring_to_open_homekit', 'ring_to_open/ring-to-open.homekit.yaml', api_variant == 'homekit'),
        ('ring_to_open_nuki', 'ring_to_open/ring-to-open-nuki.yaml', firmware == 'nuki-bridge'),
        ('indoor_station_settings', 'bus_devices/indoor-station-settings.yaml', True),
        ('addon_nuki_bridge', 'nuki/nuki-bridge.yaml', firmware == 'nuki-bridge'),
        ('interactive_setup', 'bus_devices/interactive-setup.yaml', True),
        
        ## Add outdoor station for local tests
        ('outdoor_station', 'bus_devices/outdoor-station.yaml', branch == 'local'),
        
        ## Experimental Features
        ### Virtual devices
        #('virtual_indoor_station', 'bus_devices/virtual-indoor-station.yaml', branch == 'local'),
        #('virtual_outdoor_station', 'bus_devices/virtual-outdoor-station.yaml', branch == 'local'),
        ### Configo component for serial interface
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
        ] if api_variant == "mqtt" else []),
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
                f'  package_import_url: github://azoninc/doorman/firmware/configurations/${{host_platform}}.${{api_variant}}.${{firmware_type}}.${{branch}}.yaml@${{branch}}',
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
        content.append('  host: !include ../packages/host/${host_platform}.yaml')
        for name, path in packages:
            content.append(f'  {name}: !include ../packages/{path}')
    else:
        content.append('  host: github://azoninc/doorman/firmware/packages/host/${host_platform}.yaml@${branch}')
        for name, path in packages:
            content.append(f'  {name}: github://azoninc/doorman/firmware/packages/{path}@${{branch}}')
    
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