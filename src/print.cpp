#include "multicast.h"

/***********************************************************************/

std::ofstream logfile;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
bool verbose = false;

/***********************************************************************/

/* 
 * print_current_time
 * 
 * Prints current time
 */
void print_current_time ()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
	std::string str (buffer);

	print_string ( "", true, true);
	print_string ( str, true, true);
}

/***********************************************************************/
	
/* 
 * print_string
 * 
 * Prints a string
 */

void print_string (std::string str, bool prnt, bool newline){

	if (prnt){

		pthread_mutex_lock (&print_mutex);

		if (handle_keyboard){

			// enable flushing

			if (newline)
				std::cout << "\r";

			std::cout << str;

			if (newline)
				std::cout << std::flush << std::endl;
		} else {

			std::cout << str;
			if (newline)
				std::cout << std::endl;
		}
		
		pthread_mutex_unlock (&print_mutex);
	}

	#ifdef LOG
	logfile << str;
	if (newline)
		logfile << std::endl;
	#endif

}

/***********************************************************************/

/* 
 * print_throughput
 * 
 * Returns the size of data in a human readable format
 * 
 */

void print_throughput (std::string s, unsigned long long size_of_data, bool prnt, bool newline){

	int precision = 1;

	if (prnt){

		std::cout << s << "\033[1;31m";

		double size;

		if (size_of_data > giga) {

			// Round result and show three decimal values
			size = round (size_of_data / (giga /1000));

			std::cout << std::fixed << std::setprecision(precision) << size /1000;
			std::cout << std::string (" Gb");

		} else if (size_of_data > mega){

			// Round result and show two decimal values
			size = round (size_of_data / (mega /100));

			std::cout << std::fixed << std::setprecision(precision) << size /100;

			std::cout << std::string (" Mb");

		} else if (size_of_data > kilo){

			// Round result and show one decimal values
			size = round (size_of_data / (kilo /10));

			std::cout << std::fixed << std::setprecision(precision) << size /10;

			std::cout << std::string (" Kb");

		} else
			std::cout << std::string ("b");

		std::cout << "/s\033[0m";

		if (newline)
			std::cout << std::endl;
	}
}

/***********************************************************************/
/*
 * print_variables
 * 
 * Prints the used variables on starting the process
 * 
 */

void print_variables (){

	print_string (std::string ("***************************************** "), true, true);
	print_string (std::string ("Running Variables:"), true, true);
	print_string (std::string ("******************"), true, true);

	print_string (std::string ("IP: ") + inet_ntoa (current_IP), true, true);

	print_string ("Nodes: {", true, false);

	for (unsigned p = 0; p < NUMBER_OF_NODES; p++)
		print_string (inet_ntoa(all_processes.at(p) -> IP) + std::string (" "), true, false);

	print_string ("}", true, true);

	print_string (std::string ("Control port: ") + std::to_string (CONTROL_PORT), true, true);
	print_string (std::string ("Export port: ") + std::to_string (EXPORT_PORT), true, true);

	print_string (std::string ("Network interface: ") + interface, true, true);

	print_string (std::string ("Message size: ") + std::to_string (MESSAGE_BODY_SIZE) + std::string (" bytes"), true, true);
	print_string (std::string ("sizeof (message): ") + std::to_string (sizeof (message)) + std::string (" bytes"), true, true);

	if (is_leader) print_string (std::string ("leader"), true, true);

	print_string (std::string ("print_period: ") + std::to_string (print_period), true, true);
	if (verbose) print_string (std::string ("Verbose"), true, true);
	if (handle_keyboard) print_string (std::string ("Handle Keyboard"), true, true);
	print_string (std::string ("Threshold: ") + std::to_string (threshold) + std::string (" Mb/s"), true, true);

	#ifdef LOG
	print_string (std::string ("Trace log"), true, true);
	#endif

	print_string (std::string ("***************************************** "), true, true);
}

/***********************************************************************/

/*
 * Usage
 * 
 * It prints the usage of this application
 * 
 */

void usage (){
	print_string (std::string ("Usage:"), true, true);

	print_string (std::string ("./run -[po|i|m|t|p] [parameter] [-v] [-k] [-h]"), true, true);
	print_string (std::string ("-po\tSet TCP control port"), true, true);
	print_string (std::string ("-i\tSet Interface"), true, true);
	print_string (std::string ("-m\tSet Message Size (in bytes)"), true, true);
	print_string (std::string ("-t\tSet Threshold (Mb/s)"), true, true);
	print_string (std::string ("-p\tSet the print period"), true, true);
	print_string (std::string ("-v\tVerbose"), true, true);
	print_string (std::string ("-k\tHandle Keyboard"), true, true);
	print_string (std::string ("-h\tdisplays this help and exits"), true, true);
	exit (0);
}

/***********************************************************************/

/* 
 * trace_error
 * 
 * print pending list and important data
 * 
 */

void trace_error (unsigned error_no, message * error_message, std::string err, bool terminate){

	bool prnt = true;

	print_string (std::string (""), prnt, true);
	print_string (std::string ("====================="), prnt, true);
	print_string (std::string ("Tracing Error report:"), prnt, true);
	print_string (std::string ("====================="), prnt, true);

	print_string ("", prnt, true);

	print_string (std::string ("generated_messages: ") + std::to_string (generated_messages), prnt, true);
	print_string (std::string ("message_counter: ") + std::to_string (message_counter), prnt, true);
	print_string (std::string ("received: ") + std::to_string (received), prnt, true);
	print_string (std::string ("exported_messages: ") + std::to_string (exported_messages), prnt, true);
	print_string (std::string ("exported_ACKs: ") + std::to_string (exported_ACKs), prnt, true);
	
	print_string (std::string ("exporting_queue.size (): ") + std::to_string (exporting_queue.size ()), prnt, true);

	print_string (std::string (std::to_string (error_no) + std::string (": ") + std::string (err)), prnt, true);

	if (error_message != NULL){

		print_string (std::string ("---------------------------------------------------"), prnt, true);
		error_message -> print ();
	}

	if (terminate)
		std::terminate();
}

/***********************************************************************/
