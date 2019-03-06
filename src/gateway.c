#include "gateway/modem.h"
#include "gateway/wireless.h"
#include "common/device.h"

/* size of the device id in bytes */
#define DEVICE_ID_SIZE 16u
#define LENGTH_FIELD_SIZE 1u

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
static modemPacket_t const inDataPacket;
static modemPacket_t const *pInDataPacket = &inDataPacket;
static modemPacket_t outDataPacket;
static modemPacket_t *pOutDataPacket = &outDataPacket;
static size_t packetSize;

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

/* function used to copy the packet headder in order to send a response */
static void gw_copy_buffer_headder(modemPacket_t * outputData, modemPacket_t const * inputData)
{	
	uint8_t index;

	for(index=0; index<DEVICE_ID_SIZE; index++)
	{
		outputData->deviceID.bytes[index] = inputData->deviceID.bytes[index];		
	}

	outputData->length = 2u;
	outputData->command = outputData->command;
	outputData->status = 0x00u;
}

/**
 * This function is polled by the main loop and should handle any packets coming
 * in over the modem or 868 MHz communication channel.
 */
void handle_communication(void)
{
	/* check if there is any packet coming from the modem */
	if(TRUE == modem_dequeue_incoming((uint8_t const **)&pInDataPacket, &packetSize))
	{
		/* check if the command is addressed to the gateway */
		if(TRUE == gw_own_id(pInDataPacket->deviceID))
		{
			/* process the command */
			switch(pInDataPacket->command)
			{
				case PING:	
					/* send a positive response back to the backend */
					gw_copy_buffer_headder(pOutDataPacket, pInDataPacket);
					pOutDataPacket->status = POSITIVE_RESPONSE;
					modem_enqueue_outgoing((uint8_t const *)pOutDataPacket, (pOutDataPacket->length + LENGTH_FIELD_SIZE + DEVICE_ID_SIZE));		
				break;

				case RESET:
					/* reset the gateway - no response needed */
					reset_device();
				break;

				default:
					/* command not supported - send a negative response back to the backend */
					gw_copy_buffer_headder(pOutDataPacket, pInDataPacket);
					pOutDataPacket->status = NEGATIVE_RESPONSE;
					modem_enqueue_outgoing((uint8_t const *)pOutDataPacket, (pOutDataPacket->length + LENGTH_FIELD_SIZE + DEVICE_ID_SIZE));	
				break;
			}
		}
		else
		{
			/* relay the command to the addressed device */
			/* make sure the size does not exceed the wireless limits */
			if(pInDataPacket->length <= WIRELESS_PAYLOAD_LENGTH)
			{
				/* forward the command */
				wireless_enqueue_outgoing(pInDataPacket->deviceID, &(pInDataPacket->length));
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
	if(TRUE == wireless_dequeue_incoming(&(pOutDataPacket->deviceID),&(pOutDataPacket->length)) )
	{
		/* relay the message to the backend */
		modem_enqueue_outgoing((uint8_t const *)pOutDataPacket, (pOutDataPacket->length + LENGTH_FIELD_SIZE + DEVICE_ID_SIZE));	
	}
	else
	{
		/* do nohing - no request from the sensor */
	}
}
