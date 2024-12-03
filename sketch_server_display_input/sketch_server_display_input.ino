#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <vector>

// Hardware configuration for LED Matrix and buzzer
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 18
#define DATA_PIN 23
#define CS_PIN 5
#define BUZZER_PIN 4
#define GREEN_LED_PIN 26
#define ORANGE_LED_PIN 27

// Enumeration for Enigma modes
enum class EnigmaMode {
    ENCRYPT,
    DECRYPT
};

EnigmaMode currentMode = EnigmaMode::ENCRYPT;  // Default mode

// Enigma components
class EnigmaRotor {
private:
    int position;      // Current rotor position (0-25)
    String wiring;     // Rotor wiring configuration
    int notchPosition; // Notch position for rotor stepping

public:
    EnigmaRotor(String w, char notchChar) : wiring(w), position(0) {
        notchPosition = notchChar - 'A';
    }

    // Forward signal through rotor
    char forward(char c) {
        int index = (c - 'A' + position) % 26;
        char substituted = wiring[index];
        int output = (substituted - 'A' - position + 26) % 26;
        return output + 'A';
    }

    // Backward signal through rotor
    char backward(char c) {
        int index = (c - 'A' + position) % 26;
        int mappedIndex = wiring.indexOf((index + 'A') % 256);
        int output = (mappedIndex - position + 26) % 26;
        return output + 'A';
    }

    // Advance rotor position
    void step() {
        position = (position + 1) % 26;
    }

    // Accessors and mutators
    void setPosition(int pos) { position = pos % 26; }
    int getPosition() { return position; }
    int getNotchPosition() { return notchPosition; }
};

class Reflector {
private:
    String wiring;
public:
    Reflector(String w) : wiring(w) {}
    char reflect(char c) {
        int pos = c - 'A';
        return wiring[pos];
    }
};

// Enigma rotor wirings and notch positions
const String ROTOR_I   = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
const String ROTOR_II  = "AJDKSIRUXBLHWTMCQGZNPYFVOE";
const String ROTOR_III = "BDFHJLCPRTXVZNYEIWGAKMUSQO";
const String REFLECTOR_B = "YRUHQSLDPXNGOKMIEBFZCWVJAT";

// Notch positions for rotors
const char ROTOR_I_NOTCH = 'Q';
const char ROTOR_II_NOTCH = 'E';
const char ROTOR_III_NOTCH = 'V';

std::vector<EnigmaRotor> rotors;
Reflector reflector(REFLECTOR_B);

// Wi-Fi credentials
const char* ssid = "wifi_username";
const char* password = "wifi_password";

WebServer server(80);
MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);



// Scroll control state
bool isScrolling = true;  // Default to scrolling

// Add a flag to track if the currentMessage has been processed
bool messageProcessed = false;

bool messageEnteredAsEncrypted = false;

// Add a variable to store the last rotor positions
String lastPositions = "";

// Add variables to store original and encrypted messages
String currentMessage = "HELLO";  // Default message
String originalMessage = "HELLO";     // Original plaintext
String encryptedMessage = "";         // Encrypted message

// Add a variable to track decryption success
bool decryptionSuccess = false;

// Sound effects
const int STEP_FREQ = 440;  // Frequency for step sound (A4 note)
const int STEP_DURATION = 50;  // Duration in ms

// Melody definitions for notification sound
const int NOTES[] = {262, 330, 392, 523};  // C4, E4, G4, C5
const int NOTE_DURATIONS[] = {200, 200, 200, 400};  // Duration in ms
const int NOTE_COUNT = 4;

// Logging system
String logBuffer = "";
const int MAX_LOG_SIZE = 5000;

// Function declarations
void playNotification();
void playStepSound();
void handleScrollToggle();
void log(String message);
void stepRotors();
char processChar(char c);
void handleProcessRequest();
void handleReset();
void handleLogs();
void handleCurrentMessage();
void handleRoot();
void handleUpdate();
void setupWiFi();
void setup();
void loop();

// Function implementations

// Function to play step sound
void playStepSound() {
    tone(BUZZER_PIN, STEP_FREQ, STEP_DURATION);
}

// Add new handler for scroll toggle
void handleScrollToggle() {
    isScrolling = !isScrolling;
    if (isScrolling) {
        display.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        log("Scrolling enabled");
    } else {
        display.setTextEffect(PA_NO_EFFECT, PA_NO_EFFECT);
        log("Scrolling disabled");
    }
    server.send(200, "text/plain", isScrolling ? "Scrolling" : "Static");
}

