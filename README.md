# Arduino Tetris

A Tetris game built with an sum fake arduino board my teacher gave me but its the same as arduino, a LOL Shield 14×9 LED matrix, and a Python keyboard controller because serial monitor sucks for this typa stuff.

## Features

- Tetris gameplay on a physical LED matrix
- Keyboard controls through Python controller
- Piece movement, rotation, line clearing.
- Hard drop and soft drop controls

## Hardware Requirements

- Arduino-compatible WAVGAT/LGT8F board (any arduino like board)
- LOL Shield 14×9 LED matrix (Prob works on other LED Matrixes you can edit dimensions and other necessary stuff although you'll need a different lib)

## Software Requirements

- Arduino IDE
- Compatible LGT8F board core
- LOL Shield library (`Charliplexing.h`)
- Python 3
- `pygame-ce` or `pygame`
- `pyserial`

## Installation

### 1. Set up the Arduino

Install necessary stuff and libs.

Open `Arduino/Tetris/Tetris.ino` in Arduino IDE, select the correct port, and upload the sketch.

### 2. Install Python dependencies

Open a terminal and find the project directory and run:

```bash
python -m pip install -r requirements.txt
```

### 3. Configure the serial port

Connect the Arduino to your computer via USB.

Find the correct serial port in Arduino IDE and configure the Python controller to use that port. The Arduino sketch communicates at **115200 baud**.

Close Arduino Serial Monitor before starting the Python controller.

### 4. Start the game

Run:

```bash
python Python/tetris_controller.py
```

## Controls

The Python controller sends keyboard actions to the Arduino over serial communication. The expected Arduino commands are:

| Command | Action |
|---|---|
| `L` | Move left |
| `D` | Move right |
| `W` | Rotate |
| `N` | Move down |
| `F` | Hard drop |
| `S` / `s` | Start / stop soft drop |
| `R` | Restart after game over |

The actual keyboard bindings depend on the Python controller configuration.

## Notes

This project requires compatible physical hardware. The Python controller alone cannot run the game on the LED matrix without a connected and programmed Arduino board.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

You are free to use, modify, and distribute this project in accordance with the license terms.
