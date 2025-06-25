# Data-acquisition-system-using-stm32
Data acquisition system based on microcontroller stm32F1(Blue pill) 
**STM32 Multi-Channel Data Acquisition & Visualization System**

---

## About
In this project STM32 microcontroller is used to take multiple readings and show it to the user via inbuilt display and on the PC software. The data can also be recorded in excel file for further processing and visualization. It makes it very useful when some one want to record multiple readings and multiple times. Like this V-I characteristics of 3mm LEDs obtained with this system:

---
### Features
 **1. Inputs:**
              </br> CH1 to CH4 : DC voltage upto ± 20v
              </br> CH5 : DC voltage upto ± 40v
              </br> CH6 : DC voltage upto ± 100v
              </br> Analog in : directly displays analog reding (0 to 4096) in range ± 1.65v
              </br> Speed : It takes pulse input to count number of pulses and speed

  
**2. Output:** </br>
              Analog out : Generates ramp voltage output ( -1.65v to +1.65v ) for scaning/sweeping purpose.


**3. Reading modes/Functions:** </br>
     Function 1 : Sends one set of reading from all inputs to the PC when trigger button is pressed.</br>
     Function 2 : Start sending readings from all inputs continuously at predefined intervals(TimeBase) when trigger button is pressed.</br>
     Function 3 : When trigger is pressed it sends 240 readings at predefined intervals(TimeBase) while Analog out goes to +1.65v from -1.65v. 

**4. User buttons:** </br>
    Different settings can be changed using "Menu", "Up" and "Down" button. Settings like: which reading mode to be used, TimeBase selection, What to display on the inbuilt display, etc.


**5. PC GUI Software:**
  <div align="center">
  <img src="App.png" alt="Loading..." width="500" height="400">
  </div>

  * Serial connection interface
  * Trigger button (working same as the trigger button on the device).
  * Mode selection: There are two modes "Continuous", "Sweep". In the "continuous" mode continuously data is pushed into the excel file. In "Sweep" mode, one sweep is considered when analog out goes from -1.65v to +1.65v (internaly PWM goes from 0 to 240). And each sweep is recoreded in separate sheet of same file. In this mode, when recording is on and trigger is pressed, it will record N sweeps. Additionly there is an average button that can be used to take average of the corresponding values from all sheet and store in a separate sheet.
  * Analog out progrees bar: It gives a visual representation of the instantaneous output voltage.
  * Reading display: Readings are displayed with default lables(CH1,CH2,...). These lables can be changed if requared. An UserParam is also given so that user can input values that needs to be included in the excel. 
  

---
### Some data obtained with this setup 
- **V-I characteristic of 3mm LEDs:** </br>
  <div align="center">
  <img src="V-I char_3mm_LED.png" alt="Loading..." width="800" height="400">
  </div>


### PC Software Features
- **COM Port Selector:** Auto-detects available serial ports.  
- **Directory & Filename Input:** Choose folder and base filename for logs.  
- **Excel Logging:** Writes data with default and user-defined headers via OpenPyXL.  
- **Real-Time Plots:** Select one or more channels to view vs. timestamp.  
- **Error Handling:** Detects and recovers from parse errors and serial disconnects.

### How to Use It
1. **Power & Connect:** Plug STM32 board via USB or battery, connect sensors.  
2. **Launch GUI:** Open `daq_gui.py`, select COM port and log folder.  
3. **Configure:** Enter headers, choose channels to plot.  
4. **Start Acquisition:** Click **Start**—data appears on display, streams to PC.  
5. **Stop & Review:** Click **Stop**, then open Excel file for analysis.

### Use Cases
- **Lab Experiments:** Multi-sensor recording for physics/chemistry demos.  
- **Environmental Monitoring:** Temperature, light, humidity logging.  
- **Educational Projects:** Teaching ADC, DMA, USB CDC, and GUI design.  
- **Rapid Prototyping:** Quick DAQ system without expensive hardware.

---

## Construction

### 1. Hardware Details
- **STM32F103C8T6 "Blue Pill":** 72 MHz Cortex-M3 MCU.  
- **Analog Inputs:** PA0–PA5 wired to sensors (potentiometers, photodiodes).  
- **Display Wiring:** Segment pins → PA6–PA12 (a–g + dp); digit selects → PB0–PB3.  
- **Trigger Input:** PA8 with EXTI for button.  
- **Power:** 5 V USB or Li-ion → onboard 3.3 V regulator.  
- **Optional DMA:** ADC1 → memory buffer via DMA1_Channel1.

### 2. Firmware Details
- **IDE & Toolchain:** STM32CubeIDE with HAL libraries.  
- **Clock Setup:** HSE 8 MHz → SYSCLK 72 MHz.  
- **GPIO:** Configure analog (PA0–PA5), outputs (segments, digits), input with EXTI (PA8).  
- **ADC:** Scan mode over 6 channels, continuous DMA circular buffer.  
- **Timer (TIM2):** Generates sampling triggers at user-defined rate.  
- **USB CDC:** Middleware to enumerate as COM port, send `time_ms,ch1…ch6` CSV.  
- **Display Multiplexing:** 1 ms per digit refresh for flicker-free output.

### 3. PC Software Details
- **Language & Libraries:** Python 3.10+, `tkinter`, `pyserial`, `openpyxl`, `matplotlib`.  
- **GUI Layout:** Dropdowns for COM and folder, text entries for headers, checkboxes for channels, **Start/Stop** buttons, and plot area.  
- **Data Handling:** Reads incoming lines, splits on commas, converts to floats/ints, appends to workbook and plot buffer.  
- **Logging:** Creates timestamped Excel `.xlsx` with headers in first rows, writes each sample as new row.  
- **Plotting Loop:** Updates plots every N samples without blocking UI (via `after()` callback).

---

## Conclusion

This system delivers a cost-effective, customizable data acquisition platform combining:
- **Real-time on-board display** for immediate feedback.  
- **High-speed USB streaming** for extensive logging.  
- **User-friendly PC application** for analysis and visualization.  

Ideal for educators, hobbyists, and prototype developers seeking a flexible DAQ solution without commercial hardware costs.
