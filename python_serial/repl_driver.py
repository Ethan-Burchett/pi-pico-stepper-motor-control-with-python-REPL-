import serial
import serial.tools.list_ports
import time


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
    ser.write((user_input + "\n").encode()) #convert to bytes
    # response = ser.readline().decode().strip()
    # print("<", response)
    read_from_pico()


ser.close()