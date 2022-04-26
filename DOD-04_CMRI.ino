#include <Auto485.h>

#include <CMRI.h>

String RailwayName   = "Little Wicket Railway";
                                     // DEFAULT = "Your Model Railway"              // SET THIS TO DISPLAY YOUR MODEL RAILWAY NAME IN THE INTRODUCTION SCREEN
                                                                                    //  This is purely for fun and does not alter any other part of the process.
#define SENSOR1 2
#define SENSOR2 3
#define SENSOR3 4
#define SENSOR4 5
#define SENSOR5 6
#define SENSOR6 7
#define SENSOR7 8
#define SENSOR8 9

#define DE_PIN 12

#define DIP1 15
#define DIP2 16
#define DIP3 17
#define DIP4 18
#define DIP5 19

int CMRI_ADDR = 0;
bool sensorStatus[9];
int sensorPin[9];

Auto485 bus(DE_PIN);

CMRI cmri(0, 8, 0, bus);

const String vers    = "1.0.0";            // Software Version Number

void setup() {

  sensorPin[1] = SENSOR1;                  // SETS THE sensorPin array to
  sensorPin[2] = SENSOR2;                  // the Pin numbers defined above
  sensorPin[3] = SENSOR3;                  // 
  sensorPin[4] = SENSOR4;                  // This is used so we can use a
  sensorPin[5] = SENSOR5;                  // for loop instead of repeating
  sensorPin[6] = SENSOR6;                  // the digitalRead line for each
  sensorPin[7] = SENSOR7;                  // sensor.
  sensorPin[8] = SENSOR8;

  
  pinMode(DIP1, INPUT_PULLUP);             // SET ALL SENSOR AND DIP INPUTS
  pinMode(DIP2, INPUT_PULLUP);             // TO INPUT_PULLUP TO GIVE A STEADY
  pinMode(DIP3, INPUT_PULLUP);             // STATE WITHOUT THE REQUIREMENT FOR
  pinMode(DIP4, INPUT_PULLUP);             // ADDITIONAL RESISTORS ON BOARD.
  pinMode(DIP5, INPUT_PULLUP);             //
  pinMode(SENSOR1, INPUT_PULLUP);          //
  pinMode(SENSOR2, INPUT_PULLUP);          //
  pinMode(SENSOR3, INPUT_PULLUP);          //
  pinMode(SENSOR4, INPUT_PULLUP);          //
  pinMode(SENSOR5, INPUT_PULLUP);          //
  pinMode(SENSOR6, INPUT_PULLUP);          //
  pinMode(SENSOR7, INPUT_PULLUP);          //
  pinMode(SENSOR8, INPUT_PULLUP);          //

  sensorStatus[1] = false;                 // SET THE INITIAL STATE OF ALL
  sensorStatus[2] = false;                 // SENSORS TO FALSE.
  sensorStatus[3] = false;                 // 
  sensorStatus[4] = false;                 // This forces the status monitor
  sensorStatus[5] = false;                 // and CMRI to update any occupied
  sensorStatus[6] = false;                 // blocks immediately at startup.
  sensorStatus[7] = false;                 // 
  sensorStatus[8] = false;                 // 

  /* ------------------------------------------------------------------------------------------------
      Sets the CMRI NODE ADDRESS using the dip switches on the board by adding up the displayed
      numbers to set the board ID.
     ------------------------------------------------------------------------------------------------ */  
  if (digitalRead(DIP1)==LOW) CMRI_ADDR = CMRI_ADDR + 1;
  if (digitalRead(DIP2)==LOW) CMRI_ADDR = CMRI_ADDR + 2;
  if (digitalRead(DIP3)==LOW) CMRI_ADDR = CMRI_ADDR + 4;
  if (digitalRead(DIP4)==LOW) CMRI_ADDR = CMRI_ADDR + 8;
  if (digitalRead(DIP5)==LOW) CMRI_ADDR = CMRI_ADDR + 16;
  cmri.set_address(CMRI_ADDR);
  
  bus.begin(19200);               // Opens the RS485 serial connection for CMRI to communicate.
  Serial.begin(9600);             // Opens the Serial Monitor for Arduino IDE monitoring.
  
  cls();
  displayIntro();
}

