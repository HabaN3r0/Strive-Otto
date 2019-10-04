# Notes

## Aren.cpp
Aren.init(int YL, int YR, int Rl, int RR, bool load_calibration, int NoiseSensor, int Buzzer, int USTrigger, int USEcho)

*Attach/Detach Servos*
Aren.attachServos()

Aren.detachServos()

*Oscillator Trims*
Aren.setTrims(int YL, int YR, int RL, int RR)

Aren.saveTrimsOnEEPROM()

*Basic Motion Functions*
Aren._moveServos(int time, int servo_target[])

Aren.oscillateServos(int A[4], int 0[4], int T, double phase_diff[4], float cycle = 1)

Aren._execute(int A[4], int 0[4], int T, double phase_diff[4], float steps = 1.0)

*Setting rest states*
Aren.home()

Aren.getRestState()

Aren.setRestState(bool state)

*Sensor functions*
Aren.getDistance()

Aren.getNoise()

*Sounds*
Aren._tone(float noteFrequency, long noteDuration, int silentDuration)

Aren.bendTones(float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration)

Aren.sing(int songName)