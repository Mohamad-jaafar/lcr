#include "receiving.h"

/***********************************************************************/

pthread_mutex_t seq_mutex = PTHREAD_MUTEX_INITIALIZER, generated_messages_mutex = PTHREAD_MUTEX_INITIALIZER;
double start_time = 0;
unsigned generated_messages = 0;
uint32_t message_counter = 1, sequence_number = 1;
struct timeval start_time_val{0, 0}, start_lap_time_val{0, 0};
byte message_body [MESSAGE_BODY_SIZE];

#ifdef LATENCY
	struct timeval measure_latency_start_time{0, 0}, measure_latency_finish_time{0, 0};
	unsigned waiting_ID =-1;
#endif

/***********************************************************************/
/* 
 * 
 * launch_experiment
 */

void launch_experiment () {

	// Initializes lap time
	start_lap_time_val = start_time_val;
	
	while (1){

		// Creates a message
		message * m = new message(message_counter, current_IP);

		#ifdef LATENCY
			if (message_counter % 1000 == 0){

				// Get the current time in order to track the throughput
				gettimeofday (&measure_latency_start_time, NULL);
				waiting_ID = message_counter;
			}
		#endif

		// The coordinator will set the sequence number directly for its messages
		if (is_leader){
			pthread_mutex_lock (&seq_mutex);
			m -> set_seqno (sequence_number ++);
			pthread_mutex_unlock (&seq_mutex);
		}

		// Increment message ID
		message_counter ++;

		push_to_export (m);

		// Increment message ID within a lap; this is to be initialized each lap
		pthread_mutex_lock (&generated_messages_mutex);
		generated_messages ++;
		pthread_mutex_unlock (&generated_messages_mutex);

		measure_throughput ();
	}
}

/***********************************************************************/

void measure_throughput (){

	struct timeval current_time{0, 0};

	// Get the current time in order to track the throughput
	gettimeofday (&current_time, NULL);
	struct timeval elapsed_time_val = subtract_time (&current_time, &start_lap_time_val);

	double elapsed = elapsed_time_val.tv_sec+ (elapsed_time_val.tv_usec/1000000.0);
	print_string (std::string ("sending elapsed: ") + std::to_string (elapsed), verbose, true);

	if (elapsed > 0){

		unsigned long long size_of_data;
		size_of_data = generated_messages * MESSAGE_BODY_SIZE;

		// Calculate the expected time to send sent_bytes
		double theoretical_time = size_of_data / ((threshold * mega) / 8.0);

		print_string (std::string ("theoretical_time: ") + std::to_string (theoretical_time), verbose, true);

		// Compare the expected time with the real elapsed time
		if (theoretical_time > elapsed){
			__useconds_t additional_time = (theoretical_time - elapsed) * 1000000;

			// Wait the additional time
			usleep (additional_time);

			print_string (std::string ("additional_time: ") + std::to_string (additional_time), verbose, true);
			print_string (std::string ("elapsed: ") + std::to_string ((double)elapsed_time_val.tv_sec)+ std::string (":") + std::to_string ((double)elapsed_time_val.tv_usec), verbose, true);

			unsigned long long sending_throughput = calculate_throughput(elapsed, size_of_data);

			print_throughput (std::string ("sending throughput: ") , sending_throughput, verbose, true);
		}
	}
}

/***********************************************************************/
/* 
 * measure_latency
 * 
 */

#ifdef LATENCY
void measure_latency (message *m){

	if (m -> get_ID () == waiting_ID && m -> get_sender().s_addr == current_IP.s_addr){

		gettimeofday (&measure_latency_finish_time, NULL);
		struct timeval elapsed_time_val = subtract_time (&measure_latency_finish_time, &measure_latency_start_time);

		unsigned elapsed = elapsed_time_val.tv_sec * 1000000 + elapsed_time_val.tv_usec;
		print_string (std::to_string (elapsed), true, true);
	}
}
#endif

/***********************************************************************/
