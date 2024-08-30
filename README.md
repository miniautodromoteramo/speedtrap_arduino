# Speed Trap System Using Arduino and ESP8266

This project implements a simple speed trap system using Arduino, two photoelectric sensors, and an ESP8266 module. The system measures the speed of an object passing between two sensors and sends the speed data to a server using HTTP POST requests.

## Components

- Arduino (any model with sufficient I/O pins)
- ESP8266 Wi-Fi Module
- 2x Photoelectric sensors
- Buzzer
- Wires and breadboard for connections

## Pin Configuration

- **Photoelectric Sensor 1**: Digital Pin 9
- **Photoelectric Sensor 2**: Digital Pin 10
- **Buzzer**: Digital Pin 11
- **ESP8266 RX**: Digital Pin 2 (TX on ESP8266)
- **ESP8266 TX**: Digital Pin 3 (RX on ESP8266)

## System Flow

1. **Initialization**:
   - The system initializes the serial communication between Arduino and ESP8266.
   - Attempts to connect to the specified Wi-Fi network (`speedtrap` with password `speedtrap`).
   - If connected, the system prepares the ESP8266 to make HTTP POST requests.

2. **Speed Measurement**:
   - The object passes through the first photoelectric sensor, recording the time.
   - When the object passes through the second sensor, the system calculates the time difference.
   - Speed is calculated based on the distance between the sensors (1 meter in this case).

3. **Data Transmission**:
   - The calculated speed is sent to a server via an HTTP POST request using the ESP8266 module.
   - The server's IP address is `192.168.1.200` and the data is sent to port `5000`.

4. **Reset Mechanism**:
   - If no object is detected after the first sensor is triggered, the system will reset after 2 seconds.

## Code Explanation

- **setup()**: Initializes the serial connections and attempts to connect the ESP8266 to the Wi-Fi network.
- **loop()**: Continuously checks the state of the photoelectric sensors and calculates the speed when both sensors have been triggered.
- **initESP8266()**: Sends basic AT commands to check if the ESP8266 module is responsive.
- **connectToWiFi()**: Attempts to connect the ESP8266 to the specified Wi-Fi network.
- **getSpeedKmh(double t)**: Calculates the speed in km/h based on the time difference between sensor triggers.
- **makeHttpPostRequest(double speed)**: Sends the speed data to the server using an HTTP POST request.

## Dependencies

This code uses the `SoftwareSerial` library to communicate with the ESP8266 module over software emulated serial pins.

## Usage

1. Connect the components as per the pin configuration.
2. Upload the code to your Arduino board.
3. Open the Serial Monitor to observe the system's output.
4. Ensure the ESP8266 is within range of the Wi-Fi network.

## Troubleshooting

- **ESP8266 not responding**: Ensure proper connections and that the ESP8266 module is powered correctly.
- **Wi-Fi connection fails**: Check the SSID and password, and ensure the Wi-Fi network is within range.
- **No speed readings**: Verify the alignment and functionality of the photoelectric sensors.

## Future Improvements

- Add an LCD display for real-time speed monitoring.
- Implement error handling and retries for HTTP requests.
- Add support for secured connections to the server.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
