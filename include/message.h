#ifndef __MESSAGE__
#define __MESSAGE__

#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef uint8_t byte;
const uint32_t kilo = 1000;
const uint32_t mega = kilo * kilo;
const uint32_t giga = kilo * mega;
const uint32_t MESSAGE_BODY_SIZE = 10 * kilo;

/***********************************************************************/

#define HEADER_SIZE (sizeof (uint32_t) + sizeof (uint32_t) + sizeof (uint32_t) + sizeof (struct in_addr) + sizeof (struct in_addr))

/***********************************************************************/

/*
 * Message Class
 */
class message
{
	private:

		// Message sequence number
		uint32_t seqno;

		// Message ID
		uint32_t mID;

		// IP of sender
		struct in_addr sender;

		// IP of acknowledger
		struct in_addr acknowledger;

		// flags
		uint32_t flags;

	public:

		struct in_addr get_sender();
		struct in_addr get_acknowledger();
		void set_acknowledger(struct in_addr);
		void reset_acknowledger();
		void set_ID(uint32_t);
		uint32_t get_ID();
		void set_seqno(uint32_t);
		uint32_t get_seqno();
		void set_ACK();
		uint32_t get_flags();
		bool is_ACK();
		void print();
		byte * serialize_message ();

		message();
		message(byte *);
		message(uint32_t,struct in_addr);
		message(uint32_t, uint32_t, struct in_addr, struct in_addr, uint32_t);

		bool operator == (const message&) const;
};
#endif

/***********************************************************************/
