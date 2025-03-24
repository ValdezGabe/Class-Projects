# Overview
This project was designed to monitor a Passive Infrared(PIR) Sensor which is used to dectect motion. The program tracks and logs the number of movements detected in two time intervals: every second and every ten minutes. It prints out the maximum, minimum, and average movements every ten minutes and provides an overall average of movements since the program started when prompted to.

## Details
- `Final.ino`: The sript to track motion detected by a PIR sensor, outputting summary statistics at regular intervals.
- `PIR Motion Sensor`: An electronic device that measures heat radiating from objects in it's field of view.
- `ESP32`: A microcontroller(MCU) device.

## A. Hardware Setup

### Instructions
1. Connect the PIR sensor to pin 33 of the ESP32 microcontroller.
2. Ensure that there are proper connections for power and ground of the PIR sensor.
3. Connect microcontroller to computer.

## B. Uploading Program

### Instructions
1. Upload `Final.ino` script to Arduino IDE

## C. Running the Program

### Instructions
1. Hold on to the power button on the ESP32 microcontroller and run the program.
2. Open the Serial Monitor in the Arduino IDE with the baud rate set to 115200 to view the output
3. The program should automatically track and display the number of detected movements per second, followed by the 10-minute summary.
4. To retrieve a data summary of the program since the start time to current time press `'r'` in the spaced provided within Serial Monitor.

