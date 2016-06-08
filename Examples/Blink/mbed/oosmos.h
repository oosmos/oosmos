//
// OOSMOS - The Object-Oriented State Machine Operating System
//
// Copyright (C) 2014-2016  OOSMOS, LLC
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
//

#ifndef OOSMOS_h
#define OOSMOS_h

//
// All names that begin with OOSMOS (all caps) are private and are not part of the
// official oosmos interface.
//

#include <stdint.h>
#include <stddef.h>

//
// Define the bool datatype, one way or another.
//
#if !defined(__cplusplus)
  #if !defined(oosmos_HAS_BOOL)
    #undef bool
    #undef false
    #undef true
    #define bool char
    #define false 0
    #define true  1
  #endif
#endif

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

  #define oosmos_EndProgram(Code) while(1)continue
#elif defined(__PIC32MX)
  #include <plib.h>
  extern void oosmos_ClockSpeedInMHz(int ClockSpeedMHz);
  extern int oosmos_GetClockSpeedInMHz(void);
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
#else
  #define oosmos_EndProgram(Code) OOSMOS_EndProgram(Code)
#endif

typedef struct
{
  uint32_t Start;
  uint32_t End;
} oosmos_sTimeout;

typedef enum {
  oosmos_QueueFull_TossNew = 1,
  oosmos_QueueFull_TossOld,
} oosmos_eQueueFullBehavior;

typedef struct OOSMOS_sQueueTag oosmos_sQueue;

struct OOSMOS_sQueueTag
{
  void   * pHead;
  void   * pTail;
  void   * pEnd;
  void   * pQueueData;
  uint16_t QueueDataSize;
  uint16_t QueueElementSize;
  uint16_t ByteCount;

  oosmos_eQueueFullBehavior (*pFullBehaviorFunc)(void * pContext);
  void   * pContext;
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
    #define oosmos_DebugPrint printf
    #define oosmos_DebugCode(x) x
  #endif

  extern void oosmos_DebugInit(void);
#else
  extern void oosmos_DebugDummy(const char*, ...);
  #define oosmos_DebugPrint 1 ? (void)0 : oosmos_DebugDummy
  #define oosmos_DebugCode(x)
  #define oosmos_DebugInit()
#endif

typedef struct
{
  int    Code;
  void * pContext;
} oosmos_sEvent;

#define oosmos_sStateMachine(StateMachine, EventType, MaxEvents) \
  oosmos_sQueue        EventQueue; \
  EventType            EventQueueData[MaxEvents]; \
  EventType            CurrentEvent; \
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
typedef struct OOSMOS_sStateTag        oosmos_sChoice;
typedef struct OOSMOS_sStateTag        oosmos_sFinal;

typedef bool (*OOSMOS_tCode)(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent);

typedef enum
{
  OOSMOS_CompositeType = 1,
  OOSMOS_StateMachineType,
  OOSMOS_FinalType,
  OOSMOS_LeafType,

  #ifdef oosmos_ORTHO
    OOSMOS_OrthoType,
    OOSMOS_OrthoRegionType,
  #endif
} OOSMOS_eTypes;

struct OOSMOS_sStateTag {
  oosmos_sState  * pParent;
  OOSMOS_tCode     pCode;
  oosmos_sTimeout  Timeout;

  oosmos_DebugCode(
    oosmos_sStateMachine * pStateMachine;
    const char *           pName;
  )

  oosmos_sTimeout SyncTimeout;

  //
  // The stored __LINE__ number of the current Sync-type call.
  //
  int SyncContext;

  //
  // A user could attempt to make a Sync-type call without grouping it within a
  // SyncBegin/SyncEnd.  They will not get a syntax error.  This flag helps remind
  // them of the requirement at run time.
  //
  int HasSyncBlockBegin:1;

  //
  // Following a successful SyncWaitEvent-type call, we can drop into another
  // SyncWaitEvent-type call.  We don't want the spent event being handled
  // by the subsequent function.  We must cycle back through another INSTATE
  // with a fresh event.
  //
  int SyncDirtyEvent:1;

  //
  // Flag that helps a Sync sequence exit (if a transition occurred) or
  // continue.
  //
  int TransitionOccurred:1;

