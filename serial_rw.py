import serial
import threading
import sys

try:
    port = "/dev/tty.usbmodem832101"
    ser = serial.Serial(port, 115200) ## using timeout of 1 
except Exception as e:
    print("Error:", e)
    sys.exit(1) # quit script
    


def reader():
    while True:
        try:
            line = ser.readline()
            if line:
                print("Got:", line.decode().strip())
        except Exception as e:
            print("Error:", e)
            break

def writer():
    while True:
        cmd = input("Send to Pico: ")
        ser.write((cmd + '\n').encode())

# Start both loops in parallel
threading.Thread(target=reader, daemon=True).start()
writer()