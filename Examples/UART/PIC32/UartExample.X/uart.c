/*
// OOSMOS uart Class
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
#include "uart.h"

typedef enum
{
  sendAwaitingCharToSend_State,
  sendAwaitingReady_State,
  sendAwaitingComplete_State,
} eSendStates;

struct uartTag
{  
  oosmos_sActiveObject   m_ActiveObject;

  oosmos_sQueue          m_SendDataQueue;
  uint8_t                m_SendDataQueueData[200];
  eSendStates            m_SendState;
  uint8_t                m_CharToSend;

  oosmos_sQueue          m_ReceiveDataQueue;
  uint8_t                m_ReceiveDataQueueData[10];
  oosmos_sSubscriberList m_ReceivedByteEventSubscribers[1];
  
  uint8_t  m_PlibUartID;
  uint8_t  m_UartModule;
};

#if defined _UART6
  #define MaxUARTS 6
#elif defined _UART5
  #define MaxUARTS 5
#elif defined _UART4
  #define MaxUARTS 4
#elif defined _UART3
  #define MaxUARTS 3
#elif defined _UART2
  #define MaxUARTS 2
#elif defined  _UART1
  #define MaxUARTS 1
#endif

static uart UartList[MaxUARTS];
static int UartCount;

static const int UartIndex_to_PlibUartId[] =
{
  #ifdef _UART1
    UART1,
  #endif
  #ifdef _UART2
    UART2,
  #endif
  #ifdef _UART3
    UART3,
  #endif
  #ifdef _UART4
    UART4,
  #endif
  #ifdef _UART5
    UART5,
  #endif
  #ifdef _UART6
    UART6,
  #endif
};

static void EnableInterrupt(uart * pUART)
{
  INTEnable(INT_SOURCE_UART_RX(pUART->m_PlibUartID), INT_ENABLED);
}

static void DisableInterrupt(uart * pUART)
{
  INTEnable(INT_SOURCE_UART_RX(pUART->m_PlibUartID), INT_DISABLED);
}

static uint16_t GetPriorityBits(const int PriorityNumber)
{
  switch (PriorityNumber) {
    case 1: return INT_PRIORITY_LEVEL_1;
    case 2: return INT_PRIORITY_LEVEL_2;
    case 3: return INT_PRIORITY_LEVEL_3;
    case 4: return INT_PRIORITY_LEVEL_4;
    case 5: return INT_PRIORITY_LEVEL_5;
    case 6: return INT_PRIORITY_LEVEL_6;
    case 7: return INT_PRIORITY_LEVEL_7;
    default: while(true);
  }
}

static void RunReceiverStateMachine(void * pObject)
{
  uart * pUART = (uart *) pObject;
  uint8_t Byte;

  DisableInterrupt(pUART);
    const bool PopSuccess = oosmos_QueuePop(&pUART->m_ReceiveDataQueue, &Byte, sizeof(Byte));
  EnableInterrupt(pUART);

  if (PopSuccess) {
    uart_sReceivedByteEvent ReceivedByteEvent = { { 0, NULL }, Byte };
    oosmos_SubscriberListNotifyWithArgs(pUART->m_ReceivedByteEventSubscribers, ReceivedByteEvent);
  }
}

static void RunSenderStateMachine(void * pObject)
{
  uart * pUART = (uart *) pObject;
  
  switch (pUART->m_SendState) {
    case sendAwaitingCharToSend_State: {
      const bool PopSuccess = oosmos_QueuePop(&pUART->m_SendDataQueue, &pUART->m_CharToSend, sizeof(pUART->m_CharToSend));
      
      if (PopSuccess)
        pUART->m_SendState = sendAwaitingReady_State;

      break;
    }
    case sendAwaitingReady_State:
      if (UARTTransmitterIsReady(pUART->m_PlibUartID)) {
        UARTSendDataByte(pUART->m_PlibUartID, pUART->m_CharToSend);
        pUART->m_SendState = sendAwaitingComplete_State;
      }

      break;
    case sendAwaitingComplete_State:
      if (UARTTransmissionHasCompleted(pUART->m_PlibUartID))
        pUART->m_SendState = sendAwaitingCharToSend_State;
      
      break;
  }
}

static void RunStateMachine(void * pObject)
{
  RunSenderStateMachine(pObject);
  RunReceiverStateMachine(pObject);
}

static void ISR(const int UartModule)
{
  uart * pUART = UartList;
  int Count;

  for (Count = 1; Count <= UartCount; pUART++, Count++) {
    if (UartModule <= pUART->m_UartModule)
      break;
  }

  const int PlibUartId = pUART->m_PlibUartID;

  if (INTGetFlag(INT_SOURCE_UART_RX(PlibUartId))) {
    while (UARTReceivedDataIsAvailable(PlibUartId)) {
      const uint8_t Byte = UARTGetDataByte(PlibUartId);
      oosmos_QueuePush(&pUART->m_ReceiveDataQueue, &Byte, sizeof(Byte));
    }

    INTClearFlag(INT_SOURCE_UART_RX(PlibUartId));
  }
}

static int GetPeripheralClock(void)
{
  return (oosmos_GetClockSpeedInMHz()*1000000) / (1 << OSCCONbits.PBDIV);
}

extern void uartPrintf(uart * pUART, const char * pFormat, ...)
{
  char Buffer[100];
  va_list ArgList;

  va_start(ArgList, pFormat);
  vsprintf(Buffer, pFormat, ArgList);
  uartSendString(pUART, Buffer);
}

extern void uartSendChar(uart * pUART, const char Char)
{
  oosmos_QueuePush(&pUART->m_SendDataQueue, &Char, sizeof(Char));
}

extern void uartSendString(uart * pUART, const char * pString)
{
  uint8_t Char;

  while (Char = *pString++, Char)
    uartSendChar(pUART, Char);
}

extern void uartStart(uart * pUART)
{
  const int PlibUartId = pUART->m_PlibUartID;
    
  UARTEnable(PlibUartId, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
  EnableInterrupt(pUART);
}

/*
// 'UartModule' is 1, 2, etc.
*/
extern uart * uartNew(const int UartModule, const int BaudRate)
{
  oosmos_AllocateVisible(pUART, uart, UartList, UartCount, MaxUARTS, NULL);

  pUART->m_UartModule = UartModule;
 
  const int PlibUartID = UartIndex_to_PlibUartId[UartModule-1];
  pUART->m_PlibUartID = PlibUartID;

  UARTConfigure(PlibUartID, UART_ENABLE_PINS_TX_RX_ONLY);
  UARTSetFifoMode(PlibUartID, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
  UARTSetLineControl(PlibUartID, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
  UARTSetDataRate(PlibUartID, GetPeripheralClock(), BaudRate);

  const int Priority = 1;
  INTSetVectorPriority(INT_VECTOR_UART(PlibUartID), GetPriorityBits(Priority));
  INTSetVectorSubPriority(INT_VECTOR_UART(PlibUartID), INT_SUB_PRIORITY_LEVEL_0);

  oosmos_QueueConstruct(&pUART->m_SendDataQueue, pUART->m_SendDataQueueData);

  oosmos_QueueConstruct(&pUART->m_ReceiveDataQueue, pUART->m_ReceiveDataQueueData);
  oosmos_SubscriberListInit(pUART->m_ReceivedByteEventSubscribers);

  oosmos_RegisterActiveObject(pUART, RunStateMachine, &pUART->m_ActiveObject);

  return pUART;
}

extern void uartSubscribe(uart * pUART, oosmos_sQueue * pQueue, const int EventCode, void * pContext)
{
  oosmos_SubscriberListAdd(pUART->m_ReceivedByteEventSubscribers, pQueue, EventCode, pContext);
}

#define uartVector(N) void __ISR(_UART_##N##_VECTOR) IntUart##N##Handler() { ISR(N); }

#ifdef _UART1
uartVector(1)
#endif

#ifdef _UART2
uartVector(2)
#endif

#ifdef _UART3
uartVector(3)
#endif

#ifdef _UART4
uartVector(4)
#endif

#ifdef _UART5
uartVector(5)
#endif

#ifdef _UART6
uartVector(6)
#endif
