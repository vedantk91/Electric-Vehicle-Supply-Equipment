# Level-2 Electric Vehicle Supply Equipment (EVSE)

This repository contains the design and code for a Level-2 EV Charger designed to work directly with the electrical grid to provide efficient charging solutions for electric vehicles (EVs). The project includes detailed hardware design, firmware, and testing outcomes for a commercially scalable Level-2 charger.

## Project Overview

The Level-2 EV Charger designed in this project is capable of drawing power directly from the grid, providing a faster and more efficient charging solution compared to Level-1 chargers. It is designed with a focus on scalability and usability in both residential and commercial settings.

### Features

- **Control Pilot (CP) Signal Management**: Uses an Op-amp circuitry to manage the CP signal for communication between the EVSE and the vehicle.
- **Microcontroller-based Management**: Utilizes an Atmega 328pb microcontroller to handle logic operations, including charging initiation, status monitoring, and safety checks.
- **Safety and Monitoring**: Integrates features to automatically shut down in case of overcurrent and to monitor the duration of charging to prevent battery overcharging.
- **User Interface**: Includes an LCD display for real-time status updates and a push button for manual operation start.
- **Debugging and Logs**: Provides optional serial debugging outputs for system monitoring and troubleshooting.

### Components

- Op-amp circuitry
- Atmega 328pb microcontroller
- Relay circuitry for AC supply management
- Voltage divider circuitry for CP voltage monitoring
- LCD display, push buttons, and standard electric vehicle connector

## Repository Structure

- `/hardware` - Contains all PCB design files and schematics.
- `/firmware` - Includes all source code for the Atmega 328pb.
- `/docs` - Documentation and research papers related to the project.
- `/images` - Images and diagrams used in documentation and design.

## Setup and Installation

1. **Hardware Assembly**: Refer to the `/hardware` directory for assembly instructions and PCB layouts.
2. **Firmware Deployment**: Load the firmware into Atmega 328pb using the provided code in `/firmware`.
3. **Testing**: Follow the testing procedures described in the documentation to ensure the charger is functioning correctly before full deployment.

## Usage

To use the EVSE:
1. Connect the EVSE to a 230V AC line.
2. Attach the EVSE's connector to an electric vehicle.
3. Use the push button on the unit to start the charging process.
4. Monitor the charging status through the LCD display.

![EVSE Hardware](images/evse_hardware.jpg)

*Figure: Assembled EVSE with LCD display and control unit.*


## Acknowledgements

- Manas Pange
- Seema Talmale
- [Link to the research paper](https://ssrn.com/abstract=4288891)
