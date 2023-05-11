//
// OOSMOS pin Class
//
// Copyright (C) 2014-2023  OOSMOS, LLC
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

#ifndef pinMAX
#define pinMAX 18
#endif

//===================================

#include "pin.h"
#include "oosmos.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  Unknown_State = 1,
  On_State,
  Off_State,
  ConfirmingOn_State,
  ConfirmingOff_State
} eStates;

struct pinTag
{
  #if defined(OOSMOS_PIN_ARDUINO) || defined(OOSMOS_PIN_WIRING) || defined(OOSMOS_PIN_SYSFS) || defined(OOSMOS_PIN_UM232H)
    uint8_t m_PinNumber;
  #endif

  #if defined(OOSMOS_PIN_PIC32MX)
    IoPortId m_Port;
    uint16_t m_Bit;
  #endif

  #if defined(OOSMOS_PIN_MBED)
    uint8_t m_Pin[sizeof(DigitalOut)];
    PinName m_PinName;
  #endif

  #if defined(OOSMOS_PIN_IAR_SYSTEMS_ICC)
    GPIO_TypeDef* m_Port;
    uint16_t m_Pin;
  #endif

  #if defined(OOSMOS_PIN_KEY_WINDOWS) || defined(OOSMOS_PIN_KEY_LINUX)
    int m_Key;
  #endif

  oosmos_sActiveObject m_ActiveObject;
  oosmos_sTimeout      m_Timeout;
  uint8_t              m_DebounceTimeMS;
  unsigned             m_State:4;     // eStates
  unsigned             m_Logic:4;     // pin_eLogic
  unsigned             m_Direction:4; // pin_eDirection

  #if defined(OOSMOS_PIN_DUMMY)
	 unsigned m_IsDummy:1;
  #endif
};

static bool IsPhysicallyOn(const pin * pPin);

static bool IsPhysicallyOff(const pin * pPin)
{
  return !IsPhysicallyOn(pPin);
}

static void RunStateMachine(void * pObject)
{
  oosmos_POINTER_GUARD(pObject);

  pin * pPin = (pin *) pObject;

  switch ((eStates) pPin->m_State) {
    case On_State: {
      if (IsPhysicallyOff(pPin)) {
        pPin->m_State = (unsigned) ConfirmingOff_State;
        oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      }

      break;
    }

    case Off_State: {
      if (IsPhysicallyOn(pPin)) {
        pPin->m_State = (unsigned) ConfirmingOn_State;
        oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      }

      break;
    }

    case ConfirmingOn_State: {
      if (!oosmos_TimeoutHasExpired(&pPin->m_Timeout)) {
        break;
      }

      if (!IsPhysicallyOn(pPin)) {
        pPin->m_State = (unsigned) Unknown_State;
        break;
      }

      pPin->m_State = (unsigned) On_State;
      break;
    }

    case ConfirmingOff_State: {
      if (!oosmos_TimeoutHasExpired(&pPin->m_Timeout)) {
        break;
      }

      if (!IsPhysicallyOff(pPin)) {
        pPin->m_State = (unsigned) Unknown_State;
        break;
      }

      pPin->m_State = (unsigned) Off_State;
      break;
    }

    case Unknown_State: {
      pPin->m_State = (unsigned) (IsPhysicallyOn(pPin) ? ConfirmingOn_State : ConfirmingOff_State);
      oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      break;
    }

    default: {
      oosmos_FOREVER();
    }
  }
}

extern bool pinIsOn(const pin * pPin)
{
  oosmos_POINTER_GUARD(pPin);

  #if defined(OOSMOS_PIN_DUMMY)
    if (pPin->m_IsDummy) {
      return false;
    }
  #endif


  if (pPin->m_DebounceTimeMS == 0) {
    return IsPhysicallyOn(pPin);
  }

  return pPin->m_State == (unsigned) On_State;
}

extern bool pinIsOff(const pin * pPin)
{
  return !pinIsOn(pPin);
}

