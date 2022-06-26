/*
 * RemoteControlCar
 *
 * Controll.c
 *
 * Author:     Murilo Chianfa
 * Build date: 2021-06-27 20:00
 */

#include <stdlib.h>
#include <string.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <IRremote.h>

#define CONTROLL_PORT 6
#define TRANSMIT_PORT 9

// Controll buttons
#define BUTTON_POWER 0xFD00FF
#define BUTTON_VOLUME_PLUS 0xFD807F
#define BUTTON_FUNC_STOP 0xFD00FF
#define BUTTON_LEFT_ARROW 0xFD20DF
#define BUTTON_PLAY_PAUSE 0xFDA05F
#define BUTTON_RIGHT_ARROW 0xFD609F
#define BUTTON_DOWN_ARROW 0xFD10EF
#define BUTTON_VOLUME_MINUS 0xFD906F
#define BUTTON_UP_ARROW 0xFD50AF
#define BUTTON_0 0xFD30CF
#define BUTTON_EQ 0xFDB04F
#define BUTTON_ST_REPT 0xFD708F
#define BUTTON_1 0xFD08F7
#define BUTTON_2 0xFD8877
#define BUTTON_3 0xFD48B7
#define BUTTON_4 0xFD28D7
#define BUTTON_5 0xFDA857
#define BUTTON_6 0xFD6897
#define BUTTON_7 0xFD6897
#define BUTTON_8 0xFD9867
#define BUTTON_9 0xFD58A7

LiquidCrystal lcd(12, 10, 5, 4, 3, 2);

IRrecv controll(CONTROLL_PORT);
decode_results command;

enum Packets {
    headlightOff,
    headlightOn,
    turnOnLeftFlashes,
    turnOffLeftFlashes,
    turnOnRightFlashes,
    turnOffRightFlashes,
    turnOnAlertFlashes,
    turnOffAlertFlashes,
    turnMotorOff,
    turnMotorForward,
    turnMotorReverse,
    turnDirectionMiddle,
    turnDirectionLeft,
    turnDirectionRight
};

void transmitPacket(int receivedByte)
{
    Wire.beginTransmission(TRANSMIT_PORT);
    Wire.write(receivedByte);
    Wire.endTransmission();
}

void dispatchButtonAction()
{
    if (!controll.decode(&command))
    {
        return;
    }

    Serial.println(command.value, HEX);
  
    char* pressedButton;
    pressedButton = (char*) malloc(17 * sizeof(char));
  
    switch (command.value)
    {
        case BUTTON_0: 
            transmitPacket(Packets::headlightOff);
            strcpy(pressedButton, "BUTTON_0");      
            break;
        case BUTTON_1: 
            transmitPacket(Packets::headlightOn);
            strcpy(pressedButton, "BUTTON_1");      
            break;
 
        case BUTTON_2: 
            transmitPacket(Packets::turnOnLeftFlashes);
            strcpy(pressedButton, "BUTTON_2");      
            break;
        case BUTTON_3: 
            transmitPacket(Packets::turnOffLeftFlashes);
            strcpy(pressedButton, "BUTTON_3");      
            break;
        case BUTTON_4: 
            transmitPacket(Packets::turnOnRightFlashes);
            strcpy(pressedButton, "BUTTON_4");      
            break;
        case BUTTON_5: 
            transmitPacket(Packets::turnOffRightFlashes);
            strcpy(pressedButton, "BUTTON_5");      
            break;

        case BUTTON_PLAY_PAUSE: 
            transmitPacket(Packets::turnOnAlertFlashes);
            strcpy(pressedButton, "PLAY_PAUSE");        
            break;
      
        case BUTTON_FUNC_STOP: 
            transmitPacket(Packets::turnMotorOff);
            strcpy(pressedButton, "FUNC_STOP");     
            break;
        case BUTTON_VOLUME_PLUS: 
            transmitPacket(Packets::turnMotorForward);
            strcpy(pressedButton, "VOLUME_PLUS");       
            break;
        case BUTTON_VOLUME_MINUS: 
            transmitPacket(Packets::turnMotorReverse);
            strcpy(pressedButton, "VOLUME_MINUS");      
            break;
      
        case BUTTON_LEFT_ARROW: 
            transmitPacket(Packets::turnDirectionLeft);
            strcpy(pressedButton, "LEFT_ARROW");        
            break;
        case BUTTON_RIGHT_ARROW: 
            transmitPacket(Packets::turnDirectionRight);
            strcpy(pressedButton, "RIGHT_ARROW");       
            break;
        case BUTTON_UP_ARROW: 
            transmitPacket(Packets::turnDirectionMiddle);
            strcpy(pressedButton, "UP_ARROW");
            break;
    }
  
    dispatchToDisplay(pressedButton);
  
    free(pressedButton);

    controll.resume();
}

void dispatchToDisplay(char* pressedButton)
{
    lcd.clear();

    lcd.setCursor(1, 0);

    lcd.print("Botao Apertado");

    lcd.setCursor(3, 1);
    lcd.print(pressedButton);
}

void setup()
{
    Serial.begin(9600);

    lcd.begin(16, 2);
    dispatchToDisplay("");

    controll.enableIRIn();

    Wire.begin();
}

void loop()
{
    dispatchButtonAction();

    delay(200);
}

