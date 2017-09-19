#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <bluefruit.h>

bool debug = false;

// BLE Service
BLEDis  bledis;
BLEUart bleuart;
BLEBas  blebas;

// Function prototypes for packetparser.cpp
uint8_t readPacket (BLEUart *ble_uart, uint16_t timeout);
float   parsefloat (uint8_t *buffer);
void    printHex   (const uint8_t * data, const uint32_t numBytes);

// Packet buffer
extern uint8_t packetbuffer[];

bool micMode = true;
const int sampleWindow = 20; // Sample window width in mS (50 mS = 20Hz). 20 mS ~= 100Hz, speech.
unsigned int sample;

// Used for averaging all the audio samples currently in the buffer
uint8_t oldsum = 0;
uint8_t faceMode = 0;
uint8_t counter = 0;

#ifndef _BV
  #define _BV(bit) (1<<(bit))
#endif

#define MATRIX_EYES         0
#define MATRIX_MOUTH_LEFT   1
#define MATRIX_MOUTH_MIDDLE 2
#define MATRIX_MOUTH_RIGHT  3

Adafruit_8x8matrix matrix[4] = { // Array of Adafruit_8x8matrix objects
  Adafruit_8x8matrix(), Adafruit_8x8matrix(),
  Adafruit_8x8matrix(), Adafruit_8x8matrix() };

