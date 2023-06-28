#include <IMUGY85.h>
#include <MedianFilter.h>
#include "config.h"
#include "motorController.h"
#include "FsrService.h"

#include <NimBLEDevice.h>

IMUGY85 imu;
FsrService fsrService;
MotorController motorController;
int timer = 0;

static NimBLEServer* pServer;

// Предопределение BLE методов
class ServerCallbacks: public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer) {
    Serial.println("Client connected");
    Serial.println("Multi-connect support: start advertising");
    NimBLEDevice::startAdvertising();
  }
  void onDisconnect(NimBLEServer* pServer) {
    Serial.println("Client disconnected - start advertising");
    NimBLEDevice::startAdvertising();
  }
  void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc) {
    Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
  }

  uint32_t onPassKeyRequest(){
    Serial.println("Server Passkey Request");
    // генерация случайного ключа
    return random(100000, 999999);
  }

  bool onConfirmPIN(uint32_t pass_key){
    Serial.print("The passkey YES/NO number: ");Serial.println(pass_key);
    // ToDo: сделать обработку с false
    return true;
  }

  void onAuthenticationComplete(ble_gap_conn_desc* desc){
    // Проверка успешности установления защищённого соединения, в противном случае отсоединение клента
    if(!desc->sec_state.encrypted) {
        NimBLEDevice::getServer()->disconnect(desc->conn_handle);
        Serial.println("Encrypt connection failed - disconnecting client");
        return;
    }
    Serial.println("Starting BLE work");
  }
}

// Класс-хэндлер для действий с характеристиками
class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
  void onRead(NimBLECharacteristic* pCharacteristic){
      Serial.print(pCharacteristic->getUUID().toString().c_str());
      Serial.print(": onRead(), value: ");
      Serial.println(pCharacteristic->getValue().c_str());
  }

  void onWrite(NimBLECharacteristic* pCharacteristic) {
      Serial.print(pCharacteristic->getUUID().toString().c_str());
      Serial.print(": onWrite(), value: ");
      Serial.println(pCharacteristic->getValue().c_str());
  }

  void onNotify(NimBLECharacteristic* pCharacteristic) {
      // Serial.println("Sending notification to clients");
  }

  void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) {
    String str = ("Notification/Indication status code: ");
    str += status;
    str += ", return code: ";
    str += code;
    str += ", ";
    str += NimBLEUtils::returnCodeToString(code);
    // Serial.println(str);
  }

  void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if(subValue == 0) {
        str += " Unsubscribed to ";
    }else if(subValue == 1) {
        str += " Subscribed to notfications for ";
    } else if(subValue == 2) {
        str += " Subscribed to indications for ";
    } else if(subValue == 3) {
        str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID()).c_str();

    Serial.println(str);
  }
}

// Класс-хэндлер для дескрипшнов
class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
    void onWrite(NimBLEDescriptor* pDescriptor) {
        std::string dscVal = pDescriptor->getValue();
        Serial.print("Descriptor witten value:");
        Serial.println(dscVal.c_str());
    }

    void onRead(NimBLEDescriptor* pDescriptor) {
        Serial.print(pDescriptor->getUUID().toString().c_str());
        Serial.println(" Descriptor read");
    }
}

