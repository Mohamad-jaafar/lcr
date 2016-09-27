#include "network.h"
#include <assert.h>
#include <fcntl.h>
#include <sys/sendfile.h>

/***********************************************************************/

extern uint32_t sequence_number;
extern pthread_mutex_t seq_mutex;

/***********************************************************************/
