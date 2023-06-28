#include <sys/types.h>
#pragma once

class Motor{
  public:
    Motor();
    Motor(uint8_t channel, uint8_t pin, area imuArea, area fsrArea, float motorFactor);
    void setPower(uint16_t power);
    uint16_t getImuIntersection(point imuCenter);
    uint16_t getFsrIntersection(point fsrCenter);

  private:
    static bool isInArea(point p, area a);
    area imuArea;
    area fsrArea;
    uint16_t power = 0;
    uint8_t channel;
    uint8_t pin;
    float motorFactor;
};

Motor::Motor(){}

Motor::Motor(uint8_t channel, uint8_t pin, area imuArea, area fsrArea, float motorFactor)
{
  this->channel = channel;
  this->pin = pin;
  this->motorFactor = motorFactor;

  ledcAttachPin(pin, channel);
  ledcSetup(channel, MOTOR_FREQUENCY, MOTOR_PWM_RESOLUTION);

  this->imuArea = imuArea;
  this->fsrArea = fsrArea;
}

void Motor::setPower(uint16_t p)
{
  // Serial.printf("Point %d;%d | Cannel: %d | power: %d\n", this->imuArea.center.x, this->imuArea.center.y, this->channel, p);
  if (this->power != p){
    this->power = p;
    ledcWrite(this->channel, (int) (p/motorFactor));
  }
}

int Motor::getPower()
{
  return this->power;
}

bool Motor::isInArea(point p, area a)
{
  bool xOk = (p.x <= a.center.x + a.width/2) && (p.x >= a.center.x - a.width/2);
  // Serial.printf("x_c: %d, p_x: %d, w: %d, xOk: %d\n", a.center.x, p.x, a.width, xOk);

  bool yOk = (p.y <= a.center.y + a.height/2) && (p.y >= a.center.y - a.height/2);
  // Serial.printf("y_c: %d, p_y: %d, h: %d, yOk: %d\n", a.center.y, p.y, a.height, yOk);


  if (xOk && yOk) {
    return true;
  }

  return false;
}

uint16_t Motor::getImuIntersection(point imuCenter)
{
  if(isInArea(imuCenter, this->imuArea)){
    return pow(2, MOTOR_PWM_RESOLUTION) - 1;
  }
  return 0;
}

uint16_t Motor::getFsrIntersection(point fsrCenter)
{
  if(isInArea(fsrCenter, this->fsrArea)){
    return pow(2, MOTOR_PWM_RESOLUTION) - 1;
  }
  return 0;
}