// Объявление глобального Callback для характеристик
static CharacteristicCallbacks chrCallbacks;

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

  if (INSOLE_TYPE == LEFT_INSOLE) {
    Serial.println("Starting Smart insole Server");

    // BLE имя девайса
    NimBLEDevice::init("Smart insole (L)");

    // Сила сигнала (в зависимости от платформы)
  #ifdef ESP_PLATFORM
    NimBLEDevice::setPower(ESP_PWR_LVL_N9); /** -9db */
  #else
    NimBLEDevice::setPower(1); /** -9db */
  #endif

    // Настройка защищённого соединения
    NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_SC);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    /*
      Создание сервиса левой стельки
    */
    NimBLEService* pInsoleLeftService = pServer->createService(INSOLE_LEFT_SERVICE_UUID);

    // Создание характеристики данных с IMU
    NimBLECharacteristic* pIMUDataCharacteristicL = pInsoleLeftService->createCharacteristic(
                                              CHARACTERISTIC_IMUDATA_UUID_L,
                                              NIMBLE_PROPERTY::READ |
                                              NIMBLE_PROPERTY::NOTIFY |
                                              NIMBLE_PROPERTY::READ_ENC | 
                                              NIMBLE_PROPERTY::WRITE_ENC   
                                              );
    pIMUDataCharacteristicL->setValue("12;23;199");
    pIMUDataCharacteristicL->setCallbacks(&chrCallbacks);

    // Создание характеристики данных с FSR сенсора
    NimBLECharacteristic* pFSRDataCharacteristicL = pInsoleLeftService->createCharacteristic(
                                              CHARACTERISTIC_FSRDATA_UUID_L,
                                              NIMBLE_PROPERTY::READ |
                                              NIMBLE_PROPERTY::NOTIFY |
                                              NIMBLE_PROPERTY::READ_ENC |  
                                              NIMBLE_PROPERTY::WRITE_ENC   
                                              );
    pFSRDataCharacteristicL->setValue("321;123;123;123;123");
    pFSRDataCharacteristicL->setCallbacks(&chrCallbacks);

    // Создание характеристики данных с мотора
    NimBLECharacteristic* pMotorDataCharacteristicL = pInsoleLeftService->createCharacteristic(  
                                              CHARACTERISTIC_MOTORDATA_UUID_L,
                                              NIMBLE_PROPERTY::READ |
                                              NIMBLE_PROPERTY::NOTIFY |
                                              NIMBLE_PROPERTY::READ_ENC |  
                                              NIMBLE_PROPERTY::WRITE_ENC   
                                              );
    pMotorDataCharacteristicL->setValue("122;23;223");
    pMotorDataCharacteristicL->setCallbacks(&chrCallbacks);

    // Создание характеристики с центром тяжести стельки 
    NimBLECharacteristic* pGravityCenterCharacteristicL = pInsoleLeftService->createCharacteristic(
                                                CHARACTERISTIC_GRAVITYCENTER_UUID_L,
                                                NIMBLE_PROPERTY::READ |
                                                NIMBLE_PROPERTY::NOTIFY |
                                                NIMBLE_PROPERTY::READ_ENC |  
                                                NIMBLE_PROPERTY::WRITE_ENC  
                                              );
    pGravityCenterCharacteristicL->setValue("STOP");
    pGravityCenterCharacteristicL->setCallbacks(&chrCallbacks);

    // Создание характеристики для передачи команд
    NimBLECharacteristic* pCommandCharacteristicL = pInsoleLeftService->createCharacteristic(
                                              CHARACTERISTIC_COMMAND_UUID_L,
                                              NIMBLE_PROPERTY::READ |
                                              NIMBLE_PROPERTY::READ_ENC |  
                                              NIMBLE_PROPERTY::WRITE_ENC  
                                              );
    pCommandCharacteristicL->setValue("STOP");
    pCommandCharacteristicL->setCallbacks(&chrCallbacks);

    /*
      Сервис правой стельки
    */
    NimBLEService* pInsoleRightService = pServer->createService(INSOLE_RIGHT_SERVICE_UUID);

    // Создание характеристики данных с IMU
    NimBLECharacteristic* pIMUDataCharacteristicR = pInsoleRightService->createCharacteristic(
                                              CHARACTERISTIC_IMUDATA_UUID_R,
                                              NIMBLE_PROPERTY::READ |
                                              NIMBLE_PROPERTY::NOTIFY |
                                              NIMBLE_PROPERTY::READ_ENC | 
                                              NIMBLE_PROPERTY::WRITE_ENC   
                                              );
    pIMUDataCharacteristicR->setValue("12;23;199");
    pIMUDataCharacteristicR->setCallbacks(&chrCallbacks);

    // Создание характеристики данных с FSR сенсора
    NimBLECharacteristic* pFSRDataCharacteristicR = pInsoleRightService->createCharacteristic(
                                              CHARACTERISTIC_FSRDATA_UUID_R,
                                              NIMBLE_PROPERTY::READ |
                                              NIMBLE_PROPERTY::NOTIFY |
                                              NIMBLE_PROPERTY::READ_ENC |  
                                              NIMBLE_PROPERTY::WRITE_ENC   
                                              );
    pFSRDataCharacteristicR->setValue("321;123;123;123;123");
    pFSRDataCharacteristicR->setCallbacks(&chrCallbacks);

    // Создание характеристики данных с мотора
    NimBLECharacteristic* pMotorDataCharacteristicR = pInsoleRightService->createCharacteristic(  
                                              CHARACTERISTIC_MOTORDATA_UUID_R,
                                              NIMBLE_PROPERTY::READ |
                                              NIMBLE_PROPERTY::NOTIFY |
                                              NIMBLE_PROPERTY::READ_ENC |  
                                              NIMBLE_PROPERTY::WRITE_ENC   
                                              );
    pMotorDataCharacteristicR->setValue("122;23;223");
    pMotorDataCharacteristicR->setCallbacks(&chrCallbacks);

    // Создание характеристики с центром тяжести стельки 
    NimBLECharacteristic* pGravityCenterCharacteristicR = pInsoleRightService->createCharacteristic(
                                                CHARACTERISTIC_GRAVITYCENTER_UUID_R,
                                                NIMBLE_PROPERTY::READ |
                                                NIMBLE_PROPERTY::NOTIFY |
                                                NIMBLE_PROPERTY::READ_ENC |  
                                                NIMBLE_PROPERTY::WRITE_ENC  
                                              );
    pGravityCenterCharacteristicR->setValue("STOP");
    pGravityCenterCharacteristicR->setCallbacks(&chrCallbacks);

    // Создание характеристики для передачи команд
    NimBLECharacteristic* pCommandCharacteristicR = pInsoleRightService->createCharacteristic(
                                              CHARACTERISTIC_COMMAND_UUID_R,
                                              NIMBLE_PROPERTY::READ |
                                              NIMBLE_PROPERTY::READ_ENC |  
                                              NIMBLE_PROPERTY::WRITE_ENC  
                                              );
    pCommandCharacteristicR->setValue("STOP");
    pCommandCharacteristicR->setCallbacks(&chrCallbacks);

    /*
      Создание Master сервиса  
    */
    NimBLEService* pInsoleMasterService = pServer->createService(INSOLE_MASTER_SERVICE_UUID);

    // Создание характеристики продолжительности тренировки 
    NimBLECharacteristic* pDurationCharacteristic = pInsoleMasterService->createCharacteristic(
                                                CHARACTERISTIC_DURATION_UUID,
                                                NIMBLE_PROPERTY::READ |
                                                NIMBLE_PROPERTY::WRITE |
                                                NIMBLE_PROPERTY::NOTIFY
                                              );

    pDurationCharacteristic->setValue("0");
    pDurationCharacteristic->setCallbacks(&chrCallbacks);

    // Создание характеристики данных упражнения 
    NimBLECharacteristic* pExDataCharacteristic = pInsoleMasterService->createCharacteristic(
                                                CHARACTERISTIC_EXDATA_UUID,
                                                NIMBLE_PROPERTY::READ |
                                                NIMBLE_PROPERTY::WRITE |
                                                NIMBLE_PROPERTY::NOTIFY
                                              );

    pExDataCharacteristic->setValue("0");
    pExDataCharacteristic->setCallbacks(&chrCallbacks);

    // Создание характеристики количества повторов упражнения 
    NimBLECharacteristic* pRepeatsCharacteristic = pInsoleMasterService->createCharacteristic(
                                                CHARACTERISTIC_REPEATS_UUID,
                                                NIMBLE_PROPERTY::READ |
                                                NIMBLE_PROPERTY::WRITE |
                                                NIMBLE_PROPERTY::NOTIFY
                                              );

    pRepeatsCharacteristic->setValue("0");
    pRepeatsCharacteristic->setCallbacks(&chrCallbacks);

    // Создание характеристики для передачи команд
    NimBLECharacteristic* pMasterCommandCharacteristic = pInsoleMasterService->createCharacteristic(
                                                CHARACTERISTIC_COMMAND_UUID,
                                                NIMBLE_PROPERTY::READ |
                                                NIMBLE_PROPERTY::WRITE |
                                                NIMBLE_PROPERTY::NOTIFY
                                              );

    pMasterCommandCharacteristic->setValue("0");
    pMasterCommandCharacteristic->setCallbacks(&chrCallbacks);

    // Создание характеристики вибротактильной обратной связи
    NimBLECharacteristic* pVibrationCharacteristic = pInsoleMasterService->createCharacteristic(
                                                CHARACTERISTIC_VIBRATION_UUID,
                                                NIMBLE_PROPERTY::READ |
                                                NIMBLE_PROPERTY::WRITE |
                                                NIMBLE_PROPERTY::NOTIFY
                                              );

    pVibrationCharacteristic->setValue("0");
    pVibrationCharacteristic->setCallbacks(&chrCallbacks);


    /** Start the services when finished creating all Characteristics and Descriptors */
    pInsoleLeftService->start();
    pInsoleRightService->start();
    pInsoleMasterService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    // добавить сервисы в advertising
    pAdvertising->addServiceUUID(pInsoleLeftService->getUUID());
    // pAdvertising->addServiceUUID(pInsoleRightService->getUUID());
    pAdvertising->addServiceUUID(pInsoleMasterService->getUUID());

    pAdvertising->setScanResponse(true);
              Serial.println("im here");

    pAdvertising->start();

    Serial.println("Server Insole advertising started");
  } else if (INSOLE_TYPE == INSOLE_RIGHT) {
    NimBLEDevice :: init ("Insole Module (R)");
    NimBLEDevice :: setPower( ESP_PWR_LVL_P7 ); датчика BLE


// Получение объектов характеристик от сервиса 
NimBLEService * pService = pClient -> getServiceByUUID (INSOLE_MASTER_SERVICE_UUID ); 
pImuCharacteristic = pService -> getCharacteristic (CHARACTERISTIC_IMUDATA_UUID ); 
pFsrCharacteristic = pService -> getCharacteristic (CHARACTERISTIC_FSRDATA_UUID ); 
pGravityCharacteristic = pService -> getCharacteristic (CHARACTERISTIC_GRAVITYCENTER_UUID ); 
pMotorCharacteristic = pService -> getCharacteristic (CHARACTERISTIC_MOTORDATA_UUID );


// Запуск таймера для отправки данных в сервис каждые 20мс 
TimerHandle_t sendDataTimer = xTimerCreate (" sendDataTimer " , pdMS_TO_TICKS (20) , pdTRUE , NULL , sendDataCallback ); 
xTimerStart (sendDataTimer , 0);
  }
}

