#pragma once

class Motor{
  public:
    Motor();
    Motor(uint8_t channel, uint8_t pin, area imuArea, area fsrArea);
    void setPower(uint16_t power);
    uint16_t getImuIntersection(point imuCenter);
    uint16_t getFsrIntersection(point fsrCenter);

  private:
    area imuArea;
    area fsrArea;
    uint16_t power = 0;
    uint8_t channel;
};

Motor::Motor(){}

Motor::Motor(uint8_t channel, uint8_t pin, area imuArea, area fsrArea)
{
  this->channel = channel;

  ledcSetup(channel, MOTOR_FREQUENCY, MOTOR_PWM_RESOLUTION);
  ledcAttachPin(pin, channel);

  this->imuArea = imuArea;
  this->fsrArea = fsrArea;
}

void Motor::setPower(uint16_t power)
{
  this->power = power;
  ledcWrite(this->channel, power);
}

uint16_t Motor::getImuIntersection(point imuCenter)
{
  if((abs(this->imuArea.center.x - imuCenter.x) <= this->imuArea.width/2) and (abs(this->imuArea.center.y - imuCenter.y) <= this->imuArea.height/2)){
    return pow(2, MOTOR_PWM_RESOLUTION) - 1;
  }
  return 0;
}

uint16_t Motor::getFsrIntersection(point fsrCenter)
{
  if((abs(this->fsrArea.center.x - fsrCenter.x) <= this->fsrArea.width/2) and (abs(this->fsrArea.center.y - fsrCenter.y) <= this->fsrArea.height/2)){
    return pow(2, MOTOR_PWM_RESOLUTION) - 1;
  }
  return 0;
}



