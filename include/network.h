#include "multicast.h"
#include "importer.h"
#include "exporter.h"

/***********************************************************************/

void * COORDINATOR_CONTROL (void *);
void * connect_CONTROL (void *);

/***********************************************************************/

int connect_TCP_sockets (struct in_addr, int);
void set_member_socket (struct in_addr, int);
void synchronize ();
void get_all_processes ();

/***********************************************************************/

extern unsigned long long sending_throughput;
extern double start_time;
extern struct timeval start_time_val;
extern unsigned print_period;

/***********************************************************************/
