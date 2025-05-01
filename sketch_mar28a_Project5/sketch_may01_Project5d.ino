// Project 5d - Controlling Traffic by Levi Luhring

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

#define rfWestButton 8        // RF receiver output on D8
#define rfEastButton 9        // RF receiver output on D9

#define yellowBlinkTime 500   // 0.5 seconds for yellow light blink
boolean trafficWest = true;   // west = true, east = false
int flowTime = 8000;          // Time for traffic flow
int changeDelay = 2000;       // Time between color changes

void setup() {
  // Button pins configured as inputs (external pull-up resistors used)
  pinMode(westButton, INPUT);
  pinMode(eastButton, INPUT);
  pinMode(pedestrianButton, INPUT);

  // Button pins for RF receiver
  pinMode(rfWestButton, INPUT);
  pinMode(rfEastButton, INPUT);

  // Output pins for traffic lights and pedestrian light
  pinMode(westRed, OUTPUT);
  pinMode(westYellow, OUTPUT);
  pinMode(westGreen, OUTPUT);
  pinMode(eastRed, OUTPUT);
  pinMode(eastYellow, OUTPUT);
  pinMode(eastGreen, OUTPUT);
  pinMode(pedestrianLight, OUTPUT);

  // Start with pedestrian light off
  digitalWrite(pedestrianLight, LOW);

  // Initial traffic light state: west green, east red
  digitalWrite(westRed, LOW);
  digitalWrite(westYellow, LOW);
  digitalWrite(westGreen, HIGH);
  digitalWrite(eastRed, HIGH);
  digitalWrite(eastYellow, LOW);
  digitalWrite(eastGreen, LOW);
}

void loop() {
  // Detect button presses (active LOW)
  if ((digitalRead(westButton) == LOW || digitalRead(rfWestButton) == LOW) && !trafficWest) {
    allowTrafficFlow(true);  // Switch to westbound traffic
  }
  if ((digitalRead(eastButton) == LOW || digitalRead(rfEastButton) == LOW) && trafficWest) {
    allowTrafficFlow(false); // Switch to eastbound traffic
  }
  if (digitalRead(pedestrianButton) == LOW) {
    allowPedestrianCrossing(); // Let pedestrians cross
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
  delay(flowTime);  // Allow current traffic to flow before switching

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
  delay(10000);  // 10 seconds walk time
  digitalWrite(pedestrianLight, LOW);

  // Resume traffic flow in the previous direction
  if (trafficWest) {
    digitalWrite(westRed, LOW);
    digitalWrite(westGreen, HIGH);
  } else {
    digitalWrite(eastRed, LOW);
    digitalWrite(eastGreen, HIGH);
  }
}