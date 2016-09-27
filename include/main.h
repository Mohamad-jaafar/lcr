#include "importer.h"
#include "exporter.h"
#include "receiving.h"
#include <sys/uio.h>

/***********************************************************************/

void signalHandler (int);
struct in_addr get_current_IP ();
void listen_for_all_processes();

/***********************************************************************/

extern pthread_mutex_t start_mutex;
extern pthread_cond_t start_cond;

/***********************************************************************/
