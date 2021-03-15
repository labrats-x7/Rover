// *******************************************************************
//
//  Arduino Mega (5V) as a bridge between a Raspberry Pi and 2 Hoverboards with Emanuels hoverboard firmware hack
//
//  code based on "hoverserial.ino" from https://github.com/EmanuelFeru/hoverboard-firmware-hack-FOC
//  
//  examples of non blocking serial communication for arduino: https://forum.arduino.cc/index.php?topic=288234.0
//
//  more undocumented options to send to Hoverboard over USART: https://github.com/EmanuelFeru/hoverboard-firmware-hack-FOC/issues/143
//
// *******************************************************************
// INFO:
// • This sketch uses the the (HW) Serial Interface 1 and 2 to communicate and send commands to the hoverboards
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
//#define DEBUG_RX                     // [-] Debug received data. Prints all bytes to serial (comment-out to disable)

// Global variables
int16_t SPEED_FL;
int16_t SPEED_FR;
int16_t SPEED_RL;
int16_t SPEED_RR;
int16_t STEER_SERVO;
int16_t HEARTBEAT;
const byte numChars = 64;
byte receivedChars[numChars];

typedef struct{
   uint16_t start;
   int16_t  steer;
   int16_t  speed;
   uint16_t checksum;
} SerialCommand;

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

boolean newData = false;

// ########################## SETUP ##########################
void setup() 
{
  Serial.begin(SERIAL_BAUD);
  Serial.println("Rover Serial v0.1");
  Serial1.begin(HOVER_SERIAL_BAUD);
  Serial2.begin(HOVER_SERIAL_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);
}

// ########################## LOOP ##########################

void loop(void) { 
    static unsigned long iTimeSend = 0;
    unsigned long timeNow = millis();
    static long on = 0;

    receivePi();          // Check for new received data from pi
    parseData();       // Parse data from pi to global SPEED_XX variables  
    //ReceiveFront();        // Check for new received data from Front Hoverboard
    //ReceiveRear();        // Check for new received data from Rear Hoverboard

    // Send commands to Hoverboards
    if (iTimeSend > timeNow) return;
    iTimeSend = timeNow + TIME_SEND;
    SendFront(SPEED_FL, SPEED_FR);
    SendRear(SPEED_RL, SPEED_RR);

    // Blink the LED
   //digitalWrite(LED_BUILTIN, (timeNow%2000)<1000);
   digitalWrite(LED_BUILTIN, on == 1);
   on++;
   if(on > 1){
       on = 0;
   }
}

// ########################## RECEIVE from Pi ##########################