// Function to log messages with timestamp
void log(String message) {
    String timestamp = String(millis()/1000.0, 1);
    String logMessage = "[" + timestamp + "s] " + message;
    logBuffer = logBuffer + logMessage + "\n";

    // Truncate log if it exceeds maximum size
    if (logBuffer.length() > MAX_LOG_SIZE) {
        int truncatePos = logBuffer.indexOf("\n", logBuffer.length() - MAX_LOG_SIZE/2);
        if (truncatePos > 0) {
            logBuffer = logBuffer.substring(truncatePos + 1);
        }
    }
}

// Function to step rotors based on Enigma stepping mechanism
void stepRotors() {
    bool middleAtNotchBefore = rotors[1].getPosition() == rotors[1].getNotchPosition();
    bool rightAtNotchAfter;

    // Advance right rotor (fast rotor)
    rotors[2].step();
    rightAtNotchAfter = rotors[2].getPosition() == rotors[2].getNotchPosition();

    // Implement double-stepping anomaly
    if (middleAtNotchBefore || rightAtNotchAfter) {
        rotors[1].step();  // Middle rotor steps

        // If middle rotor steps from notch, left rotor also steps
        if (middleAtNotchBefore) {
            rotors[0].step();  // Left rotor steps
        }
    }
}

// Function to encrypt or decrypt a single character
char processChar(char c) {
    if (!isalpha(c)) return c;

    c = toupper(c);
    playStepSound();

    // Step the rotors
    stepRotors();

    // Forward through rotors (right to left)
    for (int i = 2; i >= 0; i--) {
        c = rotors[i].forward(c);
    }

    // Reflect through reflector
    c = reflector.reflect(c);

    // Backward through rotors (left to right)
    for (int i = 0; i < 3; i++) {
        c = rotors[i].backward(c);
    }

    return c;
}