void loop() {
  if (INSOLE_TYPE == INSOLE_LEFT) {
    if(pServer->getConnectedCount()) {
      NimBLEService* pSvcInsL = pServer->getServiceByUUID(INSOLE_LEFT_SERVICE_UUID);
      NimBLEService* pSvcInsR = pServer->getServiceByUUID(INSOLE_RIGHT_SERVICE_UUID);
      if(pSvcInsL && pSvcInsR) {
        NimBLECharacteristic* pChrImuL = pSvcInsL->getCharacteristic(CHARACTERISTIC_IMUDATA_UUID_L);
        NimBLECharacteristic* pChrFsrL = pSvcInsL->getCharacteristic(CHARACTERISTIC_FSRDATA_UUID_L);
        NimBLECharacteristic* pChrMotL = pSvcInsL->getCharacteristic(CHARACTERISTIC_MOTORDATA_UUID_L);
        NimBLECharacteristic* pChrGvtL = pSvcInsL->getCharacteristic(CHARACTERISTIC_GRAVITYCENTER_UUID_L);

        NimBLECharacteristic* pChrImuR = pSvcInsR->getCharacteristic(CHARACTERISTIC_IMUDATA_UUID_R);
        NimBLECharacteristic* pChrFsrR = pSvcInsR->getCharacteristic(CHARACTERISTIC_FSRDATA_UUID_R);
        NimBLECharacteristic* pChrMotR = pSvcInsR->getCharacteristic(CHARACTERISTIC_MOTORDATA_UUID_R);
        NimBLECharacteristic* pChrGvtR = pSvcInsR->getCharacteristic(CHARACTERISTIC_GRAVITYCENTER_UUID_R);

        if(pChrGvtL && pChrGvtR) {
          if (millis() - timer > 20) {
            timer = millis();

            imu.update();
            fsrService.update();
            motorController.update();
    
            // левая
            pChrImuL->setValue(motorController.getImuBleData());
            pChrImuL->notify(true);

            pChrFsrL->setValue(motorController.getFsrBleData());
            pChrFsrL->notify(true);

            pChrMotL->setValue(motorController.getMotorBleData());
            pChrMotL->notify(true);

            pChrGvtL->setValue(motorController.getGravityCenterBleData());
            pChrGvtL->notify(true);
          }
        }
      }
    }
  } else if (INSOLE_TYPE == INSOLE_RIGHT) {
    // Формирование данных 
    imuData = motorController.getImuBleData()
    fsrData = motorController.getFsrBleData()
    gravityCenterData = motorController.getGravityCenterBleData(); 
    motorData = motorController.getMotorBleData();

    // Отправка данных в сервис 
    pImuCharacteristic->writeValue(imuData); 
    pFsrCharacteristic->writeValue(fsrData); 
    pGravityCharacteristic->writeValue(gravityCenterData); 
    pMotorCharacteristic->writeValue(motorData);
  }
}