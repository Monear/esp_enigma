# ESP32 Enigma Machine Project

## Overview

This project implements a modern version of the classic Enigma machine using an ESP32 microcontroller. The Enigma machine was a cipher device used primarily during World War II for encrypting and decrypting secret messages. This project allows you to encrypt and decrypt messages using a web interface, display messages on an LED matrix, and control the device through various functionalities.

## Features

- **Encryption and Decryption**: Encrypt and decrypt messages using the Enigma machine algorithm.
- **LED Matrix Display**: Display messages on an LED matrix with scrolling effects.
- **Wi-Fi Connectivity**: Connect to a Wi-Fi network to access the web interface.
- **Web Interface**: Control the Enigma machine and view logs through a web interface.
- **Sound Effects**: Play sound effects for rotor steps and notifications.
- **LED Lights**: Uses two LED lights to show the state of the machine. 
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

### Web Interface Layout
The web interface consists of two main columns:
- Left column: Controls and settings
- Right column: Message display and console output

### Controls and Features

#### Message Input
1. Enter your message in the text input field
2. Click "Display Message" to show it on the LED matrix display

#### Operation Mode Selection
- Use the dropdown to select between:
  - **Encrypt**: Converts plaintext into encrypted message
  - **Decrypt**: Converts encrypted message back to plaintext

#### Rotor Settings
Configure the three rotors (I, II, III):
- Each rotor can be set to positions A-Z
- These settings must match for encryption/decryption of the same message
- The rotor positions affect the encryption pattern

### Action Buttons

#### Process Message
- Click to encrypt/decrypt based on current mode
- Uses the configured rotor positions
- Shows results on LED matrix and in Current Message display
- Green LED indicates decryption mode
- Orange LED indicates encryption mode

#### Toggle Scroll
- Switches between scrolling and static display on LED matrix
- Affects how the message appears on the physical display
- Current state is logged in console output

#### Retry
- Resets message to its original state:
  - In encryption mode: Returns to original input
  - In decryption mode: Returns to encrypted message
- Useful for trying different rotor configurations

### Theme Toggle
- Button in top-right corner
- Switches between light and dark display modes
- Settings persist across browser sessions

### Display Panels

#### Current Message
- Shows the active message after processing
- Updates automatically after encryption/decryption
- Displays in monospace font for clarity

#### Console Output
- Shows system logs and operations
- Includes timestamps
- Auto-scrolls to latest entries
- Useful for tracking operations and debugging

## Typical Usage Flow

1. **For Encryption**:
   - Select "Encrypt" mode
   - Enter your plaintext message
   - Click "Display Message"
   - Set rotor positions
   - Click "Process Message"
   - Note the encrypted result

2. **For Decryption**:
   - Select "Decrypt" mode
   - Enter the encrypted message
   - Click "Display Message"
   - Set same rotor positions used for encryption
   - Click "Process Message"
   - View the decrypted result

The LED matrix will display the current message with optional scrolling, and status LEDs indicate the current operation mode.


### API Endpoints

- **Root Page**: `/` (GET) - Displays the web interface.
- **Update Message**: `/update` (POST) - Updates the message to be displayed.
- **Toggle Scroll**: `/toggleScroll` (POST) - Toggles the scrolling effect on the LED matrix.
- **Logs**: `/logs` (GET) - Retrieves the log messages.
- **Process Message**: `/process` (POST) - Encrypts or decrypts the message based on the rotor positions and mode.
- **Current Message**: `/currentMessage` (GET) - Retrieves the current message being displayed.
- **Reset Message**: `/reset` (POST) - Resets the message to its original state.

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

This ESP32 Enigma Machine project provides a modern implementation of the classic encryption device with a web interface for easy control and monitoring. Follow the setup and usage instructions to get started with encrypting and decrypting messages using your own Enigma machine.