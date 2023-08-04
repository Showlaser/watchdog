#define _redLaserOutputPin = A0;
#define _greenLaserOutputPin = A1;
#define _blueLaserOutputPin = A2;
#define _xGalvoFeedbackPin = A3;
#define _yGalvoFeedbackPin = A4;
#define _laserSwitch = D4; 
#define _beamShutter = D3;

enum watchdogStatus {
  Ready = 0,
  HardwareCheck = 0,
  Defect = 0,
  UnsafeOperation = 0
};

watchdogStatus _watchdogStatus;

/**
 @brief This function checks if the provided value is between the min and max values provided

 @param value the value to check if it is between min and max
 @param min the lowest value that is accepted
 @param max the highest value that is accepted 
*/
bool isBetweenOrEqual(int value, int min, int max) {
  return value >= min && value <= max;
}

/**
 @brief Performs a hardware check for the galvo feedback

 @param xMin the lowest value that is accepted from the x galvo feedback
 @param xMax the highest value that is accepted from the x galvo feedback
 @param yMin the lowest value that is accepted from the y galvo feedback
 @param yMax the highest value that is accepted from the y galvo feedback
*/
bool performGalvoCheck(int xMin, int xMax, int yMin, int yMax) {
  unsigned short attempts = 0;
  while (!isBetweenOrEqual(analogRead(_xGalvoFeedbackPin), xMin, xMax) && !isBetweenOrEqual(analogRead(_yGalvoFeedbackPin), yMin, yMax)) {
    delay(50);
    attempts++;
    if (attempts > 40) {
      return false;
    }
  }

  return true;
}

/**
 @brief Performs a hardware check for the laser feedback

 @param min the lowest value that is accepted from the laser feedback
 @param max the highest value that is accepted from the laser feedback
*/
bool performLaserOutputCheck(int min, int max) {
  unsigned short attempts = 0;
  while (!isBetweenOrEqual(analogRead(_redLaserOutputPin), 10, 100) && !isBetweenOrEqual(analogRead(_greenLaserOutputPin), 10, 100) && !isBetweenOrEqual(analogRead(_blueLaserOutputPin), 10, 100)) {
    delay(50);
    attempts++;
    if (attempts > 40) {
      return false;
    }
  }

  return true;
}

/**
 @brief This function performs an hardware check on the galvo's and the laser output feedback signal
 @brief if the check fails the _watchdogStatus will be set to "Defect"
*/
void performHardwareCheck() {
  bool galvosMovedToCenter = performGalvoCheck(-100, 100, -100, 100); // TODO get the actual values read by the Arduino Nano
  bool galvosMovedToEdge = performGalvoCheck(3900, 4000, 3900, 4000); // TODO get the actual values read by the Arduino Nano
  bool galvosMovedToEdge2 = performGalvoCheck(-3900, -4000, -3900, -4000); // TODO get the actual values read by the Arduino Nano

  bool galvoCheckFailed = !galvosMovedToCenter && !galvosMovedToEdge && !galvosMovedToEdge2;
  if (galvoCheckFailed) {
    _watchdogStatus = watchdogStatus::Defect;
  }

  bool laserOutputReadsZero = performLaserOutputCheck(0, 0); // TODO get the actual values read by the Arduino Nano
  bool laserOutputReadsHigh = performLaserOutputCheck(50, 100); // TODO get the actual values read by the Arduino Nano
  bool laserOutputReadsZeroAgain = performLaserOutputCheck(0, 0); // TODO get the actual values read by the Arduino Nano
  
  bool laserCheckFailed = !laserOutputReadsZero && !laserOutputReadsHigh && !laserOutputReadsZeroAgain;
  if (laserCheckFailed) {
    _watchdogStatus = watchdogStatus::Defect;
  }
}

void setup() {
  performHardwareCheck();
  if (_watchdogStatus == watchdogStatus::Defect) {
    while (true) {
      // Infinite loop to prevent the laser from executing commands
    }
  }

  _watchdogStatus == watchdogStatus::Ready;
  pinMode(_laserSwitch, OUTPUT);
  pinmode(_beamShutter, OUTPUT);

  digitalWrite(_laserSwitch, HIGH); 
  digitalWrite(_beamShutter, HIGH); 
}

void loop() {
  // put your main code here, to run repeatedly:
}
