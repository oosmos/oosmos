#include "oosmos.h"
#include <stdbool.h>

typedef struct {
  uint32_t Data;
} sElement;

oosmos_sQueue Queue;
sElement QueueData[10];

int main()
{
  oosmos_QueueConstruct(&Queue, &QueueData, sizeof(QueueData), sizeof(sElement));

  sElement Element = { 99 };
  oosmos_QueuePush(&Queue, &Element, sizeof(Element));

  sElement PoppedElement;
  bool success = oosmos_QueuePop(&Queue, &PoppedElement, sizeof(sElement));

  if (success)
    printf("%d", PoppedElement.Data);
  else
    printf("Failed to pop data from queue");
	
  return 0;
}
