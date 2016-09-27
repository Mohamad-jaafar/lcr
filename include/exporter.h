#ifndef __EXPORTER__
#define __EXPORTER__

#include "print.h"

#endif

/***********************************************************************/

void * exporting (void *);
void * connect_export (void *);

/***********************************************************************/

int connect_TCP_sockets (struct in_addr, int);
void determine_all_processes ();
void export_message (message *);
void push_to_export (message *);
void push_into_ACKs_queue (message *);
message * create_ACK (message);
void prepare_exporting_thread ();
void start_exporting_thread ();

/***********************************************************************/

extern std::queue <message *> exporting_queue;
extern pthread_cond_t export_connect_cond;
extern unsigned connected_exporters;
extern pthread_mutex_t export_connect_mutex;
extern process *successor, *predecessor;

/***********************************************************************/
