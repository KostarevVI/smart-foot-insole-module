#define INSOLE_LEFT 0
#define INSOLE_RIGHT 1

#define INSOLE_TYPE INSOLE_RIGHT// или INSOLE_LEFT

#define MOTOR_FREQUENCY 60000
#define MOTOR_PWM_RESOLUTION 8 // 2^8 = 0-255
#define MOTOR_PIN_LEFT 25 // не работает???
#define MOTOR_PIN_TOP 17
#define MOTOR_PIN_BOTTOM 25
#define MOTOR_PIN_RIGHT 27 // не работает???

#define FSR_PIN_LEFT 33
#define FSR_PIN_BOTTOM 34
#define FSR_PIN_RIGHT 35

// координаты расположения FSR сенсоров для соблюдения пропорций при вычислении точки давления
#define FSR_LOCATION_BOTTOM (point){0, -9}
#define FSR_LOCATION_RIGHT (point){2, 4}
#define FSR_LOCATION_LEFT (point){-2, 6}

#define MOTOR_LOCATION_BOTTOM (point){0, -9}
#define MOTOR_LOCATION_RIGHT (point){2, 4}
#define MOTOR_LOCATION_LEFT (point){-2, 6}
#define MOTOR_LOCATION_TOP (point){-1, 8}

#define MOTOR_AREA_IMU_BOTTOM (area){MOTOR_LOCATION_BOTTOM, 8, 8}
#define MOTOR_AREA_IMU_TOP (area){MOTOR_LOCATION_TOP, 8, 8}
#define MOTOR_AREA_IMU_RIGHT (area){MOTOR_LOCATION_RIGHT, 4, 26}
#define MOTOR_AREA_IMU_LEFT (area){MOTOR_LOCATION_LEFT, 4, 26}

#define MOTOR_AREA_FSR_BOTTOM (area){MOTOR_LOCATION_BOTTOM, 13, 8}
#define MOTOR_AREA_FSR_TOP (area){MOTOR_LOCATION_TOP, 13, 8}
#define MOTOR_AREA_FSR_RIGHT (area){MOTOR_LOCATION_RIGHT, 6, 26}
#define MOTOR_AREA_FSR_LEFT (area){MOTOR_LOCATION_LEFT, 6, 26}


struct point {
  int x;
  int y;
};

struct area {
  point center;
  uint8_t height;
  uint8_t width;
};