void loop() {
  /* ------------------------------------------------------------------------------------------------
      Runs though a for loop checking each sensorPin from 1 to 8, and checks it against the pinStatus
      for the current sensor (i) in the for loop.
      If the pinStatus does not match the sensorStatus, it sets sensorStatus[0] to true so we can run
      the displayStatus() procedure.
     ------------------------------------------------------------------------------------------------ */
  bool pinStatus;                           // Temporary Boolean variable to hold the status of the
                                            // current sensorPin value to allow comparison to the stored
                                            // stored value in the sensorStatus array.
  bool change = false;                      // Temporary Boolean variable to hold the status of the
                                            // change to the current sensorPin.
  
  for (int loopcount = 1; loopcount < 9; loopcount++) {
  
      // Set the current Sensor value to the relevent bit of the CMRI data string.
      // (note cmri.set_bit value is 0-7 so need to minus 1 from loopcount)
    cmri.set_bit(loopcount-1, !digitalRead(sensorPin[loopcount]));
    
    if(digitalRead(sensorPin[loopcount])==HIGH) pinStatus = false ; else pinStatus = true ;
      
                                                        // sensorStatus[0] is used for the statusChange variable storage.
      if(pinStatus != sensorStatus[loopcount]) {        // THERE IS A CHANGE IN A SENSOR SO SET sensorStatus[0] to TRUE
        sensorStatus[0]=true; change = true;            // THEN SET THE sensorStatus variable to 1 (HIGH) or 0 (LOW)
        if(pinStatus == false) sensorStatus[loopcount]=false; else sensorStatus[loopcount]=true;
      }
      
      change = false;                                   // Set change to false ready to check the next sensor.
    }
  
  if (sensorStatus[0]==true) displayStatus();           // Something changed, so display a new status line and trigger CMRI update
  
}




/* ------------------------------------------------------------------------------------------------
    Displays the Track Status to the Serial Monitor for each of the 8 blocks using a for loop,
    and then sends the CMRI update. This means the CMRI update only sends when changes occur.
   ------------------------------------------------------------------------------------------------ */
void displayStatus() {
  Serial.print("               ");      // Spaces at the start of the status line to column the data
  
  for (int loopcount = 1; loopcount < 9; loopcount++) {
    if(sensorStatus[loopcount]==true) Serial.print("XXXX"); else Serial.print("....");
    if(loopcount==8) Serial.println(" #"); else Serial.print(".");
  }
  cmri.process();
  sensorStatus[0]=false;                // Reset sensor changed variable to false.
}



/* ------------------------------------------------------------------------------------------------
    Displays the Introduction Header onto the serial monitor.
   ------------------------------------------------------------------------------------------------ */
void displayIntro() {
  Serial.println("+-----------------------------------------------------+ ");
  Serial.println("¦          DOD-04 Digital Occupancy Detector          ¦ ");
  Serial.println("¦                 by DATAjam Model Rail               ¦ ");
  Serial.println("+-----------------------------------------------------+ ");
  Serial.println("¦        www.djrail.co.uk - info@djrail.co.uk         ¦ ");
  Serial.println("+-----------------------------------------------------+ ");
    Serial.print("                                        Version: "); Serial.println(vers);                                  // Displays Version Number

  blankLN();
  Serial.print("  ");Serial.println(RailwayName);
  Serial.print("  CMRI Board Address - "); if(CMRI_ADDR <10) Serial.print("0"); Serial.print(CMRI_ADDR);
  blankLN();
  Serial.println("              [----- MASTER -----] [----- SLAVE -----]  ");
  Serial.println("Track Status:  BL-1 BL-2 BL-3 BL-4 BL-5 BL-6 BL-7 BL-8  ");
}


void cls() {                                                                        // Loops to display 80 lines of nothing to simulate CLS command
  for (int i = 0; i <= 80; i++) {
    blankLN();
  }
}

void blankLN() {                                                                    // inserts one blank line into serial monitor - used for formatting.
    Serial.println("                                                        ");
}


void intro() {                                                                      // Displays introduction at start of program.
  Serial.print(RailwayName); Serial.println(" - Servo Calibration");                // "Railway Name" - Servo Calibration
  Serial.print("Version: "); Serial.println(vers);                                  // Displays Version Number
  blankLN();
}
