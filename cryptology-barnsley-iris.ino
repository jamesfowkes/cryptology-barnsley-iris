/* Arduino library includes */

#include <Stepper.h>

#include "iris.h"

/* Defines, typedefs, constants */

static const int IRIS_MOTOR_PIN_1 = 3;
static const int IRIS_MOTOR_PIN_2 = 5;
static const int IRIS_MOTOR_PIN_3 = 4;
static const int IRIS_MOTOR_PIN_4 = 6;

static const int CURTAIN_MOTOR_PIN_1 = 7;
static const int CURTAIN_MOTOR_PIN_2 = 9;
static const int CURTAIN_MOTOR_PIN_3 = 8;
static const int CURTAIN_MOTOR_PIN_4 = 10;

static const int CURTAIN_HOME_PIN = A1;
static const int TRIGGER_PIN = A2;

static const int CURTAIN_SPEED = 3;
static const int STEPS_PER_REV = 4096;
static const int CURTAIN_CLOSE_POSITION = 340;

static const int TRIGGER_DELAY_IN_MILLISECONDS = 0;

/* Local Objects/Variables */

static Stepper s_iris_stepper(STEPS_PER_REV, IRIS_MOTOR_PIN_1, IRIS_MOTOR_PIN_2, IRIS_MOTOR_PIN_3, IRIS_MOTOR_PIN_4);
static Stepper s_curtain_stepper(STEPS_PER_REV, CURTAIN_MOTOR_PIN_1, CURTAIN_MOTOR_PIN_2, CURTAIN_MOTOR_PIN_3, CURTAIN_MOTOR_PIN_4);


/* Local Functions */

static bool curtain_open()
{
  return digitalRead(CURTAIN_HOME_PIN) == LOW;  
}

static void close_curtain()
{
  s_curtain_stepper.step(-CURTAIN_CLOSE_POSITION);
}

static void open_curtain()
{
	s_curtain_stepper.setSpeed(CURTAIN_SPEED);
	while (!curtain_open())
	{
		s_curtain_stepper.step(1);
		delayMicroseconds(100);
	}
}

static bool debounce_trigger(bool triggered, int max_count)
{
  static int count = 0;
  count += triggered ? 1 : -1;
  if (count >= max_count) { count = max_count; return true;}
  if (count < 0) { count = 0;}
  return false;
}

void setup()
{
	Serial.begin(115200);

	pinMode(IRIS_MOTOR_PIN_1, OUTPUT);
	pinMode(IRIS_MOTOR_PIN_2, OUTPUT);
	pinMode(IRIS_MOTOR_PIN_3, OUTPUT);
	pinMode(IRIS_MOTOR_PIN_4, OUTPUT);
	
	pinMode(CURTAIN_MOTOR_PIN_1, OUTPUT);
	pinMode(CURTAIN_MOTOR_PIN_2, OUTPUT);
	pinMode(CURTAIN_MOTOR_PIN_3, OUTPUT);
	pinMode(CURTAIN_MOTOR_PIN_4, OUTPUT);
	
	pinMode(IRIS_HOME_PIN, INPUT_PULLUP);
  	pinMode(CURTAIN_HOME_PIN, INPUT_PULLUP);
	pinMode(TRIGGER_PIN, INPUT_PULLUP);

	Serial.println("Finding curtain open position");
  	open_curtain();

  	delay(100);
  
  	Serial.println("Closing curtain");
  	close_curtain();
	
	Serial.println("Finding home position");
	iris_home(s_iris_stepper);

	Serial.println("Closing iris");
	iris_close(s_iris_stepper);

	Serial.println("Waiting for trigger");

  bool triggered = false;
  do
  {
    delay(10);
    triggered = digitalRead(TRIGGER_PIN) == LOW;
  } while (!debounce_trigger(triggered, 100));
  
	if (TRIGGER_DELAY_IN_MILLISECONDS)
	{
		Serial.print("Delaying opening for ");
		Serial.print(TRIGGER_DELAY_IN_MILLISECONDS);
		Serial.println("ms");
		delay(TRIGGER_DELAY_IN_MILLISECONDS);
	}

	Serial.println("Opening curtain");
	open_curtain();

	delay(100);

	Serial.println("Opening iris");

	iris_open(s_iris_stepper);

	Serial.println("Iris open");
}

void loop()
{

}
