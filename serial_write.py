import serial
import time

port = "/dev/tty.usbmodem832101"
ser = serial.Serial(port, 115200, timeout=1) ## using timeout of 1 


print("Sending data to Pico...")
time.sleep(2) # Give time for Pico to finish reset

for i in range(100):
    
    ser.write(b'1')  # Send a single character
# ser.write(b'hello\n')  # Send a full line

ser.close()
