/*
 The University of Akron
 Biomedical Engineering Senior Design Course
 Professor: Dr. James Keszenhiemer
 Group 4: Cadence - Circulatory Pulse Simulator Mannequin
 Client: Dr. Lecat
 Student Team: Andrew Soltisz | Emily O-Brien | Gregory Ruff | Olivia Petrey
 
 Author Name:  Andrew Soltisz
 Author Email: soltisz.5@buckeyemail.osu.edu
 Author Phone: 330-569-4138
 Revision: 003
 Last-Edit Date: 05/03/2017 (mm/dd/yyyy)
 
 ERM motors are controlled by PWM signal from Arduino and solenoids are switched using arduino digital-high/low.
 The heart rate is set by a potentiometer on pin A5. The pulse pattern is displayed to the user using a neopixel
 LED on pin 1. The user can cycle through each pulse pattern using a single, 2-state switch on pin 2. This code
 controls 3 ERM (Eccentric Rotating Mass) motors and 3 solenoids. The solenoids are used to simulate visual 
 pulses and the ERM motors are used to simulate palpable pulses. The solenoids are used to simulate the pulses of 
 the external jugular vein. The ERM motors are used to simulate the pulses of the external carotid artery and
 the apical pulse of the heart on the chest. When a healthy pulse is being simulated, the apical pulse occurs more 
 medially, and when a disease pulse is being simulated, the apical pulse occurs more laterally. This behavior
 is why only one ERM motor is activated at a time. 
 
 For future work, implement an IMU to sense the orientation of the mannequin. When the mannequin is upright, only 
 the lower solenoid will activate. When the mannequin is supine, all of the solenoids will be activated. And when 
 the mannequin is oriented somewhere inbetween fully upright and supine, the two lower solenoids will be activated. 
 The timing between atrial and ventricular pulses is currently arbitary (made up). Need to reseach proper timing to 
 make the pulses more accurate. The intensity of the ERM motors is currently arbitrary. Need to research proper pulse 
 intensities to make the pulses more accurate. If solenoids are to be used, their sound needs to be suppressed. The 
 vibrations from the ERM motors also needs to be acutely directed to only where the user will palpate the mannequin. 
 Currently, the ERMs vibrate the entire mannequin and the solenoids are very loud. Proper power management needs to 
 be researched and implemented and wireless communication with the mannequin would be more ideal (possibly using an 
 andoid phone app and wifi for wireless communication). CONTACT THE AUTHOR IF YOU HAVE ANY QUESTIONS.
 */
//********************************************************************************************************************
//LIBRARIES
#include <Adafruit_NeoPixel.h>

//********************************************************************************************************************
//GLOBAL VARIABLES
int dialValue, HBperiod, AVdelay, HBlatency, HR, pulseDelay;
int ERMpulseDuration, lastPos, pulseState;
int number_of_states = 4; //number of pulse states

//********************************************************************************************************************
//FUNCTION DECLARATIONS
void healthyPulse();
void tricuspid_regurgitation();
void tricuspid_stenosis();
void AV_block();
void setHeartRate();
void setPulseState();
void activate_solenoids();
void activate_ERMs(int, boolean);
void stop_solenoids();
void stop_ERMs();

//********************************************************************************************************************
//#DEFINES
#define OFF          0   //PWM: 0   = 0%   duty cycle (no   power)
#define HALFPOWER    127 //PWM: 127 = 50%  duty cycle (half power)
#define PARTIALPOWER 191 //PWM: 191 = 75%  duty cycle (3/4  power)
#define FULLPOWER    255 //PWM: 255 = 100% duty cycle (full power)
#define SOL_inferior 9   //Inferior jugular vein solenoid pin
#define SOL_middle   8   //Middle   jugular vein solenoid pin
#define SOL_superior 7   //Superior jugular vein solenoid pin
#define ERM_carotid  6   //Carotid        ERM motor pin
#define ERM_lateral  5   //Lateral apical ERM motor pin
#define ERM_medial   3   //Medial  apical ERM motor pin
#define stateSwitch  2   //Binary switch pin for changing the pulsatory state
#define NEOPIXEL     11  //Neopixel pin for displaying the state of the program/mannequin
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, NEOPIXEL, NEO_GRB + NEO_KHZ800); //(# of pixels, pin #, ..., ...)

