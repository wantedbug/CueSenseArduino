/************************************************************************/
/* This sketch is used to display data on a display module via			*/
/* a RN-42 Bluetooth module that communicates at 9600					*/
/* Author: Vikas Prabhu
/* Copyright (C) 2014 Tampere University of Technology					*/
/************************************************************************/

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

/**
 * Variables and constants
 */
// Number of rows and columns on the LCD
int numDisplayColumns = 16;
int numDisplayRows = 2;
// Integer to select row
int row = 0;
// Termination character to be able to recognize a line
char termChar = '|';
// Buffer to hold characters sent over Bluetooth
char* msgBuf = 0;
// Length of the buffer
int msgLen = 0;
// Ready signal character
char readyChar = "R";
// Flag that indicates if the ready signal was sent
bool readySent = false;
// Counter for scroll loop
int scrollCounter = 0;
// Scroll delay in milliseconds
int scrollDelay = 500;
// Bluetooth pin numbers
int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3

/**
 * Library initialisations
 */
// Initialise the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
// Bluetooth software emulated serial port
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
	// Set up the LCD's number of columns and rows
	lcd.begin(numDisplayColumns, numDisplayRows);
	
	// The Bluetooth Mate defaults to 115200bps
	bluetooth.begin(115200);
	// Send "$$$" to enter command mode with the Bluetooth Mate
	bluetooth.print("$");
	bluetooth.print("$");
	bluetooth.print("$");
	// Short delay, wait for the Mate to send back CMD
	delay(100);
	// Change the baudrate to 9600, no parity
	bluetooth.println("U,9600,N");
	// 115200 can be too fast at times for SoftwareSerial to relay the data reliably
	// Start bluetooth serial at 9600
	bluetooth.begin(9600);
	
	// Clear LCD
	lcd.clear();
	// Create string buffer
	msgBuf = new char[140];
}

void loop() {
	// Any data on the bluetooth port?
	if(bluetooth.available()) {
		// Wait a bit to allow all of the data to arrive
		delay(100);
		// Store the characters in a string till a termination character is encountered
		// Print the string if termination character is encountered
		while(bluetooth.available()) {
			char c = (char)bluetooth.read();
			if(termChar == c) {
				// Clear LCD and set cursor to rightmost position
				lcd.clear();
				lcd.setCursor(15,0);
				// Display the message
				lcd.print(msgBuf);
				// Store length of message for the scroll loop
				msgLen = strlen(msgBuf);
				scrollCounter = msgLen + 5; // a little breathing space between messages
				// We haven't displayed the whole message yet, so reset readySent
				readySent = false;
				// Make a new buffer for the next message
				refreshBuffer();
			} else {
				strncat(msgBuf, &c, 1); // TODO: max length check
			}
		}
	}
	
	// Scroll LCD one position left
	// Note: Display will scroll even if ready signal has already been sent,
	// for e.g., if there are no more messages in the queue
	lcd.scrollDisplayLeft();
	
	// Is there more of the current message to be scrolled?
	if(scrollCounter > 0) {
		--scrollCounter;
	}
	
	// If the entire message has been displayed, send ready signal
	if(scrollCounter == 0 && !readySent) {
		bluetooth.write(readyChar);
		readySent = true;
	}
	
	// Scroll delay
	delay(scrollDelay);
}

// Deletes and reallocates display string buffer
void refreshBuffer() {
	delete msgBuf;
	msgBuf = new char[140];
}

// Selects the next row for the LCD to print on
// TODO: may not be necessary
void selectNextRow() {
	row = row == 0 ? 1 : 0;
	lcd.setCursor(0, row);
}