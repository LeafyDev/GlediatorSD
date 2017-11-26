#include <FastLED.h>
#include <SPI.h>
#include <SD.h>
#include "Modes.h"

#define NUM_LEDS 37
#define DATA_PIN 6
#define MODE_PIN 15

File fxdata;
CRGB leds[NUM_LEDS];
Mode mode;

String GetName(bool filename);
bool ChangeMode();
void AllOn(int);
void Wave(int);

void setup()
{
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  AllOn(CRGB::Black);

  Wave(CRGB::Red);
  Wave(CRGB::Green);
  Wave(CRGB::Blue);

  AllOn(CRGB::White);

  delay(2000);

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  if (!SD.begin(10))
  {
    //Serial.println("SD init: ✗");
    return;
  }
  //Serial.println("SD init: ✓");

  mode = Off;

  fxdata = SD.open(GetName(true));
  if (fxdata)
  {
    fxdata.close();
  }

  delay(100);
}

void loop()
{
	fxdata = SD.open(GetName(true));
	if (!fxdata)
		return;

	while (fxdata.available())
	{
		if (ChangeMode())
			break;

		fxdata.readBytes(reinterpret_cast<char*>(leds), NUM_LEDS * 3);
		FastLED.show();
		delay(20);
	}

	fxdata.close();
}

void AllOn(int color)
{
	for (auto i = 0; i <= NUM_LEDS; i++)
	{
		leds[i] = color;
	}
	FastLED.show();
}

void Wave(int color)
{
	for (auto i = 0; i <= NUM_LEDS; i++)
	{
		if (i < 3)
		{
			leds[i] = color;
			FastLED.show();
		}
		else
		{
			leds[i] = color;
			leds[i - 3] = CRGB::Black;
			FastLED.show();
		}

		if (i == NUM_LEDS - 3)
		{
			leds[i - 2] = CRGB::Black;
			FastLED.show();
			delay(50);
			leds[i - 1] = CRGB::Black;
			FastLED.show();
			delay(50);
			leds[i] = CRGB::Black;
			FastLED.show();
		}

		delay(50);
	}
}

String GetName(bool filename)
{
	String ret;

	switch (mode)
	{
		default:
		case Off:
			ret = "Off";
			break;
		case Rainbow:
			ret = "Rainbow";
			break;
		case Rain:
			ret = "Rain";
			break;
		case RandomRain:
			ret = "RandomRain";
			break;
		case Lava:
			ret = "Lava";
			break;
	}

	if (filename)
	{
		ret.toLowerCase();
		if (ret == "randomrain") // can't read "randomrain.dat", seems to be too long? wtf
			ret = "rrain";
		return ret + ".dat";
	}

	return ret;
}

bool ChangeMode()
{
	auto newMode = null;
	auto m = map(analogRead(MODE_PIN), 0, 1023, 0, 255);

	if (m <= 10)
		newMode = Off;

	else if (m <= 60)
		newMode = Rainbow;

	else if (m <= 120)
		newMode = Rain;

	else if (m <= 180)
		newMode = RandomRain;

	else if (m > 180)
		newMode = Lava;

	if (mode == newMode)
		return false;

	mode = newMode;
	//Serial.print("new mode: ");
	//Serial.println(GetName(false));

	return true;
}