static const uint8_t PROGMEM // Bitmaps are stored in program memory
  blinkImg[][8] = {    // Eye animation frames
  { B11111000,         // Fully open eye
    B11111100,         // The eye matrices are installed
    B11111110,         // in the mask at a 45 degree angle...
    B11111111,         // you can edit these bitmaps if you opt
    B11111111,         // for a rectilinear arrangement.
    B11111111,
    B01111111,
    B00111111 },
  { B11110000,
    B11111100,
    B11111110,
    B11111110,
    B11111111,
    B11111111,
    B01111111,
    B00111111 },
  { B11100000,
    B11111000,
    B11111100,
    B11111110,
    B11111110,
    B01111111,
    B00111111,
    B00011111 },
  { B11000000,
    B11110000,
    B11111000,
    B11111100,
    B11111110,
    B01111110,
    B00111111,
    B00011111 },
  { B10000000,
    B11100000,
    B11111000,
    B11111100,
    B01111100,
    B01111110,
    B00111110,
    B00001111 },
  { B10000000,
    B11000000,
    B11100000,
    B11110000,
    B01111000,
    B01111100,
    B00111110,
    B00001111 },
  { B10000000,
    B10000000,
    B11000000,
    B01000000,
    B01100000,
    B00110000,
    B00011100,
    B00000111 },
  { B10000000,         // Fully closed eye
    B10000000,
    B10000000,
    B01000000,
    B01000000,
    B00100000,
    B00011000,
    B00000111 } },
  pupilImg[] = {      // Pupil bitmap
    B00000000,        // (only top-left 7x7 is used)
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000 },
  landingImg[] = {    // This is a bitmask of where
    B11110000,        // the pupil can safely "land" when
    B11111000,        // a new random position is selected,
    B11111100,        // so it doesn't run too far off the
    B11111110,        // edge and look bad.  If you edit the
    B11111110,        // eye or pupil bitmaps, you may want
    B01111110,        // to adjust this...use '1' for valid
    B00111100,        // pupil positions, '0' for off-limits
    B00000110 },      // points.
   pumpkinImg[] = {
    B01111000,
    B11111010,
    B11101100,
    B10111111,
    B11011011,
    B01101111,
    B00110111,
    B00011110   },
   skullImg[] = {
    B00000000,
    B00111100,
    B01101110,
    B01111110,
    B11101010,
    B01111110,
    B00111100,
    B00010000,  },
   xImg[]     = {
    B00010000,
    B00010000,
    B00010000,
    B11111110,
    B00010000,
    B00010000,
    B00010000,
    B00000000,  },
   batImg[] = {
    B11000000,
    B01100100,
    B00111110,
    B00111100,
    B00011100,
    B00101110,
    B00000011,
    B00000001
   },
   tongueImg[][24] = { 
    {
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B11111111, B11111111, B11111111, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000 
    },
    {
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B11111111, B11111111, B11111111, 
    B00000000, B11111111, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000 
    },
    {
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B11111111, B11111111, B11111111, 
    B00000000, B11110111, B00000000, 
    B00000000, B11111111, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000 
    },
    {
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B11111111, B11111111, B11111111, 
    B00000000, B11110111, B00000000, 
    B00000000, B11110111, B00000000, 
    B00000000, B00011100, B00000000, 
    B00000000, B00000000, B00000000 
    },
    {
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B11111111, B11111111, B11111111, 
    B00000000, B11110111, B00000000, 
    B00000000, B11110111, B00000000, 
    B00000000, B01111110, B00000000, 
    B00000000, B00000000, B00000000 
    },
    {
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000, 
    B11111111, B11111111, B11111111, 
    B00000000, B11110111, B00000000, 
    B00000000, B11110111, B00000000, 
    B00000000, B11111111, B00000000, 
    B00000000, B01111110, B00000000 
    } },
  mouthImg[][24] = {                 // Mouth animation frames
  { B00000000, B11000011, B00000000, // Mouth position 0
    B00000001, B01000010, B10000000, // Unlike the original 'roboface'
    B00111110, B01111110, B01111100, // sketch (using AF animation),
    B11000000, B00000000, B00000011, // the mouth positions here are
    B00000000, B00000000, B00000000, // proportional to the current
    B00000000, B00000000, B00000000, // sound amplitude.  Position 0
    B00000000, B00000000, B00000000, // is closed, #7 is max open.
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // Mouth position 1
    B00000000, B11000011, B00000000,
    B00000011, B01111110, B11000000,
    B00111110, B00000000, B01111100,
    B11000000, B00000000, B00000011,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // Mouth position 2
    B00000011, B11111111, B11000000,
    B00011011, B01111110, B11011000,
    B01111110, B01111110, B01111110,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // Mouth position 3
    B00000011, B11111111, B11000000,
    B01111011, B11111111, B11011110,
    B00011111, B01111110, B11111000,
    B00001110, B01111110, B01110000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // Mouth position 4
    B00000001, B11111111, B10000000,
    B01111001, B11111111, B10011110,
    B00111101, B11111111, B10111100,
    B00011111, B01111110, B11111000,
    B00000110, B01111110, B01100000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // Mouth position 5
    B00000001, B11111111, B10000000,
    B00111001, B11111111, B10011100,
    B00011101, B11111111, B10111000,
    B00011111, B11111111, B11111000,
    B00001111, B01111110, B11110000,
    B00000110, B01111110, B01100000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // Mouth position 6
    B00000001, B11111111, B10000000,
    B00111001, B11111111, B10011100,
    B00111101, B11111111, B10111100,
    B00011111, B11111111, B11111100,
    B00011111, B11111111, B11111000,
    B00001111, B01111110, B11110000,
    B00000110, B01111110, B01100000 },
  { B00000000, B01111110, B00000000, // Mouth position 7
    B00001001, B11111111, B10010000,
    B00011001, B11111111, B10011000,
    B00011101, B11111111, B10111000,
    B00011111, B11111111, B11111000,
    B00011111, B11111111, B11111000,
    B00001111, B01111110, B11110000,
    B00000110, B01111110, B01100000 } },
  blinkIndex[] = { 1, 2, 3, 4, 5, 6, 7, 6, 5, 3, 2, 1 }, // Blink bitmap sequence
  matrixAddr[] = { 0x70, 0x71, 0x72, 0x73 }; // I2C addresses of 4 matrices

static const uint8_t * scrollImg = skullImg;

uint8_t
  blinkCountdown = 100, // Countdown to next blink (in frames)
  gazeCountdown  =  75, // Countdown to next eye movement
  gazeFrames     =  50, // Duration of eye movement (smaller = faster)
  mouthPos       =   0, // Current image number for mouth
  mouthCountdown =  10; // Countdown to next mouth change
int8_t
  eyeX = 3, eyeY = 3,   // Current eye position
  newX = 3, newY = 3,   // Next eye position
  dX   = 0, dY   = 0;   // Distance from prior to new position


void scrollText(String t) {
  int scroll_length = 16 + (t.length() * 6); //Calculate width of each character (5), add 1 for spacing. Add 16 to the total for 2 more screens.
  for (int x=0; x >= -scroll_length; x--) {
    for(int i = 1; i <= 3; i++) {
          matrix[i].clear();
          matrix[i].setCursor(x+(16 - (i-1)*8),0);
          matrix[i].print(t);
          matrix[i].writeDisplay();
    }
    delay(50);
  }
}

