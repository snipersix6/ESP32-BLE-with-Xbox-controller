/*
Made by Sniper.six
Bluetooth Low Energy with Controller Xbox 1914 from Microsoft.
Program establishes the BLE connectivity with controller and drives a vehicle.
*/

#include <BLEDevice.h>
#include <BLEClient.h>
#include <BLERemoteService.h>

BLEAddress XboxAddr("XX:XX:XX:XX:XX:XX"); // Xbox Controller MAC add

static BLEUUID HID_SERVICE("1812");
static BLEUUID HID_INFORMATION("2A4A");
static BLEUUID HID_REPORT_MAP("2A4B");
static BLEUUID HID_CONTROL_POINT("2A4C");
static BLEUUID HID_REPORT_DATA("2A4D");

// BLE client value
BLEClient* pClient;

// movement vars
bool command = false;
bool forward = false;
bool back = false;
bool left = false;
bool right = false;
bool exitBTN = false;

// Initialization of IO ports iin Setup
void IOpinsInit()
{
    Serial.println("IO Init.");
    // IO pins setup
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);

    digitalWrite(D3, HIGH); // initialize motor driver

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

// initialization of bluetooth low energy in Setup and subscribe to get data updates from Xbox constroller
void BLEInit ()
{
    // Initialize BLE
    Serial.println("Initializing BLE...");

    BLEDevice::init("ESP32-C6");  // Initialize BLE name

    // Set BLE security + bond
    BLESecurity* pSecurity = new BLESecurity();
    //pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);  // Bonding
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND); // Bonding with MITM (Man in the middle) protection
    pSecurity->setCapability(ESP_IO_CAP_NONE);                       // No PIN required
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_MITM);  // Set encryption to BLE device

    pClient = BLEDevice::createClient();

    Serial.println("Connecting to Xbox controller.");
/*
    if (!pClient->connect(XboxAddr)) {  // Connect to the device by its MAC address
      Serial.println("Failed to connect!");
      return;
    }
    Serial.println("Connected!");
    */
 
    // connect try for 3 times
    for(int i=0;i<3;i++)
    {
      Serial.printf("%d try connect to Xbox controller.\n",(i+1));
      if(pClient->connect(XboxAddr))
      {
        Serial.println("Connected to Xbox Controller.");
        break;
      }
    }

    if(!pClient->isConnected())
    {
      Serial.println("Failed to connect to Xbox controller. Ommiting...");
      delay(1000);
      return;
    }

    // Discover the HID service
    BLERemoteService* pRemoteServiceOne = pClient->getService(HID_SERVICE);
    if (pRemoteServiceOne == nullptr) {
      Serial.println("Failed to find HID service.");
      return;
    }

    // Discover the Report characteristic
    std::map<uint16_t, BLERemoteCharacteristic*>* pCharacteristicMap;
    pCharacteristicMap = pRemoteServiceOne->getCharacteristicsByHandle();

    // Iterate through the map
    for (auto const& entry : *pCharacteristicMap) {
    uint16_t handle = entry.first;                      // Characteristic handle
    BLERemoteCharacteristic* pCharacteristic = entry.second; // Characteristic pointer
    
      if (pCharacteristic->canNotify() && (pCharacteristic->getUUID().toString() == HID_REPORT_DATA.toString())) {
        //Serial.print("can notify ");
        //Serial.println(handle);
        pCharacteristic->registerForNotify(notifyCallback);
      }
      /*
      else {
        Serial.println("Failed to find report characteristic or it does not support notifications.");
      }*/
    
    }
}

void vehicleMovement(uint8_t leftX, uint8_t leftY, uint8_t rightX, uint8_t rightY)
{
  if(leftY <= 64 && !forward)
  {
    forward = true;
    digitalWrite(D1, HIGH);
    digitalWrite(D4, HIGH);
    Serial.println("Forward!");
  }

    if(leftY >= 192 && !back)
  {
    back = true;
    digitalWrite(D2, HIGH);
    digitalWrite(D5, HIGH);
    Serial.println("Back!");
  }

    if(leftX <= 64 && !left)
  {
    left = true;
    digitalWrite(D1, HIGH);
    digitalWrite(D5, HIGH);
    Serial.println("Left!");
  }

    if(leftX >= 192 && !right)
  {
    right = true;
    digitalWrite(D2, HIGH);
    digitalWrite(D4, HIGH);
    Serial.println("Right!");
  }

  if(leftY > 64 && forward || leftY < 192 && back || leftX > 64 && left || leftX < 192 && right)
  {
    forward = false;
    back = false;
    left = false;
    right = false;

    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
    digitalWrite(D4, LOW);
    digitalWrite(D5, LOW);

    Serial.println("Halt!");
  }
}

// Callback to handle notifications from the Xbox controller and movement
void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  // Xbox controller values
  // joystick values are transfered in uint16_t but i'm moving them to uint8_t because of joisticks imperfections and value range is too great. Use of uint16_t prooven to be impractical.

  uint8_t leftX = (((pData[0] << 8) | pData[1])%256);
  uint8_t leftY = (((pData[2] << 8) | pData[3])%256);

  uint8_t rightX = (((pData[4] << 8) | pData[5])%256);
  uint8_t rightY = (((pData[6] << 8) | pData[7])%256);

  uint8_t leftTrigger = pData[8]; // values 0-255 multiplied by leftTrigger_1
  uint8_t leftTrigger_1 = pData[9]; // show 1/3, 2/3, 3/3 of trigger push

  uint8_t rightTrigger = pData[10]; // values 0-255 multiplied by rightTrigger_1
  uint8_t rightTrigger_1 = pData[11]; // show 1/3, 2/3, 3/3 of trigger push

  uint8_t directionalPad = pData[12];

  uint8_t buttons = pData[13]; // buttons A,B,Y,X + left and right trigger

  uint8_t viewButton = pData[14]; // viewButton + MenuButton + click of joystick

  uint8_t exitButton = pData[15];

  vehicleMovement(leftX,leftY,rightX,rightY);

  //Serial.printf("leftX:%d, leftY:%d, rightX:%d, rightY:%d, leftTrigger:%d, leftTrigger_1:%d, rightTrigger:%d, rightTrigger_1:%d, directionalPad:%d, buttons:%d, viewButton:%d, exitButton:%d\n",leftX,leftY,rightX,rightY,leftTrigger,leftTrigger_1,rightTrigger,rightTrigger_1,directionalPad,buttons,viewButton,exitButton);

  /*
  // original data send by controller non sorted
  Serial.print("Notification data: ");
  for (size_t i = 12; i < length; i++) {
    Serial.print(pData[i], HEX);
    Serial.print(",");
  }
  Serial.println();
  */

  if(exitButton)
  {
    exitBTN = true;
    return;
  }

}

//Setup and initializations
void setup() {
  // start serial
  Serial.begin(115200);

  IOpinsInit();

  BLEInit();

  // init done Lit the LED
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  //Serial.println("Loop");

  if(!pClient->isConnected())
  {
    Serial.println("Re-connecting to Xbox controller");
    BLEDevice::deinit();
    BLEInit();
  }

  if(exitBTN)
  {
    Serial.println("Disconnecting Xbox controller.");
    pClient->disconnect();
    exitBTN=false;
  }

}