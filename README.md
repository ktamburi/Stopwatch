# Stopwatch Simulation – PIC Microcontroller  

## 📌 Overview  
This project implements a **digital stopwatch** using a **PIC microcontroller**, programmed in **C (PIC C / CCS C compiler)**.  
The assignment was to program the stopwatch application that supports **start/stop, pause, reset, and lap-time recording** according to the given circuit design and the assignment instructions.  

The repository contains:  
- **`main.c`** → Source code for the stopwatch (CCS PIC C).  
- **`main.hex`** → Compiled hex file to load into the circuit.  
- **`Stopwatch Circuit.simu`** → Circuit simulation file for **SimulIDE**.  

---

## ⚡ Features  
The stopwatch behaves as follows:  

- When first starting or on reset, LCD shows:  
  ```
  START STOPWATCH
  00:00:00
  ```  
- **Start/Stop button:**  
  - Starts counting time in format `mm:ss:cc`.  
  - Can pause/resume the stopwatch.  
  - While paused, the display shows `"PAUSED mm:ss:cc"`.  

- **Lap button:**  
  - Records lap times.  
  - Displayed in format `"LAP# mm:ss:cc"`.  
  - Supports up to **10 laps**.  

- **Up/Down buttons:**  
  - Allow browsing through stored lap times when stopwatch is paused or running.  

- **Reset button:**  
  - First press: displays `"STOPPED mm:ss:cc"`.  
  - Second press: fully resets stopwatch and clears lap records.  

---

## 🛠 Requirements  

To run this project, you need:  

- [**SimulIDE**](https://simulide.com/p/) → for circuit simulation.  

---

## ▶️ How to Run the Stopwatch  

1. **Open the circuit in SimulIDE:**  
   - Launch **SimulIDE**.  
   - Open the file `Stopwatch Circuit.simu`.  

2. **Load the compiled code into the microcontroller:**  
   - In the circuit, click on the PIC microcontroller component.  
   - Load the provided `main.hex` file.  

3. **Run the simulation:**  
   - Press the **Play ▶️ button** in SimulIDE.  
   - Interact with the circuit using the buttons connected to the PIC pins.  
   - Observe the stopwatch behavior on the LCD.  

---

## 🎓 Assignment Reference  

This project was created as part of **Epoka University Course CEN389 – Embedded Systems** homework.  
