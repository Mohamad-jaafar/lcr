#include "print.h"
#include <sys/uio.h>
#include <ncurses.h>

/***********************************************************************/

void * sending (void *);
void * handling_keyboard (void *);

/***********************************************************************/

void launch_experiment ();
void prepare_multicast ();
void Multicasting ();
void prepare_multicasting_UDP_socket ();
void prepare_receiving_UDP_socket ();
void measure_throughput ();
void multicast_message (message);
struct timeval subtract_time (struct timeval *, struct timeval *);

/***********************************************************************/

extern pthread_mutex_t sending_mutex;
extern pthread_cond_t sending_cond;
extern std::ofstream logfile;
extern struct timeval start_time_val;
extern double start_time;
extern float threshold;
extern bool handle_keyboard;

/***********************************************************************/
