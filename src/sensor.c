#include "sensor/wireless.h"
#include "sensor/ki_store.h"
#include "sensor/door.h"
#include "common/device.h"

/* response codes to be sent back to the backend */
#define POSITIVE_RESPONSE 0x55u
#define NEGATIVE_RESPONSE 0xAAu

#define TRUE 0x01U

/* structure used to define the layout of the package sent or received from the sensor */
typedef struct 
{
	uint8_t length;       /* the length of the useful part of the rest of the package */
	uint8_t command;      /* the command sent by the backend */
	uint8_t status;       /* status of the command - should be "0x00" when it's received and it will be updated for the response */
	uint8_t data[29];	  /* data sent with the command */
} sensorPacket_t;

/* enum used to track the commands available in the sensor */
typedef enum
{
	PING = 0x00U,
	RESET = 0x01U,
	ADD_KI = 0x02U,
	REMOVE_KI = 0x03U,
	OPEN_DOOR = 0x04
} sensorCommands_t;

/* used to handle the data sent or received */
static sensorPacket_t dataPacket;

/**
 * This function is polled by the main loop and should handle any packets coming
 * in over the 868 MHz communication channel.
 */
void handle_communication(void)
{
	ki_store_result_t kiManageResult;

	/* check if there is any packet coming from the gateway */
	if(TRUE == wireless_dequeue_incoming(&(dataPacket.length)) )
	{
		/* process the command */
		switch(dataPacket.command)
		{
			case PING:	
				/* send a positive response back to the gateway */
				dataPacket.status = POSITIVE_RESPONSE;
				wireless_enqueue_outgoing((uint8_t const *)(&dataPacket));		
			break;

			case RESET:
				/* reset the gateway - no response needed */
				reset_device();
			break;

			case ADD_KI:
				/* try to store the new ki */
				kiManageResult = ki_store_add((uint8_t const *)(dataPacket.data));

				/* check the result */
				if(KI_STORE_SUCCESS == kiManageResult)
				{
					dataPacket.status = POSITIVE_RESPONSE;
				}
				else
				{
					dataPacket.status = NEGATIVE_RESPONSE;
				}

				/* send the response back to the gateway */
				wireless_enqueue_outgoing((uint8_t const *)(&dataPacket));	
			break;

			case REMOVE_KI:
				/* try to store the new ki */
				kiManageResult = ki_store_remove((uint8_t const *)(dataPacket.data));

				/* check the result */
				if(KI_STORE_SUCCESS == kiManageResult)
				{
					dataPacket.status = POSITIVE_RESPONSE;
				}
				else
				{
					dataPacket.status = NEGATIVE_RESPONSE;
				}

				/* send the response back to the gateway */
				wireless_enqueue_outgoing((uint8_t const *)(&dataPacket));	
			break;

			case OPEN_DOOR:
				/* open the door */
				door_trigger();

				/* send a positive response back to the gateway */
				dataPacket.status = POSITIVE_RESPONSE;
				wireless_enqueue_outgoing((uint8_t const *)(&dataPacket));	
			break;

			default:
				/* command not supported - send a negative response back to the gateway */
				dataPacket.status = NEGATIVE_RESPONSE;
				wireless_enqueue_outgoing((uint8_t const *)(&dataPacket));	
			break;
		}
	}
	else
	{
		/* do nohing - no request from the backend */
	}
}
