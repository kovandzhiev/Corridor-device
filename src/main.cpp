// corridordevice.ino
// Plamen Kovandzhiev's home projects
// Web: https://github.com/kovandzhiev/Corridor-device
// Supported boards:
//		PRODINo WIFI-ESP WROOM-02 V1 (https://kmpelectronics.eu/products/prodino-wifi-esp-wroom-02-v1/)
// Description:
//		Manageing corridor lightings.
// Used libraries:
//      Fluent light (https://github.com/kovandzhiev/FluentLight)
// Version: 1.0.0
// Date: 26.12.2024
// Author: Plamen Kovandzhiev <kovandjiev@gmail.com>

#include "KMPDinoWiFiESP.h"
#include "FluentLight.h"

#define DEBUG
#define MAX_BRIGHTNESS 1024 // 100 % brightness
#define NO_POWER_BRIGHTNESS MAX_BRIGHTNESS / 2 // 50 % brightness
#define NIGHT_BRIGHTNESS MAX_BRIGHTNESS / 3 // 30 % brightness
#define LIGHT_RUNNING_DURATION_MS 1 * 60 * 1000 // 1 min
#define LIGHT_BRIGHTEN_TIME_MS 5 * 1000 // 5 sec
#define LIGHT_FADE_TIME_MS 1 * 60 * 1000 // 1 min

FluentLight _lights(EXT_GROVE_D0);
bool _isMotionDetected;
bool _isMainPower;
bool _isDay;

void processInputs() {
	_isMotionDetected = 
		KMPDinoWiFiESP.GetOptoInState(OptoIn1) || // PIRs sensors
		KMPDinoWiFiESP.GetOptoInState(OptoIn2); // Dors sensors

	_isMainPower = KMPDinoWiFiESP.GetOptoInState(OptoIn3);

	_isDay = KMPDinoWiFiESP.GetOptoInState(OptoIn4);
}

void processLogic() {
	word brightness = MAX_BRIGHTNESS;

	// If there is no main power, reduce brightness
	if (!_isMainPower)
	{
		brightness = NO_POWER_BRIGHTNESS;
	}
	
	if (_isDay)
	{
		/* code */
	}
	

	_isMotionDetected = 
		KMPDinoWiFiESP.GetOptoInState(OptoIn1) || // PIRs sensors
		KMPDinoWiFiESP.GetOptoInState(OptoIn2); // Dors sensors

	_isMainPower = KMPDinoWiFiESP.GetOptoInState(OptoIn3);

	_isDay = KMPDinoWiFiESP.GetOptoInState(OptoIn4);
}

void setup()
{
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Starting...");
#endif

	delay(5000); // Do not block upload of the new sketch

	KMPDinoWiFiESP.init();

	_lights.setMaxBrightness(MAX_BRIGHTNESS);
	_lights.setRunningDuration(LIGHT_RUNNING_DURATION_MS);
	_lights.setBrightenTime(LIGHT_BRIGHTEN_TIME_MS);
	_lights.setFadeTime(LIGHT_FADE_TIME_MS);

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
