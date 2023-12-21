#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
int txValue = 0;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  }

  void onDisconnect(NimBLEServer *pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

class MyCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      Serial.print("Received Value: ");
      Serial.println(rxValue.c_str());

      // Forward the received value to Arduino Mega if needed
      Serial2.println(rxValue.c_str());
    }
  }
};


void setup() {
  Serial.begin(115200);

  NimBLEDevice::init("PEANUT");
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Waiting for a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    pCharacteristic->setValue("Hello from ESP32");
    pCharacteristic->notify();
    delay(1000);
  }
}
