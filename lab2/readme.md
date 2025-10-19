# Lab 2 - LED Matrix Control

Control of 16x16 LED matrix using parallel ports on ATmega128A microcontroller.

## Objective

Program parallel port procedures for outputting graphical information to LED matrix displays.

## Hardware Configuration

- **Display**: 16x16 LED matrix (composed of four 8x8 matrices)
- **Row Control**: PORTA (active high)
- **Column Control**: PORTD[4:7] (decoder inputs)
- **Drivers**:
  - ULN2804A for column driving
  - Power MOSFETs for row driving
- **Decoder**: 4-to-16 for column selection

## 📋 Lab Tasks

- Display specified pictograms on LED matrix
- Implement matrix scanning with persistence of vision
- Create 3-second pause after each pictogram
- Invert all points and pause again
- Repeat in infinite loop

*Part of Microprocessor Control Systems Devices course - SPbETU "LETI"*
