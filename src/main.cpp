/*!
 * main.cpp
 * This is a firmware for Corridor device. This device will manage our corridor lights. The cases are varies:
 * - switch on light 
 *   - there are 2 PIR sensors mounted on the ceiling
 *   - Dors - they have sensor which can receive signal for oper/close
 * - manage light illumination by standard is 100%
 *   - no main power (UPS) reduce to 50%
 *   - night - switch off 2 small lams, stays only 1 lamp. The illumination will br reduced to 30%
 *
 * Plamen Kovandzhiev's home projects
 * Web: https://github.com/kovandzhiev/Corridor-device
 * Supported boards:
 * 		PRODINO WIFI-ESP WROOM-02 V1 (https://kmpelectronics.eu/products/prodino-wifi-esp-wroom-02-v1/)
 * Description:
 * 		Managing corridor lightings.
 * Used libraries:
 *      Fluent light (https://github.com/kovandzhiev/FluentLight)
 * Version: 1.0.0
 * Date: 01.01.2025
 * Author: Plamen Kovandzhiev <kovandjiev@gmail.com>
*/

#include "KMPDinoWiFiESP.h"
#include "FluentLight.h"

#define DEBUG
#define MAX_BRIGHTNESS 1024 // 100 % brightness
#define NO_POWER_BRIGHTNESS MAX_BRIGHTNESS / 2 // 50 % brightness
#define NIGHT_BRIGHTNESS MAX_BRIGHTNESS / 3 // 30 % brightness
#define LIGHT_RUNNING_DURATION_MS 1 * 60 * 1000 // 1 min
#define LIGHT_BRIGHTEN_TIME_MS 5 * 1000 // 5 sec
#define LIGHT_FADE_TIME_MS 20 * 1000 // 20 sec

FluentLight _lights(EXT_GROVE_D0, MAX_BRIGHTNESS, DEFAULT_PWM_FREQUENCY);
bool _isMotionDetected;
bool _isMainPower;
bool _isDay;
bool _movements[2] {false, false};

void processInputs() {
	bool pirInput = KMPDinoWiFiESP.GetOptoInState(OptoIn1);
	bool doorInput = KMPDinoWiFiESP.GetOptoInState(OptoIn2);

	_isMotionDetected = 
		pirInput != _movements[0] || // PIR sensors
		doorInput != _movements[1]; // Dors/PIR sensors

	_movements[0] = pirInput;
	_movements[1] = doorInput;

	_isMainPower = KMPDinoWiFiESP.GetOptoInState(OptoIn3);

	_isDay = KMPDinoWiFiESP.GetOptoInState(OptoIn4);
}

void processLogic() {
	int brightness = MAX_BRIGHTNESS;

	// If there is no main power so the brightness is reduced to 50%
	if (!_isMainPower)
	{
		brightness = NO_POWER_BRIGHTNESS;
	}
	
	// If night the brightness is reduced to 30%
	if (!_isDay)
	{
		brightness = min(brightness, NIGHT_BRIGHTNESS);
	}

	// Turn Relay 1 to Off when Day include all lamps
	//   or On when night exclude 2 small lamps
	KMPDinoWiFiESP.SetRelayState(Relay1, !_isDay);
	
	_lights.setMaxBrightness(brightness);

	if (_isMotionDetected)
	{
		_lights.on();
	}
}

void setup()
{
	delay(5000); // Prevent blocking upload of the new sketch

#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Starting...");
#endif

	KMPDinoWiFiESP.init();

	_lights.setBrightenTime(LIGHT_BRIGHTEN_TIME_MS);
	_lights.setFadeTime(LIGHT_FADE_TIME_MS);
	_lights.setRunningDuration(LIGHT_RUNNING_DURATION_MS);
	_lights.setMaxBrightness(MAX_BRIGHTNESS);

	_lights.begin();

#ifdef DEBUG
	Serial.println("The corridor device is started.");
#endif
}

void loop() {
	processInputs();
	processLogic();
	_lights.process();
}
