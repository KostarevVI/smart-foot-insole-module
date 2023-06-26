#pragma once


class FsrSensor{

  public:
    FsrSensor();
    FsrSensor(int pin, point location);
    point getLocation();
    int getValue();
    float getNormValue();
    void update();
  
  private:
    static const uint8_t multiMapSize = 8; 
    int real_matrix[multiMapSize] = {400, 570, 587, 591, 593, 595, 597, 598};
    int calib_matrix[multiMapSize] = {0, 500, 1000, 1500, 2000, 2500, 3000, 3500};
    int multiMap(int val);

    MedianFilter medFilt = MedianFilter(7, 0);
    int pin;
    point location;
    int value;
};

FsrSensor::FsrSensor(){}

FsrSensor::FsrSensor(int pin, point location)
{
  this->pin = pin;
  this->location = location;
  this->value = 0;
}

// считывание актуальных показаний датчиков и фильнтация
void FsrSensor::update()
{
  this->medFilt.in((analogRead(this->pin)));
  this->value =  this->multiMap(this->medFilt.out());
}

// Функция-помошник в линеаризации входных данных с FSR
int FsrSensor::multiMap(int val)
{
  if (val <= this->real_matrix[0]) return this->calib_matrix[0];
  if (val >= this->real_matrix[this->multiMapSize-1]) return this->calib_matrix[this->multiMapSize-1];

  // ищем правильный интервал
  uint8_t pos = 1;  // значение 0 уже проверено
  while(val > this->real_matrix[pos]) pos++;

  // проверяем есть ли точное совпадение 
  if (val == this->real_matrix[pos]) return this->calib_matrix[pos];

  // для остального проводим интерполяцию в правом сегменте
  return (val - this->real_matrix[pos-1]) * (this->calib_matrix[pos] - this->calib_matrix[pos-1]) / (this->real_matrix[pos] - this->real_matrix[pos-1]) + this->calib_matrix[pos-1];
}

point FsrSensor::getLocation()
{
  return this->location;
}

int FsrSensor::getValue()
{
  return this->value;
}

float FsrSensor::getNormValue()
{
  return this->value/3500;
}
