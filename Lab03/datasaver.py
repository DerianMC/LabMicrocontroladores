 #!/usr/bin/python3
import serial
import csv 
import numpy 

ser = serial.Serial(
    port='/tmp/ttyS1',\
    baudrate=9600,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
    timeout=0\
    )
#ser=serial.Serial('/tmp/ttyS1', 9600)
#f= open('output.csv', 'w+')

with open('output.csv', 'w', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)
    header = ['Modo', 'V1', 'V2', 'V3', 'V4']
    writer.writerow(header)

    print("connected to: " + ser.portstr)


    line = []

    string = []
    clmn = ""
    i = False
    while True:
        
        for c in ser.read():
            c=chr(c)
            #print(c, end='')
            if(i == True):
                string = []
                i = False
            if(c != ":"): 
                if(c == ";"):
                    string.append(clmn)
                    clmn = ""
                else:
                    clmn = clmn+c
            else:
                print(string)
                clmn = ""
                i = True
            if(numpy.size(string) == 5):
                writer.writerow(string)
            '''if(c != ":"):
                break
            else:
                writer.writerow(string)'''
        


        
        
