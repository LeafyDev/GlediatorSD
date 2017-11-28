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
int brightness = 0;

String GetName(bool filename);
bool ChangeMode();
void Snake();
int NewApple();

void setup()
{
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

	//randomSeed(analogRead(1));

	Snake();

	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);

	if (!SD.begin(10))
		return;
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

		if (brightness < 255)
		{
			brightness++;
			FastLED.setBrightness(brightness);
		}
		
		fxdata.readBytes(reinterpret_cast<char*>(leds), NUM_LEDS * 3);
		FastLED.show();
		FastLED.delay(20);
	}

	fxdata.close();
}

void Snake()
{
	auto size = 2;
	auto snakeSpeed = 50;
	auto apple = NewApple();
	auto collects = 0;
	auto maxCollects = 10;
	auto curPos = 0;
	auto endSnake = 0;

	while(curPos != size)
	{
		leds[curPos] = CRGB::White;
		FastLED.show();
		FastLED.delay(snakeSpeed);
		curPos++;
	}
	
	while (collects != maxCollects)
	{
		if (curPos == NUM_LEDS)
			curPos = 0;

		endSnake = curPos - size;
		if (endSnake < 0)
			endSnake = NUM_LEDS + endSnake;

		leds[curPos] = CRGB::White;
		leds[endSnake] = CRGB::Black;
		FastLED.show();
		FastLED.delay(snakeSpeed);

		curPos++;

		if (curPos == apple)
		{
			collects++;
			size++;
			apple = NewApple();
		}
	}

	while (endSnake < NUM_LEDS - 1)
	{
		endSnake = curPos - size;

		if(curPos < NUM_LEDS - 1)
			leds[curPos] = CRGB::White;

		leds[endSnake] = CRGB::Black;

		curPos++;

		FastLED.show();
		FastLED.delay(snakeSpeed);
	}
}

int NewApple()
{
	int newApple;

	while (true)
	{
		newApple = random(0, NUM_LEDS);

		if (!leds[newApple])
		{
			leds[newApple] = CRGB::Red;
			FastLED.show();
			return newApple;
		}
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

	return true;
}
