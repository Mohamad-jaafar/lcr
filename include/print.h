#include "support.h"
#include <iomanip>

/***********************************************************************/

void print_current_time ();
void print_string (std::string, bool, bool);
void print_throughput (std::string, unsigned long long, bool, bool);
void print_variables ();
void trace_error (unsigned, message *, std::string, bool);
void usage ();

/***********************************************************************/

extern int EXPORT_PORT;
extern std::queue <message *> exporting_queue;
extern unsigned print_period;
extern uint32_t message_counter;
extern unsigned long long sending_throughput;
extern bool verbose;

/***********************************************************************/
