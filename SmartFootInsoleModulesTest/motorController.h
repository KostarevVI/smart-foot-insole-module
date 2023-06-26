#pragma once
#include <Arduino.h>
#include "FsrService.h"
#include "motor.h"

class MotorController {
   public:
    MotorController();
    MotorController(IMUGY85* imu, FsrService* fsrService);
    void update();
  private:
    IMUGY85* imu;
    FsrService* fsrService;
    static const uint8_t motorsAmount = 4;
    Motor motors[motorsAmount] = {
      Motor(0, MOTOR_PIN_BOTTOM, MOTOR_AREA_IMU_BOTTOM, MOTOR_AREA_FSR_BOTTOM), 
      Motor(1, MOTOR_PIN_LEFT, MOTOR_AREA_IMU_LEFT, MOTOR_AREA_FSR_LEFT), 
      Motor(2, MOTOR_PIN_RIGHT, MOTOR_AREA_IMU_RIGHT, MOTOR_AREA_FSR_RIGHT), 
      Motor(3, MOTOR_PIN_TOP, MOTOR_AREA_IMU_TOP, MOTOR_AREA_FSR_TOP)
    };
};

MotorController::MotorController(){}

MotorController::MotorController(IMUGY85* imu, FsrService* fsrService)
{
  this->imu = imu;
  this->fsrService = fsrService;
}

void MotorController::update()
{
  for(int i = 0; i < 4; i++) {
    point imuPoint = (point) {this->imu->getRoll(), this->imu->getPitch()};
    point fsrPoint = this->fsrService->getPressureCenter();
    
    uint16_t power = (motors[i].getImuIntersection(imuPoint) + motors[i].getFsrIntersection(fsrPoint) * this->fsrService->getWeightFactor());
    motors[i].setPower(power); 
  }
}
