#include <MIDI.h>
#include <math.h>
#include <MagicPot.h>
#define X_PIN A1
#define Y_PIN A0
#define SENS_PIN A2
#define SPEED_PIN A3
MagicPot joyX(X_PIN);
MagicPot joyY(Y_PIN);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

const int CHANNELIN = 1;
const int CHANNELOUT = 2;
const int padding = 100; // Analogread padding of the joystick
const float SR = 5; // sample per SR milliseconds
bool rising = true;
float phase = 0;
unsigned long timenow_m; // overflows when running for 50 days, not yet handled
unsigned long timenow_p;
unsigned long noisetime = 0;
float bend_range = 2; // semitones, currently unused while I figure out how to send the proper midi commands
int pb = 0; // current bend range, from -8192 to 8192
int mod = 0; // stores output of the triangle wave
int nmod = 0;
float noise = 0.0;
int mrange_plus = 0;
int mrange_min = 0;


void setup(){
  // set triggers for midi events to redirect.
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(57600);
  // set handlers for MIDI messages that are triggered by MIDI.read();
  MIDI.setHandleNoteOff(NoteInOff);
  MIDI.setHandleNoteOn(NoteInOn) ;
  MIDI.setHandleControlChange(ControlChangeIn); // for sustain
  joyX.begin();
  joyY.begin();
  timenow_m = millis();
  timenow_p = millis();
  }


void loop(){
  MIDI.read();
  // Calculate bends every 2 ms
  // MIDI has a maximum transfer rate, this value was safe but could be faster
  if ((millis() - timenow_p >= 2)){
    bend();
    timenow_p = millis();
  }
  // Sample the triangle wave and noise every 5ms
  if ((millis() - timenow_m >= SR)){
    float rate = mapfloat(analogRead(SPEED_PIN), 0, 1023, 0.1, 8); // range is 0.1 to 8 Hz.
    nmod = noiseMod(mrange_min);
    mod = TriangleWave2(mrange_plus, rate);
    timenow_m = millis();
  }
 }
  

void bend(){
  int pb_now = pb; // current pitch bend value
  joyX.read(2);   // Read pots with sensitivity of 2 to smooth the noise.
  joyY.read(2);
  int x = joyX.getValue();
  int y = joyY.getValue();
  float sensitivity = mapfloat(analogRead(SENS_PIN), 0, 1023, 0, 1); // Map sensitivity pot to a value between 0.0 and 1.0
  // Padding is the deadzone. The joystick I used doesn't return completely center at all times. 
  // Values are calculated from outside the deadzone
  // Joystick points left
  if (x < (512 - padding * 0.5)){
    pb = map(x, 0, 512 - padding * 0.5, 8191, 0);
    }
  // Joystick points right
  else if (x > (512 + padding * 0.5)){
    pb = map(x, 512 + padding * 0.5, 1023, 0, -8191);
    }
  else {
    pb = 0; // stick is centered within the deadzone
    }
  //Joystick points up
  if (y > (512 + padding*0.5)){
    // mod_range is used for calculating the vibrato bend
    // The sensitivity pot attenuates the modulation range of the joystick.
    mrange_plus = round(map(y, 512+padding*0.5, 1023, 0, 16383) * sensitivity);
    mrange_min = 0;
    }
  else if (y < (512 - padding * 0.5)){
    mrange_min = 16383 - map(y, 0, 512 - (2*padding) * 0.5, 0, 16383);
    mrange_plus = 0;
    }
  else {
    mrange_plus = 0;
    mrange_min = 0;
    }
  // only send pitch bend if it changes
  if (pb_now != pb || mrange_min > 0 || mrange_plus > 0){
    int final_bend = constrain(pb + mod + nmod, -8191, 8191); // add current pitch bend to the modulation value and constrain it
    MIDI.sendPitchBend(final_bend, CHANNELOUT);
  }
}


int TriangleWave2(int range, float rate){
  // Calculate phase change per sample 
  float df = (rate / (1000 / SR)) * 4;
  // The below could have been implemented more gracefully, but it works.
  if (rising){
    phase += df;
    if (phase > 1){
      rising = false;
      }
    }
  else{
    phase = phase - df;
    if (phase < -1){
      rising = true;
      }
    }
  // ensure modulation value doesn't stray outside of boundaries when modulation rate is decreased.
  // Without the constrain it tends to linger outside the boundaries.
  return constrain(round(range * 0.5 * phase), -range * 0.5, range * 0.5); 
  }

int noiseMod(int range){
  noise = random(-10000, 10000) / 10000.0; // floating point within -1 to 1 with 20K steps
  return constrain(round(range * 0.5 * noise), -range * 0.5, range * 0.5); 
  }

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  // Built-in map() only maps to integers, found this function here:
  // https://forum.arduino.cc/t/map-to-floating-point/3976
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void NoteInOff(byte channel, byte note, byte velocity){
  MIDI.sendNoteOff(note, velocity, CHANNELOUT);  
 }


void ControlChangeIn(byte channel, byte control, byte value){
  if (control == 64){
    // Only repeat sustain. Expression pedal on external keyboard is interpreted
    // as volume automatically, regardless of settings on the Electro panel
    MIDI.sendControlChange(control, value, CHANNELOUT); // 
  }
}

  
void NoteInOn(byte channel, byte note, byte velocity){
   MIDI.sendNoteOn(note, velocity, CHANNELOUT);  
 }
