import serial
import time

try:
    port = "/dev/tty.usbmodem832101"
    ser = serial.Serial(port, 115200) ## using timeout of 1 
except Exception as e:
    print("Error:", e)
    
time.sleep(2)  # Let the Pico finish booting

print("Enter a command:")
while True:
    user_input = input("> ")
    #ser.write(b"led toggle\n")  # Send the command
    if(user_input == "exit"):
        print("exiting...")
        break
    ser.write((user_input + "\n").encode()) #convert to bytes
    response = ser.readline().decode().strip()
    print("<", response)


ser.close()