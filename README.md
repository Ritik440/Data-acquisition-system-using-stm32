# Data-acquisition-system-using-stm32
Data acquisition system based on microcontroller stm32F1(Blue pill) 
**STM32 Multi-Channel Data Acquisition & Visualization System**

---

## About
In this project STM32 microcontroller is used to take multiple readings and show it to the user via inbuilt display and on the PC software. The data can also be recorded in excel file for further processing and visualization.

### Features
- **6 Analog Channels (PA0–PA5):** 12-bit ADC sampling up to several kHz per channel.  
- **Trigger Modes:** Manual (button on PA8) or periodic (timer/DMA).  
- **4-Digit 7-Segment Display:** Live readings via multiplexed GPIO.  
- **USB CDC Data Streaming:** Virtual COM port (PA11/PA12) outputs CSV.  
- **PC GUI Software:** Python-based Tkinter app with Excel logging and real-time plotting.

### Button Functions
- **Trigger Button (PA8):** Initiates a single acquisition cycle and CSV transmit.  
- **Reset Button (optional):** Clears onboard buffers and resets display.

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
