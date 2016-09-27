#include "print.h"

/***********************************************************************/

unsigned print_period = 1;
bool modified_threshold = false;

/***********************************************************************/

/* 
 * tracking
 * 
 */

void * tracking (void *){

	if (is_leader)
		print_string (std::string ("Throughput (Mb/s): "), true, true);

	bool prnt = true;

	while (1){

		sleep (print_period);

		unsigned long long concerned_messages = own_exported_messages + received;

		//std::cout << "concerned_messages: " << concerned_messages << std::endl;

		double local_throughput = round (calculate_throughput (print_period, (unsigned long long) ((unsigned long long) (concerned_messages) * (unsigned long long) (MESSAGE_BODY_SIZE))) / mega * 1.0);

		//std::cout << "calculate_throughput: " << calculate_throughput (print_period, (unsigned long long) ((unsigned long long) (concerned_messages) * (unsigned long long) (MESSAGE_BODY_SIZE))) / mega * 1.0 << std::endl;
		//std::cout << "local_throughput: " << local_throughput << std::endl;

		if (handle_keyboard){
			print_string (std::string ("["), prnt, false);
			print_string (std::to_string (exporting_queue.size ()) + std::string (" "), prnt, false);
			print_string (std::string ("]"), prnt, true);
		}

		std::cout << "\rThroughput: \033[1;31m" << std::fixed << std::setprecision(3) << local_throughput << "\033[0m" << std::flush << std::endl;

		pthread_mutex_lock (&receiving_mutex);
		received = 0;
		pthread_mutex_unlock (&receiving_mutex);

		pthread_mutex_lock (&generated_messages_mutex);
		generated_messages = 0;
		pthread_mutex_unlock (&generated_messages_mutex);

		own_exported_messages = 0;

		gettimeofday (&start_lap_time_val, NULL);
	}

	return NULL;
}

/***********************************************************************/

/*
 * initialize_arguments
 * 
 * Gets some variables from console
 * 
 */

void initialize_arguments (int argc, char * argv[]){

	int i = 1;
	while (i < argc){
		if (strcmp (argv [i], "-po") ==0){

			// Getting ports series from console
			CONTROL_PORT = atoi (argv [i + 1]);
			i+= 2;

		} else if (strcmp (argv [i], "-i") ==0){

			// Getting used interface from console
			interface = argv [i + 1];
			i+= 2;

		} else if (strcmp (argv [i], "-p") ==0){

			// Getting the print_period from console
			print_period = atoi (argv [i + 1]);
			i+= 2;

		} else if (strcmp (argv [i], "-t") ==0){

			// Getting the threshold from console, otherwise it should be
			// calculated and divided over the number of processes
			threshold = atof (argv [i + 1]);
			modified_threshold = true;
			i+= 2;

		} else if (strcmp (argv [i], "-v") ==0){

			// Print details (Verbose mode)
			verbose = true;
			i++;

		} else if (strcmp (argv [i], "-k") ==0){

			// Declaring if handle_keyboard
			handle_keyboard = true;
			i++;
		}
		else usage ();
	}
}

/***********************************************************************/
/* 
 * create_process
 * 
 * Creates a process and initializes its parameters
 * 
 */

process * create_process (struct in_addr IP){
	
	process * other_process;
	other_process = (process *) malloc (sizeof (process));
	other_process -> IP = IP;
	other_process -> control_socket = 0;
	other_process -> IMPORT_socket = 0;
	other_process -> EXPORT_socket = 0;
	other_process -> last_received_ID = 0;
	return other_process;
}

/***********************************************************************/

/* 
 * calculateBandwidth
 * 
 * Calculates the throughput, given the size of data in bytes during a given time interval 
 * 
 */
 unsigned long long calculate_throughput (double lap, unsigned long long size_in_bytes){

	return (((unsigned long long) (size_in_bytes * 8)) / (lap));
}

/***********************************************************************/

/* 
 * get_IPs_from_file
 * 
 * Gets IPs from file
 *
 */

std::vector <struct in_addr> get_IPs_from_file (std::string pth){

	std::string line;
	std::vector <struct in_addr> IPs;

	struct passwd *pw = getpwuid(getuid());

	const char *homedir = pw->pw_dir;
	std::string path = std::string(homedir) + std::string("/") + pth ;

	std::ifstream IPs_File (path.c_str ());
	while (getline (IPs_File, line)){

		struct in_addr IP;
		inet_aton (line.c_str (), &IP);
		IPs.push_back(IP);
	}

	IPs_File.close ();
	return IPs;
}

/***********************************************************************/

/* 
 * 
 * subtract_time
 * 
 * Subtracts time to handle negative values
 * 
 */

struct timeval subtract_time (struct timeval * left_operand, struct timeval * right_operand){
	
	struct timeval res{0, 0};

	if (left_operand -> tv_sec >= right_operand -> tv_sec){
		if (left_operand -> tv_usec >= right_operand -> tv_usec){

			res.tv_sec = left_operand -> tv_sec - right_operand -> tv_sec;
			res.tv_usec = left_operand -> tv_usec - right_operand -> tv_usec;
		}else{
			res.tv_sec = left_operand -> tv_sec - right_operand -> tv_sec - 1;
			res.tv_usec = 1000000 + left_operand -> tv_usec - right_operand -> tv_usec;
		}
	}

	return res;
}

/***********************************************************************/

/* 
 * signalHandler
 * 
 * Handles interrupt signal and traces variables
 * 
 */

void signalHandler ( int signum ){

	trace_error (299, NULL, std::string ("Interrupt signal <") + std::to_string (signum) + std::string ("> received"), false);

	if (handle_keyboard){

		refresh();
		getch();
		endwin();
	}

	std::exit (signum); 
}

/***********************************************************************/
