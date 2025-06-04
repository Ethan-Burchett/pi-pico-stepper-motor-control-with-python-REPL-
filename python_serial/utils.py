import serial
import serial.tools.list_ports
import time
import math

def write_to_pico(data):
    ser.write((data + "\n").encode()) #convert to bytes


def linear_oscillating_speed(
    min_delay_us=500, max_delay_us=10000, duration_sec=10, steps_per_phase=100):
    print(f"📈 Sweeping speed linearly between {max_delay_us} ↔ {min_delay_us} for {duration_sec} seconds...")
    start_time = time.time()

    # Create a linear ramp of delay values
    down_ramp = list(range(max_delay_us, min_delay_us, -((max_delay_us - min_delay_us) // steps_per_phase)))
    up_ramp = list(range(min_delay_us, max_delay_us, ((max_delay_us - min_delay_us) // steps_per_phase)))
    full_cycle = down_ramp + up_ramp

    while time.time() - start_time < duration_sec:
        for delay in full_cycle:
            write_to_pico(f"speed {delay}")
            print("delay:", delay)
            time.sleep(0.05)  # ~20Hz update rate

def variable_speed(frequency_hz=0.2, min_delay_us=500, max_delay_us=10000, duration_sec=10):
    print(f"🎛️ Running sine wave speed control for {duration_sec} seconds...")

    start_time = time.time()
    while time.time() - start_time < duration_sec:
        t = time.time() - start_time
        # Create a smooth oscillation from -1 to 1
        sine = math.sin(2 * math.pi * frequency_hz * t)
        # Map to delay range (inverted: higher delay = slower speed)
        delay = int((sine + 1) / 2 * (max_delay_us - min_delay_us) + min_delay_us)
        
        write_to_pico(f"speed {delay}")
        time.sleep(0.05)  # ~20Hz update rate