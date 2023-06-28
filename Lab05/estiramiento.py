#!/usr/bin/python3
import serial.tools.list_ports
import csv

# Get a list of available serial ports
ports = serial.tools.list_ports.comports()

# Choose the first available port
if len(ports) > 0:
    port = ports[0].device
    print("Using port: " + port)
else:
    print("No serial ports found.")
    exit()

# Set up the serial connection
ser = serial.Serial(
    port=port,
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=0
)

# Open the CSV file for writing
with open('estiramiento.csv', 'w', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)
    # Write the CSV header
    header = ['aX', 'aY', 'aZ', 'gX', 'gY', 'gZ']
    writer.writerow(header)

    print("Connected to: " + ser.portstr)

    string = []
    clmn = ""
    i = False
    while True:
        # For each character from the serial port
        for c in ser.read().decode():
            # Load the character into c
            c = str(c)

            # If i is True, initialize the string
            # This string will contain the characters to be written into the CSV
            # It is reset when i is True
            if i == True:
                string = []
                i = False
            # Use ":" as the stop character
            # When ":" is encountered, the string is complete and ready to be processed
            if c != ":":
                # Use ";" as the value delimiter
                # When ";" is encountered, append the value to the string
                if c == ";":
                    string.append(clmn)
                    clmn = ""
                else:
                    # Concatenate incoming characters until ";" is encountered
                    clmn = clmn + c
            else:
                # When ":" is encountered, process the complete string
                print(string)
                clmn = ""
                i = True
            # If the string has the required number of values, write it to the CSV
            if i == True:
                writer.writerow(string)
