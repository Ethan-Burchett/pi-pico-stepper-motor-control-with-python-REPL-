import serial
import time
import sys

try:
    port = "/dev/tty.usbmodem832101"
    ser = serial.Serial(port, 115200) ## using timeout of 1 
except Exception as e:
    print("Error:", e)
    sys.exit(1) # quit script
    

print("in serial script reading from" + port)
time.sleep(2) # Give time for Pico to finish reset

print("Reading...")
while True:
    try:
        line = ser.readline()
        if line:
            print("Got:", line.decode().strip())
    except Exception as e:
        print("Error:", e)
        break



