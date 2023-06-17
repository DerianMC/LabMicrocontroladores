 #!/usr/bin/python3
import serial
import csv 
import numpy 

#Establece ser como serial
ser = serial.Serial(
    port='/dev/ttyACM6',\
    baudrate=9600,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
    timeout=0\
    )

#Abre csv para escritura
with open('output.csv', 'w', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)
    #Envía header de csv
    header = ['aX', 'aY', 'aZ', 'gX', 'gY', 'gZ']
    writer.writerow(header)

    print("connected to: " + ser.portstr)


    line = []

    string = []
    clmn = ""
    i = False
    while True:
        #Inicia para cada valor de serial
        for c in ser.read().decode():
            #Carga caracter en c
            c=str(c)
            
            #Con i en True se inicia string
            #Este string contendrá los caracteres a ingresar en csv
            #Es reiniciado para cuando i en True
            if(i == True):
                string = []
                i = False
            #Se tiene como caracter de para los :
            #Cuando llega este caracter se corta el string
            #Mientras no haya llegado se entra en if
            if(c != ":"): 
                #Cuando llega un ";" se tiene como finalizador del valor 
                #Entonces cuando ";" llega se hace append
                if(c == ";"):
                    string.append(clmn)
                    #Se reinicia clmn
                    clmn = ""
                else:
                    #Cuando no ha llegado ";" se concatenan caracteres de llegada
                    clmn = clmn+c
            else:
                #Cuando llega ":" se corta
                #Se imprime string completo
                print(string)
                clmn = ""
                i = True
            #Cuando el tamaño de string contenga lo requerido se escribe row en csv
            if(i == True):
                writer.writerow(string)
        


        
        
