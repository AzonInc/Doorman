#pragma once

#include "homekit_bridge.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome
{
    namespace homekit
    {
        class PairingStartedTrigger : public Trigger<> {
            public:
                explicit PairingStartedTrigger(HomeKitBridgeComponent *parent) {
                    parent->add_pairing_started_callback([this]() { this->trigger(); });
                }
        };

        class PairingAbortedTrigger : public Trigger<> {
            public:
                explicit PairingAbortedTrigger(HomeKitBridgeComponent *parent) {
                    parent->add_pairing_aborted_callback([this]() { this->trigger(); });
                }
        };

        class PairingTimeoutTrigger : public Trigger<> {
            public:
                explicit PairingTimeoutTrigger(HomeKitBridgeComponent *parent) {
                    parent->add_pairing_timeout_callback([this]() { this->trigger(); });
                }
        };

        class PairingCompletedTrigger : public Trigger<std::string> {
            public:
                explicit PairingCompletedTrigger(HomeKitBridgeComponent *parent) {
                    parent->add_pairing_completed_callback([this](std::string value) { this->trigger(value); });
                }
        };

        class IdentifyTrigger : public Trigger<> {
            public:
                explicit IdentifyTrigger(HomeKitBridgeComponent *parent) {
                    parent->add_identify_callback([this]() { this->trigger(); });
                }
        };

        class ControllerConnectedTrigger : public Trigger<std::string> {
            public:
                explicit ControllerConnectedTrigger(HomeKitBridgeComponent *parent) {
                    parent->add_controller_connected_callback([this](std::string value) { this->trigger(value); });
                }
        };

        class ControllerDisconnectedTrigger : public Trigger<std::string> {
            public:
                explicit ControllerDisconnectedTrigger(HomeKitBridgeComponent *parent) {
                    parent->add_controller_disconnected_callback([this](std::string value) { this->trigger(value); });
                }
        };

        template<typename... Ts> class PairedCondition : public Condition<Ts...> {
            public:
             bool check(Ts... x) override { return global_homekit_bridge->is_paired(); }
        };

        template<typename... Ts> class ConnectedCondition : public Condition<Ts...> {
            public:
             bool check(Ts... x) override { return global_homekit_bridge->is_connected(); }
        };

    }  // namespace tc_bus
}  // namespace esphome