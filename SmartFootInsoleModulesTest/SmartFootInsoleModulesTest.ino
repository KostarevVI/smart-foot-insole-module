#include <IMUGY85.h>
#include <MedianFilter.h>
#include "config.h"
#include "motorController.h"
#include "FsrService.h"

IMUGY85 imu;
FsrService fsrService;
MotorController motorController;

void setup() {
  Serial.begin(115200);

  imu.init();

  motorController = MotorController(&imu, &fsrService);
}

void loop() {
  imu.update();
  fsrService.update();
  motorController.update();

  delay(20);
}