// Handler for encryption/decryption request
void handleProcessRequest() {
    if (!server.hasArg("positions") || !server.hasArg("mode")) {
        server.send(400, "text/plain", "Missing rotor positions or mode");
        return;
    }

    String positions = server.arg("positions");
    String mode = server.arg("mode");
    EnigmaMode requestedMode = (mode.equalsIgnoreCase("decrypt")) ? EnigmaMode::DECRYPT : EnigmaMode::ENCRYPT;

    // Check if mode has changed
    if (requestedMode != currentMode) {
        messageProcessed = false;
        currentMode = requestedMode;
        lastPositions = "";
    }

    // Check if rotor positions have changed
    if (positions != lastPositions) {
        messageProcessed = false;
        lastPositions = positions;
    }

    // Prevent processing again if already processed
    if (messageProcessed) {
        server.send(200, "text/plain", "Message already processed with current settings");
        return;
    }

    // Initialize rotors
    rotors.clear();
    rotors.push_back(EnigmaRotor(ROTOR_I, ROTOR_I_NOTCH));
    rotors.push_back(EnigmaRotor(ROTOR_II, ROTOR_II_NOTCH));
    rotors.push_back(EnigmaRotor(ROTOR_III, ROTOR_III_NOTCH));

    // Set rotor positions
    for (int i = 0; i < min(3, (int)positions.length()); i++) {
        rotors[i].setPosition(positions.charAt(i) - 'A');
    }

    String inputMessage;
    if (currentMode == EnigmaMode::ENCRYPT) {
        inputMessage = originalMessage;
        digitalWrite(ORANGE_LED_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, LOW);
        log("Encryption mode: Orange LED ON, Green LED OFF");
    } else { // DECRYPT
        if (messageEnteredAsEncrypted) {
            inputMessage = currentMessage;  // Use directly entered encrypted message
        } else if (encryptedMessage.length() > 0) {
            inputMessage = encryptedMessage;  // Use previously encrypted message
        } else {
            server.send(400, "text/plain", "No encrypted message available");
            return;
        }
        digitalWrite(ORANGE_LED_PIN, LOW);
        digitalWrite(GREEN_LED_PIN, HIGH);
        log("Decryption mode: Orange LED OFF, Green LED ON");
    }

    String result = "";
    for (char c : inputMessage) {
        result += processChar(c);
    }

    if (currentMode == EnigmaMode::ENCRYPT) {
        encryptedMessage = result;
        currentMessage = encryptedMessage;
    } else { // DECRYPT
        currentMessage = result;
        if (!messageEnteredAsEncrypted) {
            decryptionSuccess = (currentMessage.equals(originalMessage));
        }
    }

    display.displayClear();
    display.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    display.displayText(currentMessage.c_str(), PA_CENTER, 100, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

    playNotification();
    log("Message processed: " + currentMessage);
    
    server.send(200, "text/plain", "Message processed");
    messageProcessed = true;
}

// Update handleReset to reset decryptionSuccess flag
void handleReset() {
    if (currentMode == EnigmaMode::ENCRYPT) {
        currentMessage = originalMessage;
    } else { // DECRYPT
        currentMessage = encryptedMessage;
    }
    messageProcessed = false;
    decryptionSuccess = false;
    server.send(200, "text/plain", "Message reset to " + String((currentMode == EnigmaMode::ENCRYPT) ? "original" : "encrypted") + " state");
}

// Handler to return logs
void handleLogs() {
    server.send(200, "text/plain", logBuffer);
}

// Handler to return current message
void handleCurrentMessage() {
    server.send(200, "text/plain", currentMessage);
}

// Function to play notification sound
void playNotification() {
    log("Playing notification sound");
    
    if (currentMode == EnigmaMode::ENCRYPT) {
        // Lock sound - ascending pattern with final click
        tone(BUZZER_PIN, 440, 100);  // A4
        delay(120);
        tone(BUZZER_PIN, 554, 100);  // C#5
        delay(120);
        tone(BUZZER_PIN, 659, 100);  // E5
        delay(120);
        tone(BUZZER_PIN, 880, 50);   // A5 (short click)
        delay(60);
        noTone(BUZZER_PIN);
    } else {
        // Unlock sound - descending pattern with final low click
        tone(BUZZER_PIN, 880, 100);  // A5
        delay(120);
        tone(BUZZER_PIN, 659, 100);  // E5
        delay(120);
        tone(BUZZER_PIN, 554, 100);  // C#5
        delay(120);
        tone(BUZZER_PIN, 220, 50);   // A3 (short click)
        delay(60);
        noTone(BUZZER_PIN);
    }
}

// Handler for root page with updated styling and Retry functionality
void handleRoot() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en" class="dark">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Enigma Machine Control Panel</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <script>
    // Tailwind Dark Mode Configuration
    tailwind.config = {
        darkMode: 'class',
        theme: {
            extend: {},
        },
        plugins: [],
    }

    // Initialize theme before DOM loads
    function initializeTheme() {
        const savedTheme = localStorage.getItem('theme') || 'light';
        if (savedTheme === 'dark') {
            document.documentElement.classList.add('dark');
        } else {
            document.documentElement.classList.remove('dark');
        }
        return savedTheme;
    }

    // Function to toggle dark mode
    function toggleTheme() {
        const htmlElement = document.documentElement;
        const isDark = htmlElement.classList.contains('dark');
        
        if (isDark) {
            htmlElement.classList.remove('dark');
            localStorage.setItem('theme', 'light');
        } else {
            htmlElement.classList.add('dark');
            localStorage.setItem('theme', 'dark');
        }
        updateThemeIcons();
    }

    // Function to update theme icons
    function updateThemeIcons() {
        const isDark = document.documentElement.classList.contains('dark');
        document.getElementById('theme-toggle-dark-icon').classList.toggle('hidden', isDark);
        document.getElementById('theme-toggle-light-icon').classList.toggle('hidden', !isDark);
    }

    // Initialize theme immediately
    const currentTheme = initializeTheme();

    // Update icons when DOM is ready
    document.addEventListener('DOMContentLoaded', function() {
        updateThemeIcons();
        generateRotorOptions();
        updateDisplayedMessage();
        updateLogs();
        setInterval(updateLogs, 1000);
    });

        // Existing JavaScript functions
        function processMessage() {
            document.getElementById('processButton').disabled = true;
            const r1 = document.getElementById('rotor1').value;
            const r2 = document.getElementById('rotor2').value;
            const r3 = document.getElementById('rotor3').value;
            const positions = r1 + r2 + r3;
            const mode = document.getElementById('enigmaMode').value;
            
            fetch('/process', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: `positions=${positions}&mode=${mode}`
            })
            .then(response => response.text())
            .then(data => {
                console.log('Message processing complete');
                document.getElementById('feedback').innerText = data;
                updateDisplayedMessage();
                updateLogs();
                document.getElementById('processButton').disabled = false;
            });
        }

        function resetMessage() {
            fetch('/reset', {
                method: 'POST'
            })
            .then(response => response.text())
            .then(data => {
                document.getElementById('feedback').innerText = data;
                updateDisplayedMessage();
                updateLogs();
            });
        }

        function updateDisplayedMessage() {
            fetch('/currentMessage')
                .then(response => response.text())
                .then(text => {
                    document.getElementById('displayedMessage').innerText = text;
                });
        }

        function updateLogs() {
            fetch('/logs')
                .then(response => response.text())
                .then(text => {
                    document.getElementById('console').innerText = text;
                    const consoleDiv = document.getElementById('console');
                    consoleDiv.scrollTop = consoleDiv.scrollHeight;
                });
        }

        function toggleScroll() {
            document.getElementById('scrollButton').disabled = true;
            fetch('/toggleScroll', {method: 'POST'})
                .then(response => response.text())
                .then(data => {
                    document.getElementById('feedback').innerText = data;
                    updateLogs();
                    document.getElementById('scrollButton').disabled = false;
                });
        }
                
        function generateRotorOptions() {
            const rotors = ['rotor1', 'rotor2', 'rotor3'];
            rotors.forEach(id => {
                const select = document.getElementById(id);
                select.innerHTML = '';  // Clear existing options
                for (let i = 0; i < 26; i++) {
                    const letter = String.fromCharCode(65 + i);
                    const option = document.createElement('option');
                    option.value = letter;
                    option.text = letter;
                    select.appendChild(option);
                }
            });
        }

        window.addEventListener('load', function() {
            initializeTheme();
            updateThemeIcons();
            generateRotorOptions();
            updateDisplayedMessage();
            updateLogs();
            setInterval(updateLogs, 1000);
        });
    </script>
