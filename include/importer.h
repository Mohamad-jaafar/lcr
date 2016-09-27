#ifndef __IMPORTER__
#define __IMPORTER__

#include "print.h"

#endif

/***********************************************************************/

void * importing (void *);

/***********************************************************************/

void determine_all_processes ();
void start_importing_threads ();
void import_message ();
void set_exporter_socket (struct in_addr, int);

/***********************************************************************/

extern pthread_mutex_t receiving_mutex, import_accept_mutex;
extern pthread_cond_t import_accept_cond;
extern unsigned connected_importers;

/***********************************************************************/
