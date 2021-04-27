#!/usr/bin/env python3
import pygame as pg
import serial
from datetime import datetime

# Init config
pg.init()
pg.mixer.init()
display = pg.display.set_mode((0, 0), pg.FULLSCREEN)
# ser = serial.Serial("COM3", 9600, timeout=0.01)
ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=0.01)
# display = pg.display.set_mode((1920, 1080))
pg.display.toggle_fullscreen()

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
BLUE = (1, 73, 154)

# Rects
passX = 200
passY = 250

idX = 200
idY = 750

rectW = 400
rectH = 200

lastPassX = 870
lastPassY = 250

listX = 300
listY = 600

# Text
passLbl = "SENHA"
histPass = "SENHA"
idLbl = "BALANÇA"
histId = "BALANÇA"
histTime = "HORA"
senha = ""
id = ""
passArr = [""] * 10
idArr = [""] * 10
timeArr = [""] * 10
print(pg.font.get_fonts())
newFont = '/home/pedro/Desktop/Gestor/arial.ttf'
#newFont = '/home/acougue/Desktop/Gestor/arial.ttf'
# Image
logo = pg.image.load('/home/pedro/Desktop/Gestor/logo.png')
#logo = pg.image.load('/home/acougue/Desktop/Gestor/logo.png')
logoRect = logo.get_rect(center=(lastPassX+listX+950//2, 960))

# Sound
chime = pg.mixer.Sound('/home/pedro/Desktop/Gestor/Door_bell.ogg')
#chime = pg.mixer.Sound('/home/acougue/Desktop/Gestor/Door_bell.ogg')



def write(text, x, y, color, font=128, rectW=400, rectH=200):
    font = pg.font.Font(newFont, font)
    text = font.render(text, True, pg.Color(color))
    textRect = text.get_rect(center = (x + rectW//2, y + rectH//2))
    return text, textRect

def printArr(array, x, y, color, font=54, rectW=listX, rectH=listY):
    font = pg.font.Font(newFont, font)
    array = font.render(array, True, pg.Color(color))
    arrayRect = array.get_rect(center = (x + rectW//2, y))
    return array, arrayRect

def rxText(password):
    passTxt = password.split('-')[0]
    idTxt = password.split('-')[1]
    idTxt = idTxt.strip()
    return passTxt, idTxt

def updateScreen():
    display.fill(WHITE)
    pg.draw.rect(display, BLUE, (passX, passY, rectW, rectH), 10, 40)
    pg.draw.rect(display, BLUE, (idX, idY, rectW, rectH), 10, 40)
    pg.draw.rect(display, BLUE, (lastPassX, lastPassY, listX, listY), 3, 0, 25, 0, 25)
    pg.draw.rect(display, BLUE, (lastPassX+305, lastPassY, listX, listY), 3, 1)
    pg.draw.rect(display, BLUE, (lastPassX+610, lastPassY, listX, listY), 3, 25, 0, 25, 0)
    display.blit(passLblText, passLblRect)
    display.blit(idLblText, idLblRect)
    display.blit(histPass, histPassRect)
    display.blit(histId, histIdRect)
    display.blit(histTime, histTimeRect)
    display.blit(text, textRect)
    display.blit(idText, idRect)
    #display.blit(logo,logoRect)
    soma = 30
    for i in range(10):
        if (passArr[i] == ""):
            continue
        lastId, lastIdRect = printArr(idArr[i], lastPassX, lastPassY+soma, BLACK, 48)
        lastPass, lastPassRect = printArr(passArr[i], lastPassX+305, lastPassY+soma, BLACK, 48)
        lastTime, lastTimeRect = printArr(timeArr[i], lastPassX+610, lastPassY+soma, BLACK, 48)
        soma += 60
        display.blit(lastPass, lastPassRect)
        display.blit(lastId, lastIdRect)
        display.blit(lastTime, lastTimeRect)
    pg.display.update()

# Labels
passLblText, passLblRect = write(passLbl, passX, passY-200, BLACK, 96)
idLblText, idLblRect = write(idLbl, idX, idY-200, BLACK, 96)

histId, histIdRect = write(histId, lastPassX-50, lastPassY-130, BLACK, 48)
histPass, histPassRect = write(histPass, lastPassX+305-50, lastPassY-130, BLACK, 48)
histTime, histTimeRect = write(histTime, lastPassX+610-50, lastPassY-130, BLACK, 48)

cont = 9
main_loop = True

while main_loop:
    USB = ser.readline()
    now = datetime.now()
    
    if (USB):
        senha = USB.decode()
        senha, id = rxText(senha)
        print(id)
        current_time = now.strftime("%H:%M:%S")
        chime.play()
        if (cont >= 0):
            passArr[cont] = senha
            idArr[cont] = id
            timeArr[cont] = current_time
            cont -= 1
        else:
            for i in range(9, 0, -1):
                passArr[i] = passArr[i-1]
                idArr[i] = idArr[i-1]
                timeArr[i] = timeArr[i-1]
            passArr[0] = senha
            idArr[0] = id
            timeArr[0] = current_time
        
    if (senha.find('P') == 0):
        text, textRect = write(senha, passX, passY, RED)
        idText, idRect = write(id, idX, idY, RED)
    
    else:
        text, textRect = write(senha, passX, passY, BLACK)
        idText, idRect = write(id, idX, idY, BLACK)

    for event in pg.event.get():
        if event.type == pg.QUIT:
            main_loop = False
    keys = pg.key.get_pressed()
    if (keys[pg.K_ESCAPE]):
        main_loop = False
    
    updateScreen()

pg.quit()
    