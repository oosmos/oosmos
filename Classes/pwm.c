//
// OOSMOS pwm Class
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef pwmMAX
#define pwmMAX 2
#endif

//===================================

#include "pwm.h"
#include "oosmos.h"
#include "prt.h"
#include <stdint.h>

//
// This pwm class provides a more intuitve and portable interface to control
// PWM signals.  By specifying PWM duty cycle units as a percentage (0.0%-100.0%), the
// user intent is clear.  The alternative is non-portable value in your application
// whose magnitude varies from platform to platform.  The code below is a bit messy,
// but it is preferable to hide the multi-platform ugliness in the PWM implementation
// rather than pollute the principle application logic.
//
// Some processors have more capabilities than others. For example,
// the Arduino ESP32 allows for complete control over the PWM SignalFrequency where
// others do not without a lot of low-level timer reconfiguration.
// For those platforms, more APIs are exposed and and there is a more complete pwmNew()
// constructor provided.
//
// This class can be compiled cleanly on Windows, although it only outputs debug
// information.
//
// To better read through all the preprocessor conditionals, it is recommended that
// you view the code using Visual Studio Code and collapse the sections that are
// not important to you.
//
// It has been tested on the following boards: ESP32, ESP8266, ChipKIT family,
// Arduino AVR family, ST Nucleo family.  Add your board with the appropriate
// #ifdef's if it is currently not supported.
//

struct pwmTag
{
  int      m_PinNumber;
  uint32_t m_DutyCycleValue;

  #if defined(ESP32)
    int    m_ChannelNumber;
    int    m_DutyCycleResolution;
    double m_SignalFrequency;
  #endif
};

extern void pwmSetDutyCyclePercent(pwm * pPWM, double DutyCyclePercent)
{
  #if defined(ARDUINO_ESP8266_NODEMCU)
    const uint32_t DutyCycleValue = (uint32_t) oosmos_AnalogMapAccurate(DutyCyclePercent, 0.0, 100.0, 0.0, 1023.0);
  #elif defined(ESP32)
    const uint32_t DutyCycleValue = (uint32_t) oosmos_AnalogMapAccurate(DutyCyclePercent, 0.0, 100.0, 0.0, (1 << pPWM->m_DutyCycleResolution) - 1);
  #else
    const uint32_t DutyCycleValue = (uint32_t) oosmos_AnalogMapAccurate(DutyCyclePercent, 0.0, 100.0, 0.0, 255.0);
  #endif

  #if defined(ESP32)
    ledcWrite(pPWM->m_ChannelNumber, DutyCycleValue);
  #elif defined(ARDUINO)
    analogWrite(pPWM->m_PinNumber, DutyCycleValue);
  #elif defined(_MSC_VER)
    // Do nothing
  #else
    #error pwm.cpp: Unsupported platform.
  #endif

  pPWM->m_DutyCycleValue = DutyCycleValue;

  #if defined(pwm_DEBUG)
    prtFormatted("pwmDutyCycle: pPWM: %p, pinNumber: %d, DutyCyclePercent: %f, DutyCycleValue: %u\n",
                  pPWM, pPWM->m_PinNumber, DutyCyclePercent, (unsigned int) pPWM->m_DutyCycleValue);
  #endif
}

extern void pwmOn(pwm * pPWM)
{
  #if defined(ESP32)
    ledcWrite(pPWM->m_ChannelNumber, pPWM->m_DutyCycleValue);
  #elif defined(ARDUINO)
    analogWrite(pPWM->m_PinNumber, pPWM->m_DutyCycleValue);
  #elif defined(_MSC_VER)
    // Do nothing
  #else
    #error pwm.cpp: Unsupported platform.
  #endif
}

extern void pwmOff(pwm * pPWM)
{
  #if defined(ESP32)
    ledcWrite(pPWM->m_ChannelNumber, 0);
  #elif defined(ARDUINO)
    analogWrite(pPWM->m_PinNumber, 0);
  #elif defined(_MSC_VER)
    // Do nothing
  #else
    #error: pwm.cpp: Unsupported platform.
  #endif
}

extern pwm * pwmNew(int PinNumber, double DutyCyclePercent)
{
  oosmos_Allocate(pPWM, pwm, pwmMAX, NULL);
  pPWM->m_PinNumber = PinNumber;

  pwmSetDutyCyclePercent(pPWM, DutyCyclePercent);

  #if defined(ARDUINO)
    pinMode(pPWM->m_PinNumber, OUTPUT);
  #endif

  #if defined(ESP32)
    static int Channels = 0;

    pPWM->m_ChannelNumber       = Channels++;
    pPWM->m_DutyCycleResolution = 15;
    pPWM->m_SignalFrequency     = 1000.0;

    ledcSetup(pPWM->m_ChannelNumber, pPWM->m_SignalFrequency, pPWM->m_DutyCycleResolution);
    ledcAttachPin(pPWM->m_PinNumber, pPWM->m_ChannelNumber);
  #endif

  return pPWM;
}

#if defined(ESP32)
  extern double pwmGetSignalFrequency(pwm * pPWM)
  {
    return pPWM->m_SignalFrequency;
  }

  extern void pwmSetSignalFrequency(pwm * pPWM, double SignalFrequency)
  {
    pPWM->m_SignalFrequency = SignalFrequency;
    ledcSetup(pPWM->m_ChannelNumber, pPWM->m_SignalFrequency, pPWM->m_DutyCycleResolution);
  }

  extern uint32_t pwmGetDutyCycleResolution(pwm * pPWM)
  {
    return pPWM->m_DutyCycleResolution;
  }

  extern void pwmSetDutyCycleResolution(pwm * pPWM, uint32_t DutyCycleResolution)
  {
    pPWM->m_DutyCycleResolution = DutyCycleResolution;
    ledcSetup(pPWM->m_ChannelNumber, pPWM->m_SignalFrequency, pPWM->m_DutyCycleResolution);
  }
#endif
