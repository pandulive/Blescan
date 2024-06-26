#include <simplebluez/Bluez.h>
#include "nlohmann/json.hpp"
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <string>

using json = nlohmann::json;
SimpleBluez::Bluez bluez;

std::atomic_bool async_thread_active = true;
void async_thread_function() {
    while (async_thread_active) {
        bluez.run_async();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void millisecond_delay(int ms) {
    for (int i = 0; i < ms; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main(int argc, char* argv[]) {
    int selection = -1;
        std::vector<std::string> macid{};

    bluez.init();
    std::thread* async_thread = new std::thread(async_thread_function);

    auto adapters = bluez.get_adapters();
    std::cout << "The following adapters were found:" << std::endl;
    for (int i = 0; i < adapters.size(); i++) {
        std::cout << "[" << i << "] " << adapters[i]->identifier() << " [" << adapters[i]->address() << "]"
                  << std::endl;
        // macid.push_back(adapters[i]->address());
    }


    // std::cout << "Please select an adapter to scan: ";
    // std::cin >> selection;
    // if (selection < 0 || selection >= adapters.size()) {
    //     std::cout << "Invalid selection" << std::endl;
    //     return 1;
    // }
    selection = 0;
    auto adapter = adapters[selection];
    std::cout << "Scanning " << adapter->identifier() << " [" << adapter->address() << "]" << std::endl;

    SimpleBluez::Adapter::DiscoveryFilter filter;
    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
    adapter->discovery_filter(filter);

    adapter->set_on_device_updated([&](std::shared_ptr<SimpleBluez::Device> device) {
        std::cout << "Update received for " << device->address() << std::endl;
        macid.push_back(device->address());
        std::cout << "\tName " << device->name() << std::endl;
        std::cout << "\tAddress Type " << device->address_type() << std::endl;
        std::cout << "\tRSSI " << std::dec << device->rssi() << std::endl;
        std::cout << "\tTxPower " << std::dec << device->tx_power() << std::endl;
        auto manufacturer_data = device->manufacturer_data();
        for (auto& [manufacturer_id, value_array] : manufacturer_data) {
            std::cout << "\tManuf ID 0x" << std::setfill('0') << std::setw(4) << std::hex << (int)manufacturer_id;
            std::cout << ": ";
            for (int i = 0; i < value_array.size(); i++) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex << ((int)value_array[i]) << " ";
            }
            std::cout << std::endl;
        }
        
    });

    adapter->discovery_start();
    millisecond_delay(3000);
    adapter->discovery_stop();

    // Sleep for a bit to allow the adapter to stop discovering.
    millisecond_delay(4000);

    async_thread_active = false;
    while (!async_thread->joinable()) {
        millisecond_delay(10);
    }
    async_thread->join();
    delete async_thread;

    std::ofstream outFile("macids.txt");
    // the important part
    for (const auto &e : macid) outFile << e << "\n";

    time_t timestamp;
    time(&timestamp);
    // std::cout << ctime(&timestamp);

    json j;
    j["time"] = ctime(&timestamp).strip();
    j["health"] = "I am fine";
    j["age"] = 36;

    std::cout << j << std::endl;


// For conncting to specified BLE
    // if (selection >= 0 && selection < peripherals.size()) {
    //     auto peripheral = peripherals[selection];
    //     std::cout << "Connecting to " << peripheral->name() << " [" << peripheral->address() << "]" << std::endl;

    //     for (int attempt = 0; attempt < 3; attempt++) {
    //         try {
    //             peripheral->connect();
    //         } catch (SimpleDBus::Exception::SendFailed& e) {
    //             millisecond_delay(100);
    //         }
    //     }

    //     if (!peripheral->connected() || !peripheral->services_resolved()) {
    //         std::cout << "Failed to connect to " << peripheral->name() << " [" << peripheral->address() << "]"
    //                   << std::endl;
    //         return 1;
    //     }

    //     std::cout << "Successfully connected, testing NUS service." << std::endl;

    //     auto characteristic_rx = peripheral->get_characteristic("6e400001-b5a3-f393-e0a9-e50e24dcca9e",
    //                                                             "6e400002-b5a3-f393-e0a9-e50e24dcca9e");

    //     characteristic_rx->write_command("Hello World");

    //     auto characteristic_tx = peripheral->get_characteristic("6e400001-b5a3-f393-e0a9-e50e24dcca9e",
    //                                                             "6e400003-b5a3-f393-e0a9-e50e24dcca9e");

    //     characteristic_tx->set_on_value_changed([&](SimpleBluez::ByteArray new_value) { print_byte_array(new_value); });

    //     characteristic_tx->start_notify();
    //     millisecond_delay(3000);
    //     characteristic_tx->stop_notify();
    //     millisecond_delay(1000);

    //     peripheral->disconnect();

    //     // Sleep for an additional second before returning.
    //     // If there are any unexpected events, this example will help debug them.
    //     millisecond_delay(1000);
    // }


    return 0;
}
