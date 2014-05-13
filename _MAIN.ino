#include "SerialFloatReader.h"
#include "Joint.h"
#include "Dancer.h"

#include "Metronome.h"
#include "DancerPushups.h"
#include "DancerHandup.h"
#include "DancerThrusts.h"

#include "Adafruit_NeoPixel.h"
#include "Adafruit_PWMServoDriver.h"

const int onBoardLedPin = 13;

const int inputSwitchPin = 44;
bool switchPressed = false;
int inputMode = 0;

const int bpmPotPin = 2;

const int neoPixelPin = 40;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, neoPixelPin, NEO_GRB + NEO_KHZ800);

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver();

Joint FL1;
Joint FL2;
Joint FR1;
Joint FR2;
Joint BR1;
Joint BR2;
Joint BL1;
Joint BL2;

Metronome metronome;

Dancer *currentDancer;

const int dancerCount = 3;
Dancer *dancers[] = {
  new DancerPushups(),
  new DancerHandup(),
  new DancerThrusts(),
};

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(4) * analogRead(5) * analogRead(6) * analogRead(7));

  // strip.begin();
  // strip.show();

  pwmDriver.begin();
  pwmDriver.setPWMFreq(60);

  pinMode(onBoardLedPin, OUTPUT);
  pinMode(inputSwitchPin, INPUT);

  FL1.init(pwmDriver, 0,  10,  1, 30);
  FL2.init(pwmDriver, 1,   6, -1, 80);
  FR1.init(pwmDriver, 2, -10, -1, 30);
  FR2.init(pwmDriver, 3,  17,  1, 80);
  BR1.init(pwmDriver, 4,   7,  1, 30);
  BR2.init(pwmDriver, 5, -10, -1, 80);
  BL1.init(pwmDriver, 6,   3, -1, 30);
  BL2.init(pwmDriver, 7,  -4,  1, 80);

  for (int i = 0; i < dancerCount; i++) {
    dancers[i]->init(FL1, FL2, FR1, FR2, BR1, BR2, BL1, BL2);
  }

  currentDancer = dancers[random(0, dancerCount)];
  currentDancer->start();

  metronome.start(90);
}

// unsigned long us;

void loop() {
  float currentTime = (float)millis() / 1000.0;

  // sinePixels();

  digitalWrite(onBoardLedPin, inputMode == 0 ? LOW : HIGH);
  
  if (!switchPressed && digitalRead(inputSwitchPin) == HIGH) {
    switchPressed = true;
    if (inputMode == 0) {
      inputMode = 1;
    } else {
      inputMode = 0;
    }
  } else if (digitalRead(inputSwitchPin) == LOW) {
    switchPressed = false;
  }

  float bpmScalar = (float)analogRead(bpmPotPin) / 1023;
  metronome.setBPM(60.0 + 180.0 * bpmScalar);

  switch (inputMode) {
    case 0:
      updateViaSerial(); break;
    case 1:
      updateViaMetronome(); break;
  }

  FL1.update(currentTime);
  FR1.update(currentTime);
  BL1.update(currentTime);
  BR1.update(currentTime);
  FL2.update(currentTime);
  FR2.update(currentTime);
  BL2.update(currentTime);
  BR2.update(currentTime);
}

void updateViaSerial() {
  while (Serial.available() > 0) {
    float duration;
    // segment
    readSerialFloat();

    // tatum
    readSerialFloat();
    
    // beat
    duration = readSerialFloat();
    if (duration > 0) {
      currentDancer->onBeatStart(duration);
    }

    // bar
    duration = readSerialFloat();
    if (duration > 0) {
      currentDancer->onBarStart(duration);
    }

    // section
    duration = readSerialFloat();
    if (duration > 0) {
      currentDancer = dancers[random(0, dancerCount)];
      currentDancer->start();
      // Serial.print("currentDancerIndex: ");
      // Serial.println(currentDancerIndex);
    }
  }
}

void updateViaMetronome() {
  metronome.update();

  if (metronome.triggerSection()) {
    currentDancer = dancers[random(0, dancerCount)];
    currentDancer->start();

    // Serial.print("currentDancerIndex: ");
    // Serial.println(currentDancerIndex);
  }

  if (metronome.triggerBeat()) {
    // Serial.println("beat ");
    currentDancer->onBeatStart(metronome.spb);
  }

  if (metronome.triggerBar()) {
    // Serial.println("bar ");
    currentDancer->onBarStart(metronome.spb * 4.0);
  }
}                                                                                                                                   

// void sinePixels() {
//   float brightness = .5;
//   float timeScale = 3.0;
//   float time = (float)millis() / 1000.0;
  
//   float r, g, b;

//   for (uint8_t x = 0; x < 8; x++) {
//     for (uint8_t y = 0; y < 5; y++) {

//       r = sin(x - time*timeScale*3.0) * 0.15 + 0.5;
//       r = 1.0 - abs((float)y / 4.0 - r);
//       r = pow(r, 3);

//       g = sin(x + time*timeScale*2.1) * 0.2 + 0.5;
//       g = 1.0 - abs((float)y / 4.0 - g);
//       g = pow(g, 3);

//       b = sin(x - time*timeScale*1.3) * 0.4 + 0.5;
//       b = 1.0 - abs((float)y / 4.0 - b);
//       b = pow(b, 3);

//       setPixelColor(
//         x, y,
//         r * brightness,
//         g * brightness,
//         b * brightness
//       );
//     }
//   }
//   strip.show();
// }

// void setPixelColor(uint8_t x, uint8_t y, float r, float g, float b) {
//   strip.setPixelColor(pixelId(x, y), (uint8_t)(r*255), (uint8_t)(g*255), (uint8_t)(b*255));
// }

// uint8_t pixelId(uint8_t x, uint8_t y) {
//   return y * 8 + x;
// }