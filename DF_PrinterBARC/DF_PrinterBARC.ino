/*------------------------------------------------------------------------
  Example sketch for Adafruit Thermal Printer library for Arduino.
  Demonstrates a few text styles & layouts, bitmap printing, etc.

  IMPORTANT: DECLARATIONS DIFFER FROM PRIOR VERSIONS OF THIS LIBRARY.
  This is to support newer & more board types, especially ones that don't
  support SoftwareSerial (e.g. Arduino Due).  You can pass any Stream
  (e.g. Serial1) to the printer constructor.  See notes below.
  ------------------------------------------------------------------------*/

#include "Adafruit_Thermal.h"

// Here's the new syntax when using SoftwareSerial (e.g. Arduino Uno) ----
// If using hardware serial instead, comment out or remove these lines:

#include "SoftwareSerial.h"
#define TX_PIN 11 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 10 // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor
// Then see setup() function regarding serial & printer begin() calls.

// Here's the syntax for hardware serial (e.g. Arduino Due) --------------
// Un-comment the following line if using hardware serial:

//Adafruit_Thermal printer(&Serial1);      // Or Serial2, Serial3, etc.

// -----------------------------------------------------------------------

int incomingByte = 0;
String a;

const char DELIMITER = '@';
const char START_MARKER = '<';
const char END_MARKER = '>';
const char BARCODE_MARKER = '#';
const char TEXT_MARKERHEADER = '~';
const char TEXT_MARKERLEFT = '^';
const char TEXT_MARKERCENTER = '&';
const char TEXT_MARKERRIGHT = '*';
const char TEXT_INVERSEBOLD = '(';

void setup() {

  // This line is for compatibility with the Adafruit IotP project pack,
  // which uses pin 7 as a spare grounding point.  You only need this if
  // wired up the same way (w/3-pin header into pins 5/6/7):
  pinMode(7, OUTPUT); digitalWrite(7, LOW);

  Serial.begin(9600);
  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  mySerial.begin(9600);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)
  printer.setTimes(500,50);
  //
  //  printer.println("potato2");
  //
  //  // Barcode examples:
  //  // CODE39 is the most common alphanumeric barcode:
  //    printer.printBarcode("ADAFRUT", CODE128);
  //  printer.setBarcodeHeight(100);
  //  // Print UPC line on product barcodes:
  //  printer.printBarcode("123456789123", UPC_A);
  //
  //  printer.feed(2);
  //
  //  //printer.sleep();      // Tell printer to sleep
  //  delay(3000L);         // Sleep for 3 seconds
  //  //printer.wake();       // MUST wake() before printing again, even if reset
  //  printer.setDefault(); // Restore printer to defaults
}

void loop() {
  while (Serial.available()) {
    // Read the incoming data until the end marker is found
    String receivedData = readData();

    // Process the received data
    processString(receivedData);

  }
}

String readData() {
  
  char receivedChar;
  String receivedData = "";

  // Wait for the start marker
  while (true) {
    if (Serial.available() > 0) {
      receivedChar = Serial.read();
      if (receivedChar == START_MARKER) {
        Serial.println("0"); //feedback busy printing
        break;
      }
    }
  }

  // Read the incoming data until the end marker is found
  while (true) {
    if (Serial.available() > 0) {
      receivedChar = Serial.read();

      // Check for the end marker
      if (receivedChar == END_MARKER) {
        break;
      }

      // Add the character to the received data
      receivedData += receivedChar;
    }
  }

  return receivedData;
}

void processString(String inputString) {
  // Print the original string
//  Serial.print("Original String: ");
//  Serial.println(inputString);
  //printer.println(inputString);

  printer.println("--------------------------------");

  // Split the string into substrings using '@' as the separator
  int delimiterIndex = inputString.indexOf(DELIMITER);
  while (delimiterIndex != -1) {
    // Extract the substring before the '@' character
    String substring = inputString.substring(0, delimiterIndex);

    // Print the substring
//    Serial.print("Substring: ");
//    Serial.println(substring);

    //process the substring for receipt printing
    processSubstring(substring);

    // Remove the processed substring and the 'delimiter' character
    inputString = inputString.substring(delimiterIndex + 1);

    // Find the next occurrence of 'delimiter'
    delimiterIndex = inputString.indexOf(DELIMITER);
  }

  // Print the last remaining substring
//  Serial.print("Last Substring: ");
//  Serial.println(inputString);
  processSubstring(inputString);

  //space print out
  printer.println("--------------------------------");
  printer.feed(2);

  delay(1000);
  Serial.println("1"); //feedback ready to print
}

void processSubstring(String inputString) {
  char dataMarker = inputString.charAt(0);
  inputString.remove(0, 1);
  switch (dataMarker) {
    case BARCODE_MARKER:
      printer.justify('C');
      printer.printBarcode(inputString.c_str(), CODE128);
      printer.justify('L');
      break;
    case TEXT_MARKERHEADER:
      printer.setSize('L');
      printer.justify('C');
      printer.println(inputString);
      printer.setSize('S');
      printer.justify('L');
      break;
      case TEXT_INVERSEBOLD:
      printer.inverseOn();
      printer.setSize('L');
      printer.justify('C');
      printer.println(inputString);
      printer.setSize('S');
      printer.justify('L');
      printer.inverseOff();
      break;
    case TEXT_MARKERLEFT:
      printer.justify('L');
      printer.println(inputString);
      break;
    case TEXT_MARKERCENTER:
      printer.justify('C');
      printer.println(inputString);
      printer.justify('L');
      break;
    case TEXT_MARKERRIGHT:
      printer.justify('R');
      printer.println(inputString);
      printer.justify('L');
      break;
    default:
      break;

  }
}
