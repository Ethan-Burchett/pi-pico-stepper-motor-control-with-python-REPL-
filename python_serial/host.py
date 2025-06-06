import serial
import serial.tools.list_ports
import time
import math

from utils import *

# global serial port var
ser: serial.Serial | None = None 

connect_to_pico()

print("Enter a command:")
while True:
    user_input = input("> ")
    if(user_input == "exit"):
        print("exiting...")
        break
    if(user_input == "reset"):
        connect_to_pico()
   
    write_to_pico(user_input)
    read_from_pico()

ser.close()