## Weather Station with ESP32

The sensor system is built on an **ESP32** and collects real-time data from various environmental sensors. The ESP32 is connected to the internet to transmit the data to the **Chacka360** platform.

![Alternative Text](https://github.com/yerson001/chakra360/blob/main/images/dronapp.PNG)

### Integrated Sensors:
- **GPS**: Accurate location of the drone and its readings.
- **Atmospheric Pressure**: Measured in millibars (mb).
- **Altitude**: Measured in meters above sea level (masl).
- **Humidity**: Expressed as a percentage (%).
- **Temperature**: Expressed in degrees Celsius (°C).
- **Air Quality**: Measured in parts per million (PPM).

**Sensor System Repository**: [Link to the ESP32 Repository](https://github.com/yysy001/NasaSpaceChallengeHard)

## 1. 3D Printed Case

![Alternative Text](https://github.com/yerson001/chakra360/blob/main/images/case.PNG)

The weather station is housed in a 3D printed case that protects the sensors from weather conditions and allows efficient use on the **Bebop 2** drone.

## 2. Parrot Bebop 2 Drone

Development drone

![Alternative Text](https://github.com/yerson001/chakra360/blob/main/images/drone.PNG)

## Instructions to Run the Arduino Code:
1. **Install the Required Libraries:**
   - Install the following libraries in your Arduino IDE:
     - `TinyGPS++`
     - `Adafruit_Sensor`
     - `Adafruit_BME280_Library`
     - `ESP32WiFi`
   
2. **Upload the Code:**
   - Connect your ESP32 to your computer.
   - Open the Arduino IDE and paste your code.
   - Select the correct board (`ESP32 Dev Module`) and port.
   - Click on the upload button to upload the code to the ESP32.

3. **Monitor the Serial Output:**
   - Open the Serial Monitor in the Arduino IDE to view the data being transmitted to the **Chacka360** platform.
