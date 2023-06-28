#pragma once
#include "fsrSensor.h"

class FsrService 
{
  public:
    FsrService();
    void update();
    point getGravityCenter();
    float getWeightFactor();

  private:
    point gravityCenter;
    float weightFactor;
    static const uint8_t fsrAmount = 3;
    point midPoint = FSR_MID_POINT;

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
  float xCenter = 0;
  float yCenter = 0;
  float weightMax = 0;
  float sumVal = 0;

  for(int i = 0; i < this->fsrAmount; i++){
    this->fsrArray[i].update();
    float val = this->fsrArray[i].getNormValue();

    if(weightMax < val){
      weightMax = val;
    }
    sumVal += val;

    point loc = this->fsrArray[i].getLocation();    
    xCenter += (loc.x - this->midPoint.x) * val;
    yCenter += (loc.y - this->midPoint.y) * val;
  }

  // проверяем, что ноги не в воздухе
  if (sumVal <= 0) {
    this->gravityCenter = (point){0, 0};
  } else {
    this->gravityCenter = (point){(int) (xCenter/sumVal) + this->midPoint.x, (int) (yCenter/sumVal) + this->midPoint.y};
  }
  this->weightFactor = weightMax;
}

point FsrService::getGravityCenter()
{
  return this->gravityCenter;
}

float FsrService::getWeightFactor()
{
  return this->weightFactor;
}

String FsrService::getBleData()
{
  String out = "";
  for (i = 0; i < 3; i++){
    out += fsrArray[i].getValue() + ",";
  }
  return out;
}