void scrollBMP(const uint8_t *bmp) {
  int scroll_length = 24;
  for (int x=8; x >= -scroll_length; x--) {
    for(int i = 1; i <= 3; i++) {
          matrix[i].clear();
          matrix[i].drawBitmap(x+(16 - (i-1)*8),0, bmp, 8,8, LED_ON);
          matrix[i].writeDisplay();
    }
    delay(50);
  }
}

//////////////////////////////////// SETUP

void setup() {
  // Seed random number generator from an unused analog input:
  randomSeed(42);
  
  if(debug) {
    Serial.begin(115200);
    Serial.println("Connected");
  }

  Bluefruit.begin();
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("Mask");
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.setConnectCallback(connect_callback);
  Bluefruit.setDisconnectCallback(disconnect_callback);

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();
  
  // Initialize each matrix object:
  for(uint8_t i=0; i<4; i++) {
    matrix[i].begin(pgm_read_byte(&matrixAddr[i]));
    matrix[i].setBrightness(13);
    matrix[i].setTextSize(1);
    matrix[i].setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix[i].setTextColor(LED_ON);
    matrix[i].setRotation(3);
  }

}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  
  //Disable the blue LED
  Bluefruit.autoConnLed(false);
  digitalWrite(19,LOW);
  
  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void connect_callback(uint16_t conn_handle)
{
  char central_name[32] = { 0 };
  Bluefruit.Gap.getPeerName(conn_handle, central_name, sizeof(central_name));

  if(debug) {
    Serial.print("Connected to ");
    Serial.println(central_name);
  }
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  if(debug)
    Serial.println("Disconnected");
}

void rtos_idle_callback(void)
{
  // Don't call any other FreeRTOS blocking API()
  // Perform background task(s) here

  // Request CPU to enter low-power mode until an event/interrupt occurs
  waitForEvent();
}


int readMic() {
  //Read Mic and return 0-255 amplitude value
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   unsigned int minRegister = 40; //Min amount to report. Ignore small noises (0-255).
   
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(A0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   //double volts = (peakToPeak * 5.0) / 1024;  // convert to volts

   int aout = map(peakToPeak/4, 0, 185, 0, 255); //0-255
   
   if(aout > minRegister) {
      if(debug) {
        Serial.println("Mic:");
        Serial.println(aout);
      }
   }
   else
      aout = 0;

   //Return 0-255 for amplitude. This will get divided by 32 to give 0-8 to the mouth function so it can draw
   //the appropriate mouth bitmap.
   return aout; 
}

void readBT() {
  if(!bleuart.available())
    return;
  uint8_t len = readPacket(&bleuart, 500);
  if (len == 0) return;
  
  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';

    if(buttnum == 1 && pressed) {
        matrix[MATRIX_EYES].clear();
        matrix[MATRIX_EYES].drawBitmap(0,0,pumpkinImg, 8, 8, LED_ON);
        matrix[MATRIX_EYES].writeDisplay();
        scrollText("Happy Halloween");
    }
    if(buttnum == 2 && pressed) {
        matrix[MATRIX_EYES].clear();
        matrix[MATRIX_EYES].drawBitmap(0,0,skullImg, 8, 8, LED_ON);
        matrix[MATRIX_EYES].writeDisplay();
        scrollText("Trick Or Treat!");
    }
    if(buttnum == 3 && pressed) {
        matrix[MATRIX_EYES].clear();
        matrix[MATRIX_EYES].drawBitmap(0,0,batImg, 8, 8, LED_ON);
        matrix[MATRIX_EYES].writeDisplay();
        scrollText("We're Batty!");
    }
    if(buttnum == 4 && pressed) {
       drawDead();
    }
    if(buttnum == 5 && pressed) {
      faceMode = faceMode + 1;
      if(faceMode > 1)
        faceMode = 0;
    }
  }
  //Text Input
  if(packetbuffer[1] == 'T') {
    String s;
    matrix[MATRIX_EYES].clear();
    matrix[MATRIX_EYES].drawBitmap(0,0,scrollImg, 8, 8, LED_ON);
    matrix[MATRIX_EYES].writeDisplay();
    for(int i=2; i < len-1; i++) {
      s = s + String((char)packetbuffer[i]);
    }
    scrollText(s);
  }

  if(debug) {
    for(int i = 0; i < len; i++) {
      Serial.write(packetbuffer[i]);
    }
    Serial.println();
  }
   
  //See Bluefruit Controller example for Color Changer, Gyro, etc.
}

