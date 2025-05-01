#  Pico Stepper Serial REPL

This is a REPL-style serial interface for controlling a stepper motor via a Raspberry Pi Pico. It includes:

- A **Python host script** that discovers and talks to the Pico over USB serial
- A **Pico C++ firmware** that handles stepping, LED control, and command parsing

---

## 🧠 What It Does

From your computer, you can:

- Send simple commands like `start`, `stop`, `step`, and `speed`
- Control LED behavior for debugging/feedback
- Use built-in sine or linear oscillation profiles to modulate stepper speed

The Pico handles:

- Parsing commands over USB serial
- Running a non-blocking stepper control loop on core 1
- Stepping a motor continuously or by discrete steps

---

## 🖥️ Host: Python Script (`host.py`)

**Dependencies:**
```bash
pip install pyserial
```

**Usage:**
```bash
python host.py
```

You should see:
```
Connecting to pico on port : /dev/tty.usbmodemXXXX
Enter a command:
>
```

**Commands:**
- `help` — prints Pico command help
- `sine` — sends a smooth sine-modulated speed signal for 30 seconds
- `linear` — sends a linear ramp up/down pattern for 20 seconds
- Anything else is forwarded as-is to the Pico (e.g. `step 100 cw 5000`)

---

## 🧩 Device: Pico Firmware (`main.cpp`)

**Build with:** [Pico SDK](https://github.com/raspberrypi/pico-sdk)

**Pins:**
- `GP21` → `STEP`
- `GP20` → `DIR`
- `GPIO25` → onboard LED

**Commands the Pico understands:**

```text
start               # starts continuous stepping
stop / s            # stops the motor
speed <delay_us>    # sets stepping delay (lower = faster)
step <steps> <cw/ccw> <delay_us>  # performs fixed-step move
led toggle          # toggles onboard LED
led blink <n>       # blinks LED n times
help                # prints command list
```

Stepper runs on core 1 in a tight loop; command handling happens on core 0.

---

## 🔧 Wiring

This depends on your driver (A4988/DRV8825 etc), but basics are:

- STEP and DIR → driver inputs
- 3.3V and GND → logic power
- Motor power and driver wiring handled externally

---

