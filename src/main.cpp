#include "main.h"
#define BW 960

/***********************************************************************/

int EXPORT_PORT;
bool is_leader = false, handle_keyboard = false;
float threshold = 0;
struct in_addr current_IP, anonymous_IP, leader_IP;

/***********************************************************************/

int main (int argc, char * argv[]) {

	// Handle ^z signal to trace buffers
	signal (SIGINT, signalHandler);

	print_current_time ();

	// An option to log everything into file
	#ifdef LOG
		logfile.open (std::string ("logfile_") + inet_ntoa (current_IP));
	#endif

	// Defines anonymous IP
	inet_aton ("0.0.0.0",&anonymous_IP);

	// Initializes arguments according to the parameteres given from console
	initialize_arguments (argc, argv);

	// Gets the current IP according to the given interface if not initialized yet
	if (current_IP.s_addr == anonymous_IP.s_addr)
		current_IP = get_current_IP ();

	// Handle keyboard for manual throughput modification during experiment
	if (handle_keyboard){
		pthread_t handle_keyboard_thread;
		pthread_create(&handle_keyboard_thread , NULL, handling_keyboard, NULL);
	}

	// Initialize EXPORT_PORT
	EXPORT_PORT = CONTROL_PORT + 2;

	// Create processes according to IPs mentioned in the IPs file
	get_all_processes ();

	int receivedPackage = 0, written_bytes = 0;

	if (is_leader){

		prepare_exporting_thread ();

		listen_for_all_processes();
		
		bool go_ahead = true;
		if ((written_bytes = write (successor -> EXPORT_socket, &go_ahead, sizeof(bool))) == -1){
			perror ("ERROR writing synchronization command to socket!");
			std::terminate();
		}
		if ((receivedPackage = read(predecessor -> IMPORT_socket, &go_ahead, sizeof(bool)))<0){
			perror ("ERROR reading synchronization command from socket!");
			std::terminate();
		}

	} else {

		listen_for_all_processes();

		prepare_exporting_thread ();

		bool go_ahead = false;
		if ((receivedPackage = read(predecessor -> IMPORT_socket, &go_ahead, sizeof(bool)))<0){
			perror ("ERROR reading synchronization command from socket!");
			std::terminate();
		}
		if ((written_bytes = write (successor -> EXPORT_socket, &go_ahead, sizeof(bool))) == -1){
			perror ("ERROR writing synchronization command to socket!");
			std::terminate();
		}
	}

	start_importing_threads ();
	start_exporting_thread ();

	// Initialize throughput
	if (!modified_threshold)
		threshold = BW / all_processes.size ();

	// Print experimnet parameteres
	print_variables ();

	//#ifndef LATENCY
	// Print throughput frequently
	pthread_t tracking_thread;
	pthread_create(&tracking_thread , NULL, tracking, NULL);
	//#endif

	// Set the start time
	gettimeofday (&start_time_val, NULL);
	start_time = start_time_val.tv_sec+ (start_time_val.tv_usec/1000000.0);

	// Start the experiment
	launch_experiment();

	return 0;
}

/***********************************************************************/

/* 
 * handling_keyboard
 * 
 * Handle keyboard for manual throughput modification during experiment
 * 
 */

void * handling_keyboard (void *){

	int ch;

	/* Curses Initialisations */
	initscr();
	raw();
	keypad(stdscr, TRUE);
	//noecho();

	while((ch = getch()) != '#'){

		switch(ch){

			case KEY_UP:
	 			threshold ++;
			break;
			case KEY_DOWN:
	 			threshold --;
			break;
			case KEY_RIGHT:
	 			threshold +=10;
			break;
			case KEY_LEFT:
	 			threshold -=10;
			break;
		}
	}

	 signalHandler (2);
	return NULL;
}

/***********************************************************************/
