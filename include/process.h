#ifndef __PROCESS__
#define __PROCESS__

#include <sys/types.h>
#include "message.h"

/***********************************************************************/

typedef struct {

	// Node ID/IP
	struct in_addr IP;

	// TCP socket ID - used for coordination
	int control_socket = 0;

	int IMPORT_socket = 0;

	int EXPORT_socket = 0;

	// last received ID
	unsigned last_received_ID = 0;

	//byte message_body [MESSAGE_BODY_SIZE];

}process;

/***********************************************************************/

#endif
