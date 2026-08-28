# Hardware Project: Interactive Robotic Maze Game

This project implements an interactive robotic maze game that combines embedded systems, wireless communication, autonomous navigation, and dynamic maze reconfiguration.  
It uses a distributed multi-controller design built around Arduino Mega/Nano/Uno-class boards, ESP32-class control logic, NRF24L01 wireless links, IR sensing, servo-actuated maze elements, motor drivers, and encoder-based DC motors.

At runtime, a player controls one bot wirelessly while an autonomous ghost bot navigates the maze and reacts to game state updates.  
The maze can reconfigure dynamically, game progression is trophy-driven, and LEDs/audio provide adaptive feedback for a more immersive game experience.

## Project Goals

- Build a real-time robotic game with both human-controlled and autonomous agents.
- Support dynamic maze behavior through servo-controlled path changes.
- Coordinate multiple embedded nodes over low-latency wireless communication.
- Deliver clear game-state feedback using light and sound effects.

## Repository Structure

- `/remote/remote.ino`  
  Reads joystick input and transmits control data over NRF24L01.

- `/car/car.ino`  
  Receives joystick commands and drives the player bot motors.

- `/maze/maze.ino`  
  Core maze/game controller. Monitors IR checkpoints, actuates servos to reconfigure paths, handles trophy logic, and sends pattern commands to the ghost bot.

- `/ghost_bot/ghost_bot.ino`  
  Autonomous ghost bot firmware. Receives pattern IDs wirelessly, performs encoder-based motion control, executes turn/forward path sequences, and reports detection events.

- `/sound_light/sound_light.ino`  
  LED animation and DFPlayer-based audio feedback controller.

## High-Level Architecture

### 1) Player Control Path
1. Joystick values are sampled by the remote controller.
2. Values are sent via NRF24L01 radio.
3. Player bot receives data and maps it to motor directions/speeds.

### 2) Maze/Game Logic Path
1. Maze IR sensors detect progression and trigger events.
2. Servos rotate to open/close alternate paths dynamically.
3. Trophy state and timing determine valid exits and win/loss conditions.
4. Maze controller sends pattern commands to the ghost bot.

### 3) Ghost Bot Autonomy Path
1. Ghost bot receives a pattern number.
2. It executes predefined movement patterns using:
   - encoder-based distance/turn calculations,
   - PID-style balancing between wheels,
   - IR-based obstacle/checkpoint detection.
3. Important events are transmitted back to the game controller.

## Hardware/Subsystem Overview

- **Wireless:** NRF24L01 transceivers (player control + maze/ghost coordination)
- **Motion (player bot):** DC motors + motor driver (L298N-style interface in code)
- **Motion (ghost bot):** Dual DC motors with encoder feedback
- **Maze actuation:** Multiple servo motors for dynamic path switching
- **Sensing:** IR sensors for triggers, progression, and exit checks
- **Feedback:** Addressable LED effects and DFPlayer-based audio cues

## Gameplay Logic Summary

- The game starts with an initial maze configuration and trophy objective.
- Triggering defined IR checkpoints changes maze paths and progression state.
- Trophy acquisition enables alternate game phases and affects valid winning routes.
- Exit sensor conditions are evaluated against current state to determine **win** or **loss**.

## Development Notes

- Each folder contains firmware for one hardware node.
- Upload each `.ino` sketch to its corresponding controller board.
- Verify board pin mappings against your wiring before power-up.
- Calibrate IR thresholds, motion distances, and turn constants for your physical build.

## Future Improvements

- Formal protocol definitions for all wireless message types.
- Centralized configuration for pin maps and tuning constants.
- Automated hardware-in-the-loop test scenarios for repeatable validation.
