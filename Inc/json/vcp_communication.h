/*
 * vcp_communication.h
 *
 *  Created on: 27 juin 2018
 *      Author: coren
 */

#ifndef VCP_COMMUNICATION_H_
#define VCP_COMMUNICATION_H_

#include <json/json_decoder.h>
#include <stdint.h>

/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048

//max 10 commands can be received and saved without overwriting. Each command has a max size of APP_RX_DATA_SIZE
#define  MaxCommandsInBuffer 10

void VCP_SendJsonObjectReceivedValues(JsonObject_t *object);

void VCP_SendString(char* string);
int8_t VCP_retrieveInputData(uint8_t* Buf, uint32_t *Len);
int8_t VCP_Receive_FS(uint8_t* Buf, uint32_t *Len);
void VCP_SendBuffer(uint8_t* Buf, uint16_t Len);
void VCP_SendString(char* string);

#endif /* VCP_COMMUNICATION_H_ */
