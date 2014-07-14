/*
 This sketch is used to display data on a display module via
 a RN-42 Bluetooth module that communicates at 9600 bps.
 */
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
// Number of rows and columns on the LCD
int numDisplayColumns = 16;
int numDisplayRows = 2;
// Integer to select row
int row = 0;
// Termination character to be able to recognize a line
char termChar = '|';
// String buffer to hold characters sent over Bluetooth
char* s = 0;
// Scroll delay in milliseconds
int scrollDelay = 500;

// Bluetooth pins
int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3

// Bluetooth software emulated serial port
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup()
{
  // Set up the LCD's number of columns and rows
  lcd.begin(numDisplayColumns, numDisplayRows);
  
  // Only for monitoring. TODO: remove
  Serial.begin(9600);  // Begin the serial monitor at 9600bps

  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // to enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600
  
  // Set up LCD cursor
  lcd.clear();
  // Create string buffer for the 1st time
  s = new char[140];
}

void loop()
{
  if(bluetooth.available())  // Any data on the bluetooth port?
  {
	// Wait a bit to allow all of the data to arrive.
	delay(100);
    // Store the characters in a string till a termination character is encountered.
    // Print the string if termination character is encountered.
	while(bluetooth.available())
	{
		char c = (char)bluetooth.read();
		if(termChar == c)
		{
			Serial.print(s);
			lcd.clear();
			lcd.setCursor(15,0);
			lcd.print(s);
			//selectNextRow();
			refreshString();
		}
		else
		{
			strncat(s, &c, 1);
		}
	}
  }
  // Scroll LCD one position left
  lcd.scrollDisplayLeft();
  // Scroll delay
  delay(scrollDelay);
}

// Deletes and reallocates display string buffer
void refreshString()
{
  delete s;
  s = new char[140];
}

// Selects the next row for the LCD to print on
// TODO: may not be needed
void selectNextRow()
{
  row = row == 0 ? 1 : 0;
  lcd.setCursor(0, row);
}

