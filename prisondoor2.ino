//An Arduino program to control the keypad, lasers, and the laser sensors at the exit door for Escape from Bleak Prison
//When the correct combo is entered through the keypad, the lasers are shutoff.

int piOut_pin = 2; //signal pin to raspberry pi
int piIn_pin = 11; //listens for input from the raspberry pi for reset
int key_pin = 3; //keypad input
int piezo_pin = 4; //output pin for the piezo speaker
int ls_pin1 = 5; //sensor for laser output 1
int ls_pin2 = 6; //sensor for laser output 2
int ls_pin3 = 7; //sensor for laser output 3
int laser_pin1 = 8; //laser output
int led_pin = 10; //led warning light

bool correct = true; //boolean for if the key combo is correct

void setup() {
  pinMode(key_pin, INPUT_PULLUP); //all inputs are pulledup as we are listening for low
  pinMode(ls_pin1, INPUT_PULLUP);
  pinMode(ls_pin2, INPUT_PULLUP);
  pinMode(ls_pin3, INPUT_PULLUP);
  pinMode(piIn_pin, INPUT_PULLUP);
  pinMode(laser_pin1, OUTPUT);
  pinMode(piezo_pin, OUTPUT);
  pinMode(piOut_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);

  //begins with the lasers off
  digitalWrite(led_pin, LOW);
  digitalWrite(laser_pin1, LOW);
  digitalWrite(piOut_pin, HIGH);
  Serial.begin(9600);
  Serial.println("Begin");
}

bool correctKey() //function for checking if we are receiving correct symbol from keypad. Checks it for half a second. Return true if the correct keypad code has been entered. Else return false.
{
  for (int i = 0; i < 5; i++)
  {
    if (digitalRead(key_pin) == 1)
      return false;
    else
      delay(100);
  }
  return true;
}

bool correctPi() //function for checking if we are receiving reset signal from the raspberry pi. Checks it for a second. Return true if the signal from raspberry pi indicates reset. Else return false
{
  for (int i = 0; i < 10; i++)
  {
    if (digitalRead(piIn_pin) == 1)
      return false;
    else
      delay(100);
  }
  return true;
}

void shutOff() //plays deactivation sound, shut off lasers, turn off warning light, signal the pi, stop checking for sensors
{
  if (correct == false) //make sure to only run this once
  {
    success();
    correct = true;
    digitalWrite(laser_pin1, LOW); //shut off laser
    digitalWrite(piOut_pin, LOW); //signal the pi
    digitalWrite(led_pin, LOW); //turn off the warning light
  }
}

void success () //plays deactivation sounds from the piezo
{
  for (int t = 1300; t < 1500; t++)
  {
    for (long i = 0; i < 3000L; i += t)
    {
      digitalWrite(piezo_pin, HIGH);
      delayMicroseconds(t);
      digitalWrite(piezo_pin, LOW);
      delayMicroseconds(t);
    }
  }
}

void alarm() //plays warning sounds from the piezo
{
  for (int i = 0; i < 5; i++)
  {
    if (correctKey())//if the correct keypad input has been entered
      shutOff();
    if (correct)//stop alarm immediately upon receiving correct keypad input
      return;
    digitalWrite(piezo_pin, HIGH);
    delay(500);
    digitalWrite(piezo_pin, LOW);
    delay(500);
  }
}

void loop() {
  if (correctKey())//if the correct keypad input has been entered
    shutOff();
  if (!correct)//if laser sensors are not shut off
  {
    if (digitalRead(ls_pin1) == 1 || digitalRead(ls_pin2) == 1 || digitalRead(ls_pin3) == 1) //if any laser has been triggered
      alarm();
  }
  if (correctPi()) //if pi communicates, reset all, reactivate the lasers
  {
    correct = false;
    digitalWrite(laser_pin1, HIGH);
    digitalWrite(piOut_pin, HIGH);
    digitalWrite(led_pin, HIGH);
    delay(10000); //provides 10 seconds reset time
    Serial.println("Reset");
  }
}
