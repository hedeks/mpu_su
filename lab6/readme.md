# Lab 6 - Matrix Keyboard Input

Input of information through parallel port using matrix keyboard on ATmega128A.

## Objective
Program procedures for inputting information through parallel port using keyboard.

## Hardware Configuration
- **Keyboard**: 4x3 matrix membrane keyboard
- **Scan Lines**: PORTD[2:0] (column outputs)
- **Return Lines**: PORTA[3:0] (row inputs)
- **Pull-up Resistors**: External resistors for input lines
- **Debouncing**: Software debouncing implementation

## 📋 Lab Tasks
- Implement keyboard scanning algorithm
- Handle key press detection and debouncing
- Develop calculator functionality based on variant
- Display input and results on LCD
- Support various arithmetic and logical operations

*Part of Microprocessor Control Systems Devices course - SPbETU "LETI"*