#if defined(OOSMOS_PIN_ARDUINO) || defined(OOSMOS_PIN_WIRING)
  static bool IsPhysicallyOn(const pin * pPin)
  {
    const unsigned PinValue = digitalRead(pPin->m_PinNumber);
    return PinValue == (pPin->m_Logic == pinActiveHigh ? HIGH : LOW);
  }

  extern void pinOn(const pin * pPin)
  {
    const unsigned PinNumber = pPin->m_PinNumber;

    if (pPin->m_Direction == pinInOut) {
      pinMode(PinNumber, pPin->m_Logic == pinActiveHigh ? INPUT : OUTPUT);
    }

    digitalWrite(PinNumber, pPin->m_Logic == pinActiveHigh ? HIGH : LOW);
  }

  extern void pinOff(const pin * pPin)
  {
    const unsigned PinNumber = pPin->m_PinNumber;

    if (pPin->m_Direction == pinInOut) {
      pinMode(PinNumber, pPin->m_Logic == pinActiveHigh ? OUTPUT : INPUT);
    }

    digitalWrite(PinNumber, pPin->m_Logic == pinActiveHigh ? LOW : HIGH);
  }

  extern unsigned pinGetPinNumber(pin * pPin)
  {
    return pPin->m_PinNumber;
  }

  extern pin * pinNew(const unsigned PinNumber, const pin_eDirection Direction, const pin_eLogic Logic)
  {
    #if defined(OOSMOS_PIN_WIRING)
      static bool WiringPi_Initialized = false;

      if (!WiringPi_Initialized) {
        wiringPiSetup();
        WiringPi_Initialized = true;
      }
    #endif

    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_PinNumber      = PinNumber;
    pPin->m_Logic          = (unsigned) Logic;
    pPin->m_State          = (unsigned) Unknown_State;
    pPin->m_Direction      = (unsigned) Direction;
    pPin->m_DebounceTimeMS = 0;

    switch (Direction) {
      case pinIn:
      case pinInOut: {
        pinMode(PinNumber, INPUT);
        break;
      }
      case pinIn_Pullup: {
        pinMode(PinNumber, INPUT_PULLUP);
        break;
      }
      case pinOut: {
        pinMode(PinNumber, OUTPUT);
        break;
      }
    }

    return pPin;
  }

  extern pin * pinNew_Debounce(const unsigned PinNumber, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    pin * pPin = pinNew(PinNumber, Direction, Logic);

    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }
#endif

#if defined(OOSMOS_PIN_SYSFS)
  #include <stdio.h>
  #include <string.h>

  static int sysfs_export(int sysfs_pin_number)
  {
      FILE *pFile = fopen("/sys/class/gpio/export", "w");

      if (pFile == NULL) {
          perror("Failed to open /sys/class/gpio/export");
          return -1;
      }

      char buffer[4];
      const int length = snprintf(buffer, sizeof(buffer), "%d", sysfs_pin_number);
      fwrite(buffer, length, sizeof(char), pFile);
      fclose(pFile);

      return 0;
  }

  static int sysfs_set_direction(int sysfs_pin_number, const char *direction)
  {
      char path[40];
      snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", sysfs_pin_number);

      FILE *pFile = fopen(path, "w");

      if (pFile == NULL) {
          perror("Failed to open direction file");
          return -1;
      }

      fwrite(direction, strlen(direction), sizeof(char), pFile);
      fclose(pFile);

      return 0;
  }

  static int sysfs_read(int sysfs_pin_number)
  {
      char path[40];
      snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", sysfs_pin_number);

      FILE *pFile = fopen(path, "r");

      if (pFile == NULL) {
          perror("Failed to open value file");
          return -1;
      }

      char value;
      fread(&value, 1, sizeof(char), pFile);
      fclose(pFile);

      return value - '0';
  }

  static int sysfs_write(int sysfs_pin_number, int value)
  {
      char path[40];
      snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", sysfs_pin_number);

      FILE *pFile = fopen(path, "w");

      if (pFile == NULL) {
          perror("Failed to open value file");
          return -1;
      }

      const char str_value = value ? '1' : '0';
      fwrite(&str_value, 1, sizeof(char), pFile);
      fclose(pFile);

      return 0;
  }

  extern pin * pinNew(int PinNumber, pin_eDirection direction, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_PinNumber      = PinNumber;
    pPin->m_Logic          = (unsigned) Logic;
    pPin->m_State          = (unsigned) Unknown_State;
    pPin->m_DebounceTimeMS = 0;

    sysfs_export(PinNumber);

    const char * pDirection;

    if (direction == pinIn)
      pDirection = "in";
    else if (direction == pinOut) {
	  // Set direction to out and the value to "off" in one operation. (See https://www.kernel.org/doc/Documentation/gpio/sysfs.txt)
      pDirection = pPin->m_Logic == pinActiveHigh ? "low": "high";
    }
	else
      oosmos_FOREVER();

    sysfs_set_direction(PinNumber, pDirection);

    #if defined(OOSMOS_PIN_DUMMY)
       pPin->m_IsDummy = 0;
    #endif

    return pPin;
  }

  static bool IsPhysicallyOn(const pin * pPin)
  {
    const int PinValue = sysfs_read(pPin->m_PinNumber);
    return PinValue == (pPin->m_Logic == pinActiveHigh ? 1 : 0);
  }

  extern void pinOn(const pin * pPin)
  {
    #if defined(OOSMOS_PIN_DUMMY)
      if (pPin->m_IsDummy) {
    	  return;
      }
    #endif

    sysfs_write(pPin->m_PinNumber, pPin->m_Logic == pinActiveHigh ? 1 : 0);
  }

  extern void pinOff(const pin * pPin)
  {
    #if defined(OOSMOS_PIN_DUMMY)
      if (pPin->m_IsDummy) {
	    return;
      }
    #endif

    sysfs_write(pPin->m_PinNumber, pPin->m_Logic == pinActiveHigh ? 0 : 1);
  }
