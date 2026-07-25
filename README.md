# Remote-Control Bus

A scratch-built 1:18 scale articulated bus combining CAD, 3D printing, custom
electronics and embedded software. The completed model uses six OLED destination
displays, six powered doors, realistic lighting and sound, dual microcontrollers and a
PlayStation controller.

![Completed articulated model bus](PROJECT%20PROGRESS%20IMAGES/Complete%20Bus.jpeg)

## Project at a glance

- 141 successful printed parts across multiple design iterations
- A Fusion 360 body model split for practical printing and assembly
- Home-etched PCBs and a modular master/light-controller architecture
- I²C communication between ESP8266 and AVR controllers
- PCA9685 servo control, OLED destination displays and over-the-air updates
- Documented renders, schematics, PCB artwork, build photographs and firmware

## Repository guide

| Area | Contents |
| --- | --- |
| `RC_VOLVO_B12B_ESP8266_CONTROL_MASTER/` | Main controller, doors, displays, drivetrain and controller input |
| `RC_VOLVO_B12B_AVR_LIGHT_CONTROLLER/` | Lighting and auxiliary command controller |
| `PCB Design/` | Fabrication artwork and board photographs |
| `PROJECT PROGRESS IMAGES/` | Build and assembly record |
| `RENDERED PHOTOS/` | CAD renders used during development |

## Firmware setup

1. Copy `credentials.example.h` to `credentials.h` inside the ESP8266 controller
   folder.
2. Enter the Wi-Fi network used for over-the-air updates.
3. Install the
   [ESP8266 PS2X fork](https://github.com/miathedev/Arduino-PS2X),
   [RedMP3](https://github.com/mattmattbobcat/RedMP3),
   `Adafruit PWM Servo Driver Library`, and
   `ESP8266 and ESP32 OLED driver for SSD1306 displays`.
4. Flash the ESP8266 master and AVR light controller separately.

`credentials.h` is ignored deliberately so device credentials cannot be committed.
Every pull request compiles both controllers in GitHub Actions.

## Portfolio

A guided, interactive version of this build is available at
[matthewpicone.com/projects/remote-control-bus/](https://matthewpicone.com/projects/remote-control-bus/).

### Introduction
In November 2020, I purchased my first 3D printer, and after printing a lot of designs from Thingiverse, I decided to create a remote control bus using Fusion 360. After 141 successful prints, and a few failed attempts, I completed the model. I went through several iterations of electronic designs until I settled on the final one, with separate controllers and PWM drivers to allow for easy separation of the bus. The bus features six OLED displays, functional doors, and realistic lights and decals.

### Design and Construction
The construction of the bus was challenging but rewarding. I had to overcome many difficulties, including etching PCBs at home, drilling holes, and soldering boards. The code is designed as a Master/Slave setup, with everything communicating through IIC. The project is controlled using a PlayStation 2 controller, with the master checking for inputs from the controller and sending a command to the slave to execute anything not directly connected.

### Features and Improvements
The finished 1:18 scale articulated model bus boasts many impressive features, such as functioning doors and realistic lights. The diagnostic display in the rear door will display live information from the main controller in the next version of the project. The rear internal panel contains two barrel jacks for charging and master kill switches for the two battery packs on board. Despite some challenges during construction, such as issues with weight and finding suitable gearboxes, the final product is remarkable. Future improvements for version 2 include a web server, flexible resin curtains, addressable LED lights, larger screens, resin printed corners for the windscreen, more efficient and robust code, and a lighter design achieved by printing bigger, thinner parts.

### Conclusion
Months of testing and playing around resulted in the first prototype, which was a great success. I learned a lot during this project, including the importance of addressable LEDs, which could have saved me a lot of time. Overall, the knowledge and experience gained have been invaluable.

### Project Media

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/RENDERED%20PHOTOS/Volvo_B12B_Volgren_BLEA_EEV_2009_V23_2021-Dec-11_11-58-15AM-000_CustomizedView34863880336_png.png?raw=true)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/RENDERED%20PHOTOS/Volvo_B12B_Volgren_BLEA_EEV_2009_V23_2021-Dec-11_11-51-50AM-000_CustomizedView7718614231.png?raw=true)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/RENDERED%20PHOTOS/Volvo_B12B_Volgren_BLEA_EEV_2009_V23_2021-Dec-11_11-46-42AM-000_CustomizedView1347082340.png?raw=true)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/Drawing%20-%20RC%20Volvo%20B12B%20Volgren%20BLEA%20EEV%20v2.png?raw=true)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PROJECT%20PROGRESS%20IMAGES/Printed%20Parts.jpeg?raw=true)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/RC%202009%20Volvo%20B12B%20Current_schem.png?raw=true)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PCB%20Design/RC%202009%20Volvo%20B12B_pcb%20LCTR.jpg?raw=true)


![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PCB%20Design/RC%202009%20Volvo%20B12B_pcb%20LCFE.jpg?raw=true)


![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PCB%20Design/RC%202009%20Volvo%20B12B_pcb%20DESC.jpg?raw=true)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PROJECT%20PROGRESS%20IMAGES/PCB-%20Etched.png?raw=true)


![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PROJECT%20PROGRESS%20IMAGES/PCB-%20Complete.png?raw=true)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PROJECT%20PROGRESS%20IMAGES/Lights%20going%20in.jpeg?raw=true)

[![Watch the video](https://img.youtube.com/vi/FKAv8QJCuBY/hqdefault.jpg)](https://youtu.be/FKAv8QJCuBY)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PROJECT%20PROGRESS%20IMAGES/Desto%20Fixed.jpeg?raw=true)

[![Watch the video](https://img.youtube.com/vi/ezMInmQuV0o/hqdefault.jpg)](https://youtu.be/ezMInmQuV0o)

[![Watch the video](https://img.youtube.com/vi/OTbiOpmcDCU/hqdefault.jpg)](https://youtu.be/OTbiOpmcDCU)

[![Watch the video](https://img.youtube.com/vi/yMuCuTXPN7s/hqdefault.jpg)](https://youtu.be/yMuCuTXPN7s)

![alt text](https://github.com/matthewjpicone/Remote-Control-Bus/blob/main/PROJECT%20PROGRESS%20IMAGES/Complete%20Bus.jpeg?raw=true)
