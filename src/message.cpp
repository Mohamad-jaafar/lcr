#include "message.h"

/***********************************************************************/
/*
 * Constructor used for initializing complete messages
 * 
 */

message::message(){

	mID = 0;
	seqno = 0;
	inet_aton("1.1.1.1",&(sender));
	inet_aton("1.1.1.1",&(acknowledger));
	flags = 0;
}

/***********************************************************************/
/*
 * Constructor used for de-serializing messages
 * 
 */

message::message(byte * header){

	memcpy (&seqno, header, sizeof (uint32_t));
	memcpy (&mID, header + sizeof (uint32_t), sizeof (uint32_t));
	memcpy (&flags, header + sizeof (uint32_t) + sizeof (uint32_t), sizeof (uint32_t));
	memcpy (&sender, header + sizeof (uint32_t) + sizeof (uint32_t) + sizeof (uint32_t), sizeof (struct in_addr));
	memcpy (&acknowledger, header + sizeof (uint32_t) + sizeof (uint32_t) + sizeof (uint32_t) + sizeof (struct in_addr), sizeof (struct in_addr));
}

/***********************************************************************/
/*
 * Another constructor used for initializing complete messages
 * 
 */

message::message(uint32_t seq, uint32_t ID, struct in_addr sen, struct in_addr ack, uint32_t fl) : seqno(seq), mID (ID), sender(sen), acknowledger(ack), flags (fl){}

/***********************************************************************/
/*
 * Another constructor used for initializing complete messages
 * 
 */

message::message(uint32_t ID,struct in_addr IP){

	mID = ID;
	seqno = 0;
	sender = IP;
	inet_aton("1.1.1.1",&(acknowledger));
	flags = 0;
}

/***********************************************************************/
/*
 * Equality operator
 * 
 */

bool message::operator == (const message& other) const {

	return (mID == other.mID && sender.s_addr == other.sender.s_addr);
}

/***********************************************************************/

/*
* is_ACK
*
* ACK accessor
*/

bool message::is_ACK(){

	//return (flags %2 == 1);
	return (flags == 1);
}

/***********************************************************************/

/*
* get_sender
*
* Sender IP accessor
*/

struct in_addr message::get_sender(){

	return sender;
}

/***********************************************************************/

/*
* get_acknowledger
*
* Acknowledger IP accessor
*/

struct in_addr message::get_acknowledger(){

	return acknowledger;
}

/***********************************************************************/

/*
* set_acknowledger
*
* Acknowledger IP mutator
*/
void message::set_acknowledger(struct in_addr ip){

	acknowledger = ip;
}

/***********************************************************************/

/*
* set_ID
*
* ID mutator
*/
void message::set_ID(uint32_t ID) {

	mID = ID;
}

/***********************************************************************/

/*
* get_ID
*
* ID Accessor
*/
uint32_t message::get_ID() {

	return mID;
}

/***********************************************************************/

/*
* set_seqno
*
* Sequence number mutator
*/
void message::set_seqno(uint32_t seq) {

	seqno = seq;
}

/***********************************************************************/

/*
* get_seqno
*
* Sequence number accessor
*/
uint32_t message::get_seqno() {

	return seqno;
}

/***********************************************************************/

/*
* get_flags
*
* Flags accessor
*/
uint32_t message::get_flags() {

	return flags;
}

/***********************************************************************/

/*
* set_ACK
*
* ACK mutator
*/
void message::set_ACK() {

	if (flags %2 == 0)
		flags = 1;
		//flags ++;
}

/***********************************************************************/

/*
 * print
 *
 * Prints message Data
 */
void message::print (){

	std::cout << std::to_string (seqno) + std::string ("\t") + std::to_string (mID) + std::string (",") + std::string (inet_ntoa (sender));

	if (is_ACK()){
		std::cout << std::string (",ACK,");
		std::cout << std::string (inet_ntoa (acknowledger));
	}

	std::cout << std::endl;
}

/***********************************************************************/

byte * message::serialize_message (){

	byte * header = (byte *) malloc (HEADER_SIZE);
	memcpy (header, &seqno, sizeof (uint32_t));
	memcpy (header + sizeof (uint32_t), &mID, sizeof (uint32_t));
	memcpy (header + sizeof (uint32_t) + sizeof (uint32_t), &flags, sizeof (uint32_t));
	memcpy (header + sizeof (uint32_t) + sizeof (uint32_t) + sizeof (uint32_t), &sender, sizeof (struct in_addr));
	memcpy (header + sizeof (uint32_t) + sizeof (uint32_t) + sizeof (uint32_t) + sizeof (struct in_addr), &acknowledger, sizeof (struct in_addr));
	return header;
}

/***********************************************************************/
