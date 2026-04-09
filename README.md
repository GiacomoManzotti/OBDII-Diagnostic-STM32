# OBDII-Diagnostic-STM32
Basic STM32F429 OBD-II diagnostic implementation on STM32 using CAN bus and UART interface.

## Project Overview

This project implements a minimal OBD-II diagnostic interface on an STM32 platform.  
The system sends PID requests over CAN (ISO 15765-4) and forwards parsed diagnostic data via UART.

The implementation focuses on:

- CAN communication using STM32 HAL
- OBD-II PID request handling
- Response parsing
- UART data forwarding
- Validation on a real vehicle

## Usage

1. Connect OBD-II cable to vehicle
2. Connect STM32 board to PC via USB
3. Open terminal (115200 baudrate, 8N1)
4. Reset board
5. Select PID from menu

## System Architecture

PC / Serial Terminal  
        ↓  
UART  
        ↓  
STM32  
        ↓  
CAN Transceiver  
        ↓  
Vehicle OBD-II Port

## Implemented Features

- PID request over CAN
- Engine RPM, Engine Load, Speed, Oil Temp reading (PIDs 0x0C, 0x04, 0x0D, 0x5C)
- UART interface for command and output
- Basic response parsing

## Hardware

- STM32F429 via STM32F429IDISC-1  
- CAN transceiver SN65HVD230
- OBD-II connector

## Tested Vehicles

- Mini R56 LCI
- Subaru BRZ Gen 2 ZD8

## Author

Giacomo Manzotti
Cyber Physical Systems
DITEN
University of Genoa
2026