//////////////////////////////////// LOOP
  String buff;
void loop() {

  readBT();

  if(micMode)
    oldsum = readMic(); 

  if(faceMode == 0)
    drawFace(oldsum); 
  if(faceMode == 1)
    drawHypno();
    
  // Refresh all of the matrices in one quick pass
  for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();

  if(!micMode)
    delay(20);
}

void drawDead() {
    matrix[MATRIX_EYES].clear();
    matrix[MATRIX_EYES].drawBitmap(0, 0, xImg, 8, 8, LED_ON);

    for(int m = 0; m < 6; m++) {
      drawMouth(tongueImg[m]);      
      for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
      delay(20);
    }
    delay(1000);
    for(int m = 5; m >= 0; m--) {
      drawMouth(tongueImg[m]);      
      for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
      delay(20);
    }
    delay(500);
}

void drawHypno() {
  for (uint8_t i=0; i<8; i++) {
    // draw a diagonal row of pixels
    for(uint8_t x = 0; x < 5; x++) {
      matrix[x].displaybuffer[i] = _BV((counter+i) % 8) | _BV((counter+i+8) % 8);
      matrix[x].writeDisplay();
    }
  }
  // write the changes we just made to the display
  
  delay(20);

  counter++;
  if (counter >= 16) counter = 0;  
}

void drawFace(uint8_t os) {
  // Draw eyeball in current state of blinkyness (no pupil).  Note that
  // only one eye needs to be drawn.  Because the two eye matrices share
  // the same address, the same data will be received by both.
  matrix[MATRIX_EYES].clear();
  // When counting down to the next blink, show the eye in the fully-
  // open state.  On the last few counts (during the blink), look up
  // the corresponding bitmap index.
  matrix[MATRIX_EYES].drawBitmap(0, 0,
    blinkImg[
      (blinkCountdown < sizeof(blinkIndex)) ? // Currently blinking?
      pgm_read_byte(&blinkIndex[blinkCountdown]) :            // Yes, look up bitmap #
      0                                       // No, show bitmap 0
    ], 8, 8, LED_ON);
  // Decrement blink counter.  At end, set random time for next blink.
  if(--blinkCountdown == 0) blinkCountdown = random(5, 180);

  // Add a pupil atop the blinky eyeball bitmap.
  // Periodically, the pupil moves to a new position...
  if(--gazeCountdown <= gazeFrames) {
    // Eyes are in motion - draw pupil at interim position
    matrix[MATRIX_EYES].drawBitmap(
      newX - (dX * gazeCountdown / gazeFrames) - 2,
      newY - (dY * gazeCountdown / gazeFrames) - 2,
      pupilImg, 7, 7, LED_OFF);
    if(gazeCountdown == 0) {    // Last frame?
      eyeX = newX; eyeY = newY; // Yes.  What's new is old, then...
      do { // Pick random positions until one is within the eye circle
        newX = random(6)-1; newY = random(6);
      } while(!((pgm_read_byte(&landingImg[newY]) << newX) & 0x80));
      dX            = newX - eyeX;             // Horizontal distance to move
      dY            = newY - eyeY;             // Vertical distance to move
      gazeFrames    = random(3, 15);           // Duration of eye movement
      gazeCountdown = random(gazeFrames *10, 120); // Count to end of next movement
    }
  } else {
    // Not in motion yet -- draw pupil at current static position
    matrix[MATRIX_EYES].drawBitmap(eyeX - 2, eyeY - 2, pupilImg, 7, 7, LED_OFF);
  }

  drawMouth(mouthImg[os / 32]);
}

// Draw mouth image across three adjacent displays
void drawMouth(const uint8_t *img) {
  for(uint8_t i=0; i<3; i++) {
    matrix[MATRIX_MOUTH_LEFT + i].clear();
    matrix[MATRIX_MOUTH_LEFT + i].drawBitmap(i * -8, 0, img, 24, 8, LED_ON);
  }
}




