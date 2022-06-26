/*
 * RemoteControlCar
 *
 * Car.cpp
 *
 * Author:     Murilo Chianfa
 * Build date: 2021-06-27 20:00
 */

#include <Wire.h>
#include <Servo.h>

#define SERVO_MOTOR 3
Servo servoMotor;

#define MOTOR_DC 11
#define MOTOR_DC_REVERSE 9

#define HEADLIGHTS 2

#define LEFT_FLASHES 4
#define RIGHT_FLASHES 5

#define BREAK_LIGHT 6
#define TAILLIGHT 7

using namespace std;

enum State {
    off,
    on
};

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

void swapState(int *var)
{
    if (State::off == *var)
    {
        *var = State::on;
        return;
    }

    *var = State::off;
}

class DirectionController
{
    public:
        int directionState;

        enum DirectionControllerState {
            middle = 90,
            left = 120,
            right = 60
        };

        DirectionController()
        {
            directionState = DirectionControllerState::middle;
        }
  
        void dispatch()
        {
            servoMotor.write(directionState);
        }
  
        void turnMiddle()
        {
            directionState = DirectionControllerState::middle;
        }

        void turnLeft()
        {
            directionState = DirectionControllerState::left;
        }
  
        void turnRight()
        {
            directionState = DirectionControllerState::right;
        }
};

DirectionController directionController;

class MainMotor
{
    public:
        int motorState;

        enum MotorMarchState {
            off = 0,
            forward = 254,
            reverse = -254
        };

        MainMotor()
        {
            digitalWrite(MOTOR_DC_REVERSE, State::on);
            motorState = MotorMarchState::off;
        }
  
        void dispatch()
        {
            analogWrite(MOTOR_DC, motorState);
        }
  
        void turnForward()
        {
            digitalWrite(TAILLIGHT, State::off);
            digitalWrite(TAILLIGHT, State::off);
          
            digitalWrite(MOTOR_DC_REVERSE, State::on);
            motorState = MotorMarchState::forward;
        }

        void turnReverse()
        {
            digitalWrite(TAILLIGHT, State::on);
            digitalWrite(TAILLIGHT, State::on);
          
            digitalWrite(MOTOR_DC_REVERSE, State::off);
            motorState = MotorMarchState::reverse;
        }
  
        void turnOff()
        {
            digitalWrite(TAILLIGHT, State::off);
            digitalWrite(TAILLIGHT, State::off);

            digitalWrite(MOTOR_DC_REVERSE, State::off);
            motorState = MotorMarchState::off;
        }
};

MainMotor mainMotor;

class Headlights
{
    public:
        int headlightsState;
  
        Headlights()
        {
            headlightsState = State::off;
        }
  
        void dispatch()
        {
            if (State::on == headlightsState)
            {
                digitalWrite(HEADLIGHTS, State::on);
                return;
            }
          
            digitalWrite(HEADLIGHTS, State::off);
            return;
        }
  
        void turnOn()
        {
            headlightsState = State::on;
        }
  
        void turnOff()
        {
            headlightsState = State::off;
        }
};

Headlights headlights;

class Flashes
{
    public:
        int leftFlashState;
        int rightFlashState;
  
        Flashes()
        {
            leftFlashState = State::off;
            rightFlashState = State::off;

            currentLeftState = State::off;
            currentRightState = State::off;
        }

        void blink()
        {
            blinkLeftFlesh();
            blinkRightFlesh();
        }

        void turnOn(char side)
        {
            if ('a' == side)
            {
                leftFlashState = State::on;
                currentLeftState = State::on;

                rightFlashState = State::on;
                currentRightState = State::on;
                return;
            }

            if ('l' == side)
            {
                leftFlashState = State::on;
                currentLeftState = State::on;
              
                turnOff('r');
                return;
            }

            rightFlashState = State::on;
            currentRightState = State::on;
          
            turnOff('l');
            return;
        }

        void turnOff(char side)
        {
            if ('a' == side)
            {
                leftFlashState = State::off;
                currentLeftState = State::off;

                rightFlashState = State::off;
                currentRightState = State::off;
                return;
            }

            if ('l' == side)
            {
                leftFlashState = State::off;
                currentLeftState = State::off;
                return;
            }

            rightFlashState = State::off;
            currentRightState = State::off;
            return;
        }

    private:
        int currentLeftState;
        int currentRightState;

        void blinkLeftFlesh()
        {
            if (State::on == currentLeftState)
            {
                if (State::off == leftFlashState)
                {
                    return;
                }

                swapState(&currentLeftState);
                digitalWrite(LEFT_FLASHES, State::on);
                return;
            }

            swapState(&currentLeftState);
            digitalWrite(LEFT_FLASHES, State::off);
            return;
        }
  
        void blinkRightFlesh()
        {
            if (State::on == currentRightState)
            {
                if (State::off == rightFlashState)
                {
                    return;
                }

                swapState(&currentRightState);
                digitalWrite(RIGHT_FLASHES, State::on);
                return;
            }

            swapState(&currentRightState);
            digitalWrite(RIGHT_FLASHES, State::off);
            return;
        }
};

Flashes flashes;

void dispatchPacket(char receivedPacket)
{
    switch (receivedPacket)
    {
        case Packets::headlightOff:         headlights.turnOff(); break;
        case Packets::headlightOn:          headlights.turnOn();  break;
        case Packets::turnOnLeftFlashes:    flashes.turnOn('l'); break;
        case Packets::turnOffLeftFlashes:   flashes.turnOff('l'); break;
        case Packets::turnOnRightFlashes:   flashes.turnOn('r'); break;
        case Packets::turnOffRightFlashes:  flashes.turnOff('r'); break;
        case Packets::turnOnAlertFlashes:   flashes.turnOn('a'); break;
        case Packets::turnMotorOff:         mainMotor.turnOff(); break;
        case Packets::turnMotorForward:     mainMotor.turnForward(); break;
        case Packets::turnMotorReverse:     mainMotor.turnReverse(); break;
        case Packets::turnDirectionMiddle:  directionController.turnMiddle(); break;
        case Packets::turnDirectionLeft:    directionController.turnLeft(); break;
        case Packets::turnDirectionRight:   directionController.turnRight(); break;
    }
}

void receiveEvent(int bytes)
{
    dispatchPacket(Wire.read());
}

void setup()
{
    Serial.begin(9600);

    pinMode(HEADLIGHTS, OUTPUT);
    pinMode(LEFT_FLASHES, OUTPUT);
    pinMode(RIGHT_FLASHES, OUTPUT);
    pinMode(BREAK_LIGHT, OUTPUT);
    pinMode(TAILLIGHT, OUTPUT);
    pinMode(MOTOR_DC_REVERSE, OUTPUT);
    
    digitalWrite(MOTOR_DC_REVERSE, State::on);
  
    Wire.begin(9);
    servoMotor.attach(SERVO_MOTOR, 500, 2500);
    pinMode(MOTOR_DC, OUTPUT);
  
    flashes.turnOn('l');
    headlights.turnOn();
    mainMotor.turnForward();
    directionController.turnMiddle();
}

void loop()
{
    Wire.onReceive(receiveEvent);

    flashes.blink();
    headlights.dispatch();
    mainMotor.dispatch();
    directionController.dispatch();

    delay(200);
}