//********************************************************************************************************************
//SETUP
void setup() {
  pinMode(SOL_inferior, OUTPUT);
  pinMode(SOL_middle,   OUTPUT); 
  pinMode(SOL_superior, OUTPUT);
  pinMode(stateSwitch,  INPUT);
  Serial.begin(9600);                 //To view outputs in the serial monitor
  lastPos = digitalRead(stateSwitch); //initialize switch state
  pulseState = 0;                     //initialize pulse state to standby
  pixels.begin();                     //start neopixel address feed
}

//********************************************************************************************************************
//SIMULATION LOOP
void loop() {
  setHeartRate();  //set the heart rate based on potentiometer input
  setPulseState(); //set the pulse pattern based on a 2-state switch input

  switch(pulseState){
    case 0: //standby state
      pixels.setPixelColor(0, pixels.Color(0,0,0));     //Neopixel -> off
      pixels.show();
      //do nothing
      break;
    case 1: //healthy pulse pattern
      pixels.setPixelColor(0, pixels.Color(0,150,0));   //Neopixel -> green
      pixels.show();
      healthyPulse();
      break;
    case 2: //tricuspid regurgitation pulse pattern
      pixels.setPixelColor(0, pixels.Color(150,0,0));   //Neopixel -> red
      pixels.show();
      tricuspid_regurgitation();
      break;
    case 3: //tricuspid stenosis pulse pattern
      pixels.setPixelColor(0, pixels.Color(0,0,150));   //Neopixel -> blue
      pixels.show();
      tricuspid_stenosis();
      break;
    case 4: //tricuspid stenosis pulse pattern
      pixels.setPixelColor(0, pixels.Color(150,0,150)); //Neopixel -> purple
      pixels.show();
      AV_block();
      break;
    default:
      break;
  }
}

//*********************************************************************************************************************
//SET HEART BEAT VARIABLES BASED ON POTENTIOMETER INPUT
void setHeartRate(){
  dialValue = analogRead(A5);                    //Analog input (0-1023 @ 5v) from potentiometer 
  HBperiod = map(dialValue, 0, 1023, 272, 2143); //time(ms) from start of HB to start of next HB
  AVdelay = HBperiod/3;                          //time(ms) from start of atrial pulse and start of ventricular pulse
  pulseDelay = map(HBperiod, 272, 2143, 30, 50); //time(ms) for ERM motors and solenoids to startup (lag time)
  HR = (60000 / HBperiod);                       //heart rate (beats/minute)
}

//********************************************************************************************************************
//SWITCH FUNCTION TO CYCLE THROUGH PULSE PATTERNS
void setPulseState(){
  int switchPos = digitalRead(stateSwitch); 
  if(switchPos != lastPos){         //increment the puslse state everytime the switch position is changed
    ++pulseState;
    lastPos = switchPos; 
  }
  if(pulseState > number_of_states) //return to first pulse state after switching past the last state
    pulseState = 0;
}

//********************************************************************************************************************
//EXTEND ALL SOLENOID PLUNGERS
void activate_solenoids(){ //IMU input to control which solenoids are active still needs to be implemented
  digitalWrite(SOL_inferior, HIGH);        
  digitalWrite(SOL_middle,   HIGH);         
  digitalWrite(SOL_superior, HIGH);          
}

//********************************************************************************************************************
//ACTIVARE ALL ERM MOTORS
void activate_ERMs(int pulseIntensity, boolean heartFailure){
  analogWrite(ERM_carotid,   pulseIntensity); //the carotid ERM motor will always be active
  if(heartFailure) 
    analogWrite(ERM_lateral, pulseIntensity); //during disease states, the lateral ERM motor will be active
  else 
    analogWrite(ERM_medial,  pulseIntensity); //during healthy states, the medial ERM motor will be active
}

//********************************************************************************************************************
//RETRACT ALL SOLENOIDS PLUNGERS
void stop_solenoids(){
  digitalWrite(SOL_inferior, LOW);   
  digitalWrite(SOL_middle,   LOW);  
  digitalWrite(SOL_superior, LOW);  
}

//********************************************************************************************************************
//STOP ALL ERM MOTORS
void stop_ERMs(){
  analogWrite(ERM_carotid, OFF);  
  analogWrite(ERM_lateral, OFF); 
  analogWrite(ERM_medial,  OFF);   
}