</head>
<body class="bg-gray-100 dark:bg-gray-900 text-gray-900 dark:text-gray-100 min-h-screen">
    <div class="container mx-auto px-4 py-6">
        <!-- Header with Theme Toggle -->
        <header class="flex justify-between items-center mb-8">
            <div>
                <h1 class="text-3xl font-bold">ESP32 Enigma Machine</h1>
                <p class="text-sm text-gray-600 dark:text-gray-400">Modern implementation of the classic encryption device</p>
            </div>
            <button onclick="toggleTheme()" 
                class="flex items-center bg-gray-200 dark:bg-gray-700 p-2 rounded hover:bg-gray-300 dark:hover:bg-gray-600 transition-colors">
                <svg id="theme-toggle-dark-icon" class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" 
                        d="M20.354 15.354A9 9 0 018.646 3.646 9.003 9.003 0 0012 21a9.003 9.003 0 008.354-5.646z" />
                </svg>
                <svg id="theme-toggle-light-icon" class="w-5 h-5 hidden" fill="currentColor" viewBox="0 0 20 20">
                    <path fill-rule="evenodd" 
                        d="M10 2a1 1 0 011 1v1a1 1 0 11-2 0V3a1 1 0 011-1zm4 8a4 4 0 11-8 0 4 4 0 018 0zm-.464 4.95l.707.707a1 1 0 001.414-1.414l-.707-.707a1 1 0 00-1.414 1.414zm2.12-10.607a1 1 0 010 1.414l-.706.707a1 1 0 11-1.414-1.414l.707-.707a1 1 0 011.414 0zM17 11a1 1 0 100-2h-1a1 1 0 100 2h1zm-7 4a1 1 0 011 1v1a1 1 0 11-2 0v-1a1 1 0 011-1zM5.05 6.464A1 1 0 106.465 5.05l-.708-.707a1 1 0 00-1.414 1.414l.707.707zm1.414 8.486l-.707.707a1 1 0 01-1.414-1.414l.707-.707a1 1 0 011.414 1.414zM4 11a1 1 0 100-2H3a1 1 0 000 2h1z" 
                        clip-rule="evenodd" />
                </svg>
            </button>
        </header>

        <!-- Main Content with Two Columns -->
        <div class="flex flex-col lg:flex-row lg:space-x-8">
            <!-- Left Column -->
            <div class="lg:w-1/2 space-y-8">
                <!-- Message Input Card -->
                <div class="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
                    <h2 class="text-2xl font-semibold mb-4">Message Input</h2>
                    <form action="/update" method="POST" class="space-y-4">
                        <div>
                            <input type="text" name="message" 
                                class="w-full px-4 py-2 rounded-lg border border-gray-300 dark:border-gray-700 bg-gray-50 dark:bg-gray-700 text-gray-900 dark:text-gray-100 focus:outline-none focus:ring-2 focus:ring-blue-500"
                                placeholder="Enter your message" required>
                        </div>
                        <div>
                            <button type="submit"
                                class="w-full bg-blue-600 text-white py-2 rounded-lg hover:bg-blue-700 transition-colors">
                                Display Message
                            </button>
                        </div>
                    </form>
                </div>

                <!-- Enigma Settings Card -->
                <div class="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
                    <h2 class="text-2xl font-semibold mb-4">Enigma Settings</h2>
                    
                    <!-- Mode Selector -->
                    <div class="mb-6">
                        <label class="block text-gray-700 dark:text-gray-300 mb-2">Operation Mode</label>
                        <select id="enigmaMode" 
                            class="w-full px-4 py-2 rounded-lg border border-gray-300 dark:border-gray-700 bg-gray-50 dark:bg-gray-700 text-gray-900 dark:text-gray-100 focus:outline-none focus:ring-2 focus:ring-blue-500">
                            <option value="encrypt">Encrypt</option>
                            <option value="decrypt">Decrypt</option>
                        </select>
                    </div>

                    <!-- Rotor Settings -->
                    <div class="space-y-4 mb-6">
                        <div>
                            <label class="block text-gray-700 dark:text-gray-300 mb-2">Rotor I</label>
                            <select id="rotor1" 
                                class="w-full px-4 py-2 rounded-lg border border-gray-300 dark:border-gray-700 bg-gray-50 dark:bg-gray-700 text-gray-900 dark:text-gray-100 focus:outline-none focus:ring-2 focus:ring-blue-500">
                            </select>
                        </div>
                        <div>
                            <label class="block text-gray-700 dark:text-gray-300 mb-2">Rotor II</label>
                            <select id="rotor2" 
                                class="w-full px-4 py-2 rounded-lg border border-gray-300 dark:border-gray-700 bg-gray-50 dark:bg-gray-700 text-gray-900 dark:text-gray-100 focus:outline-none focus:ring-2 focus:ring-blue-500">
                            </select>
                        </div>
                        <div>
                            <label class="block text-gray-700 dark:text-gray-300 mb-2">Rotor III</label>
                            <select id="rotor3" 
                                class="w-full px-4 py-2 rounded-lg border border-gray-300 dark:border-gray-700 bg-gray-50 dark:bg-gray-700 text-gray-900 dark:text-gray-100 focus:outline-none focus:ring-2 focus:ring-blue-500">
                            </select>
                        </div>
                    </div>

                    <!-- Action Buttons -->
                    <div class="flex space-x-4">
                        <button id="processButton" type="button" onclick="processMessage()"
                            class="flex-1 bg-green-600 text-white py-2 rounded-lg hover:bg-green-700 transition-colors">
                            Process Message
                        </button>
                        <button id="scrollButton" type="button" onclick="toggleScroll()"
                            class="flex-1 bg-purple-600 text-white py-2 rounded-lg hover:bg-purple-700 transition-colors">
                            Toggle Scroll
                        </button>
                        <button id="retryButton" type="button" onclick="resetMessage()"
                            class="flex-1 bg-red-600 text-white py-2 rounded-lg hover:bg-red-700 transition-colors">
                            Retry
                        </button>
                    </div>
                    
                    <!-- Feedback Message -->
                    <p id="feedback" class="mt-4 text-center text-sm text-red-500"></p>
                </div>
            </div>

            <!-- Right Column -->
            <div class="lg:w-1/2 space-y-8 mt-8 lg:mt-0">
                <!-- Current Message Display -->
                <div class="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
                    <h2 class="text-2xl font-semibold mb-4">Current Message</h2>
                    <div class="bg-gray-100 dark:bg-gray-700 rounded-lg p-4 font-mono h-24 overflow-auto">
                        <span id="displayedMessage" class="text-lg">)rawliteral" + currentMessage + R"rawliteral(</span>
                    </div>
                </div>

                <!-- Console Output -->
                <div class="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
                    <h2 class="text-2xl font-semibold mb-4">Console Output</h2>
                    <div id="console" 
                        class="bg-gray-900 dark:bg-gray-700 text-green-400 font-mono text-sm p-4 rounded-lg h-48 overflow-y-auto">
                    </div>
                </div>
            </div>
        </div>
    </div>

    <!-- Theme Toggle Icon Script -->
    <script>
        // Function to update theme icons based on current theme
        function updateThemeIcons() {
            const isDark = document.documentElement.classList.contains('dark');
            const darkIcon = document.getElementById('theme-toggle-dark-icon');
            const lightIcon = document.getElementById('theme-toggle-light-icon');

            if (isDark) {
                darkIcon.classList.add('hidden');
                lightIcon.classList.remove('hidden');
            } else {
                darkIcon.classList.remove('hidden');
                lightIcon.classList.add('hidden');
            }
        }

        // Update theme icons whenever theme is toggled
        const themeToggleButton = document.querySelector('button[onclick="toggleTheme()"]');
        themeToggleButton.addEventListener('click', updateThemeIcons);
    </script>
</body>
</html>
    )rawliteral";
    server.send(200, "text/html", html);
}

