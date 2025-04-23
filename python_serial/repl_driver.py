import serial
import serial.tools.list_ports
import time
import math


def find_pico_port_mac():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "Pico" in port.description:
            return port.device
    return None


def read_from_pico():
    # Read until no more data comes in for 0.5s
    start_time = time.time()
    while True:
        if ser.in_waiting:
            response = ser.readline().decode(errors='ignore').strip()
            print("<", response)
            start_time = time.time()  # reset timeout
        elif time.time() - start_time > 0.5:
            break

def write_to_pico(data):
    ser.write((data + "\n").encode()) #convert to bytes


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
            time.sleep(0.05)  # ~20Hz update rate


try:
    port = find_pico_port_mac()       # "/dev/tty.usbmodem212101"
    ser = serial.Serial(port, 115200) ## using timeout of 1 
    print("Connecting to pico on port : " + port)
except Exception as e:
    print("Error:", e)

print(".")    
time.sleep(1)  # Let the Pico finish booting
print(".")    
time.sleep(1) 
print(".")    


print("Enter a command:")
while True:
    user_input = input("> ")
    if(user_input == "exit"):
        print("exiting...")
        break
    if(user_input == "sine"):
        variable_speed(0.3,1000,8000,30)
    if(user_input == "linear"):
        linear_oscillating_speed(1,1000,20,200)
   
    write_to_pico(user_input)
    read_from_pico()


ser.close()