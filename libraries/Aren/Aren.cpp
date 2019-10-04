// Check whether to include the Arduino Library
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif

// Include headers
#include <Servo.h>
#include <Aren.h>
#include <Oscillator.h>
#include <US.h>

void Aren::init(int YL, int YR, int RL, int RR, bool load_calibration, int NoiseSensor, int Buzzer, int USTrigger, int USEcho)
{
  servo_pins[0] = YL;
  servo_pins[1] = YR;
  servo_pins[2] = RL;
  servo_pins[3] = RR;

  attachServos();
  isResting = false;

  if (load_calibration)
  {
    for (int i = 0; i < 4; i++)
    {
      int servo_trim = EEPROM.read(i);
      if (servo_trim > 128)
        servo_trim -= 256;
      servo[i].SetTrim(servo_trim);
    }
  }

  for (int i = 0; i < 4; i++)
    servo_position[i] = 90;

  //US sensor init with the pins:
  us.init(USTrigger, USEcho);

  //Buzzer & noise sensor pins:
  pinBuzzer = Buzzer;
  pinNoiseSensor = NoiseSensor;

  pinMode(Buzzer, OUTPUT);
  pinMode(NoiseSensor, INPUT);
}

///////////////////////////////////////////////////////////////////
//-- ATTACH & DETACH FUNCTIONS ----------------------------------//
///////////////////////////////////////////////////////////////////
void Aren::attachServos()
{
  servo[0].attach(servo_pins[0]);
  servo[1].attach(servo_pins[1]);
  servo[2].attach(servo_pins[2]);
  servo[3].attach(servo_pins[3]);
}

void Aren::detachServos()
{
  servo[0].detach();
  servo[1].detach();
  servo[2].detach();
  servo[3].detach();
}

///////////////////////////////////////////////////////////////////
//-- OSCILLATORS TRIMS ------------------------------------------//
///////////////////////////////////////////////////////////////////
void Aren::setTrims(int YL, int YR, int RL, int RR)
{
  servo[0].SetTrim(YL);
  servo[1].SetTrim(YR);
  servo[2].SetTrim(RL);
  servo[3].SetTrim(RR);
}

void Aren::saveTrimsOnEEPROM()
{

  for (int i = 0; i < 4; i++)
  {
    EEPROM.write(i, servo[i].getTrim());
  }
}

///////////////////////////////////////////////////////////////////
//-- BASIC MOTION FUNCTIONS -------------------------------------//
///////////////////////////////////////////////////////////////////
void Aren::_moveServos(int time, int servo_target[])
{
  attachServos();
  if (getRestState() == true)
  {
    setRestState(false);
  }

  if (time > 10)
  {
    for (int i = 0; i < 4; i++)
    {
      increment[i] = ((servo_target[i]) - servo_position[i]) / (time / 10.0);
    }
    final_time = millis() + time;

    for (int iteration = 1; millis() < final_time; iteration++)
    {
      partial_time = millis() + 10;
      for (int i = 0; i < 4; i++)
      {
        servo[i].SetPosition(servo_position[i] + (iteration * increment[i]));
      }
      while (millis() < partial_time)
      {
      }; //pause
    }
  }
  else
  {
    for (int i = 0; i < 4; i++)
    {
      servo[i].SetPosition(servo_target[i]);
    }
  }
  for (int i = 0; i < 4; i++)
  {
    servo_position[i] = servo_target[i];
  }
}

void Aren::oscillateServos(int A[4], int O[4], int T, double phase_diff[4], float cycle = 1)
{
  for (int i = 0; i < 4; i++)
  {
    servo[i].SetO(O[i]);
    servo[i].SetA(A[i]);
    servo[i].SetT(T);
    servo[i].SetPh(phase_diff[i]);
  }

  double ref = millis();
  for (double x = ref; x <= T * cycle + ref; x = millis())
  {
    for (int i = 0; i < 4; i++)
    {
      servo[i].refresh();
    }
  }
}

void Aren::_execute(int A[4], int O[4], int T, double phase_diff[4], float steps = 1.0)
{
  attachServos();
  if (getRestState() == true)
  {
    setRestState(false);
  }

  int cycles = (int)steps;

  //-- Execute complete cycles
  if (cycles >= 1)
  {
    for (int i = 0; i < cycles; i++)
    {
      oscillateServos(A, O, T, phase_diff);
    }
  }
  //-- Execute the final not complete cycle
  oscillateServos(A, O, T, phase_diff, (float)steps - cycles);
}

///////////////////////////////////////////////////////////////////
//-- HOME = Otto at rest position -------------------------------//
///////////////////////////////////////////////////////////////////
void Aren::home()
{
  if (isResting == false)
  { //Go to rest position only if necessary

    int homes[4] = {90, 90, 90, 90}; //All the servos at rest position
    _moveServos(500, homes);         //Move the servos in half a second

    detachServos();
    isResting = true;
  }
}

bool Aren::getRestState()
{
  return isResting;
}

void Aren::setRestState(bool state)
{
  isResting = state;
}

///////////////////////////////////////////////////////////////////
//-- SENSORS FUNCTIONS  -----------------------------------------//
///////////////////////////////////////////////////////////////////

