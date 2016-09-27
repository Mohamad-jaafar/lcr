#include "importer.h"
#include "exporter.h"
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

/***********************************************************************/

pthread_mutex_t receiving_mutex = PTHREAD_MUTEX_INITIALIZER, importing_mutex = PTHREAD_MUTEX_INITIALIZER;
int initial_listening_socket;
unsigned received = 0;
byte * body;

/***********************************************************************/

void listen_for_all_processes(){

	initial_listening_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (initial_listening_socket < 0)
		std::cerr << "ERROR opening socket, initial_listening_socket" << std::endl;

	int reuse = 1;
	if (setsockopt (initial_listening_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (int)) ==-1){
		perror ("ERROR on reusing port!");
		std::terminate ();
	}

	struct sockaddr_in local_socket_address;
	bzero ((char *) &local_socket_address, sizeof (local_socket_address));

	local_socket_address.sin_family = AF_INET;
	local_socket_address.sin_port = htons (EXPORT_PORT);
	local_socket_address.sin_addr.s_addr = current_IP.s_addr;

	//Bind to a port
	if (bind (initial_listening_socket, (struct sockaddr *) &local_socket_address, sizeof (local_socket_address)) < 0)
		std::cerr << "ERROR on binding" << std::endl;

	listen (initial_listening_socket, NUMBER_OF_NODES);

	struct sockaddr_in client_address;
	socklen_t length_of_client_address = sizeof (client_address);

	predecessor -> IMPORT_socket = accept(initial_listening_socket, (struct sockaddr *) &client_address, &length_of_client_address);

	if (predecessor -> IMPORT_socket < 0) {
		perror("ERROR on accept");
		exit(1);
	}
}

/***********************************************************************/

void start_importing_threads (){

	body = (byte *) malloc (MESSAGE_BODY_SIZE);
	pthread_t importing_thread;
	pthread_create(&importing_thread , NULL, importing, NULL);
}

/***********************************************************************/

void * importing (void *){

	while (1){

		pthread_mutex_lock (&importing_mutex);
		import_message ();
		pthread_mutex_unlock (&importing_mutex);
	}

	return NULL;
}

/***********************************************************************/

void import_message (){

	int receivedPackage = 0;

	byte * header = (byte *) malloc (HEADER_SIZE);

	int pos = 0;
	int expected_bytes = HEADER_SIZE;
	while (expected_bytes >0){

		if ((receivedPackage = read(predecessor -> IMPORT_socket, header + pos, expected_bytes)) < 0){
			perror ("ERROR importing message data from socket!");
			std::terminate();
		}

		pos += receivedPackage;
		expected_bytes -= receivedPackage;
	}

	// deserialize message
	message * m = new message (header);
	free (header);

	if (m -> get_ID () == 0){
		trace_error (649, m, std::string ("Imported faulty data"), true);
	}

	if (!m -> is_ACK()){

		receivedPackage = 0;
		pos = 0;
		expected_bytes = MESSAGE_BODY_SIZE;

		while (expected_bytes >0){

			if ((receivedPackage = read(predecessor -> IMPORT_socket, body + pos, expected_bytes)) < 0){
				perror ("ERROR importing message data from socket!");
				std::terminate();
			}

			pos += receivedPackage;
			expected_bytes -= receivedPackage;
		}

		pthread_mutex_lock (&receiving_mutex);
		received++;
		pthread_mutex_unlock (&receiving_mutex);

		if (successor -> IP.s_addr != m -> get_sender ().s_addr){

			//if (successor -> IP.s_addr == leader_IP.s_addr || is_leader)
			push_to_export (m);

		} else {

			message * ack = create_ACK (*m);

			//export_message (ack);
			//delete ack;
			push_to_export(ack);
		}

	} else {

		if (successor -> IP.s_addr != m -> get_acknowledger ().s_addr){

			//export_message (m);
			push_to_export (m);
		}
		#ifdef LATENCY
		measure_latency (m);
		#endif
	}

	if (m -> get_ID () == 0){
		trace_error (652, m, std::string ("Imported faulty data"), true);
	}
}

/***********************************************************************/

/* 
 * set_exporter_socket
 * 
 * Assigns the input socket for that IP
 * 
 */

void set_exporter_socket (struct in_addr ip, int sock){
	print_string (std::string ("Received connection from ") + inet_ntoa (ip), true, true);
	for (unsigned i = 0; i< NUMBER_OF_NODES;i++){
		if (all_processes.at(i) -> IP.s_addr == ip.s_addr){
			all_processes.at(i) -> IMPORT_socket = sock;
			print_string (std::string ("sock: ") + std::to_string (sock), verbose, true);
		}
	}
}


/***********************************************************************/
/* 
 * create_ACK
 * 
 */

message * create_ACK (message m){

	message * msg = new message (m);

	// and changing some attributes
	msg -> set_acknowledger(current_IP);
	msg -> set_ACK();

	return msg;
}

/***********************************************************************/
