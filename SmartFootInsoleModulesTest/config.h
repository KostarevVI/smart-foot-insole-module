#define INSOLE_LEFT 0
#define INSOLE_RIGHT 1

#define INSOLE_TYPE INSOLE_RIGHT// или INSOLE_LEFT

#define MOTOR_FREQUENCY 60000
#define MOTOR_PWM_RESOLUTION 8 // 2^8 = 0-255

// пины моотров 16, 17, 25, 27
#define MOTOR_PIN_LEFT 16 // не работает???
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
#define FSR_MID_POINT (point) {0, 0}

#define MOTOR_LOCATION_BOTTOM (point){0, -9}
#define MOTOR_LOCATION_TOP (point){-1, 8}
#define MOTOR_LOCATION_RIGHT (point){2, 4}
#define MOTOR_LOCATION_LEFT (point){-2, 6}

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

// управление вибрацией
#define ENABLE_VIBRO true
#define ENABLE_IMU_VIBRO true
#define ENABLE_FSR_VIBRO true

struct point {
  int x;
  int y;
};

struct area {
  point center;
  uint8_t width;
  uint8_t height;
};