// Handler to update message from web form
void handleUpdate() {
    if (server.hasArg("message")) {
        currentMessage = server.arg("message");
        currentMessage.toUpperCase();

        // Check if message was entered in encryption or decryption mode
        if (currentMode == EnigmaMode::DECRYPT) {
            encryptedMessage = currentMessage;  // Store as encrypted message
            messageEnteredAsEncrypted = true;
        } else {
            originalMessage = currentMessage;  // Store as original message
            messageEnteredAsEncrypted = false;
            encryptedMessage = "";  // Clear any previous encrypted message
        }
        
        // Reset processing flags
        messageProcessed = false;
        lastPositions = "";
        
        log("New message: " + currentMessage);

        display.displayClear();
        display.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display.displayText(currentMessage.c_str(), PA_CENTER, 100, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

        playNotification();

        server.sendHeader("Location", "/");
        server.send(303);
    } else {
        server.send(400, "text/plain", "No message received");
    }
}

// Function to set up Wi-Fi connection and display IP address
void setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    log("Connecting to WiFi");

    // Show "CONNECTING" on the display
    display.displayClear();
    display.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    display.displayText("CONNECTING", PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

    // Animate "CONNECTING" while waiting for connection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40) { // Increased attempts for better reliability
        if (display.displayAnimate()) {
            // Animation step completed
        }
        delay(500); // Wait for half a second before next attempt
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        log("Connected to WiFi");
        String ip = WiFi.localIP().toString();
        log("IP address: " + ip);

        // Show the IP address on the display
        String displayMessage = "IP: " + ip;
        display.displayClear();
        display.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display.displayText(displayMessage.c_str(), PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

        // Animate the IP address display for a certain duration
        unsigned long startTime = millis();
        while (millis() - startTime < 10000) { // Display IP for 10 seconds
            if (display.displayAnimate()) {
                // Animation step completed
            }
            delay(100); // Small delay to prevent rapid looping
        }

        // After displaying the IP, revert to the default message
        display.displayClear();
        display.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display.displayText(currentMessage.c_str(), PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    } else {
        log("Failed to connect to WiFi");

        // Show error message on display
        display.displayClear();
        display.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display.displayText("WiFi Failed", PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }
}

// Function to initialize hardware and server
void setup() {
    log("--- Starting ESP32 Enigma Machine ---");

    // Initialize buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    log("Buzzer initialized");

    // Initialize SPI for LED Matrix
    SPI.begin(CLK_PIN, DATA_PIN, CS_PIN);
    log("SPI initialized");

    // Initialize LEDs
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(ORANGE_LED_PIN, OUTPUT);
    log("LEDs initialized");

    digitalWrite(ORANGE_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);

    // Initialize display
    display.begin();

    display.setZone(0, 0, MAX_DEVICES-1);
    display.setIntensity(8);
    display.setTextAlignment(PA_CENTER);
    display.setSpeed(100);
    display.setPause(1000);
    display.displayClear();

    log("Display initialized with single zone");

    // Set up Wi-Fi and web server
    setupWiFi();

    // Set initial message on display
    display.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    display.displayText(currentMessage.c_str(), PA_CENTER, 100, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    log("Initial message set: " + currentMessage);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/update", HTTP_POST, handleUpdate);
    server.on("/toggleScroll", HTTP_POST, handleScrollToggle);
    server.on("/logs", HTTP_GET, handleLogs);
    server.on("/process", HTTP_POST, handleProcessRequest);
    server.on("/currentMessage", HTTP_GET, handleCurrentMessage);
    server.on("/reset", HTTP_POST, handleReset);
    
    server.begin();

    log("HTTP server started");
}

// Main loop function
void loop() {
    if (display.displayAnimate()) {
        display.displayReset();
    }
    server.handleClient();
    delay(10);
}