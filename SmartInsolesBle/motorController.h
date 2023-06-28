#include "HardwareSerial.h"
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
    point _imuPoint;
    int rollOffset;
    int pitchOffset;

    FsrService* fsrService;
    static const uint8_t motorsAmount = 4;
    Motor motors[motorsAmount] = {
      Motor(0, MOTOR_PIN_BOTTOM, MOTOR_AREA_IMU_BOTTOM, MOTOR_AREA_FSR_BOTTOM, MOTOR_FACTOR_BOTTOM), 
      Motor(1, MOTOR_PIN_LEFT, MOTOR_AREA_IMU_LEFT, MOTOR_AREA_FSR_LEFT, MOTOR_FACTOR_LEFT), 
      Motor(2, MOTOR_PIN_RIGHT, MOTOR_AREA_IMU_RIGHT, MOTOR_AREA_FSR_RIGHT, MOTOR_FACTOR_RIGHT), 
      Motor(3, MOTOR_PIN_TOP, MOTOR_AREA_IMU_TOP, MOTOR_AREA_FSR_TOP, MOTOR_FACTOR_TOP)
    };
};

MotorController::MotorController(){}

MotorController::MotorController(IMUGY85* imu, FsrService* fsrService)
{
  this->imu = imu;
  this->rollOffset = imu->getRoll();
  this->pitchOffset = imu->getPitch();

  this->fsrService = fsrService;
}

void MotorController::update()
{
  int imuX = (this->imu->getRoll() - this->rollOffset)/IMU_FACTOR_X;
  if (imuX > MOTOR_AREA_WIDTH/2) {
    imuX = MOTOR_AREA_WIDTH/2;
  } else if (imuX < -MOTOR_AREA_WIDTH/2) {
    imuX = -MOTOR_AREA_WIDTH/2;
  }

  int imuY = (this->imu->getPitch() - this->pitchOffset)/IMU_FACTOR_Y;
  if (imuY > MOTOR_AREA_HEIGHT/2) {
    imuY = MOTOR_AREA_HEIGHT/2;
  } else if (imuY < -MOTOR_AREA_HEIGHT/2) {
    imuY = -MOTOR_AREA_HEIGHT/2;
  }
  point imuPoint = (point) {imuX, imuY};
  this->_imuPoint = imuPoint;
  int weightFactor = this->fsrService->getWeightFactor();
  // Serial.printf("IMU point: %d,%d\n", imuX, imuY);
  
  DATA_PRINT(imuX, imuY, weightFactor);

  point fsrPoint = this->fsrService->getGravityCenter();
  // Serial.printf("FSR point: %d,%d\n", fsrPoint.x, fsrPoint.y);

  for(int i = 0; i < 4; i++) {
    int imuIntersection = motors[i].getImuIntersection(imuPoint);
    int fsrIntersection = motors[i].getFsrIntersection(fsrPoint);
    // Serial.printf("M: %d | %d, %d, %d\n", i, imuIntersection, fsrIntersection, weightFactor);
    
    uint16_t power = ENABLE_VIBRO * (ENABLE_IMU_VIBRO * imuIntersection + ENABLE_FSR_VIBRO * fsrIntersection * weightFactor);
    if (power > pow(2, MOTOR_PWM_RESOLUTION) - 1) {
      power = pow(2, MOTOR_PWM_RESOLUTION) - 1;
    }
    motors[i].setPower(power); 
  }
}

String MotorController::getImuBleData()
{
  String out = Serial.sprintf("%d,%d", this->_imuPoint.x, this->_imuPoint.y);
  return out;
}

String MotorController::getFsrBleData()
{
  return this->fsrService->getBleData();
}

String MotorController::getGravityCenterBleData()
{
  float w = this->fsrService->getWeightFactor();
  point gc = this->fsrService->getGravityCenter();
  String out = Serial.sprintf("%d,%d,%.2f", gc.x, gc.y, w);
  return out;
}

String MotorController::getMotorBleData()
{
  String out = "";
  for (i = 0; i < 4; i++){
    out += motors[i].getPower() + ",";
  }
  return out;
}