#endif

#if defined(OOSMOS_PIN_PIC32MX)
  static bool IsPhysicallyOn(const pin * pPin)
  {
    const uint32_t PinValue = PORTReadBits(pPin->m_Port, pPin->m_Bit);
    return (pPin->m_Logic == pinActiveHigh) ? PinValue != 0 : PinValue == 0;
  }

  extern void pinOn(const pin * pPin)
  {
    (pPin->m_Logic == pinActiveHigh ? PORTSetBits : PORTClearBits)(pPin->m_Port, pPin->m_Bit);
  }

  extern void pinOff(const pin * pPin)
  {
    (pPin->m_Logic == pinActiveHigh ? PORTClearBits : PORTSetBits)(pPin->m_Port, pPin->m_Bit);
  }

  extern pin * pinNew(const IoPortId Port, const unsigned Bit, const pin_eDirection Direction, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_Port           = Port;
    pPin->m_Bit            = Bit;
    pPin->m_Logic          = (unsigned) Logic;
    pPin->m_State          = (unsigned) Unknown_State;
    pPin->m_Direction      = (unsigned) Direction;
    pPin->m_DebounceTimeMS = 0;

    switch (pPin->m_Direction) {
      case pinOut: {
        pinOff(pPin);
        PORTSetPinsDigitalOut(Port, Bit);
        break;
      }
      case pinIn: {
        pinOff(pPin);
        PORTSetPinsDigitalIn(Port, Bit);
        break;
      }
    }

    return pPin;
  }

  extern pin * pinNew_Debounce(const IoPortId Port, const unsigned Bit, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    pin * pPin = pinNew(Port, Bit, Direction, Logic);

    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }
#endif

#if defined(OOSMOS_PIN_MBED)
  #include "mbed.h"
  #include <new>

  extern pin * pinNew(const PinName Pin, const pin_eDirection Direction, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);
    ::new(&pPin->m_Pin) DigitalOut(Pin);

    pPin->m_PinName        = Pin;
    pPin->m_Logic          = (unsigned) Logic;
    pPin->m_State          = (unsigned) Unknown_State;
    pPin->m_Direction      = (unsigned) Direction;
    pPin->m_DebounceTimeMS = 0;

    return pPin;
  }

  extern pin * pinNew_Debounce(const PinName PinNumber, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    pin * pPin = pinNew(PinNumber, Direction, Logic);

    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }

  static bool IsPhysicallyOn(const pin * pPin)
  {
    DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
    const unsigned PinValue = pDigitalOut->read();
    return PinValue == (pPin->m_Logic == pinActiveHigh ? 1 : 0);
  }

  extern void pinOn(const pin * pPin)
  {
    DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
    pDigitalOut->write(pPin->m_Logic == pinActiveHigh ? 1 : 0);
  }

  extern void pinOff(const pin * pPin)
  {
    DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
    pDigitalOut->write(pPin->m_Logic == pinActiveHigh ? 0 : 1);
  }

  extern PinName pinGetPinName(pin * pPin)
  {
    return pPin->m_PinName;
  }
#endif

