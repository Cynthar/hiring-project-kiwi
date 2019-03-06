#include "gateway/modem.h"
#include "gateway/wireless.h"
#include "common/device.h"

/* size of the device id in bytes */
#define DEVICE_ID_SIZE 16u

/* response codes to be sent back to the backend */
#define POSITIVE_RESPONSE 0x55u
#define NEGATIVE_RESPONSE 0xAAu

#define TRUE 0x01U
#define FALSE 0x00U

/* structure used to define the layout of the package sent or received from the backend */
typedef struct 
{
	device_id_t deviceID; /* the ID of the device that is being addressd - if it's all "0" the command is addressed to the gateway */
	uint8_t length;       /* the length of the useful part of the rest of the package */
	uint8_t command;      /* the command sent by the backend */
	uint8_t status;       /* status of the command - should be "0x00" when it's received and it will be updated for the response */
	uint8_t data[109];	  /* data sent with the command */
} modemPacket_t;

/* enum used to track the commands available in the gateway */
typedef enum
{
	PING = 0x00U,
	RESET = 0x01U
} gatewayCommands_t;

/* used to handle the data sent or received */
modemPacket_t dataPacket;
modemPacket_t *pDataPacket = &dataPacket;
size_t packetSize;

/* function returns true if the the id is "0" */
static bool gw_own_id(device_id_t id)
{
	uint8_t index;
	bool return_value = TRUE;

	for(index=0; index<DEVICE_ID_SIZE; index++)
	{
		if(id.bytes[index] != 0u)
		{	
			/* if any byte in the id field is different than 0 it means 
			 * a sensor is addressed or the packet is junk 
			 */
			return_value = FALSE;
		}
		else
		{
			/* do nothing */
		}
	}

	return return_value;
}

/**
 * This function is polled by the main loop and should handle any packets coming
 * in over the modem or 868 MHz communication channel.
 */
void handle_communication(void)
{
	/* check if there is any packet coming from the modem */
	if(TRUE == modem_dequeue_incoming((uint8_t const **)&pDataPacket, &packetSize))
	{
		/* check if the command is addressed to the gateway */
		if(TRUE == gw_own_id(pDataPacket->deviceID))
		{
			/* process the command */
			switch(pDataPacket->command)
			{
				case PING:	
					/* send a positive response back to the backend */
					pDataPacket->status = POSITIVE_RESPONSE;
					modem_enqueue_outgoing((uint8_t const *)pDataPacket, pDataPacket->length);		
				break;

				case RESET:
					/* reset the gateway - no response needed */
					reset_device();
				break;

				default:
				/* do nothing - command not supported */
				break;
			}
		}
		{
			/* relay the command to the addressed device */
			/* make sure the size does not exceed the wireless limits */
			if(pDataPacket->length <= WIRELESS_PAYLOAD_LENGTH)
			{
				/* forward the command */
				wireless_enqueue_outgoing(pDataPacket->deviceID, &(pDataPacket->length));
			}
			else
			{
				/* do nothing - request is invalid */
			}
		}
	}
	else
	{
		/* do nohing - no request from the backend */
	}

	/* check if there is any packet coming from the sensor */
	if(TRUE == wireless_dequeue_incoming(&(pDataPacket->deviceID),&(pDataPacket->length)) )
	{
		/* relay the message to the backend */
		modem_enqueue_outgoing((uint8_t const *)pDataPacket, pDataPacket->length);	
	}
	else
	{
		/* do nohing - no request from the sensor */
	}
}
