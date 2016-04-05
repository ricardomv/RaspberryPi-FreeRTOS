/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

/* Holds the handle of the task used as a deferred interrupt processor.  The
handle is used so direct notifications can be sent to the task for all EMAC/DMA
related interrupts. */
xTaskHandle xEMACTaskHandle = NULL;

static void prvEMACHandlerTask( void *pvParameters ){
const portTickType ulMaxBlockTime = pdMS_TO_TICKS( 100 );

	( void ) pvParameters;

	configASSERT( xEMACTaskHandle );

	for( ;; )
	{
		/* Wait for the EMAC interrupt to indicate that another packet has been
		received. */
		//ulTaskNotifyTake( pdFALSE, ulMaxBlockTime );
		//prvEMACRxPoll();
	}
}

portBASE_TYPE xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor, portBASE_TYPE bReleaseAfterSend ){
	int usPHYLinkStatus = 42;
	if( ( usPHYLinkStatus & 4 ) != 0 ){
		/* Not interested in a call-back after TX. */
		//gmac_dev_write( &gs_gmac_dev, (void *)pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength, NULL );
	}

	if( bReleaseAfterSend != pdFALSE ){
		vReleaseNetworkBufferAndDescriptor( pxDescriptor );
	}
	return 1;
}
