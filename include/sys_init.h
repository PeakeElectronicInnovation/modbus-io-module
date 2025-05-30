#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <W5500lwIP.h>
#include <WebServer.h>
#include <ArduinoModbus.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// Watchdog timer
#define WDT_TIMEOUT 5000

// Pin Definitions
#define PIN_ETH_MISO 16
#define PIN_ETH_CS 17
#define PIN_ETH_SCK 18
#define PIN_ETH_MOSI 19
#define PIN_ETH_RST 20
#define PIN_ETH_IRQ 21
#define PIN_EXT_LED 22

// Constants
#define CONFIG_FILE "/config.json"
#define CONFIG_VERSION 6  // Increment this when config structure changes
#define HOSTNAME_MAX_LENGTH 32
#define MAX_MODBUS_CLIENTS 4  // Maximum number of concurrent Modbus clients

// Global flags
bool core0setupComplete = false;

// Digital IO pins
const uint8_t DIGITAL_INPUTS[] = {0, 1, 2, 3, 4, 5, 6, 7};  // Digital input pins
const uint8_t DIGITAL_OUTPUTS[] = {8, 9, 10, 11, 12, 13, 14, 15}; // Digital output pins
const uint8_t ANALOG_INPUTS[] = {26, 27, 28};   // ADC pins

// Network and Modbus Configuration
struct Config {
    uint8_t version;
    bool dhcpEnabled;
    uint8_t ip[4];
    uint8_t gateway[4];
    uint8_t subnet[4];
    uint16_t modbusPort;
    char hostname[HOSTNAME_MAX_LENGTH];
    bool diPullup[8];         // Enable internal pullup for digital inputs
    bool diInvert[8];         // Invert logic for digital inputs
    bool diLatch[8];          // Enable latching for digital inputs (stay ON until read)
    bool doInvert[8];         // Invert logic for digital outputs
    bool doInitialState[8];   // Initial state for digital outputs (true = ON, false = OFF)
} config;

// Default configuration
const Config DEFAULT_CONFIG = {
    CONFIG_VERSION,              // version
    true,                       // DHCP enabled
    {192, 168, 1, 10},         // Default IP
    {192, 168, 1, 1},          // Default Gateway
    {255, 255, 255, 0},        // Default Subnet
    502,                       // Default Modbus port
    "modbus-io",                // Default hostname
    {false, false, false, false, false, false, false, false},  // diPullup (all disabled)
    {false, false, false, false, false, false, false, false},  // diInvert (no inversion)
    {false, false, false, false, false, false, false, false},  // diLatch (no latching)
    {false, false, false, false, false, false, false, false},  // doInvert (no inversion)
    {false, false, false, false, false, false, false, false},  // doInitialState (all OFF)
};

struct IOStatus {
    bool dIn[8];          // Current state of digital inputs (including latching behavior if enabled)
    bool dInRaw[8];       // Actual physical state of digital inputs (without latching)
    bool dInLatched[8];   // Tracks if an input has been latched (true = latched)
    bool dOut[8];
    uint16_t aIn[3];
};

IOStatus ioStatus;

// Ethernet and Server instances
Wiznet5500lwIP eth(PIN_ETH_CS, SPI, PIN_ETH_IRQ);
WiFiServer modbusServer; 
WebServer webServer(80);
WiFiClient client;

// Client management
struct ModbusClientConnection {
    WiFiClient client;
    ModbusTCPServer server;
    bool connected;
    IPAddress clientIP;
    unsigned long connectionTime;
};

ModbusClientConnection modbusClients[MAX_MODBUS_CLIENTS];
int connectedClients = 0;

// Function declarations
void loadConfig();
void saveConfig();
void setPinModes();
void setupEthernet();
void setupModbus();
void setupWebServer();
void updateIOpins();
void updateIOForClient(int clientIndex);
void handleRoot();
void handleCSS();
void handleJS();
void handleFavicon();
void handleLogo();
void handleGetConfig();
void handleSetConfig();
void handleSetOutput();
void handleGetIOStatus();
void handleGetIOConfig();
void handleSetIOConfig();
void resetLatches();
void handleResetLatches();
void handleResetSingleLatch();