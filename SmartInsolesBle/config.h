#define INSOLE_LEFT 0
#define INSOLE_RIGHT 1

// левая - сервер, правая - клиент

/*
  Основные настройки
*/
#define INSOLE_TYPE INSOLE_RIGHT// или INSOLE_LEFT
// управление вибрацией
#define ENABLE_VIBRO true
#define ENABLE_IMU_VIBRO true
#define ENABLE_FSR_VIBRO true

#define MOTOR_FREQUENCY 60000
#define MOTOR_PWM_RESOLUTION 8 // 2^8 = 0-255

// пины моотров 16, 17, 25, 27
#define MOTOR_PIN_LEFT 16
#define MOTOR_PIN_TOP 17
#define MOTOR_PIN_BOTTOM 25
#define MOTOR_PIN_RIGHT 27 

#define FSR_PIN_LEFT 33
#define FSR_PIN_BOTTOM 34
#define FSR_PIN_RIGHT 35

// координаты расположения FSR сенсоров для соблюдения пропорций при вычислении точки давления
#define FSR_LOCATION_BOTTOM (point){0, -9}
#define FSR_MID_POINT (point) {0, 0}
#if (INSOLE_TYPE == INSOLE_RIGHT)
#define FSR_LOCATION_RIGHT (point){2, 4}
#define FSR_LOCATION_LEFT (point){-2, 6}
#elif (INSOLE_TYPE == INSOLE_LEFT)
#define FSR_LOCATION_RIGHT (point){-2, 6}
#define FSR_LOCATION_LEFT (point){2, 4}
#endif

#define MOTOR_LOCATION_BOTTOM (point){0, -9}
#define MOTOR_LOCATION_TOP (point){-1, 8}
#if (INSOLE_TYPE == INSOLE_RIGHT)
#define MOTOR_LOCATION_RIGHT (point){2, 4}
#define MOTOR_LOCATION_LEFT (point){-2, 6}
#elif (INSOLE_TYPE == INSOLE_LEFT)
#define MOTOR_LOCATION_RIGHT (point){-2, 6}
#define MOTOR_LOCATION_LEFT (point){2, 4}
#endif

// вспомогательные директивы для преобразования показателей гироскопа в координатную плоскость
#define IMU_FACTOR_X 15
#define IMU_FACTOR_Y 4
#define MOTOR_AREA_WIDTH 4
#define MOTOR_AREA_HEIGHT 26

#define MOTOR_AREA_IMU_BOTTOM (area){MOTOR_LOCATION_BOTTOM, MOTOR_AREA_WIDTH * 2, 12}
#define MOTOR_AREA_IMU_TOP (area){MOTOR_LOCATION_TOP, MOTOR_AREA_WIDTH * 2, 8}
#define MOTOR_AREA_IMU_RIGHT (area){MOTOR_LOCATION_RIGHT, 2, MOTOR_AREA_HEIGHT * 2}
#define MOTOR_AREA_IMU_LEFT (area){MOTOR_LOCATION_LEFT, 2, MOTOR_AREA_HEIGHT * 2}

#define MOTOR_AREA_FSR_BOTTOM (area){MOTOR_LOCATION_BOTTOM, MOTOR_AREA_WIDTH * 2, 9 * 2}
#define MOTOR_AREA_FSR_TOP (area){MOTOR_LOCATION_TOP, MOTOR_AREA_WIDTH * 2, 8 * 2}
#define MOTOR_AREA_FSR_RIGHT (area){MOTOR_LOCATION_RIGHT, 2, MOTOR_AREA_HEIGHT * 2}
#define MOTOR_AREA_FSR_LEFT (area){MOTOR_LOCATION_LEFT, 2, MOTOR_AREA_HEIGHT * 2}

// ограничивающие мощность моторов коэфиценты
#define MOTOR_FACTOR_BOTTOM 1.5 
#define MOTOR_FACTOR_TOP 1 
#define MOTOR_FACTOR_RIGHT 1.5 
#define MOTOR_FACTOR_LEFT 1.5 

// отправка данных
#define INSOLES_DISTANCE 13
#if (INSOLE_TYPE == INSOLE_RIGHT)
#define DATA_PRINT(imuX, imuY, weightFactor) Serial.printf("%d,%d,%.2f", imuX + INSOLES_DISTANCE, imuY, weightFactor)
#elif (INSOLE_TYPE == INSOLE_LEFT)
#define DATA_PRINT(imuX, imuY, weightFactor) Serial.printf("%d,%d,%.2f", imuX - INSOLES_DISTANCE, imuY, weightFactor)
#endif

struct point {
  int x;
  int y;
};

struct area {
  point center;
  uint8_t width;
  uint8_t height;
};

// UUID сервисов
#define INSOLE_LEFT_SERVICE_UUID "164e3f21-1664-49c8-81ee-16bc6f14d75a"
#define INSOLE_RIGHT_SERVICE_UUID "767f3bcf-5464-4b2b-8f37-73391cf0f1f0"
#define INSOLE_MASTER_SERVICE_UUID "18828b254-a426-4cb4-8b5b-35d82e9e511b"

// Master характеристики
#define CHARACTERISTIC_DURATION_UUID "8fb51a2d-a224-48ed-95b7-ba6cddbd992e"
#define CHARACTERISTIC_REPEATS_UUID "0dc552e9-9cf3-4cc7-9d5e-7c0e4cc1dba9"
#define CHARACTERISTIC_EXDATA_UUID "4b4e35ee-799d-4bd7-9e91-1d19a9d1fae0"
#define CHARACTERISTIC_VIBRATION_UUID "00a26856-8f0a-474a-8ad5-0615baf93cef"
#define CHARACTERISTIC_COMMAND_UUID "a7236701-cf02-449d-b2bd-dfdde31bee50"
#define CHARACTERISTIC_STATE_UUID "8fb51a2d-a224-48ed-95b7-ba6cddbd992e"

// Характеристики левого модуля-стельки
#define CHARACTERISTIC_IMUDATA_UUID_L "3afe7e11-2590-4c1d-8de7-9840eed05940"
#define CHARACTERISTIC_FSRDATA_UUID_L "38367ab9-3dd6-4cb3-a697-678159344a69"
#define CHARACTERISTIC_GRAVITYCENTER_UUID_L "98367ab9-3dd6-4cb3-a697-678159344a69"
#define CHARACTERISTIC_MOTORDATA_UUID_L "88a38dd3-529c-438b-9089-2a806f042664"
#define CHARACTERISTIC_COMMAND_UUID_L "6f99f925-cfce-4676-92aa-5aa8f3844c27"

// Характеристики правого модуля-стельки
#define CHARACTERISTIC_IMUDATA_UUID_R "3afe7e11-2590-4c1d-8de7-9840eed05942"
#define CHARACTERISTIC_FSRDATA_UUID_R "38367ab9-3dd6-4cb3-a697-678159344a62"
#define CHARACTERISTIC_GRAVITYCENTER_UUID_R "98367ab9-3dd6-4cb3-a697-678159344a62"
#define CHARACTERISTIC_MOTORDATA_UUID_R "88a38dd3-529c-438b-9089-2a806f042662"
#define CHARACTERISTIC_COMMAND_UUID_R "6f99f925-cfce-4676-92aa-5aa8f3844c22"

