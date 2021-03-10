// *******************************************************************
//
//  Arduino Mega (5V) as a bridge between a Raspberry Pi and 2 Hoverboards with Emanuels hoverboard firmware hack
//
//  code based on "hoverserial.ino" from https://github.com/EmanuelFeru/hoverboard-firmware-hack-FOC
//  
//  examples of non blocking serial communication for arduino: https://forum.arduino.cc/index.php?topic=288234.0
//
// *******************************************************************
// INFO:
// • This sketch uses the the (HW) Serial Interface 1 to communicate and send commands to the hoverboard
// • The built-in (HW) Serial interface 0 is used for debugging and visualization. In case the debugging is not needed,
//   it is recommended to use the built-in Serial interface for full speed perfomace.
// • The data packaging includes a Start Frame, checksum, and re-syncronization capability for reliable communication
// 
// CONFIGURATION on the hoverboard side in config.h:
// • Option 1: Serial on Right Sensor cable (short wired cable) - recommended, since the USART3 pins are 5V tolerant.
//   #define CONTROL_SERIAL_USART3
//   #define FEEDBACK_SERIAL_USART3
//   // #define DEBUG_SERIAL_USART3
// *******************************************************************

// ########################## DEFINES ##########################
#define HOVER_SERIAL_BAUD   115200      // [-] Baud rate for HoverSerial (used to communicate with the hoverboard)
#define SERIAL_BAUD         115200      // [-] Baud rate for built-in Serial (used for the Serial Monitor)
#define START_FRAME         0xABCD      // [-] Start frme definition for reliable serial communication
#define TIME_SEND           100         // [ms] Sending time interval
#define SPEED_MAX_TEST      300         // [-] Maximum speed for testing
// #define DEBUG_RX                     // [-] Debug received data. Prints all bytes to serial (comment-out to disable)

// Global variables
unsigned long iTimeSend = 0;
uint8_t idx = 0;                        // Index for new data pointer
uint16_t bufStartFrame;                 // Buffer Start Frame
byte *p;                                // Pointer declaration for the new received data
byte incomingByte1;
byte incomingBytePrev1;
int STEER;
int SPEED;
const byte numChars = 32;
byte receivedChars[numChars];
boolean newData = false;

typedef struct{
   uint16_t start;
   int16_t  steer;
   int16_t  speed;
   uint16_t checksum;
} SerialCommand;
SerialCommand Command;

typedef struct{
   uint16_t start;
   int16_t  cmd1;
   int16_t  cmd2;
   int16_t  speedR_meas;
   int16_t  speedL_meas;
   int16_t  batVoltage;
   int16_t  boardTemp;
   uint16_t cmdLed;
   uint16_t checksum;
} SerialFeedback;
SerialFeedback Feedback;
SerialFeedback NewFeedback;

// ########################## SETUP ##########################
void setup() 
{
  Serial.begin(SERIAL_BAUD);
  Serial.println("Hoverboard Serial v1.0");

  Serial1.begin(HOVER_SERIAL_BAUD);
  Serial2.begin(HOVER_SERIAL_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);
}

// ########################## LOOP ##########################

void loop(void) { 
  unsigned long timeNow = millis();

  recvPi();           // Check for new received data from pi

  Receive1();         // Check for new received data from Hoverboard
  

  // Send commands
  if (iTimeSend > timeNow) return;
  iTimeSend = timeNow + TIME_SEND;
  Send(STEER, SPEED);

  // Blink the LED
  digitalWrite(LED_BUILTIN, (timeNow%2000)<1000);
}

// ########################## RECEIVE from Pi ##########################

void recvPi() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    byte rc;
 
 // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

// ########################## SEND to Hoverboards ##########################
void Send(int16_t uSteer, int16_t uSpeed)
{
  // Create command
  Command.start    = (uint16_t)START_FRAME;
  Command.steer    = (int16_t)uSteer;
  Command.speed    = (int16_t)uSpeed;
  Command.checksum = (uint16_t)(Command.start ^ Command.steer ^ Command.speed);

  // Write to Serial
  Serial1.write((uint8_t *) &Command, sizeof(Command));
  Serial2.write((uint8_t *) &Command, sizeof(Command));
}

// ########################## RECEIVE from Hoverboard 1 ##########################
void Receive1()
{
    // Check for new data availability in the Serial buffer
    if (Serial1.available()) {
        incomingByte1    = Serial1.read();                                          // Read the incoming byte
        bufStartFrame = ((uint16_t)(incomingByte1) << 8) | incomingBytePrev1;       // Construct the start frame
    }
    else {
        return;
    }

  // If DEBUG_RX is defined print all incoming bytes
  #ifdef DEBUG_RX
        Serial.print(incomingByte1);
        return;
    #endif

    // Copy received data
    if (bufStartFrame == START_FRAME) {                     // Initialize if new data is detected
        p       = (byte *)&NewFeedback;
        *p++    = incomingBytePrev1;
        *p++    = incomingByte1;
        idx     = 2;  
    } else if (idx >= 2 && idx < sizeof(SerialFeedback)) {  // Save the new received data
        *p++    = incomingByte1; 
        idx++;
    } 
    
    // Check if we reached the end of the package
    if (idx == sizeof(SerialFeedback)) {
        uint16_t checksum;
        checksum = (uint16_t)(NewFeedback.start ^ NewFeedback.cmd1 ^ NewFeedback.cmd2 ^ NewFeedback.speedR_meas ^ NewFeedback.speedL_meas
                            ^ NewFeedback.batVoltage ^ NewFeedback.boardTemp ^ NewFeedback.cmdLed);

        // Check validity of the new data
        if (NewFeedback.start == START_FRAME && checksum == NewFeedback.checksum) {
            // Copy the new data
            memcpy(&Feedback, &NewFeedback, sizeof(SerialFeedback));

            // Print data to built-in Serial
            Serial.print("1: ");   Serial.print(Feedback.cmd1);
            Serial.print(" 2: ");  Serial.print(Feedback.cmd2);
            Serial.print(" 3: ");  Serial.print(Feedback.speedR_meas);
            Serial.print(" 4: ");  Serial.print(Feedback.speedL_meas);
            Serial.print(" 5: ");  Serial.print(Feedback.batVoltage);
            Serial.print(" 6: ");  Serial.print(Feedback.boardTemp);
            Serial.print(" 7: ");  Serial.println(Feedback.cmdLed);
        } else {
          Serial.println("Non-valid data skipped");
        }
        idx = 0;    // Reset the index (it prevents to enter in this if condition in the next cycle)
    }

    // Update previous states
    incomingBytePrev1 = incomingByte1;
}


// ########################## END ##########################
