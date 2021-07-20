/*
  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// initial Time display is 12:59:45 PM
int hours = 12;
int minutes = 59;
int seconds = 45;
int am_pm = 1; //PM

// Time Set Buttons
int hour_button;
int minute_button;

// Pins definition for Time Set Buttons
int hour_pin = 0;// pin 0 for Hours Setting
int minute_pin = 1;// pin 1 for Minutes Setting

// Backlight Time Out
const int timeout_length = 150;
int backlight_timer = timeout_length;// Backlight Time-Out
int backlight_pin = 5; // Backlight pin
const int backlight_power = 120; // no more then 7mA !!!

// For accurate Time reading, use Arduino Real Time Clock and not just delay()
static uint32_t last_time, now = 0; // RTC

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  pinMode(hour_pin,INPUT_PULLUP);// avoid external Pullup resistors for Button 1
  pinMode(minute_pin,INPUT_PULLUP);// and Button 2
  now = millis(); // read RTC initial value
}

void overflow()
{
  if(seconds == 60)
  {
    seconds = 0;
    minutes++;
  }
  if(minutes == 60)
  {
    minutes = 0;
    hours++;
  }
  if(hours == 13)
  {
    hours = 1;
    am_pm++;
    am_pm = (am_pm == 1) ? 0 : 1;
  }
  return;
}

void update_display()
{
	lcd.setCursor(0,0);
	lcd.print("Time ");
	if(hours < 10)
		lcd.print("0");// always 2 digits
	lcd.print(hours);
	lcd.print(":");
	if(minutes < 10)
		lcd.print("0");
	lcd.print(minutes);
	lcd.print(":");
	if(seconds < 10)
		lcd.print("0");
	lcd.print(seconds);
	(am_pm == 0) ? lcd.print(" AM") : lcd.print(" PM");
	// for Line 2
	lcd.setCursor(0,1);
	lcd.print("UwU");
}

void loop()
{
	lcd.begin(16,2);// every second
	// Update LCD Display
	// Print TIME in Hour, Min, Sec + AM/PM
	update_display();
	// improved replacement of delay(1000)
	// Much better accuracy, no more dependant of loop execution time
	for (int index = 0; index < 5; index++)// make 5 time 200ms loop, for faster Button response
	{
		while ((now - last_time) < 200) //delay200ms
		{
		  	now = millis();
		}
		// inner 200ms loop
		last_time = now; // prepare for next loop
		// read Setting Buttons
		hour_button = digitalRead(hour_pin);// Read Buttons
		minute_button = digitalRead(minute_pin);
	 	//Backlight time out
		backlight_timer--;
		if (backlight_timer <= 0)
		{
			analogWrite(backlight_pin, 0);// Backlight OFF
			backlight_timer++;
		}
		// Hit any to activate Backlight
		if(((hour_button == 0)|(minute_button == 0)) & (backlight_timer == 1))
		{
			backlight_timer = timeout_length;
			analogWrite(backlight_pin, backlight_power);
			// wait until Button released
			while ((hour_button == 0)|(minute_button == 0))
			{
				hour_button = digitalRead(hour_pin);// Read Buttons
				minute_button = digitalRead(minute_pin);
			}
		}
		else // Process Button 1 or Button 2 when hit while Backlight on
		{
			if (hour_button == 0)
			{
				hours++;
				backlight_timer = timeout_length;
				analogWrite(backlight_pin, backlight_power);
			}
			if (minute_button == 0)
			{
				seconds = 0;
				minutes++;
				backlight_timer = timeout_length;
				analogWrite(backlight_pin, backlight_power);
			 }
			 /* ---- manage seconds, minutes, hours am/pm overflow ----*/
			overflow();
			if ((hour_button == 0) | (minute_button == 0))// Update display if time set button pressed
				update_display();
		} // end if else
	}// end for
	// outer 1000ms loop
	seconds++; //increment sec. counting
	// ---- manage seconds, minutes, hours am/pm overflow ----
	overflow();
	// Loop end
}
