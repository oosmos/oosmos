/*
// OOSMOS keyer Class
//
// Copyright (C) 2014-2015  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <http://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "oosmos.h"
#include "keyer.h"
#include "prt.h"

struct keyerTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              Idle_State;
    oosmos_sComposite         Dah_State;
      oosmos_sLeaf            Dah_Tone_State;
    oosmos_sComposite         Dit_State;
      oosmos_sLeaf            Dit_Tone_State;

  pin * m_pDahPin;
  pin * m_pDitPin;
  pin * m_pSpeakerPin;
  
  int m_DitTimeMS;
  int m_DahTimeMS;
  int m_SpaceTimeMS;
   
  struct {
    struct {
      bool m_DitWasPressed;
    } Dah;
  
    struct {
      bool m_DahWasPressed;
    } Dit;
  } m_StateData;
};

static void CheckDahIsPressed(keyer * pKeyer)
{
  if (!pKeyer->m_StateData.Dit.m_DahWasPressed && pinIsOn(pKeyer->m_pDahPin))
    pKeyer->m_StateData.Dit.m_DahWasPressed = true;
}

static void CheckDitIsPressed(keyer * pKeyer)
{
  if (!pKeyer->m_StateData.Dah.m_DitWasPressed && pinIsOn(pKeyer->m_pDitPin))
    pKeyer->m_StateData.Dah.m_DitWasPressed = true;
}

static bool Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      if (pinIsOn(pKeyer->m_pDahPin))
        return oosmos_Transition(pRegion, &pKeyer->Dah_State);
      else if (pinIsOn(pKeyer->m_pDitPin))
        return oosmos_Transition(pRegion, &pKeyer->Dit_State);
  }

  return false;
}

static bool Dah_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      pKeyer->m_StateData.Dah.m_DitWasPressed = false;
      return true; 
    case oosmos_INSTATE: 
      CheckDitIsPressed(pKeyer);
      return true;
  }

  return false;
}

static bool Dah_Tone_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        pinOn(pKeyer->m_pSpeakerPin);
        oosmos_SyncDelayMS(pRegion, pKeyer->m_DahTimeMS);
        pinOff(pKeyer->m_pSpeakerPin);
        oosmos_SyncDelayMS(pRegion, pKeyer->m_SpaceTimeMS);
      oosmos_SyncEnd(pRegion);

      if (pKeyer->m_StateData.Dah.m_DitWasPressed)
        return oosmos_Transition(pRegion, &pKeyer->Dit_State);
       
      return oosmos_Transition(pRegion, &pKeyer->Idle_State);
  }

  return false;
}

static bool Dit_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;
  
  switch (pEvent->Code) {
    case oosmos_ENTER:
      pKeyer->m_StateData.Dit.m_DahWasPressed = false;
      return true; 
    case oosmos_INSTATE: 
      CheckDahIsPressed(pKeyer);
      return true;
  }

  return false;
}

static bool Dit_Tone_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        pinOn(pKeyer->m_pSpeakerPin);
        oosmos_SyncDelayMS(pRegion, pKeyer->m_DitTimeMS);
        pinOff(pKeyer->m_pSpeakerPin);
        oosmos_SyncDelayMS(pRegion, pKeyer->m_SpaceTimeMS);
      oosmos_SyncEnd(pRegion);

      if (pKeyer->m_StateData.Dit.m_DahWasPressed)
        return oosmos_Transition(pRegion, &pKeyer->Dah_State);

      return oosmos_Transition(pRegion, &pKeyer->Idle_State);
  }

  return false;
}

extern keyer * keyerNew(pin * pDahPin, pin * pDitPin, pin * pSpeakerPin, const int WPM)
{
  oosmos_Allocate(pKeyer, keyer, 2, NULL);
   
  /*                                     State Name      Parent        Default         */
  /*                             ===================================================== */
  oosmos_StateMachineInitNoQueue(pKeyer, StateMachine,   NULL,         Idle_State    );
    oosmos_LeafInit             (pKeyer, Idle_State,     StateMachine                );
     oosmos_CompositeInit       (pKeyer, Dah_State,      StateMachine, Dah_Tone_State);
       oosmos_LeafInit          (pKeyer, Dah_Tone_State, Dah_State                   );
     oosmos_CompositeInit       (pKeyer, Dit_State,      StateMachine, Dit_Tone_State);
       oosmos_LeafInit          (pKeyer, Dit_Tone_State, Dit_State                   );
    
  pKeyer->m_pDahPin     = pDahPin;
  pKeyer->m_pDitPin     = pDitPin;
  pKeyer->m_pSpeakerPin = pSpeakerPin;
  pKeyer->m_DitTimeMS   = 1200 / WPM;
  pKeyer->m_DahTimeMS   = pKeyer->m_DitTimeMS * 3;
  pKeyer->m_SpaceTimeMS = pKeyer->m_DitTimeMS;

  oosmos_DebugCode(
    oosmos_Debug(&pKeyer->StateMachine, true, NULL);
  )
    
  return pKeyer;
}
