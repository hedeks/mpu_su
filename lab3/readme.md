# Lab 3 - Stepper Motor Control

Control of stepper motor using parallel ports on ATmega128A microcontroller.

## 🎯 Objective

Program software control procedures for stepper motor using parallel port MCU.

## 🔧 Hardware Configuration

- **Motor**: Unipolar stepper motor (1.8° step angle, 200 steps/revolution)
- **Control Port**: PORTC[3:0] (phase control)
- **Driver**: ULN2003A for motor phase driving
- **Connection**: 4-phase control through driver IC

## 📋 Lab Tasks

- Implement different stepping modes:
  - Single-phase full-step mode
  - Two-phase full-step mode
  - Half-step mode
- Control rotation speed through pulse timing
- Implement speed variation profiles
- Manage rotation direction

*Part of Microprocessor Control Systems Devices course - SPbETU "LETI"**
