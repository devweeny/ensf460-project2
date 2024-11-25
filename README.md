README.md
Isabelle Gaudet/Group 4
2024-11-23

# How to use TransmissionLogger.py / Our program

## Intro

App project 2 involves developing a microcontroller-based LED intensity control system. TransmissionLogger.py is a part of this project which is a Python data logging and visualization program. The LED brightness is adjustable by a potentiometer and responds to user input by push buttons. The Python script logs the intensity levels and ADC readings transmitted from the microcontroller over UART, saves them in a CSV file and generates a plot for analysis.

## Requirements to interact with our program

1. **Hardware Setup:**

   - Microcontroller with LED connected to pin 12.
   - Push Buttons (PB1, PB2, PB3) - connected to ports RA2,RA4, and RB$, respectively.
   - Potentiometer for intensity adjustment.
   - UART interface for data transmission.

2. **Python Dependencies:**

   - Python 3.x
   - pyserial
   - pandas
   - matplotlib

## How to Run the Python Script

### 1. **Ensure Hardware Setup:**
   - Connect the microcontroller to your PC using a UART interface.
   - Verify the correct COM port (UART port) for your setup.

### 2. **Run the Script:**
   Execute the script using:
   ```bash
   python TransmissionLogger.py
   ```
   The script will do the following:
   - Log data for 60 seconds.
   - Save the data to a csv file titled `Group4_Data.csv`.
   - Generate a graph with two plots:
     - **Intensity Level (%) vs. Time (s)**
     - **ADC Reading vs. Time (s)**

### 3. **View the Outputs:**
   - Check the downloaded csv file, `Group4_Data.csv` file for recorded intensity and ADC values.
   - View the generated graph for visual analysis.

## Project Behavior

### **Functionalities of PBx (push buttons)**
- **PB1 (System ON/OFF Mode):**
  - Turns the system ON and allows LED intensity adjustment
  - Turns the system OFF and enters low-power mode

- **PB2 (Blink Mode):**
  - Blinks the LED at 0.5s intervals, adjusting brightness via the potentiometer
  - When in OFF mode, blinks at 100% intensity
  - Stops blinking when clicked again

- **PB3 (UART Transmission):**
  - Starts transmission of intensity and ADC readings over UART
  - Stops transmission upon another click or when exiting ON mode

### **Python Script Behavior**
- Logs data transmitted from the microcontroller when PB3 is clicked.
- Data includes:
  - **Timestamp** of each reading.
  - **Intensity Level (%)** of the LED.
  - **ADC Reading**.
- Visualizes data with two subplots:
  - **Intensity vs. Time.**
  - **ADC Reading vs. Time.**

## Example Outputs

### **CSV File (Sample)**:
Timestamp,Intensity,ADC
2024-11-23 12:00:01,75,512
2024-11-23 12:00:02,80,520


### **Graph - the 2 graphs are subplots**:
- **Plot 1:** Intensity Level (%) vs. Time (s).
- **Plot 2:** ADC Reading vs. Time (s).


