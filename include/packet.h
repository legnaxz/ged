/*
 * packet.h
 *
 *  Created on: 2018. 12. 4.
 *      Author: ukju
 */

#ifndef INCLUDE_PACKET_H_
#define INCLUDE_PACKET_H_

typedef struct PACKET_COMMON_HEADER
{
	char interface_id[4];
	char body_size[5];
} PACKET_COMMON_HEADER;

typedef struct PACKET_HEALTH_CHECK
{
	PACKET_COMMON_HEADER common_header;
	char timestamp[17];
	char health_check[1];
	char resultcode[1];
} PACKET_HEALTH_CHECK;


#endif /* INCLUDE_PACKET_H_ */
