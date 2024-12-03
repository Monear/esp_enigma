# ESP32 Enigma Machine Project

## Overview

This project implements a modern version of the classic Enigma machine using an ESP32 microcontroller. The Enigma machine was a cipher device used primarily during World War II for encrypting and decrypting secret messages. This project allows you to encrypt and decrypt messages using a web interface, display messages on an LED matrix, and control the device through various functionalities.

## Features

- **Encryption and Decryption**: Encrypt and decrypt messages using the Enigma machine algorithm.
- **LED Matrix Display**: Display messages on an LED matrix with scrolling effects.
- **Wi-Fi Connectivity**: Connect to a Wi-Fi network to access the web interface.
- **Web Interface**: Control the Enigma machine and view logs through a web interface.
- **Sound Effects**: Play sound effects for rotor steps and notifications.
- **Logging**: Log messages and events with timestamps.

## Hardware Requirements

- ESP32 microcontroller
- LED matrix display (4 devices)
- Buzzer
- LEDs (Green and Orange)
- Wi-Fi network

## Software Requirements

- Arduino IDE or PlatformIO
- Required libraries:
  - WiFi
  - WebServer
  - ESPmDNS
  - MD_Parola
  - MD_MAX72xx
  - SPI

## Circuit Diagram

Connect the hardware components as follows:

- **LED Matrix**:
  - CLK_PIN: GPIO 18
  - DATA_PIN: GPIO 23
  - CS_PIN: GPIO 5
- **Buzzer**: GPIO 4
- **Green LED**: GPIO 26
- **Orange LED**: GPIO 27

## Setup Instructions

1. **Install Libraries**: Ensure you have the required libraries installed in your Arduino IDE or PlatformIO.
2. **Configure Wi-Fi**: Update the ssid and password variables in the code with your Wi-Fi credentials.
3. **Upload Code**: Upload the code to your ESP32 microcontroller.
4. **Connect Hardware**: Connect the hardware components as per the circuit diagram.
5. **Power Up**: Power up the ESP32 and wait for it to connect to the Wi-Fi network.

## Usage Instructions

### Web Interface

1. **Access the Web Interface**: Open a web browser and navigate to the IP address displayed on the LED matrix.
2. **Message Input**:
   - Enter your message in the input field and click "Display Message" to show it on the LED matrix.
3. **Enigma Settings**:
   - Select the operation mode (Encrypt or Decrypt).
   - Set the rotor positions for Rotor I, Rotor II, and Rotor III.
   - Click "Process Message" to encrypt or decrypt the message.
4. **Toggle Scroll**:
   - Click "Toggle Scroll" to enable or disable scrolling of the message on the LED matrix.
5. **Retry**:
   - Click "Retry" to reset the message to its original state.
6. **View Logs**:
   - The console output section displays logs and events.

### API Endpoints

- **Root Page**: `/` (GET) - Displays the web interface.
- **Update Message**: `/update` (POST) - Updates the message to be displayed.
- **Toggle Scroll**: `/toggleScroll` (POST) - Toggles the scrolling effect on the LED matrix.
- **Logs**: `/logs` (GET) - Retrieves the log messages.
- **Process Message**: `/process` (POST) - Encrypts or decrypts the message based on the rotor positions and mode.
- **Current Message**: `/currentMessage` (GET) - Retrieves the current message being displayed.
- **Reset Message**: `/reset` (POST) - Resets the message to its original state.

### Example Usage

1. **Encrypt a Message**:
   - Set the operation mode to "Encrypt".
   - Set the rotor positions (e.g., A, B, C).
   - Enter the message "HELLO" and click "Display Message".
   - Click "Process Message" to encrypt the message.
   - The encrypted message will be displayed on the LED matrix and logged.

2. **Decrypt a Message**:
   - Set the operation mode to "Decrypt".
   - Set the rotor positions to the same values used during encryption.
   - Click "Process Message" to decrypt the message.
   - The decrypted message will be displayed on the LED matrix and logged.

## Code Explanation

### Enigma Components

- **EnigmaRotor**: Represents a rotor in the Enigma machine with forward and backward signal processing.
- **Reflector**: Represents the reflector in the Enigma machine.

### Main Functions

- **playStepSound()**: Plays a sound effect for rotor steps.
- **handleScrollToggle()**: Toggles the scrolling effect on the LED matrix.
- **log(String message)**: Logs messages with timestamps.
- **stepRotors()**: Steps the rotors based on the Enigma stepping mechanism.
- **processChar(char c)**: Encrypts or decrypts a single character.
- **handleProcessRequest()**: Handles encryption/decryption requests from the web interface.
- **handleReset()**: Resets the message to its original state.
- **handleLogs()**: Returns the log messages.
- **handleCurrentMessage()**: Returns the current message being displayed.
- **handleRoot()**: Serves the web interface.
- **handleUpdate()**: Updates the message to be displayed.
- **setupWiFi()**: Sets up the Wi-Fi connection and displays the IP address.
- **setup()**: Initializes the hardware and server.
- **loop()**: Main loop function to handle server requests and display animations.

## Conclusion

This ESP32 Enigma Machine project provides a modern implementation of the classic encryption device with a web interface for easy control and monitoring. Follow the setup and usage instructions to get started with encrypting and decrypting messages using your own Enigma machine. Enjoy exploring the fascinating world of cryptography!