void receivePi() {
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

// ########################## PARSE Data from Pi ##########################
void parseData() {      // split the data into its parts
    if(!newData)return;
    newData = false;
    char * strtokIndx; // this is used by strtok() as an index
  
    strtokIndx = strtok(receivedChars,",");      // get the first part - STEER
    SPEED_FL = atoi(strtokIndx); // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off 
    SPEED_FR = atoi(strtokIndx);     // convert this part to an integer

    /*
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off 
    SPEED_RL = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off 
    SPEED_RR = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off 
    STEER_SERVO = atoi(strtokIndx);  // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off 
    HEARTBEAT = atoi(strtokIndx);  // convert this part to an integer
    */
}

// ########################## SEND to Front Hoverboard ##########################
void SendFront(int16_t SPEED_FL, int16_t SPEED_FR){
    SerialCommand CommandFront;

    // Create command
    CommandFront.start    = (uint16_t)START_FRAME;
    CommandFront.steer    = (int16_t)SPEED_FL;
    CommandFront.speed    = (int16_t)SPEED_FR;
    CommandFront.checksum = (uint16_t)(CommandFront.start ^ CommandFront.steer ^ CommandFront.speed);

    // Write to Serial
    Serial1.write((uint8_t *) &CommandFront, sizeof(CommandFront));
    //Serial.write((uint8_t *) &CommandFront, sizeof(CommandFront));
}

// ########################## SEND to Rear Hoverboard ##########################
void SendRear(int16_t SPEED_RL, int16_t SPEED_RR){
    SerialCommand CommandRear;

    // Create command
    CommandRear.start    = (uint16_t)START_FRAME;
    CommandRear.steer    = (int16_t)SPEED_RL;
    CommandRear.speed    = (int16_t)SPEED_RR;
    CommandRear.checksum = (uint16_t)(CommandRear.start ^ CommandRear.steer ^ CommandRear.speed);

    // Write to Serial
    Serial2.write((uint8_t *) &CommandRear, sizeof(CommandRear));
}

// ########################## RECEIVE debug from Front Hoverboard and send to Pi ##########################
void ReceiveFront(){
    static byte *p;                // Pointer declaration for the new received data
    static byte incomingByte;
    static byte incomingBytePrev;
    static uint8_t idx = 0;                        // Index for new data pointer
    static uint16_t bufStartFrame;                 // Buffer Start Frame
    static SerialFeedback Feedback;
    static SerialFeedback NewFeedback;

    // Check for new data availability in the Serial buffer
    if (Serial1.available()) {
        incomingByte    = Serial1.read();                                          // Read the incoming byte
        bufStartFrame = ((uint16_t)(incomingByte) << 8) | incomingBytePrev;       // Construct the start frame
    }
    else {
        return;
    }

    // If DEBUG_RX is defined print all incoming bytes
    #ifdef DEBUG_RX
        Serial.print(incomingByte);
        return;
    #endif

    // Copy received data
    if (bufStartFrame == START_FRAME) {                     // Initialize if new data is detected
        p       = (byte *)&NewFeedback;
        *p++    = incomingBytePrev;
        *p++    = incomingByte;
        idx     = 2;  
    }
    else if (idx >= 2 && idx < sizeof(SerialFeedback)) {  // Save the new received data
        *p++    = incomingByte; 
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
            Serial.print("F1: ");   Serial.print(Feedback.cmd1);
            Serial.print(" F2: ");  Serial.print(Feedback.cmd2);
            Serial.print(" F3: ");  Serial.print(Feedback.speedR_meas);
            Serial.print(" F4: ");  Serial.print(Feedback.speedL_meas);
            Serial.print(" F5: ");  Serial.print(Feedback.batVoltage);
            Serial.print(" F6: ");  Serial.print(Feedback.boardTemp);
            Serial.print(" F7: ");  Serial.println(Feedback.cmdLed);
        } else {
          Serial.println("F: Non-valid data skipped");
        }
        idx = 0;    // Reset the index (it prevents to enter in this if condition in the next cycle)
    }

    // Update previous states
    incomingBytePrev = incomingByte;
}

// ########################## RECEIVE debug from Rear Hoverboard and send to Pi ##########################
void ReceiveRear(){
    static byte *p;                // Pointer declaration for the new received data
    static byte incomingByte;
    static byte incomingBytePrev;
    static uint8_t idx = 0;                        // Index for new data pointer
    static uint16_t bufStartFrame;                 // Buffer Start Frame
    static SerialFeedback Feedback;
    static SerialFeedback NewFeedback;

    // Check for new data availability in the Serial buffer
    if (Serial2.available()) {
        incomingByte    = Serial2.read();                                          // Read the incoming byte
        bufStartFrame = ((uint16_t)(incomingByte) << 8) | incomingBytePrev;       // Construct the start frame
    }
    else {
        return;
    }

    // If DEBUG_RX is defined print all incoming bytes
    #ifdef DEBUG_RX
        Serial.print(incomingByte);
        return;
    #endif

    // Copy received data
    if (bufStartFrame == START_FRAME) {                     // Initialize if new data is detected
        p       = (byte *)&NewFeedback;
        *p++    = incomingBytePrev;
        *p++    = incomingByte;
        idx    = 2;  
    } else if (idx >= 2 && idx < sizeof(SerialFeedback)) {  // Save the new received data
        *p++    = incomingByte; 
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
            Serial.print("R1: ");   Serial.print(Feedback.cmd1);
            Serial.print(" R2: ");  Serial.print(Feedback.cmd2);
            Serial.print(" R3: ");  Serial.print(Feedback.speedR_meas);
            Serial.print(" R4: ");  Serial.print(Feedback.speedL_meas);
            Serial.print(" R5: ");  Serial.print(Feedback.batVoltage);
            Serial.print(" R6: ");  Serial.print(Feedback.boardTemp);
            Serial.print(" R7: ");  Serial.println(Feedback.cmdLed);
        }
        else {
          Serial.println("R: Non-valid data skipped");
        }
        idx = 0;    // Reset the index (it prevents to enter in this if condition in the next cycle)
    }

    // Update previous states
    incomingBytePrev = incomingByte;
}

// ########################## END ##########################