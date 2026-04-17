# ZedProjects - Zynq-7000 FPGA Applications

A collection of embedded applications for Xilinx Zynq-7000 FPGA boards (ZedBoard/similar) featuring dual-channel AXI GPIO designs.

## 📁 Projects

### dual_channel_axi_gpio
Main hardware platform with dual-channel AXI GPIO (8 switches input, 8 LEDs output).

**Applications included:**

#### 1. **8leds_8sws_gpio** - Basic GPIO Mirror
Simple demonstration that mirrors switch states directly to LEDs.

#### 2. **Binary Counter** - Interactive Counter Demo
- Configurable speed and direction
- Multiple counting patterns (binary, odd/even, powers of 2, Fibonacci)
- Pause/resume and reset functionality
- 5 pattern modes controlled by switches

#### 3. **Knight Rider / Cylon Scanner** - LED Animation Suite
- 8 different visual effects including classic Knight Rider sweep
- Adjustable speed and LED width
- Multiple chase patterns and sparkle effects
- Auto-cycling demo mode

#### 4. **Reaction Time Game** - Test Your Reflexes
- Random LED challenges with timing measurement
- 4 difficulty levels (1-5 second timeouts)
- Sequential and random modes
- Score tracking with accuracy percentage

#### 5. **Binary Trainer** - Educational Quiz Game
- 4 game modes: Decimal→Binary, Binary→Decimal, Free Play, Pattern Match
- Easy (4-bit) and Hard (8-bit) difficulty levels
- Real-time feedback and score tracking
- Perfect for learning binary number systems

## 🛠️ Hardware Requirements

- **Board**: Xilinx Zynq-7000 based board (ZedBoard or compatible)
- **FPGA**: XC7Z020-CLG484
- **Peripherals**:
  - 8x DIP switches (input)
  - 8x LEDs (output)
  - UART connection for console output

## 💻 Software Requirements

- Xilinx Vivado (2020.1 or later)
- Xilinx Vitis IDE
- UART terminal (115200 baud, 8N1)

## 🚀 Getting Started

### 1. Clone the Repository
```bash
git clone <your-repo-url>
cd ZedProjects/dual_channel_axi_gpio
```

### 2. Build Hardware Design
1. Open `dual_channel_axi_gpio.xpr` in Vivado
2. Generate bitstream
3. Export hardware (including bitstream)

### 3. Build Software Applications
1. Open Vitis IDE
2. Import the platform and application projects
3. Build desired application
4. Program FPGA and run

### 4. Connect UART
- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1
- Flow control: None

## 📖 Application Usage

### Binary Counter
- **SW0**: Pause/Resume
- **SW1**: Direction (up/down)
- **SW2-SW3**: Speed control
- **SW4**: Reset counter
- **SW5-SW7**: Pattern mode selection

### Knight Rider
- **SW0-SW2**: Animation speed (8 levels)
- **SW3-SW4**: LED width (1-4 LEDs)
- **SW5-SW7**: Effect selection (8 effects)

### Reaction Time Game
- **SW0**: Start game
- **SW1**: Reset score
- **SW5**: Game mode (Sequential/Random)
- **SW6-SW7**: Difficulty level
- **Game**: Flip corresponding switch when LED lights up!

### Binary Trainer
- **SW0**: Submit answer / Next question
- **SW1**: Show hint
- **SW5**: Difficulty (Easy/Hard)
- **SW6-SW7**: Game mode selection

## 🔧 Key Features

- ✅ C99 compliant code
- ✅ No external dependencies beyond Xilinx BSP
- ✅ Comprehensive error handling
- ✅ Real-time UART feedback
- ✅ Educational and entertaining applications
- ✅ Configurable difficulty and modes

## 📝 Notes

- All applications use GPIO Device ID `0` (first AXI GPIO instance)
- LEDs display on Channel 2, Switches read from Channel 1
- Each application is standalone and can run independently
- UART output provides instructions and real-time feedback

## 🤝 Contributing

Feel free to submit issues, fork the repository, and create pull requests for any improvements.

## 📄 License

This project is for educational purposes.

## 👤 Author

**JGEALON**

---

**Hardware Platform**: Zynq-7000 PS7 Cortex-A9 @ 666 MHz
**OS**: Standalone (Bare-metal)
**Last Updated**: April 2026
