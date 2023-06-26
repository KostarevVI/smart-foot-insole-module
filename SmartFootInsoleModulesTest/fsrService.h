#pragma once
#include "fsrSensor.h"

class FsrService 
{
  public:
    FsrService();
    void update();
    point getPressureCenter();
    float getWeightFactor();

  private:
    point pressureCenter;
    float weightFactor;
    static const uint8_t fsrAmount = 3;

    FsrSensor fsrArray[fsrAmount] = {
      FsrSensor(FSR_PIN_BOTTOM, FSR_LOCATION_BOTTOM), 
      FsrSensor(FSR_PIN_LEFT, FSR_LOCATION_LEFT), 
      FsrSensor(FSR_PIN_RIGHT, FSR_LOCATION_RIGHT)
    };
};

FsrService::FsrService(){}

// Рассчёт точки давления и фактора веса на стопе происходит вместе с обновлением показаний сенсоров
void FsrService::update()
{
  int xCenter = 0;
  int yCenter = 0;
  float weightMax = 0;

  for(int i = 0; i < this->fsrAmount; i++){
    this->fsrArray[i].update();
    float val = this->fsrArray[i].getNormValue();
    point loc = this->fsrArray[i].getLocation();
    xCenter += loc.x * val;
    yCenter += loc.y * val;

    if(weightMax < val){
      weightMax = val;
    }
  }

  this->pressureCenter = (point){xCenter/this->fsrAmount, yCenter/this->fsrAmount};
  this->weightFactor = weightMax;

  Serial.printf("fsr:%d\t%d\t%d", weightMax)
}

point FsrService::getPressureCenter()
{
  return this->pressureCenter;
}

float FsrService::getWeightFactor()
{
  return this->weightFactor;
}

