//
// OOSMOS - The Object-Oriented State Machine Operating System
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

#ifndef OOSMOS_h
#define OOSMOS_h

//
// All names that begin with OOSMOS (all caps) are private and are not part of the
// official oosmos interface.
//

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//
// Include header files so the application doesn't have to.
//
#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include "Arduino.h"
  #elif defined(ENERGIA)
    #include "Energia.h"
  #else
    #include "WProgram.h"
  #endif

  #define oosmos_EndProgram(Code) do { } while(1)
#elif defined(__PIC32MX)
  #include <plib.h>
  extern void oosmos_ClockSpeedInMHz(uint32_t ClockSpeedMHz);
  extern uint32_t oosmos_GetClockSpeedInMHz(void);
  #define oosmos_EndProgram(Code) while(1)continue
#elif defined(oosmos_RASPBERRY_PI)
  #include <wiringPi.h>
  #define __USE_BSD
  #include <unistd.h>
  #define oosmos_EndProgram(Code) OOSMOS_EndProgram(Code)
#elif defined(__linux__) || defined(__APPLE__)
  #ifndef __USE_BSD
    #define __USE_BSD
  #endif
  #include <unistd.h>
  #define oosmos_EndProgram(Code) OOSMOS_EndProgram(Code)
#elif defined(__MBED__)
  #include "mbed.h"
  #define oosmos_EndProgram(Code) OOSMOS_EndProgram(Code)
#elif defined(__IAR_SYSTEMS_ICC__)
  #ifdef oosmos_INCLUDE
    #include oosmos_INCLUDE
  #else
    #error Must define oosmos_INCLUDE.  Example: "stm32l1xx_hal.h"
  #endif
#else
  #define oosmos_EndProgram(Code) OOSMOS_EndProgram(Code)
#endif

