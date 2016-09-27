#include "exporter.h"
#include "importer.h"
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

/***********************************************************************/

pthread_mutex_t exporting_mutex = PTHREAD_MUTEX_INITIALIZER, export_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t exporting_cond = PTHREAD_COND_INITIALIZER, export_cond = PTHREAD_COND_INITIALIZER;
std::queue <message *> exporting_queue;
unsigned own_exported_messages = 0, exported_messages = 0, exported_ACKs = 0;
byte * data_to_export;

/***********************************************************************/

void prepare_exporting_thread (){
	data_to_export = (byte *) malloc (MESSAGE_BODY_SIZE);
	memset (data_to_export, 0, MESSAGE_BODY_SIZE);
	successor -> EXPORT_socket = connect_TCP_sockets (successor -> IP, EXPORT_PORT);
}

/***********************************************************************/

void start_exporting_thread (){

	pthread_t exporting_thread;
	pthread_create(&exporting_thread , NULL, exporting, NULL);
}

/***********************************************************************/

void * exporting (void *){
	
	while (1){

		pthread_mutex_lock (&exporting_mutex);
		int ret = 0;
		while (exporting_queue.empty () && ret == 0)
			ret = pthread_cond_wait (&exporting_cond, &exporting_mutex);

		message * m = exporting_queue.front ();

		exporting_queue.pop ();
		pthread_mutex_unlock (&exporting_mutex);

		export_message (m);
		delete m;
	}

	return NULL;
}

/***********************************************************************/

void push_to_export (message * m){

	pthread_mutex_lock (&exporting_mutex);

	//exporting_queue.push (m);

	//to be reviewed:
	//---------------
	message * msg = new message (*m);
	exporting_queue.push (msg);

	pthread_cond_signal (&exporting_cond);
	pthread_mutex_unlock (&exporting_mutex);
}

/***********************************************************************/

void export_message (message * m){

	pthread_mutex_lock (&export_mutex);
	int written_bytes = 0;

	byte * header = m -> serialize_message ();

	if ((written_bytes = write (successor -> EXPORT_socket, header, HEADER_SIZE)) == -1)
		trace_error (444, m, "Error exporting header", true);

	free (header);

	if (!m -> is_ACK()){

		written_bytes = 0;

		if (write (successor -> EXPORT_socket, data_to_export, MESSAGE_BODY_SIZE) < 0){

			print_string (std::string ("written_bytes: ") + std::to_string(written_bytes), true, true);
			trace_error (449, m, "Error exporting Data", false);
			perror ("Error exporting Data");

			close (successor -> EXPORT_socket);
			exit (EXIT_FAILURE);
		}

		if (m -> get_sender ().s_addr == current_IP.s_addr)
			own_exported_messages ++;

		exported_messages ++;

	} else{
		exported_ACKs ++;
	}
	pthread_mutex_unlock (&export_mutex);
}

/***********************************************************************/
