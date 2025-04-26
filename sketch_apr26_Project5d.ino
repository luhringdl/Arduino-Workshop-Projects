// Project 5d - Controlling Traffic by Levi Luhring

#include <IRremote.h>  // Add IR remote library

// Define the pins for buttons and lights
                              //wire color
#define eastButton 13         //black
#define eastGreen 10          //white
#define eastYellow 11         //gray
#define eastRed 12            //purple

#define westButton 3          //blue
#define westGreen 0           //green
#define westYellow 1          //yellow
#define westRed 2             //orange

#define pedestrianButton 5    //red
#define pedestrianLight 6     //brown

#define irReceive 9 //left bump black
/*      IR GND mid bump         white
        IR Vs  right bump       gray
        Ground                  purple */

#define yellowBlinkTime 500         // 0.5 seconds for yellow light blink
boolean trafficWest = true;         // west = true, east = false
int flowTime = 10000;               // Time for traffic flow
int changeDelay = 2000;             // Time between color changes

IRrecv irrecv(irReceive);           // Set up IR receiver
unsigned long lastIRCode = 0;       // Track last IR code

void setup() {
  pinMode(westButton, INPUT);
  pinMode(eastButton, INPUT);
  pinMode(westRed, OUTPUT);
  pinMode(westYellow, OUTPUT);
  pinMode(westGreen, OUTPUT);
  pinMode(eastRed, OUTPUT);
  pinMode(eastYellow, OUTPUT);
  pinMode(eastGreen, OUTPUT);

  pinMode(pedestrianButton, INPUT);
  pinMode(pedestrianLight, OUTPUT);
  digitalWrite(pedestrianLight, LOW);  // Turn off pedestrian light initially

  // Set initial state: west starts green, east red
  digitalWrite(westRed, LOW);
  digitalWrite(westYellow, LOW);
  digitalWrite(westGreen, HIGH);
  digitalWrite(eastRed, HIGH);
  digitalWrite(eastYellow, LOW);
  digitalWrite(eastGreen, LOW);

  Serial.begin(9600);    // Start serial for debug
  irrecv.enableIRIn();   // Start IR receiver
}

void loop() {
  if (digitalRead(westButton) == HIGH && !trafficWest) {
    allowTrafficFlow(true);
  }
  if (digitalRead(eastButton) == HIGH && trafficWest) {
    allowTrafficFlow(false);
  }
  if (digitalRead(pedestrianButton) == HIGH) {
    allowPedestrianCrossing();
  }

  // IR Remote Handling
  if (irrecv.decode()) {
    unsigned long code = irrecv.decodedIRData.decodedRawData;
    if (code != 0 && code != lastIRCode) { // Ignore repeated signals
      lastIRCode = code;
      // Check which button was pressed
      switch (code) {
        case 0xD02B3085: // Button 4 for westbound
          if (!trafficWest) {
            Serial.println("IR: Westbound Triggered");
            allowTrafficFlow(true);
          }
          break;
        case 0xD02E3085: // Button 7 for eastbound
          if (trafficWest) {
            Serial.println("IR: Eastbound Triggered");
            allowTrafficFlow(false);
          }
          break;
        default:
          Serial.print("IR: Unknown Code ");
          Serial.println(code, HEX);
          break;
      }
    }
    irrecv.resume(); // Ready for next IR signal
  }
}

// Function to change the traffic lights from green to red
void changeLightsToRed(int greenPin, int yellowPin, int redPin) {
  digitalWrite(greenPin, LOW);
  digitalWrite(yellowPin, HIGH);
  delay(changeDelay);
  digitalWrite(yellowPin, LOW);
  digitalWrite(redPin, HIGH);
  delay(changeDelay);
}

// Function to blink the yellow light before allowing traffic flow
void blinkYellowLight(int yellowPin) {
  for (int i = 0; i < 5; i++) {
    digitalWrite(yellowPin, LOW);
    delay(yellowBlinkTime);
    digitalWrite(yellowPin, HIGH);
    delay(yellowBlinkTime);
  }
  digitalWrite(yellowPin, LOW);
}

// Function to handle traffic flow switch
void allowTrafficFlow(bool toWest) {
  trafficWest = toWest;
  delay(flowTime);  // Allow current traffic to flow

  if (toWest) {
    changeLightsToRed(eastGreen, eastYellow, eastRed);
    blinkYellowLight(westYellow);
    digitalWrite(westRed, LOW);
    digitalWrite(westGreen, HIGH);
  } else {
    changeLightsToRed(westGreen, westYellow, westRed);
    blinkYellowLight(eastYellow);
    digitalWrite(eastRed, LOW);
    digitalWrite(eastGreen, HIGH);
  }
}

// Function to handle pedestrian crossing
void allowPedestrianCrossing() {
  // Stop both directions
  changeLightsToRed(westGreen, westYellow, westRed);
  changeLightsToRed(eastGreen, eastYellow, eastRed);

  // Turn on pedestrian light
  digitalWrite(pedestrianLight, HIGH);

  // Keep it on for 20 seconds
  delay(20000);

  // Turn off pedestrian light
  digitalWrite(pedestrianLight, LOW);

  // Resume traffic flow in the direction it was before
  if (trafficWest) {
    digitalWrite(westRed, LOW);
    digitalWrite(westGreen, HIGH);
  } else {
    digitalWrite(eastRed, LOW);
    digitalWrite(eastGreen, HIGH);
  }
}
