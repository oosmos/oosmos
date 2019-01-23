//
// OOSMOS matrix Class
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef matrixMAX
#define matrixMAX 1
#endif

#ifndef matrixMAX_ROWS
#define matrixMAX_ROWS 8
#endif

#ifndef matrixMAX_COLS
#define matrixMAX_COLS 8
#endif

static const int RowOnSettleTimeUS  = 200;
static const int RowOffSettleTimeUS = 200;

#include "oosmos.h"
#include "matrix.h"
#include "sw.h"
#include "pin.h"
#include <stdarg.h>

struct matrixTag
{
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf RowTurningOn_State;
    oosmos_sLeaf RowTurningOff_State;

  pin * m_pRowPins[matrixMAX_ROWS];
  pin * m_pColumnPins[matrixMAX_COLS];
  sw  * m_pSwitch[matrixMAX_ROWS][matrixMAX_COLS];

  int m_CurrentRowIndex;
  int m_Rows;
  int m_Columns;
};

static void InterrogateColumns(const matrix * pMatrix)
{
  const int RowIndex = pMatrix->m_CurrentRowIndex;
  const int Columns  = pMatrix->m_Columns;

  int ColumnIndex;

  for (ColumnIndex = 0; ColumnIndex < Columns; ColumnIndex++) {
    sw * pSwitch = pMatrix->m_pSwitch[RowIndex][ColumnIndex];

    if (pSwitch == NULL) {
      continue;
    }

    swRunStateMachine(pSwitch);
  }
}

static void Advance(matrix * pMatrix)
{
  const int RowIndex = pMatrix->m_CurrentRowIndex;

  if (RowIndex < (pMatrix->m_Rows)-1) {
    pMatrix->m_CurrentRowIndex = RowIndex+1;
  }
  else {
    pMatrix->m_CurrentRowIndex = 0;
  }
}

static void AddRow(matrix * pMatrix, const int Row, pin * pPin)
{
  const int RowIndex = Row - 1;

  pMatrix->m_pRowPins[RowIndex] = pPin;

  if (Row > pMatrix->m_Rows) {
    pMatrix->m_Rows = Row;
  }
}

static void AddColumn(matrix * pMatrix, const int Column, pin * pPin)
{
  const int ColumnIndex = Column - 1;
  pMatrix->m_pColumnPins[ColumnIndex] = pPin;

  if (Column > pMatrix->m_Columns) {
    pMatrix->m_Columns = Column;
  }
}

static void Action1(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  matrix * pMatrix = (matrix *) pObject;

  InterrogateColumns(pMatrix);

  oosmos_UNUSED(pState);
  oosmos_UNUSED(pEvent);
}

static bool RowTurningOn_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  matrix * pMatrix = (matrix *) pObject;

  switch (pEvent->m_Code) {
    case oosmos_DEFAULT: {
      pMatrix->m_CurrentRowIndex = 0;
      return true;
    }
    case oosmos_ENTER: {
      const int RowIndex = pMatrix->m_CurrentRowIndex;
      pin * pOutputPin   = pMatrix->m_pRowPins[RowIndex];

      if (pOutputPin != NULL) {
        pinOn(pOutputPin);
      }

      return oosmos_StateTimeoutUS(pState, RowOnSettleTimeUS);
    }
    case oosmos_TIMEOUT: {
      return oosmos_TransitionAction(pMatrix, pState, RowTurningOff_State, pEvent, Action1);
    }
  }

  return false;
}

static void Action2(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  matrix * pMatrix = (matrix *) pObject;

  Advance(pMatrix);

  oosmos_UNUSED(pState);
  oosmos_UNUSED(pEvent);
}

static bool RowTurningOff_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  matrix * pMatrix = (matrix *) pObject;

  switch (pEvent->m_Code) {
    case oosmos_ENTER: {
      const int RowIndex = pMatrix->m_CurrentRowIndex;
      pin * pOutputPin   = pMatrix->m_pRowPins[RowIndex];

      if (pOutputPin != NULL) {
        pinOff(pOutputPin);
      }

      return oosmos_StateTimeoutUS(pState, RowOffSettleTimeUS);
    }
    case oosmos_TIMEOUT: {
      return oosmos_TransitionAction(pMatrix, pState, RowTurningOn_State, pEvent, Action2);
    }
  }

  return false;
}

extern matrix * matrixNew(int Rows, int Columns, ...)
{
  oosmos_Allocate(pMatrix, matrix, matrixMAX, NULL);

  pMatrix->m_Rows    = 0;
  pMatrix->m_Columns = 0;

  int RowIndex;

  for (RowIndex = 0; RowIndex < matrixMAX_ROWS; RowIndex++) {
    pMatrix->m_pRowPins[RowIndex] = NULL;

    int ColumnIndex;

    for (ColumnIndex = 0; ColumnIndex < matrixMAX_COLS; ColumnIndex++) {
      pMatrix->m_pColumnPins[ColumnIndex]       = NULL;
      pMatrix->m_pSwitch[RowIndex][ColumnIndex] = NULL;
    }
  }

  oosmos_StateMachineInitNoQueue(pMatrix, ROOT, NULL, RowTurningOn_State);
    oosmos_LeafInit(pMatrix, RowTurningOn_State,  ROOT, RowTurningOn_State_Code);
    oosmos_LeafInit(pMatrix, RowTurningOff_State, ROOT, RowTurningOff_State_Code);

  va_list ArgList;
  va_start(ArgList, Columns);

  int Row;

  for (Row = 1; Row <= Rows; Row += 1) {
    AddRow(pMatrix, Row, va_arg(ArgList, pin *));
  }

  int Column;

  for (Column = 1; Column <= Columns; Column += 1) {
    AddColumn(pMatrix, Column, va_arg(ArgList, pin *));
  }

  va_end(ArgList);

  oosmos_UNUSED(ArgList);

  return pMatrix;
}

extern void matrixAssignSwitch(matrix * pMatrix, sw * pSwitch, int Row, int Column)
{
  const int RowIndex    = Row - 1;
  const int ColumnIndex = Column - 1;

  //
  // Check if this Row/Column slot has already been assigned.
  //
  if (pMatrix->m_pSwitch[RowIndex][ColumnIndex] != NULL) {
    oosmos_FOREVER();
  }

  pMatrix->m_pSwitch[RowIndex][ColumnIndex] = pSwitch;
}

extern pin * matrixGetColumnPin(const matrix * pMatrix, int Column)
{
  const int ColumnIndex = Column - 1;
  return pMatrix->m_pColumnPins[ColumnIndex];
}
