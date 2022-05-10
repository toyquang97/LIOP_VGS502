#ifndef _CAN_H_
#define	_CAN_H_

#include "AllHeader.h"
/************************************************************************************************/
/* Definitions for configuration register CANCON												*/
/************************************************************************************************/
#define CAN_MODE_BITS		0xE0		   				/* Mode bits in configuration register	*/
#define CAN_MODE_CONFIG		0x80		   				/* Configuration mode					*/
#define CAN_MODE_LISTEN		0x60		   				/* Listen Only mode						*/
#define CAN_MODE_NORMAL1		0x00		   				/* Normal mode							*/

/************************************************************************************************/
/* Definitions for interrupt code in CANSTAT register											*/
/************************************************************************************************/
#define INTCODE				0x0E		   				/* Mask for interrupt code bits			*/
#define WAKEINT				0x0E		   				/* Wake up interrupt					*/
#define RXB0INT				0x0C		   				/* RX buffer 0 interrupt				*/
#define RXB1INT				0x0A		   				/* RX buffer 1 interrupt				*/
#define TXB0INT				0x08		   				/* TX buffer 0 interrupt				*/
#define TXB1INT				0x06		   				/* TX buffer 1 interrupt				*/
#define TXB2INT				0x04		   				/* TX buffer 2 interrupt				*/
#define ERRORINT			0x02		   				/* error interrupt						*/
#define NOINT				0x00		   				/* kein interrupt						*/

/************************************************************************************************/
/* Definitions for Win bits in CANCON register													*/
/************************************************************************************************/
#define TXB0REG				0x08		   				/* TX buffer 0 register access			*/
#define TXB1REG				0x06		   				/* TX buffer 1 register access			*/

#define	BPS_125K			0
#define	BPS_50K				1
#define	BPS_25K				2

#define	C_BAUDRATE_LISTEN_10S	780 * 4
#define	C_MINIMAM_TIMES			4

#define RX_SIZE						30
#define TX_SIZE						30

#ifdef _CAN_C_

uint8_t volatile rc;			// RX message counter
uint8_t volatile tc;			// TX message counter
uint8_t volatile ro;			// pointer for FIFO read
uint8_t volatile ri;			// pointer for FIFO write
uint8_t volatile to;			// pointer for TX FIFO read
uint8_t volatile ti;			// pointer for TX FIFO write
uint8_t volatile tr;			// Senden fertig

uint8_t volatile rx[RX_SIZE][10];	// RX message buffer
uint8_t volatile tx[TX_SIZE][10];	// TX message buffer






void Init_Can (void);
void abort_sdo(uint32_t errorcode);
void sdo_response(uint8_t command, uint16_t index, uint8_t subindex, uint32_t value);
void sdo_segment(uint8_t command, uint8_t size, uint8_t *value);
void lss_response(uint8_t command, uint8_t value);
void transmit_error(void);
void transmit_in(uint8_t *input);
void transmit_load (void);
uint8_t check_for_call (uint8_t value);
void can_transmit (void);
void sent_heartbeat(void);
void read_rx(void);
void set_output (uint8_t *virt);
void set_io_config (void);

#else
extern uint8_t volatile rc;
extern uint8_t volatile tc;
extern uint8_t volatile ro;
extern uint8_t volatile ri;
extern uint8_t volatile to;
extern uint8_t volatile ti;
extern uint8_t volatile tr;
extern uint8_t volatile rx[RX_SIZE][10];
extern uint8_t volatile tx[TX_SIZE][10];


extern void Init_Can (void);
extern void abort_sdo(uint32_t errorcode);
extern void sdo_response(uint8_t command, uint16_t index, uint8_t subindex, uint32_t value);
extern void sdo_segment(uint8_t command, uint8_t size, uint8_t *value);
extern void lss_response(uint8_t command, uint8_t value);
extern void transmit_error(void);
extern void transmit_in(uint8_t *input);
extern void transmit_load (void);
extern uint8_t check_for_call (uint8_t value);
extern void can_transmit (void);
extern void sent_heartbeat(void);
extern void read_rx(void);
extern void set_output (uint8_t *virt);
extern void set_io_config (void);


#endif
#endif

