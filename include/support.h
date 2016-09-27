#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/time.h>
#include <time.h>
#include <ctime>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <csignal>
#include <sys/time.h>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <execinfo.h>
#include <cstdlib>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pwd.h>
#include <ncurses.h>

#include "message.h"
#include "process.h"

/***********************************************************************/

void * tracking (void *);

/***********************************************************************/

void push_into_send_queue(message);
unsigned long long calculate_throughput (double, unsigned long long);
int get_process_ID (struct in_addr);
void get_lines_from_file (std::string);
void initialize_arguments (int, char **);
void get_group_members ();
struct in_addr get_multicast_IP ();
struct timeval subtract_time (struct timeval *, struct timeval *);
std::vector <struct in_addr> get_IPs_from_file (std::string);
process * create_process (struct in_addr);
void measure_latency (message *);

/***********************************************************************/

extern std::vector <process *> all_processes;
extern bool verbose, is_leader, handle_keyboard, modified_threshold;
extern int coordinator_TCP_socket, CONTROL_PORT;
extern std::string interface;
extern struct in_addr current_IP, anonymous_IP, leader_IP;
extern struct timeval start_time_val, start_lap_time_val;
extern unsigned own_exported_messages, exported_messages, exported_ACKs, generated_messages, received;
extern process * current_process;
extern float threshold;
extern pthread_mutex_t receiving_mutex, generated_messages_mutex;

/***********************************************************************/
