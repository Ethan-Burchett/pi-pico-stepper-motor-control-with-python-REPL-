import serial
import time

try:
    port = "/dev/tty.usbmodem832101"
    ser = serial.Serial(port, 115200) ## using timeout of 1 
except Exception as e:
    print("Error:", e)
    
time.sleep(2)  # Let the Pico finish booting

ser.write(b"led toggle\n")  # Send the command
response = ser.readline().decode().strip()
print("Pico says:", response)

ser.close()