#include <IMUGY85.h>
#include <MedianFilter.h>
#include "config.h"
#include "motorController.h"
#include "FsrService.h"

IMUGY85 imu;
FsrService fsrService;
MotorController motorController;
int timer = 0;

void setup() {
  Serial.begin(115200);

  imu.init();
  for(;;) {
    imu.update();
    delay(10);
    if (millis() - timer >= 5000) {
      timer = millis();
      break;
    }
  }

  motorController = MotorController(&imu, &fsrService);
}

void loop() {
  if (millis() - timer >= 20) {
    timer = millis();

    imu.update();
    fsrService.update();
    motorController.update();
  }
}