//********************************************************************************************************************
void healthyPulse(){
  HBlatency = HBperiod - (2 * pulseDelay) - AVdelay; //time(ms) when their is no pulse activity
  boolean heartFailure = false;
  
  //Atrial Pulse
  activate_solenoids();
  activate_ERMs(FULLPOWER, heartFailure);
  delay(pulseDelay); //wait for plungers to fully extend and ERM motors to reach full speed 
  stop_solenoids();
  stop_ERMs();
  delay(AVdelay);    //wait for ventricular pulse 
  
  //Ventricular Pulse
  activate_solenoids();
  activate_ERMs(FULLPOWER, heartFailure);
  delay(pulseDelay); //wait for plungers to fully extend and ERM motors to reach full speed
  stop_solenoids();
  stop_ERMs();
  delay(HBlatency);  //wait for next heart beat
}

//********************************************************************************************************************
void tricuspid_regurgitation(){
  HBlatency = HBperiod - pulseDelay; //time(ms) when their is no pulse activity
  boolean heartFailure = true;
  
  //Ventrical Pulse (no atrial)
  activate_solenoids();
  activate_ERMs(FULLPOWER, heartFailure);
  delay(pulseDelay); //wait for plungers to fully extend and ERM motors to reach full speed
  stop_solenoids();
  stop_ERMs();
  delay(HBlatency);  //wait for next heart beat
}

//********************************************************************************************************************
void tricuspid_stenosis(){
  HBlatency = HBperiod - (pulseDelay*2) - AVdelay; //time(ms) when their is no pulse activity
  boolean heartFailure = false;
  
  //Atrial Pulse
  activate_solenoids();
  activate_ERMs(FULLPOWER, heartFailure);
  delay(pulseDelay); //wait for plungers to fully extend and ERM motors to reach full speed 
  stop_solenoids();
  stop_ERMs();
  delay(AVdelay);    //wait for ventricular pulse  
  
  //Ventricular Pulse
  activate_solenoids();
  activate_ERMs(HALFPOWER, heartFailure);
  delay(pulseDelay * 0.1); //wait for plungers to fully extend (shorter pulse delays create a weaker pulse)
  stop_solenoids();
  delay(pulseDelay * 0.9); //wait for ERM motors to reach full speed (longer pulse delays create a stronger pulse)
  stop_ERMs();
  delay(HBlatency);        //wait for next heart beat
}

//********************************************************************************************************************
void AV_block(){
  HBlatency = HBperiod -(pulseDelay*2) - AVdelay; //time(ms) when their is no pulse activity
  boolean heartFailure = true;
  
  //Atrial Pulse
  activate_solenoids();
  activate_ERMs(HALFPOWER, heartFailure);
  delay(pulseDelay * 0.1); //wait for plungers to fully extend
  stop_solenoids();
  delay(pulseDelay * 0.9); //wait for ERM motors to reach full speed
  stop_ERMs();
  delay(AVdelay);          //wait for ventricular pulse 
  
  //Ventricular Pulse
  activate_solenoids();
  activate_ERMs(HALFPOWER, heartFailure);
  delay(pulseDelay * 0.1); //wait for plungers to fully extend
  stop_solenoids();
  delay(pulseDelay * 0.9); //wait for ERM motors to reach full speed
  stop_ERMs();
  delay(HBlatency);         //wait for next heart beat
  
  //Atrial Pulse (no ventricular)
  HBlatency = HBperiod - pulseDelay;
  activate_solenoids();
  activate_ERMs(FULLPOWER, heartFailure);
  delay(pulseDelay); //wait for plungers to fully extend and ERM motors to reach full speed
  stop_solenoids();
  stop_ERMs();
  delay(HBlatency);  //wait for next heart beat

  //Atrial Pulse (no ventricular)
  HBlatency = HBperiod - pulseDelay;
  activate_solenoids();
  activate_ERMs(PARTIALPOWER, heartFailure);
  delay(pulseDelay * 0.5);  //wait for plungers to fully extend
  stop_solenoids();
  delay(pulseDelay * 0.5);  //wait for ERM motors to reach full speed
  stop_ERMs();
  delay(HBlatency);         //wait for next heart beat
}