  //
  // Support for oosmos_SyncYield.
  //
  int HasYielded:1;

  //
  // Really an eTypes enumeration, but ANSI won't allow an enum on a bit field.
  //
  int Type:4;
};

struct OOSMOS_sCompositeTag {
  oosmos_sState     State;
  oosmos_sState   * pDefault;
};

struct OOSMOS_sRegionTag {
  oosmos_sComposite Composite;
  oosmos_sState   * pCurrent;
  void            * pObject;
};

#ifdef oosmos_ORTHO
  struct OOSMOS_sOrthoRegionTag
  {
    oosmos_sRegion        Region;
    oosmos_sOrthoRegion * pNextOrthoRegion;
  };

  struct OOSMOS_sOrthoTag {
    oosmos_sState         State;
    oosmos_sOrthoRegion * pFirstOrthoRegion;
  };
#endif

struct OOSMOS_sStateMachineTag
{
  oosmos_sRegion         Region;

  oosmos_sQueue        * pEventQueue;
  void                 * pCurrentEvent;
  oosmos_sStateMachine * pNext;

  oosmos_DebugCode(
    const char * (*pEventNameConverter)(int);
    bool         Debug;
  )

  uint16_t CurrentEventSize;
  bool     IsStarted;
};

#define OOSMOS_xstr(s) OOSMOS_str(s)

#ifdef oosmos_DEBUG
  #define OOSMOS_str(s) #s
#else
  #define OOSMOS_str(s) ""
#endif

#ifdef __cplusplus
  extern "C" {
#endif

//--------
extern void OOSMOS_EndProgram(int);

//--------
extern void OOSMOS_StateMachineDetach(oosmos_sStateMachine * pStateMachine);

#define oosmos_StateMachineDetach(pObject, StateMachine) \
          OOSMOS_StateMachineDetach(&(pObject)->StateMachine)

//--------
extern void OOSMOS_StateMachineInit(const char * pName, oosmos_sStateMachine * pStateMachine, oosmos_sState * pDefault, oosmos_sQueue * pEventQueue,
                 void * pCurrentEvent, size_t CurrentEventSize, void * pObject);

#define oosmos_StateMachineInit(pObject, StateMachine, Parent, Default)\
        oosmos_QueueConstruct(&(pObject)->EventQueue, (pObject)->EventQueueData) \
        OOSMOS_StateMachineInit(#StateMachine, &(pObject)->StateMachine, (oosmos_sState*) &(pObject)->Default, &(pObject)->EventQueue,\
                 &(pObject)->CurrentEvent, sizeof((pObject)->EventQueueData[0]), (pObject))

#define oosmos_StateMachineInitNoQueue(pObject, StateMachine, Parent, Default)\
        OOSMOS_StateMachineInit(#StateMachine, &(pObject)->StateMachine, (oosmos_sState*) &(pObject)->Default, NULL,\
                 NULL, 0, (pObject))
//--------
extern void OOSMOS_CompositeInit(const char * pName, oosmos_sComposite *pComposite,
                       oosmos_sState *pParent, oosmos_sState * pDefault, OOSMOS_tCode pCode);
#define oosmos_CompositeInit(pObject, Composite, Parent, Default) \
        OOSMOS_CompositeInit(#Composite, &(pObject)->Composite, (oosmos_sState*) &(pObject)->Parent, (oosmos_sState*) &(pObject)->Default, Composite ## _Code)

#define oosmos_CompositeInitNoDefault(pObject, Composite, Parent) \
        OOSMOS_CompositeInit(OOSMOS_xstr(Composite), &(pObject)->Composite, (oosmos_sState*) &(pObject)->Parent, NULL, Composite ## _Code)

#define oosmos_CompositeInitNoCode(pObject, Composite, Parent, Default) \
        OOSMOS_CompositeInit(OOSMOS_xstr(Composite), &(pObject)->Composite, (oosmos_sState*) &(pObject)->Parent, (oosmos_sState*) &(pObject)->Default, NULL)

#define oosmos_CompositeInitNoDefaultNoCode(pObject, Composite, Parent) \
        OOSMOS_CompositeInit(OOSMOS_xstr(Composite), &(pObject)->Composite, (oosmos_sState*) &(pObject)->Parent, NULL, NULL)
//--------
extern void OOSMOS_LeafInit(const char * pName, oosmos_sState *pState, oosmos_sState *pParent, OOSMOS_tCode pCode);

#define oosmos_LeafInit(pObject, LeafState, Parent)\
        OOSMOS_LeafInit(#LeafState, &(pObject)->LeafState, (oosmos_sState *) &(pObject)->Parent, LeafState ## _Code)

#define oosmos_LeafInitNoCode(pObject, LeafState, Parent)\
        OOSMOS_LeafInit(#LeafState, &(pObject)->LeafState, (oosmos_sState *) &(pObject)->Parent, NULL)
#ifdef oosmos_ORTHO
//--------
extern void OOSMOS_OrthoInit(const char * pName, oosmos_sOrtho *pOrtho, oosmos_sState *pParent, OOSMOS_tCode pCode);

#define oosmos_OrthoInit(pObject, Ortho, Parent)\
        OOSMOS_OrthoInit(#Ortho, &(pObject)->Ortho, (oosmos_sState *) &(pObject)->Parent, Ortho ## _Code)

#define oosmos_OrthoInitNoCode(pObject, Ortho, Parent)\
        OOSMOS_OrthoInit(#Ortho, &(pObject)->Ortho, (oosmos_sState *) &(pObject)->Parent, NULL)
//--------
extern void OOSMOS_OrthoRegionInit(const char * pName, oosmos_sOrthoRegion * pOrthoRegion,
                              oosmos_sOrtho * pOrthoParent, oosmos_sState * pDefault, OOSMOS_tCode pHandler, void * pObject);

#define oosmos_OrthoRegionInit(pObject, OrthoRegion, Parent, Default) \
        OOSMOS_OrthoRegionInit(#OrthoRegion, &(pObject)->OrthoRegion, &(pObject)->Parent, (oosmos_sState*) &(pObject)->Default, OrthoRegion ## _Code, (pObject))

#define oosmos_OrthoRegionInitNoCode(pObject, OrthoRegion, Parent, Default) \
        OOSMOS_OrthoRegionInit(#OrthoRegion, &(pObject)->OrthoRegion, &(pObject)->Parent, (oosmos_sState*) &(pObject)->Default, NULL, (pObject))
#endif
//--------
extern void OOSMOS_ChoiceInit(const char * pName, oosmos_sState *pState, oosmos_sState *pParent, OOSMOS_tCode pCode);

#define oosmos_ChoiceInit(pObject, State, Parent)\
        OOSMOS_ChoiceInit(#State, &(pObject)->State, (oosmos_sState*) &(pObject)->Parent, State ## _Code)
//--------
extern void OOSMOS_FinalInit(const char * pName, oosmos_sState *pState, oosmos_sState *pParent, OOSMOS_tCode pCode);

#define oosmos_FinalInit(pObject, State, Parent)\
        OOSMOS_FinalInit(#State, &(pObject)->State, (oosmos_sState*) &(pObject)->Parent, State ## _Code)

#define oosmos_FinalInitNoCode(pObject, State, Parent)\
        OOSMOS_FinalInit(#State, &(pObject)->State, (oosmos_sState*) &(pObject)->Parent, NULL)
//--------
extern bool OOSMOS_Transition(oosmos_sRegion * pRegion, oosmos_sState * pToState);

#define oosmos_Transition(pRegion, pToState)\
        OOSMOS_Transition(pRegion, (oosmos_sState*) pToState)
//--------
extern bool OOSMOS_IsInState(const oosmos_sStateMachine * pStateMachine, const oosmos_sState * pState);

#define oosmos_IsInState(pStateMachine, pState) \
        OOSMOS_IsInState(pStateMachine, (oosmos_sState*) pState)
//--------

extern void oosmos_RunStateMachine(oosmos_sStateMachine * pStateMachine);
extern void oosmos_RunStateMachines(void);

extern bool oosmos_StateTimeoutSeconds(oosmos_sRegion * pRegion, uint32_t TimeoutInSeconds);
extern bool oosmos_StateTimeoutMS(oosmos_sRegion * pRegion, uint32_t TimeoutInMS);
extern bool oosmos_StateTimeoutUS(oosmos_sRegion * pRegion, uint32_t TimeoutInUS);

extern bool oosmos_PushEventToQueue(oosmos_sQueue * pNotifyQueue, int EventCode);
extern bool oosmos_PushEventToStateMachine(oosmos_sStateMachine * pStateMachine, int EventCode);

#define oosmos_SendEvent(pObject, EventCode) \
  oosmos_PushEventToStateMachine(&((pObject)->StateMachine), EventCode)

#ifdef oosmos_DEBUG
  extern void oosmos_Debug(oosmos_sStateMachine * pStateMachine, bool Debug, const char * (*pEventName)(int));
#else
  #define oosmos_Debug(pStateMachine, Debug, pEventName)
#endif

#define oosmos_TransitionAction(pRegion_, pToState_, Code_)                             \
  do {                                                                                  \
    oosmos_sState * pLCA_ = OOSMOS_LCA((pRegion_)->pCurrent, (oosmos_sState*)(pToState_)); \
                                                                                        \
    OOSMOS_Exit((pRegion_), (pRegion_)->pCurrent, pLCA_);                               \
    do { Code_ } while(0);                                                              \
    OOSMOS_Enter((pRegion_), pLCA_, (oosmos_sState*)(pToState_));                       \
  } while(0);

typedef void (*oosmos_tOutOfMemory)(const char*, int, const char*);

#define OOSMOS_Allocate(List, Count, Type, Pointer, Elements, OutOfMemory) \
  {                                                                        \
    if (Count > Elements-1) {                                              \
      oosmos_tOutOfMemory pOutOfMemory = OutOfMemory;                      \
                                                                           \
      if (pOutOfMemory != NULL)                                            \
        pOutOfMemory(__FILE__, __LINE__, # Type);                          \
                                                                           \
      while (true);                                                        \
    }                                                                      \
    else {                                                                 \
      Pointer = &List[Count++];                                            \
    }                                                                      \
  }

#define oosmos_AllocateVisible(Pointer, Type, List, Count, Elements, OutOfMemory) \
  Type * Pointer;                                                                 \
  OOSMOS_Allocate(List, Count, Type, Pointer, Elements, OutOfMemory);

#define oosmos_Allocate(Pointer, Type, Elements, OutOfMemory)            \
  Type * Pointer;                                                        \
                                                                         \
  {                                                                      \
    static Type List[Elements];                                          \
    static int  Count = 0;                                               \
    OOSMOS_Allocate(List, Count, Type, Pointer, Elements, OutOfMemory);  \
  }

#define oosmos_ENTER    -1
#define oosmos_EXIT     -2
#define oosmos_INSTATE  -3
#define oosmos_TIMEOUT  -4
#define oosmos_DEFAULT  -5
#define oosmos_COMPLETE -6


//
// oosmos_SyncDelayMS
//
// oosmos_SyncWaitCond
// oosmos_SyncWaitCond_TimeoutMS
// oosmos_SyncWaitCond_TimeoutMS_Event
// oosmos_SyncWaitCond_TimeoutMS_Exit
//
// oosmos_SyncWaitEvent
// oosmos_SyncWaitEvent_TimeoutMS
// oosmos_SyncWaitEvent_TimeoutMS_Event
// oosmos_SyncWaitEvent_TimeoutMS_Exit
//
// oosmos_SyncYield
//


extern bool OOSMOS_SyncYield(oosmos_sRegion * pRegion);

extern bool OOSMOS_SyncDelayMS(oosmos_sRegion * pRegion, int MS);

extern bool OOSMOS_SyncWaitCond(oosmos_sRegion * pRegion, bool Condition);

extern bool OOSMOS_SyncWaitCond_TimeoutMS(oosmos_sRegion * pRegion,
                       int TimeoutMS, bool * pTimeoutStatus, bool Condition);

extern bool OOSMOS_SyncWaitCond_TimeoutMS_Event(oosmos_sRegion * pRegion,
                       int TimeoutMS, int NotificationEventCode, bool Condition);

extern bool OOSMOS_SyncWaitCond_TimeoutMS_Exit(oosmos_sRegion * pRegion,
                       int TimeoutMS, bool Condition);

extern bool OOSMOS_SyncWaitEvent(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent,
                       int WaitEventCode);

extern bool OOSMOS_SyncWaitEvent_TimeoutMS(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent,
                       int TimeoutMS, bool * pTimedOut, int WaitEventCode);

extern bool OOSMOS_SyncWaitEvent_TimeoutMS_Event(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent,
                       int TimeoutMS, int NotificationEventCode, int WaitEventCode);

extern bool OOSMOS_SyncWaitEvent_TimeoutMS_Exit(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent,
                       int TimeoutMS, int WaitEventCode);

//
// Use the Protothread __LINE__ trick to implement synchronous functions.  Synchronous
// functions block the object, not the entire thread of execution.
//
// Very powerful.
//


#define oosmos_SyncBegin(pRegion) switch ((pRegion)->pCurrent->SyncContext) { \
                                    case 0: \
                                      (pRegion)->pCurrent->HasSyncBlockBegin = 1

#define oosmos_SyncDelayMS(pRegion, MS) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncDelayMS(pRegion, (MS))) \
                                        return false

#define oosmos_SyncYield(pRegion) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncYield(pRegion)) \
                                        return false

#define oosmos_SyncWaitCond(pRegion, Cond) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!(Cond)) \
                                        return false

#define oosmos_SyncWaitCond_TimeoutMS(pRegion, TimeoutMS, pTimeoutStatus, Cond) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncWaitCond_TimeoutMS(pRegion, TimeoutMS, pTimeoutStatus, Cond)) \
                                        return false

#define oosmos_SyncWaitCond_TimeoutMS_Event(pRegion, TimeoutMS, NotificationEventCode, Cond) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncWaitCond_TimeoutMS_Event(pRegion, TimeoutMS, NotificationEventCode, Cond)) \
                                        return false

#define oosmos_SyncWaitCond_TimeoutMS_Exit(pRegion, TimeoutMS, Cond) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncWaitCond_TimeoutMS_Exit(pRegion, TimeoutMS, Cond)) \
                                        return false

#define oosmos_SyncWaitEvent(pRegion, pEvent, WaitEventCode) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncWaitEvent(pRegion, pEvent, WaitEventCode)) \
                                        return false

#define oosmos_SyncWaitEvent_TimeoutMS(pRegion, pEvent, TimeoutMS, pTimeoutResult, WaitEventCode) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncWaitEvent_TimeoutMS(pRegion, pEvent, TimeoutMS, pTimeoutResult, WaitEventCode)) \
                                        return false

#define oosmos_SyncWaitEvent_TimeoutMS_Event(pRegion, pEvent, TimeoutMS, NotificationEventCode, WaitEventCode) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncWaitEvent_TimeoutMS_Event(pRegion, pEvent, TimeoutMS, NotificationEventCode, WaitEventCode)) \
                                        return false

#define oosmos_SyncWaitEvent_TimeoutMS_Exit(pRegion, pEvent, TimeoutMS, WaitEventCode) \
                                    case __LINE__: (pRegion)->pCurrent->SyncContext = __LINE__; \
                                      if (!OOSMOS_SyncWaitEvent_TimeoutMS_Exit(pRegion, pEvent, TimeoutMS, WaitEventCode)) \
                                        return false

#define oosmos_SyncExit(pRegion) \
                                      (pRegion)->pCurrent->SyncContext = -2; \
                                      return false

#define oosmos_SyncFinally(pRegion) \
                                    case -2: (pRegion)->pCurrent->SyncContext = -1

#define oosmos_SyncEnd(pRegion)     default: \
                                      break; \
                                  } do {} while(0)

typedef struct
{
  oosmos_sQueue * pNotifyQueue;
  oosmos_sEvent   Event;
} oosmos_sSubscriberList;

//--------
#define oosmos_SubscriberListInit(Subscriber) \
        OOSMOS_SubscriberListInit(Subscriber, sizeof(Subscriber)/sizeof(Subscriber[0]))
extern void OOSMOS_SubscriberListInit(oosmos_sSubscriberList * pSubscriberList, size_t ListElements);
//--------
#define oosmos_SubscriberListNotify(Subscriber) \
        OOSMOS_SubscriberListNotify(Subscriber, sizeof(Subscriber)/sizeof(Subscriber[0]))
extern bool OOSMOS_SubscriberListNotify(oosmos_sSubscriberList * pSubscriberList, size_t ListElements);
//--------
#define oosmos_SubscriberListNotifyWithArgs(Subscriber, Event) \
        OOSMOS_SubscriberListNotifyWithArgs(Subscriber, &Event, sizeof(Event), sizeof(Subscriber)/sizeof(Subscriber[0]))
extern bool OOSMOS_SubscriberListNotifyWithArgs(oosmos_sSubscriberList * pSubscriberList, void * pEvent, size_t EventSize, size_t ListElements);
//--------
#define oosmos_SubscriberListAdd(Subscriber, pNotifyQueue, EventCode, pContext) \
        OOSMOS_SubscriberListAdd(Subscriber, sizeof(Subscriber)/sizeof(Subscriber[0]), pNotifyQueue, EventCode, pContext)
extern void OOSMOS_SubscriberListAdd(oosmos_sSubscriberList * pSubscriberList, size_t ListElements, oosmos_sQueue * pNotifyQueue, int EventCode, void * pContext);

extern void oosmos_DelayUS(int US);
extern void oosmos_DelayMS(int MS);
extern void oosmos_DelaySeconds(int Seconds);

extern void oosmos_TimeoutInMS(oosmos_sTimeout * pTimeout, uint32_t Milliseconds);
extern void oosmos_TimeoutInUS(oosmos_sTimeout * pTimeout, uint32_t Microseconds);
extern void oosmos_TimeoutInSeconds(oosmos_sTimeout * pTimeout, uint32_t Seconds);
extern bool oosmos_TimeoutHasExpired(const oosmos_sTimeout * pTimeout);

//--------
extern void OOSMOS_QueueConstruct(oosmos_sQueue * pQueue, void * pQueueData, size_t QueueDataSize, size_t QueueElementSize);
#define oosmos_QueueConstruct(pQueue, pQueueData) \
  OOSMOS_QueueConstruct((pQueue), (pQueueData),sizeof(pQueueData),sizeof((pQueueData)[0]));

extern bool oosmos_QueuePush(oosmos_sQueue * pQueue, const void * pElement, size_t ElementSize);
extern bool oosmos_QueuePop(oosmos_sQueue * pQueue, void * pElement, size_t ElementSize);
extern void oosmos_QueueSetBehaviorFunc(oosmos_sQueue * pQueue, oosmos_eQueueFullBehavior (*pFunc)(void *), void * pContext);

typedef struct OOSMOS_sActiveObjectTag oosmos_sActiveObject;

struct OOSMOS_sActiveObjectTag
{
  void * pObject;
  void (*pFunction)(void * pObject);
  oosmos_sActiveObject * pNext;
};

extern void oosmos_RegisterActiveObject(void * pObject, void (*pFunction)(void *), oosmos_sActiveObject * pActiveObject);

extern oosmos_sState * OOSMOS_LCA(oosmos_sState * pSource, oosmos_sState * pTarget);
extern void OOSMOS_Exit(oosmos_sRegion * pRegion, oosmos_sState * pSource, const oosmos_sState * pLCA);
extern void OOSMOS_Enter(oosmos_sRegion * pRegion, const oosmos_sState * pLCA, oosmos_sState * pTarget);

extern float oosmos_AnalogMapAccurate(float Value, float InMin, float InMax, float OutMin, float OutMax);
extern long  oosmos_AnalogMapFast(long Value, long InMin, long InMax, long OutMin, long OutMax);

#define oosmos_Min(a,b) (((a)<(b))?(a):(b))
#define oosmos_Max(a,b) (((a)>(b))?(a):(b))

#ifdef __cplusplus
  }
#endif


#endif