//---------------------------------------------------------
//-- getDistance: return ultrasonic sensor measure
//---------------------------------------------------------
float Aren::getDistance()
{
  return us.read();
}

//---------------------------------------------------------
//-- getNoise: return noise sensor measure
//---------------------------------------------------------
int Aren::getNoise()
{
  int noiseLevel = 0;
  int noiseReadings = 0;
  int numReadings = 2;

  noiseLevel = analogRead(pinNoiseSensor);

  for (int i = 0; i < numReadings; i++)
  {
    noiseReadings += analogRead(pinNoiseSensor);
    delay(4); // delay in between reads for stability
  }

  noiseLevel = noiseReadings / numReadings;

  return noiseLevel;
}

///////////////////////////////////////////////////////////////////
//-- SOUNDS -----------------------------------------------------//
///////////////////////////////////////////////////////////////////

void Aren::_tone(float noteFrequency, long noteDuration, int silentDuration)
{
  if (silentDuration == 0)
  {
    silentDuration = 1;
  }

  tone(Otto::pinBuzzer, noteFrequency, noteDuration);
  delay(noteDuration); //milliseconds to microseconds
  //noTone(PIN_Buzzer);
  delay(silentDuration);
}

void Aren::bendTones(float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration)
{

  //Examples:
  //  bendTones (880, 2093, 1.02, 18, 1);
  //  bendTones (note_A5, note_C7, 1.02, 18, 0);

  if (silentDuration == 0)
  {
    silentDuration = 1;
  }

  if (initFrequency < finalFrequency)
  {
    for (int i = initFrequency; i < finalFrequency; i = i * prop)
    {
      _tone(i, noteDuration, silentDuration);
    }
  }
  else
  {
    for (int i = initFrequency; i > finalFrequency; i = i / prop)
    {
      _tone(i, noteDuration, silentDuration);
    }
  }
}

void Aren::sing(int songName)
{
  switch (songName)
  {

  case S_connection:
    _tone(note_E5, 50, 30);
    _tone(note_E6, 55, 25);
    _tone(note_A6, 60, 10);
    break;

  case S_disconnection:
    _tone(note_E5, 50, 30);
    _tone(note_A6, 55, 25);
    _tone(note_E6, 50, 10);
    break;

  case S_buttonPushed:
    bendTones(note_E6, note_G6, 1.03, 20, 2);
    delay(30);
    bendTones(note_E6, note_D7, 1.04, 10, 2);
    break;

  case S_mode1:
    bendTones(note_E6, note_A6, 1.02, 30, 10); //1318.51 to 1760
    break;

  case S_mode2:
    bendTones(note_G6, note_D7, 1.03, 30, 10); //1567.98 to 2349.32
    break;

  case S_mode3:
    _tone(note_E6, 50, 100); //D6
    _tone(note_G6, 50, 80);  //E6
    _tone(note_D7, 300, 0);  //G6
    break;

  case S_surprise:
    bendTones(800, 2150, 1.02, 10, 1);
    bendTones(2149, 800, 1.03, 7, 1);
    break;

  case S_OhOoh:
    bendTones(880, 2000, 1.04, 8, 3); //A5 = 880
    delay(200);

    for (int i = 880; i < 2000; i = i * 1.04)
    {
      _tone(note_B5, 5, 10);
    }
    break;

  case S_OhOoh2:
    bendTones(1880, 3000, 1.03, 8, 3);
    delay(200);

    for (int i = 1880; i < 3000; i = i * 1.03)
    {
      _tone(note_C6, 10, 10);
    }
    break;

  case S_cuddly:
    bendTones(700, 900, 1.03, 16, 4);
    bendTones(899, 650, 1.01, 18, 7);
    break;

  case S_sleeping:
    bendTones(100, 500, 1.04, 10, 10);
    delay(500);
    bendTones(400, 100, 1.04, 10, 1);
    break;

  case S_happy:
    bendTones(1500, 2500, 1.05, 20, 8);
    bendTones(2499, 1500, 1.05, 25, 8);
    break;

  case S_superHappy:
    bendTones(2000, 6000, 1.05, 8, 3);
    delay(50);
    bendTones(5999, 2000, 1.05, 13, 2);
    break;

  case S_happy_short:
    bendTones(1500, 2000, 1.05, 15, 8);
    delay(100);
    bendTones(1900, 2500, 1.05, 10, 8);
    break;

  case S_sad:
    bendTones(880, 669, 1.02, 20, 200);
    break;

  case S_confused:
    bendTones(1000, 1700, 1.03, 8, 2);
    bendTones(1699, 500, 1.04, 8, 3);
    bendTones(1000, 1700, 1.05, 9, 10);
    break;

  case S_fart1:
    bendTones(1600, 3000, 1.02, 2, 15);
    break;

  case S_fart2:
    bendTones(2000, 6000, 1.02, 2, 20);
    break;

  case S_fart3:
    bendTones(1600, 4000, 1.02, 2, 20);
    bendTones(4000, 3000, 1.02, 2, 20);
    break;
  }
}