#ifdef __cplusplus
  extern "C" {
#endif


typedef struct {
  uint32_t m_Start;
  uint32_t m_End;
} oosmos_sTimeout;

typedef enum {
  oosmos_QueueFull_TossNew = 1,
  oosmos_QueueFull_TossOld
} oosmos_eQueueFullBehavior;

typedef struct OOSMOS_sQueueTag oosmos_sQueue;

struct OOSMOS_sQueueTag {
  void   * m_pHead;
  void   * m_pTail;
  void   * m_pEnd;
  void   * m_pQueueData;
  uint16_t m_QueueDataSize;
  uint16_t m_QueueElementSize;
  uint16_t m_ByteCount;

  oosmos_eQueueFullBehavior (*m_pFullBehaviorFunc)(void * pContext);
  void   * m_pContext;
};

#if defined(oosmos_DEBUG)
  #if defined(__PIC32MX) && defined(PIC32_STARTER_KIT)
    #include <plib.h>
    #define oosmos_DebugPrint DBPRINTF
    #define oosmos_DebugCode(x) x
  #elif defined(ARDUINO) || defined(ENERGIA)
    #define oosmos_DebugPrint OOSMOS_ArduinoPrintf
    #define oosmos_DebugCode(x) x
    extern void OOSMOS_ArduinoPrintf(const char * pFormat, ...);
  #else
    #include <stdio.h>
    #define oosmos_DebugPrint printf
    #define oosmos_DebugCode(x) x
  #endif

  extern void oosmos_DebugInit(void);
#else
  extern void oosmos_DebugDummy(const char*, ...);
  /*lint -e773 suppress "Expression-like macro not parenthesized" */
  #define oosmos_DebugPrint 1 ? (void)0 : oosmos_DebugDummy
  #define oosmos_DebugCode(x)
  #define oosmos_DebugInit()
#endif

#define oosmos_UNUSED(x) (void)(x)

/*lint -e506 suppress "consant value boolean" */
/*lint -e717 suppress "do { } while(0)" */
#define oosmos_POINTER_GUARD(x) \
        do {                     \
          if ((x) == NULL) {     \
            oosmos_FOREVER();    \
          }                      \
        } while (false)

/*lint -e506 suppress "consant value boolean" */
/*lint -e717 suppress "do { } while(0)" */
#define oosmos_FOREVER() \
        do {              \
          continue;       \
        } while (true)

/*lint -e506 suppress "consant value boolean" */
/*lint -e717 suppress "do { } while(0)" */
#define oosmos_ASSERT(Condition) \
        do {                      \
          if (!(Condition)) {     \
            oosmos_FOREVER();     \
          }                       \
        } while (false)

typedef struct {
  int    m_Code;
  void * m_pContext;
} oosmos_sEvent;

#define oosmos_Divide_Integral_Rounded(Dividend, Divisor) (((Dividend) + ((Divisor) / 2)) / (Divisor))

#define oosmos_Minutes2US(Minutes)                  ((Minutes) * (60ul * 1000ul * 1000ul))
#define oosmos_Minutes2MS(Minutes)                  ((Minutes) * (60ul * 1000ul))
#define oosmos_Minutes2Seconds(Minutes)             ((Minutes) * 60ul)
#define oosmos_Minutes2Hours_Truncated(Minutes)     ((Minutes) / 60ul)
#define oosmos_Minutes2Hours_Rounded(Minutes)       oosmos_Divide_Integral_Rounded(Minutes, 60ul)

#define oosmos_Seconds2US(Seconds)                  ((Seconds) * (1000ul * 1000ul))
#define oosmos_Seconds2MS(Seconds)                  ((Seconds) * 1000ul)
#define oosmos_Seconds2Hours_Truncated(Seconds)     ((Seconds) / 3600ul)
#define oosmos_Seconds2Hours_Rounded(Seconds)       oosmos_Divide_Integral_Rounded(Seconds, 3600ul)

#define oosmos_US2MS_Truncated(MicroSeconds)        ((MicroSeconds) / 1000ul))
#define oosmos_US2MS_Rounded(MicroSeconds)          oosmos_Divide_Integral_Rounded(MicroSeconds, 1000ul)
#define oosmos_US2Seconds_Truncated(MicroSeconds)   ((MicroSeconds) / (1000ul * 1000ul))
#define oosmos_US2Seconds_Rounded(MicroSeconds)     oosmos_Divide_Integral_Rounded(MicroSeconds, 1000ul * 1000ul)
#define oosmos_US2Minutes_Truncated(MicroSeconds)   ((MicroSeconds) / (60ul * 1000ul * 1000ul))
#define oosmos_US2Minutes_Rounded(MicroSeconds)     oosmos_Divide_Integral_Rounded(MicroSeconds, 60ul * 1000ul * 1000ul)

#define oosmos_MS2US(MicroSeconds)                  ((MicroSeconds) * 1000ul)
#define oosmos_MS2Seconds_Truncated(MicroSeconds)   ((MicroSeconds) / 1000ul)
#define oosmos_MS2Seconds_Rounded(MicroSeconds)     oosmos_Divide_Integral_Rounded(MicroSeconds, 1000ul)
#define oosmos_MS2Minutes_Truncated(MicroSeconds)   ((MicroSeconds) / (60ul * 1000ul))
#define oosmos_MS2Minutes_Rounded(MicroSeconds)     oosmos_Divide_Integral_Rounded(MicroSeconds, 60ul * 1000ul)

#define oosmos_EMPTY_EVENT { 0, NULL }

/*lint -e123 suppress "Macro 'oosmos_sStateMachine' defined" */
#define oosmos_sStateMachine(StateMachine, EventType, MaxEvents) \
  oosmos_sQueue        m_EventQueue; \
  EventType            m_EventQueueData[MaxEvents]; \
  EventType            m_CurrentEvent; \
  oosmos_sStateMachine StateMachine

#define oosmos_sStateMachineNoQueue(StateMachine) \
  oosmos_sStateMachine StateMachine

typedef struct OOSMOS_sStateTag        oosmos_sState;
typedef struct OOSMOS_sRegionTag       oosmos_sRegion;

//
// User-facing state machine declaration types.
//
typedef struct OOSMOS_sStateMachineTag oosmos_sStateMachine;
typedef struct OOSMOS_sOrthoTag        oosmos_sOrtho;
typedef struct OOSMOS_sCompositeTag    oosmos_sComposite;
typedef struct OOSMOS_sStateTag        oosmos_sLeaf;
typedef struct OOSMOS_sOrthoRegionTag  oosmos_sOrthoRegion;
typedef struct OOSMOS_sStateTag        oosmos_sFinal;

typedef bool (*OOSMOS_tCode)(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent);
typedef void (*oosmos_tAction)(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent);

typedef enum {
  OOSMOS_CompositeType = 1,
  OOSMOS_LeafType,
  OOSMOS_FinalType,

  #ifdef oosmos_ORTHO
    OOSMOS_OrthoType,
    OOSMOS_OrthoRegionType,
  #endif

  OOSMOS_StateMachineType
} OOSMOS_eTypes;

struct OOSMOS_sStateTag {
  oosmos_sState        * m_pParent;
  OOSMOS_tCode           m_pCode;
  oosmos_sTimeout        m_Timeout;
  oosmos_sStateMachine * m_pStateMachine;

  oosmos_DebugCode(
    const char *         m_pName;
  )

  oosmos_sTimeout m_ThreadTimeout;

  //
  // The stored __LINE__ number of the current Thread-type call.
  //
  int32_t m_ThreadContext;

  //
  // Flag that helps a Thread sequence exit (if a transition occurred) or
  // continue.
  //
  unsigned int m_TransitionOccurred:1;

  //
  // Support for oosmos_ThreadYield.
  //
  unsigned int m_ThreadHasYielded:1;

  //
  // Really an eTypes enumeration, but ANSI won't allow an enum on a bit field.
  //
  unsigned int m_Type:4;
};

struct OOSMOS_sCompositeTag {
  oosmos_sState     m_State;
  oosmos_sState   * m_pDefault;
};

struct OOSMOS_sRegionTag {
  oosmos_sComposite m_Composite;
  oosmos_sState   * m_pCurrent;
};

#ifdef oosmos_ORTHO
  struct OOSMOS_sOrthoRegionTag {
    oosmos_sRegion        m_Region;
    oosmos_sOrthoRegion * m_pNextOrthoRegion;
  };

  struct OOSMOS_sOrthoTag {
    oosmos_sState         m_State;
    oosmos_sOrthoRegion * m_pFirstOrthoRegion;
  };
#endif

struct OOSMOS_sStateMachineTag {
  oosmos_sRegion         m_Region;

  oosmos_sQueue        * m_pEventQueue;
  oosmos_sEvent        * m_pCurrentEvent;
  oosmos_sStateMachine * m_pNext;
  void                 * m_pObject;

  oosmos_DebugCode(
    const char * (*m_pEventNameConverter)(int);
    const char * m_pFileName;
    bool         m_Debug;
  )

  uint16_t m_CurrentEventSize;
  bool     m_IsStarted;
};

#define OOSMOS_xstr(s) OOSMOS_str(s)

// To reduce footprint, only materialize these strings when compiling for debug.
#ifdef oosmos_DEBUG
  #define OOSMOS_str(s) #s
  #define OOSMOS_FILE __FILE__
#else
  #define OOSMOS_str(s) ""
  #define OOSMOS_FILE ""
#endif

//--------
extern void OOSMOS_EndProgram(int);

//--------
extern void OOSMOS_StateMachineDetach(const oosmos_sStateMachine * pStateMachine);

#define oosmos_StateMachineDetach(pObject, StateMachine) \
          OOSMOS_StateMachineDetach(&(pObject)->StateMachine)
//--------
extern void OOSMOS_StateMachineInit(const char * pFileName, const char * pName, oosmos_sStateMachine * pStateMachine, oosmos_sState * pDefault, oosmos_sQueue * pEventQueue,
                 void * pCurrentEvent, size_t CurrentEventSize, void * pObject);

#define oosmos_StateMachineInit(pObject, StateMachine, Parent, Default)\
        oosmos_QueueConstruct(&(pObject)->m_EventQueue, (pObject)->m_EventQueueData) \
        OOSMOS_StateMachineInit(OOSMOS_FILE, OOSMOS_xstr(StateMachine), &(pObject)->StateMachine, (oosmos_sState*) &(pObject)->Default, &(pObject)->m_EventQueue,\
                 &(pObject)->m_CurrentEvent, sizeof((pObject)->m_EventQueueData[0]), (pObject))

#define oosmos_StateMachineInitNoQueue(pObject, StateMachine, Parent, Default)\
        OOSMOS_StateMachineInit(OOSMOS_FILE, OOSMOS_xstr(StateMachine), &(pObject)->StateMachine, (oosmos_sState*) &(pObject)->Default, NULL, NULL, 0, (pObject))
//--------
extern void OOSMOS_CompositeInit(const char * pName, oosmos_sComposite *pComposite,
                       oosmos_sState *pParent, oosmos_sState * pDefault, OOSMOS_tCode pCode);
#define oosmos_CompositeInit(pObject, Composite, Parent, Default, Code) \
        OOSMOS_CompositeInit(OOSMOS_xstr(Composite), &(pObject)->Composite, (oosmos_sState*) &(pObject)->Parent, (oosmos_sState*) &(pObject)->Default, Code)
//--------
extern void OOSMOS_LeafInit(const char * pName, oosmos_sState *pState, oosmos_sState *pParent, OOSMOS_tCode pCode);

#define oosmos_LeafInit(pObject, LeafState, Parent, Code)\
        OOSMOS_LeafInit(OOSMOS_xstr(LeafState), &(pObject)->LeafState, (oosmos_sState *) &(pObject)->Parent, Code)

#ifdef oosmos_ORTHO

extern void OOSMOS_OrthoInit(const char * pName, oosmos_sOrtho *pOrtho, oosmos_sState *pParent, OOSMOS_tCode pCode);

#define oosmos_OrthoInit(pObject, Ortho, Parent, Code)\
        OOSMOS_OrthoInit(OOSMOS_xstr(Ortho), &(pObject)->Ortho, (oosmos_sState *) &(pObject)->Parent, Code)
//--------
extern void OOSMOS_OrthoRegionInit(const char * pName, oosmos_sOrthoRegion * pOrthoRegion,
                                   oosmos_sOrtho * pOrthoParent, oosmos_sState * pDefault, OOSMOS_tCode pHandler);

#define oosmos_OrthoRegionInit(pObject, OrthoRegion, Parent, Default, Code) \
        OOSMOS_OrthoRegionInit(OOSMOS_xstr(OrthoRegion), &(pObject)->OrthoRegion, &(pObject)->Parent, (oosmos_sState*) &(pObject)->Default, Code)

#endif
//--------
extern void OOSMOS_ChoiceInit(const char * pName, oosmos_sState *pState, oosmos_sState *pParent, OOSMOS_tCode pCode);

#define oosmos_ChoiceInit(pObject, State, Parent)\
        OOSMOS_ChoiceInit(OOSMOS_xstr(State), &(pObject)->State, (oosmos_sState*) &(pObject)->Parent, State ## _Code)
//--------
extern void OOSMOS_FinalInit(const char * pName, oosmos_sState *pState, oosmos_sState *pParent, OOSMOS_tCode pCode);

#define oosmos_FinalInit(pObject, State, Parent, Code)\
        OOSMOS_FinalInit(OOSMOS_xstr(State), &(pObject)->State, (oosmos_sState*) &(pObject)->Parent, Code)
//--------
extern bool OOSMOS_TransitionAction(oosmos_sState * pFromState, oosmos_sState * pToState, const oosmos_sEvent * pEvent, oosmos_tAction pActionCode);

#define oosmos_TransitionAction(pObject, pFromState, ToState, pEvent, ActionCode) \
        OOSMOS_TransitionAction(pFromState, (oosmos_sState*) (&(pObject)->ToState), pEvent, ActionCode)

extern bool OOSMOS_Transition(oosmos_sState * pFromState, oosmos_sState * pToState);

#define oosmos_Transition(pObject, pFromState, ToState) \
        OOSMOS_Transition(pFromState, (oosmos_sState*) (&(pObject)->ToState))
//--------
extern bool OOSMOS_IsInState(const oosmos_sStateMachine * pStateMachine, const oosmos_sState * pState);

#define oosmos_IsInState(pObject, pState) \
        OOSMOS_IsInState(&((pObject)->ROOT), (oosmos_sState*) (pState))
//--------
#define oosmos_RunStateMachine(pObject) \
  OOSMOS_RunStateMachine(&(pObject)->ROOT)

extern void OOSMOS_RunStateMachine(oosmos_sStateMachine * pStateMachine);
//--------

extern void oosmos_RunStateMachines(void);

extern bool oosmos_StateTimeoutSeconds(oosmos_sState * pState, uint32_t TimeoutInSeconds);
extern bool oosmos_StateTimeoutMS(oosmos_sState * pState, uint32_t TimeoutInMS);
extern bool oosmos_StateTimeoutUS(oosmos_sState * pState, uint32_t TimeoutInUS);
//--------
extern void OOSMOS_PushEventCodeToStateMachine(const oosmos_sStateMachine * pStateMachine, int EventCode);
extern void OOSMOS_PushEventToStateMachine(const oosmos_sStateMachine * pStateMachine, const void * pEvent, size_t EventSize);

#define oosmos_PushEventCode(pObject, EventCode) \
  OOSMOS_PushEventCodeToStateMachine(&((pObject)->ROOT), EventCode)

#define OOSMOS_CompileTimeAssert(cond) \
    typedef char OOSMOS_CompileTimeAssertArray[(cond) ? 1 : 0]

#define oosmos_PushEvent(pObject, Event) \
  /* Catch common error of passing &Event instead of Event. */ \
  OOSMOS_CompileTimeAssert(sizeof(Event) != sizeof(void *)); \
  OOSMOS_PushEventToStateMachine(&((pObject)->ROOT), &(Event), sizeof(Event))
//--------
#ifdef oosmos_DEBUG
  #define oosmos_Debug(pObject, pEventName) \
    OOSMOS_Debug(&(pObject)->ROOT, pEventName)

  extern void OOSMOS_Debug(oosmos_sStateMachine * pStateMachine, const char * (*pEventName)(int));
#else
  #define oosmos_Debug(pStateMachine, pEventName)
#endif
//--------

typedef void (*oosmos_tOutOfMemory)(const char*, int, const char*);

#define OOSMOS_Allocate(List, Count, Type, Pointer, Elements, OutOfMemory) \
  {                                                                        \
    /*lint -e774 Suppress "if always evaluates to false" */                \
    if (Count > Elements-1) {                                              \
      oosmos_tOutOfMemory pOutOfMemory = OutOfMemory;                      \
                                                                           \
      if (pOutOfMemory != NULL) {                                          \
        pOutOfMemory(OOSMOS_FILE, __LINE__, # Type);                       \
      }                                                                    \
                                                                           \
      oosmos_FOREVER();                                                    \
    }                                                                      \
    else {                                                                 \
      Pointer = &List[Count++];                                            \
    }                                                                      \
  }

#define oosmos_AllocateVisible(Pointer, Type, List, Count, Elements, OutOfMemory) \
  Type * Pointer;                                                                 \
  OOSMOS_Allocate(List, Count, Type, Pointer, Elements, OutOfMemory);

#define oosmos_Allocate(Pointer, Type, Elements, OutOfMemory)                          \
  Type * Pointer;                                                                      \
                                                                                       \
  {                                                                                    \
    static Type OOSMOS_List[Elements];                                                 \
    static int  OOSMOS_Count = 0;                                                      \
    OOSMOS_Allocate(OOSMOS_List, OOSMOS_Count, Type, Pointer, Elements, OutOfMemory);  \
  }

#define oosmos_EventCode(pEvent) ((pEvent)->m_Code)

#define oosmos_EventQueue(pObject) (&((pObject)->m_EventQueue))

#define oosmos_ENTER    (-1)
#define oosmos_EXIT     (-2)
#define oosmos_POLL     (-3)
#define oosmos_TIMEOUT  (-4)
#define oosmos_DEFAULT  (-5)
#define oosmos_COMPLETE (-6)

//
// oosmos_ThreadDelayMS
//
// oosmos_ThreadWaitCond
// oosmos_ThreadWaitCond_TimeoutMS
// oosmos_ThreadWaitCond_TimeoutMS_Event
// oosmos_ThreadWaitCond_TimeoutMS_Exit
//
// oosmos_ThreadWaitEvent
// oosmos_ThreadWaitEvent_TimeoutMS
// oosmos_ThreadWaitEvent_TimeoutMS_Event
// oosmos_ThreadWaitEvent_TimeoutMS_Exit
//
// oosmos_ThreadYield
//

extern bool OOSMOS_ThreadDelayMS(oosmos_sState * pState,
                        uint32_t MS);

extern bool OOSMOS_ThreadWaitCond(oosmos_sState * pState,
                        bool Condition);

extern bool OOSMOS_ThreadWaitCond_TimeoutMS(oosmos_sState * pState,
                        bool Condition, uint32_t TimeoutMS, bool * pTimeoutStatus);

extern bool OOSMOS_ThreadWaitCond_TimeoutMS_Event(oosmos_sState * pState,
                        bool Condition, uint32_t TimeoutMS, int NotificationEventCode);

extern bool OOSMOS_ThreadWaitCond_TimeoutMS_Exit(oosmos_sState * pState,
                        bool Condition, uint32_t TimeoutMS);

extern bool OOSMOS_ThreadWaitEvent(const oosmos_sState * pState,
                        int WaitEventCode);

extern bool OOSMOS_ThreadWaitEvent_TimeoutMS(oosmos_sState * pState,
                        int WaitEventCode, uint32_t TimeoutMS, bool * pTimedOut);

extern bool OOSMOS_ThreadWaitEvent_TimeoutMS_Event(oosmos_sState * pState,
                        int WaitEventCode, uint32_t TimeoutMS, int NotificationEventCode);

extern bool OOSMOS_ThreadWaitEvent_TimeoutMS_Exit(oosmos_sState * pState,
                        int WaitEventCode, uint32_t TimeoutMS);

extern bool OOSMOS_ThreadYield(oosmos_sState * pState);

//
// Use the Protothread __LINE__ trick to implement OOSMOS thread functions. OOSMOS thread
// functions block the object, not the entire thread of execution.
//
// Very powerful.
//

#define OOSMOS_THREAD_CONTEXT_BEGIN   (-1)
#define OOSMOS_THREAD_CONTEXT_FINALLY (-2)
#define OOSMOS_THREAD_CONTEXT_END     (-3)

#define oosmos_ThreadBegin() \
                                    switch (pState->m_ThreadContext) { \
                                      case OOSMOS_THREAD_CONTEXT_BEGIN:

#define oosmos_ThreadDelayMS(MS) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadDelayMS(pState, (MS))) \
                                          return

#define oosmos_ThreadYield() \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadYield(pState)) \
                                          return

#define oosmos_ThreadWaitCond(Cond) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!(Cond)) \
                                          return

#define oosmos_ThreadWaitCond_TimeoutMS(Cond, TimeoutMS, pTimeoutStatus) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadWaitCond_TimeoutMS(pState, Cond, TimeoutMS, pTimeoutStatus)) \
                                          return

#define oosmos_ThreadWaitCond_TimeoutMS_Event(Cond, TimeoutMS, NotificationEventCode) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadWaitCond_TimeoutMS_Event(pState, Cond, TimeoutMS, NotificationEventCode)) \
                                          return

#define oosmos_ThreadWaitCond_TimeoutMS_Exit(Cond, TimeoutMS) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadWaitCond_TimeoutMS_Exit(pState, Cond, TimeoutMS)) \
                                          return

#define oosmos_ThreadWaitEvent(WaitEventCode) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadWaitEvent(pState, WaitEventCode)) \
                                          return

#define oosmos_ThreadWaitEvent_TimeoutMS(WaitEventCode, TimeoutMS, pTimeoutResult) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadWaitEvent_TimeoutMS(pState, WaitEventCode, TimeoutMS, pTimeoutResult)) \
                                          return

#define oosmos_ThreadWaitEvent_TimeoutMS_Event(WaitEventCode, TimeoutMS, NotificationEventCode) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadWaitEvent_TimeoutMS_Event(pState, WaitEventCode, TimeoutMS, NotificationEventCode)) \
                                          return

#define oosmos_ThreadWaitEvent_TimeoutMS_Exit(WaitEventCode, TimeoutMS) \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      case __LINE__: pState->m_ThreadContext = __LINE__; \
                                        if (!OOSMOS_ThreadWaitEvent_TimeoutMS_Exit(pState, WaitEventCode, TimeoutMS)) \
                                          return

#define oosmos_ThreadExit() \
                                        pState->m_ThreadContext = OOSMOS_THREAD_CONTEXT_FINALLY; \
                                        return

#define oosmos_ThreadFinally() \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      case OOSMOS_THREAD_CONTEXT_FINALLY: pState->m_ThreadContext = OOSMOS_THREAD_CONTEXT_END

#define oosmos_ThreadEnd() \
                                      /*lint -e646 suppress "case/default within for loop; may have been misplaced" */ \
                                      /*lint -fallthrough*/ \
                                      default: pState->m_ThreadContext = OOSMOS_THREAD_CONTEXT_END; \
                                        (void) oosmos_ThreadComplete(pState); \
                                    } return

//
// General notes:
// 1. lint directives are embedded to suppress fallthrough messages as well as
//    "case/default within for loop" messages.
//

//
// Note the following regarding the oosmos_ThreadEnd macro:
// 1. It must be the last statement in the function.
// 2. It causes the delivery of a completion event to the state.
//

typedef struct {
  oosmos_sQueue * m_pNotifyQueue;
  oosmos_sEvent   m_Event;
} oosmos_sSubscriberList;

//--------
#define oosmos_SubscriberListInit(Subscriber) \
        OOSMOS_SubscriberListInit(Subscriber, sizeof(Subscriber)/sizeof(Subscriber[0]))
extern void OOSMOS_SubscriberListInit(oosmos_sSubscriberList * pSubscriberList, size_t ListElements);
//--------
#define oosmos_SubscriberListNotify(Subscriber) \
        OOSMOS_SubscriberListNotify(Subscriber, sizeof(Subscriber)/sizeof(Subscriber[0]))
extern void OOSMOS_SubscriberListNotify(const oosmos_sSubscriberList * pSubscriberList, size_t ListElements);
//--------
#define oosmos_SubscriberListNotifyWithArgs(Subscriber, Event) \
        OOSMOS_SubscriberListNotifyWithArgs(Subscriber, &Event, sizeof(Event), sizeof(Subscriber)/sizeof(Subscriber[0]))
extern void OOSMOS_SubscriberListNotifyWithArgs(const oosmos_sSubscriberList * pSubscriberList, void * pEvent, size_t EventSize, size_t ListElements);
//--------
#define oosmos_SubscriberListAdd(Subscriber, pNotifyQueue, EventCode, pContext) \
        OOSMOS_SubscriberListAdd(Subscriber, sizeof(Subscriber)/sizeof(Subscriber[0]), pNotifyQueue, EventCode, pContext)
extern void OOSMOS_SubscriberListAdd(oosmos_sSubscriberList * pSubscriberList, size_t ListElements, oosmos_sQueue * pNotifyQueue, int EventCode, void * pContext);

extern void oosmos_DelayUS(uint32_t US);
extern void oosmos_DelayMS(uint32_t MS);
extern void oosmos_DelaySeconds(uint32_t Seconds);

extern void oosmos_TimeoutInMS(oosmos_sTimeout * pTimeout, uint32_t Milliseconds);
extern void oosmos_TimeoutInUS(oosmos_sTimeout * pTimeout, uint32_t Microseconds);
extern void oosmos_TimeoutInSeconds(oosmos_sTimeout * pTimeout, uint32_t Seconds);
extern bool oosmos_TimeoutHasExpired(const oosmos_sTimeout * pTimeout);

//--------
extern void OOSMOS_QueueConstruct(oosmos_sQueue * pQueue, void * pQueueData, size_t QueueDataSize, size_t QueueElementSize);
#define oosmos_QueueConstruct(pQueue, pQueueData) \
  OOSMOS_QueueConstruct((pQueue), (pQueueData),sizeof(pQueueData),sizeof((pQueueData)[0]));

extern void oosmos_QueuePush(oosmos_sQueue * pQueue, const void * pElement, size_t ElementSize);
extern bool oosmos_QueuePop(oosmos_sQueue * pQueue, void * pElement, size_t ElementSize);
extern void oosmos_QueueSetBehaviorFunc(oosmos_sQueue * pQueue, oosmos_eQueueFullBehavior (*pFunc)(void *), void * pContext);

#define oosmos_GetCurrentEvent(pState) \
  ((const oosmos_sEvent *) OOSMOS_GetCurrentEvent(pState))
extern oosmos_sEvent * OOSMOS_GetCurrentEvent(const oosmos_sState * pState);

extern bool oosmos_ThreadComplete(oosmos_sState *);

typedef struct OOSMOS_sActiveObjectTag oosmos_sActiveObject;

struct OOSMOS_sActiveObjectTag {
  void * m_pObject;
  void (*m_pFunction)(void * pObject);
  oosmos_sActiveObject * m_pNext;
};

extern void oosmos_RegisterActiveObject(void * pObject, void (*pFunction)(void *), oosmos_sActiveObject * pActiveObject);
extern uint32_t oosmos_GetFreeRunningMicroseconds(void);

extern float   oosmos_AnalogMapAccurate(float Value, float InMin, float InMax, float OutMin, float OutMax);
extern int32_t oosmos_AnalogMapFast(int32_t Value, int32_t InMin, int32_t InMax, int32_t OutMin, int32_t OutMax);

#define oosmos_Min(a,b) (((a)<(b))?(a):(b))
#define oosmos_Max(a,b) (((a)>(b))?(a):(b))

#ifdef __cplusplus
  }
#endif

#endif