#if defined(OOSMOS_PIN_IAR_SYSTEMS_ICC)
  static bool IsPhysicallyOn(const pin * pPin)
  {
    oosmos_POINTER_GUARD(pPin);

    const GPIO_PinState PinValue = HAL_GPIO_ReadPin(pPin->m_Port, pPin->m_Pin);
    return (pPin->m_Logic == pinActiveHigh) ? (PinValue == GPIO_PIN_SET) : (PinValue == GPIO_PIN_RESET);
  }

  extern void pinOn(const pin * pPin)
  {
    oosmos_POINTER_GUARD(pPin);

    HAL_GPIO_WritePin(pPin->m_Port, pPin->m_Pin, pPin->m_Logic == pinActiveHigh ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }

  extern void pinOff(const pin * pPin)
  {
    oosmos_POINTER_GUARD(pPin);

    HAL_GPIO_WritePin(pPin->m_Port, pPin->m_Pin, pPin->m_Logic == pinActiveHigh ? GPIO_PIN_RESET : GPIO_PIN_SET);
  }

  extern pin * pinNew(GPIO_TypeDef* Port, uint16_t Pin, const pin_eDirection Direction, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_Port           = Port;
    pPin->m_Pin            = Pin;
    pPin->m_Logic          = (unsigned) Logic;
    pPin->m_State          = (unsigned) Unknown_State;
    pPin->m_Direction      = (unsigned) Direction;
    pPin->m_DebounceTimeMS = 0;

    pinOff(pPin);
    return pPin;
  }

  extern pin * pinNew_Debounce(GPIO_TypeDef* Port, const uint16_t Bit, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    pin * pPin = pinNew(Port, Bit, Direction, Logic);

    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }
#endif

#if defined(OOSMOS_PIN_KEY_WINDOWS)
  #include <stdio.h>
  #include <windows.h>

  static bool   pinFirst = true;
  static bool   KeyIsDown[0xFF];
  static HANDLE hStdin;

  static bool IsPhysicallyOn(const pin * pPin)
  {
    oosmos_POINTER_GUARD(pPin);

    DWORD         NumRead = 0;
    INPUT_RECORD  InputRecord;

    while (PeekConsoleInput(hStdin, &InputRecord, 1, &NumRead) && NumRead > 0) {
      if (InputRecord.EventType == KEY_EVENT) {
        const KEY_EVENT_RECORD KER  = InputRecord.Event.KeyEvent;
        const int              Char = KER.uChar.AsciiChar;

        KeyIsDown[Char] = (KER.bKeyDown == TRUE);
      }

      (void) ReadConsoleInput(hStdin, &InputRecord, 1, &NumRead);
    }

    const bool IsDown = KeyIsDown[pPin->m_Key];
    return pPin->m_Logic == ((unsigned) pinActiveHigh) ? IsDown : !IsDown;
  }

  extern pin * pinNew_Key(char Key, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_Key            = Key;
    pPin->m_Logic          = (unsigned) Logic;
    pPin->m_State          = (unsigned) Unknown_State;
    pPin->m_DebounceTimeMS = 0;

    if (pinFirst) {
      memset(KeyIsDown, false, sizeof(KeyIsDown));

      hStdin = GetStdHandle(STD_INPUT_HANDLE);
      pinFirst = false;
    }

    return pPin;
  }

  extern pin* pinNew_Key_Debounce(char Key, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_Key = Key;
    pPin->m_Logic = (unsigned)Logic;
    pPin->m_State = (unsigned)Unknown_State;
    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (pinFirst) {
      memset(KeyIsDown, false, sizeof(KeyIsDown));

      hStdin = GetStdHandle(STD_INPUT_HANDLE);
      pinFirst = false;
    }

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }

  extern void pinOn(const pin * pPin)
  {
    #if defined(pin_DEBUG)
      printf("%p Pin ON\n", pPin);
    #else
      oosmos_UNUSED(pPin);
    #endif
  }

  extern void pinOff(const pin * pPin)
  {
    #if defined(pin_DEBUG)
      printf("%p Pin OFF\n", pPin);
    #else
      oosmos_UNUSED(pPin);
    #endif
  }

  void (*pin_pDummy)(void *) = RunStateMachine; // To satisfy compiler

#endif

#if defined(OOSMOS_PIN_DUMMY)
  extern pin* pinNew_Dummy(void)
  {
	 oosmos_Allocate(pPin, pin, pinMAX, NULL);
	 pPin->m_IsDummy = 1;
	 return pPin;
  }
#endif

#if defined(OOSMOS_PIN_UM232H)
  #include <windows.h>
  #include "ftd2xx.h"

  static bool      UM232H_First = true;
  static DWORD     UM232H_DeviceNumber = -1;
  static UCHAR     UM232H_ModeMask  = 0b11111111; // A 0 bit means input, a 1 bit means output.
  static UCHAR     UM232H_ValueMask = 0b00000000; // All low initially.
  static FT_HANDLE UM232H_ftHandle;

  static void FT_Init()
  {
      FT_STATUS ftStatus = FT_Rescan();

      DWORD dwNumDevices = 0;

      // Get the number of FTDI devices
      ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);

      if (ftStatus != FT_OK) {
          for (;;);
      }

      int UM232H_Devices = 0;

      for (DWORD i = 0; i < dwNumDevices; i++) {
          DWORD Flags;
          DWORD ID;
          DWORD Type;
          DWORD LocId;
          char SerialNumber[16] = { 0 };
          char Description[64]  = { 0 };
          FT_HANDLE Handle;

          ftStatus = FT_GetDeviceInfoDetail(i, &Flags, &Type, &ID, &LocId, SerialNumber, Description, &Handle);

          if (ftStatus != FT_OK) {
              for (;;);
          }

          if (strncmp(Description, "UM232H", sizeof("UM232H")) == 0) {
              UM232H_DeviceNumber = i;
              UM232H_Devices += 1;
          }
      }

      if (UM232H_Devices == 0) {
          //printf("UM232H device not found.");
          for (;;);
      }

      if (UM232H_Devices != 1) {
          //printf("Only one UM232H device supported.");
          for (;;);
      }

      ftStatus = FT_Open(UM232H_DeviceNumber, &UM232H_ftHandle);
      if (ftStatus != FT_OK) {
          for (;;);
      }
  }

  static bool IsPhysicallyOn(const pin* pPin)
  {
      UCHAR PinNumber = pPin->m_PinNumber;

      DWORD dwBytesRead = 0;
      UCHAR data = 0;

      FT_STATUS ftStatus = FT_Read(UM232H_ftHandle, &data, sizeof(char), &dwBytesRead);

      if (ftStatus != FT_OK) {
          for (;;);
      }

      if (dwBytesRead != sizeof(char))
      {
          for (;;);
      }

      const int PinValue = ((data >> PinNumber) & 1) == 1;
      return PinValue == (pPin->m_Logic == pinActiveHigh ? 1 : 0);
  }

  static void WritePin(UCHAR PinNumber, int Value)
  {
      if (Value) {
          UM232H_ValueMask = UM232H_ValueMask | (1 << PinNumber);  // Set the pin high
      }
      else {
          UM232H_ValueMask = UM232H_ValueMask & ~(1 << PinNumber);  // Set the pin low but keep other pins as they were
      }

      DWORD BytesWritten = 0;

      FT_STATUS ftStatus = FT_Write(UM232H_ftHandle, &UM232H_ValueMask, sizeof(char), &BytesWritten);
      if (ftStatus != FT_OK) {
          for (;;);
      }

      if (BytesWritten != sizeof(char)) {
          for (;;);
      }
  }

  extern pin* pinNew_UM232H(int PinNumber, pin_eDirection Direction, pin_eLogic Logic)
  {
      if (UM232H_First) {
          FT_Init();
          UM232H_First = false;
      }

      oosmos_Allocate(pPin, pin, pinMAX, NULL);

      pPin->m_PinNumber = PinNumber;
      pPin->m_Logic = (unsigned)Logic;
      pPin->m_State = (unsigned)Unknown_State;
      pPin->m_Direction = (unsigned)Direction;
      pPin->m_DebounceTimeMS = 0;

      if (Direction == pinIn) {
          // Set pin as input (0), all others as they were.
          UM232H_ModeMask = UM232H_ModeMask & ~(1 << PinNumber);
      }
      else if (Direction == pinOut) {
          // Set pin as output (1), all others as they were.
          UM232H_ModeMask = UM232H_ModeMask | (1 << PinNumber);
      }
      else {
          for (;;);
      }

      FT_STATUS ftStatus = FT_SetBitMode(UM232H_ftHandle, UM232H_ModeMask, FT_BITMODE_ASYNC_BITBANG);

      if (ftStatus != FT_OK) {
          for (;;);
      }

      return pPin;
  }

  extern void pinOn(const pin* pPin)
  {
      const int Value = pPin->m_Logic == pinActiveHigh ? 1 : 0;
      WritePin(pPin->m_PinNumber, Value);
  }

  extern void pinOff(const pin* pPin)
  {
      const int Value = pPin->m_Logic == pinActiveHigh ? 0 : 1;
      WritePin(pPin->m_PinNumber, Value);
  }
#endif
