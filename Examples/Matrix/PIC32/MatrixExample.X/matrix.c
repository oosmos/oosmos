/*
// OOSMOS matrix Class
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

#ifndef matrixMAX
#define matrixMAX 1
#endif

#ifndef matrixMAX_ROWS
#define matrixMAX_ROWS 8
#endif

#ifndef matrixMAX_COLS
#define matrixMAX_COLS 8
#endif

const static int RowOnSettleTimeUS  = 50;
const static int RowOffSettleTimeUS = 50;

#include "oosmos.h"
#include "matrix.h"
#include "sw.h"
#include "pin.h"
#include <stdarg.h>

struct matrixTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              Running_State;

  pin * m_pRowPins[matrixMAX_ROWS];
  pin * m_pColumnPins[matrixMAX_COLS];
  sw  * m_pSwitch[matrixMAX_ROWS][matrixMAX_COLS];
  
  int m_Rows;
  int m_Columns;

  /*
  // State variables...
  */
  int m_CurrentRowIndex;
};

static void InterrogateColumns(matrix * pMatrix)
{
  const int RowIndex = pMatrix->m_CurrentRowIndex;
  const int Columns  = pMatrix->m_Columns;

  int ColumnIndex;

  for (ColumnIndex = 0; ColumnIndex < Columns; ColumnIndex++) {
    sw * pSwitch = pMatrix->m_pSwitch[RowIndex][ColumnIndex];

    if (pSwitch != NULL) {
      swRunStateMachine(pSwitch);
    }
  }
}

static void AddRow(matrix * pMatrix, const int Row, pin * pPin)
{
  const int RowIndex = Row - 1;

  pMatrix->m_pRowPins[RowIndex] = pPin;

  if (Row > pMatrix->m_Rows)
    pMatrix->m_Rows = Row;
}

static void AddColumn(matrix * pMatrix, const int Column, pin * pPin)
{
  const int ColumnIndex = Column - 1;
  pMatrix->m_pColumnPins[ColumnIndex] = pPin;

  if (Column > pMatrix->m_Columns)
    pMatrix->m_Columns = Column;
}

static bool Running_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  matrix * pMatrix = (matrix *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        while (true) {
          for (pMatrix->m_CurrentRowIndex = 0; pMatrix->m_CurrentRowIndex < pMatrix->m_Rows; pMatrix->m_CurrentRowIndex++) {
            if (pMatrix->m_pRowPins[pMatrix->m_CurrentRowIndex] == NULL)
              continue;

            pinOn(pMatrix->m_pRowPins[pMatrix->m_CurrentRowIndex]);
            oosmos_SyncDelayMS(pRegion, RowOnSettleTimeUS);

            InterrogateColumns(pMatrix);

            pinOff(pMatrix->m_pRowPins[pMatrix->m_CurrentRowIndex]);
            oosmos_SyncDelayMS(pRegion, RowOffSettleTimeUS);
          }
        }
      oosmos_SyncEnd(pRegion);
      return true;
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

  /*                                      StateName      Parent        Default        */
  /*                            ===================================================== */
  oosmos_StateMachineInitNoQueue(pMatrix, StateMachine,  NULL,         Running_State);
    oosmos_LeafInit             (pMatrix, Running_State, StateMachine               );
    
  va_list ArgList;
  va_start(ArgList, Columns);

  int Row;
  
  for (Row = 1; Row <= Rows; Row += 1)
    AddRow(pMatrix, Row, va_arg(ArgList, pin *));

  int Column;
  
  for (Column = 1; Column <= Columns; Column += 1)
    AddColumn(pMatrix, Column, va_arg(ArgList, pin *));

  va_end(ArgList);
  
  return pMatrix;
}

extern void matrixAssignSwitch(matrix * pMatrix, sw * pSwitch, const int Row, const int Column)
{
  const int RowIndex    = Row - 1;
  const int ColumnIndex = Column - 1;

  /*
  // Check if this Row/Column slot has already been assigned.
  */
  if (pMatrix->m_pSwitch[RowIndex][ColumnIndex] != NULL)
    while (true);

  pMatrix->m_pSwitch[RowIndex][ColumnIndex] = pSwitch;
}

extern pin * matrixGetColumnPin(const matrix * pMatrix, const int Column)
{
  const int ColumnIndex = Column - 1;
  return pMatrix->m_pColumnPins[ColumnIndex];
}
