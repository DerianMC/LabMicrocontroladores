 #!/usr/bin/python3
import serial
import csv 
import numpy 
import paho.mqtt.client as mqtt
import json 
import ssl, socket

line = []
data=dict()
broker = "iot.eie.ucr.ac.cr"
port = 1883
topic = "v1/devices/me/telemetry"
username = "STM32-C04938"
password = "obdu7hromba0can6t8pt"


def on_publish(client,userdata,result):             #create function for callback
    print("data published to thingsboard \n")
    pass
client1= mqtt.Client("control1")                    #create client object
client1.on_publish = on_publish                     #assign function to callback
client1.username_pw_set(password)               #access token from thingsboard device
client1.connect(broker,port,keepalive=60)           #establish connection


#Establece ser como serial screen -U /dev/ttyACM0 115200
ser = serial.Serial(
    port='/dev/ttyACM1',\
    baudrate=115200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
    timeout=0\
    )

#Abre csv para escritura
with open('output.csv', 'w', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)
    #Envía header de csv
    header = ['Modo', 'V1', 'V2', 'V3', 'V4']
    writer.writerow(header)

    print("connected to: " + ser.portstr)


    line = []

    string = []
    clmn = ""
    i = False
    while True:
        #Inicia para cada valor de serial
        for c in ser.read():
            #Carga caracter en c
            c=chr(c)
            
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
                payload="{"
                payload+="\"x\":" + string[0] + ","; 
                payload+="\"y\":"+string[1] + ","; 
                payload+="\"z\":"+string[2] + ","; 
                payload+="\"v\":"+string[3]; 
                payload+="}"
                client1.publish(topic, payload)
            #Cuando el tamaño de string contenga lo requerido se escribe row en csv
            if(numpy.size(string) == 5):
                writer.writerow(string)
        


        
        
