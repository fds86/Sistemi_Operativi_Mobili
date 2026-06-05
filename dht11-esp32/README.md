# DHT11 ESP32 PlatformIO Workspace

## What this project does
Reads temperature and humidity from a DHT11 sensor connected to an ESP32 and prints the values to the serial monitor.

## Wiring
- DHT11 VCC -> 3.3V
- DHT11 GND -> GND
- DHT11 DATA -> GPIO 4

If your module needs a pull-up resistor, place a 10k resistor between DATA and 3.3V.

## Build and upload
1. Open the `dht11-esp32` folder in VS Code.
2. Make sure the PlatformIO extension is installed.
3. Build with PlatformIO.
4. Upload to the board.
5. Open the serial monitor at 9600 baud.

## Notes
- The project uses the Arduino framework.
- The DHT11 is read every 2 seconds.
- If your wiring uses a different data pin, change `DHT_PIN` in `src/main.cpp`.
