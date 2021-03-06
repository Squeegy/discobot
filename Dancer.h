#ifndef Dancer_H
#define Dancer_H

#include "FastLED.h"
#include <Arduino.h>
#include "Joint.h"

class Dancer {
  public:
    CRGB *neoPixels;
    Joint *FL1;
    Joint *FR1;
    Joint *BL1;
    Joint *BR1;
    Joint *FL2;
    Joint *FR2;
    Joint *BL2;
    Joint *BR2;

    void init(
      CRGB neoPixels[],
      Joint& _FL1,
      Joint& _FR1,
      Joint& _BL1,
      Joint& _BR1,
      Joint& _FL2,
      Joint& _FR2,
      Joint& _BL2,
      Joint& _BR2
    );

    virtual void start() {};
    virtual void update() {};
    virtual void onBeatStart(float duration) {};
    virtual void onBarStart(float duration) {};

};
#endif