#include "network.h"

/***********************************************************************/

std::string interface = "eth0";
int CONTROL_PORT = 46670;

/***********************************************************************/

std::vector <process * > all_processes;
int local_socket, coordinator_control_socket;
pthread_mutex_t sending_mutex = PTHREAD_MUTEX_INITIALIZER, start_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sending_cond = PTHREAD_COND_INITIALIZER, start_cond = PTHREAD_COND_INITIALIZER;
bool start = false;
std::queue <message> sending_queue;
process *successor = NULL, *predecessor = NULL;

/***********************************************************************/

/* 
 * get_all_processes
 * 
 * Gets all the nodes from the IPs file
 *
 */

void get_all_processes () {

	std::vector <struct in_addr> IPs = get_IPs_from_file ("all_IPs");

	for (unsigned p = 0; p < IPs.size(); p++){
	
		if (IPs.at(p).s_addr != current_IP.s_addr){

			process * new_process = create_process(IPs.at(p));
			all_processes.push_back(new_process);

			if (p == 0){
				if (IPs.at(IPs.size() - 1).s_addr == current_IP.s_addr){
					successor = new_process;
				}
				if (IPs.at(1).s_addr == current_IP.s_addr){
					predecessor = new_process;
				}

			} else if (p == IPs.size()-1){

				if (IPs.at(0).s_addr == current_IP.s_addr){
					predecessor = new_process;
				}
				if (IPs.at(IPs.size() - 2).s_addr == current_IP.s_addr){
					successor = new_process;
				}
			} else {

				if (IPs.at(p - 1).s_addr == current_IP.s_addr){
					successor = new_process;
				}
				if (IPs.at(p + 1).s_addr == current_IP.s_addr){
					predecessor = new_process;
				}
			}
		}
	}

	std::cout << "successor: " << inet_ntoa (successor -> IP) << std::endl;
	std::cout << "predecessor: " << inet_ntoa (predecessor -> IP) << std::endl;

	leader_IP = IPs.at(0);

	if (leader_IP.s_addr == current_IP.s_addr)
		is_leader = true;
}

/***********************************************************************/

/* 
 * connect_TCP_sockets
 * 
 * Establishes TCP connection with another machine
 * 
 */
int connect_TCP_sockets (struct in_addr dest, int port) {

	struct sockaddr_in send_addr;

	int conn_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (conn_socket < 0){
		perror ("ERROR opening socket!");
		std::terminate ();
	}

	bzero ( (char *) &send_addr, sizeof (send_addr));
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons (port);
	send_addr.sin_addr = dest;

	while (connect (conn_socket, (struct sockaddr *)&send_addr, sizeof (send_addr))<0){
		print_string (std::string ("Connecting to ") + inet_ntoa(dest)  + std::string (":") + std::to_string (port), true, true);
		sleep (1);
	}

	print_string (std::string ("Connected to ") + inet_ntoa(dest)  + std::string (":") + std::to_string (port), true, true);

	return conn_socket;
}

/***********************************************************************/

/* 
 * get_current_IP
 * 
 * Gets the local IP assigned to interface dynamically.
 * 
 */
struct in_addr get_current_IP (){

	int fd_to_parent = 0;
	struct ifreq ifr;
	struct in_addr IP;

	// Checking the interface
	fd_to_parent = socket (AF_INET, SOCK_STREAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy (ifr.ifr_name, interface.c_str (), IFNAMSIZ);
	if (ioctl (fd_to_parent, SIOCGIFFLAGS, &ifr) < 0){
		std::string err = "SIOCGIFFLAGS, interface: " + interface;
		perror (err.c_str ());
	}
	// If there is a network connected to interface and it is running
	if ( (ifr.ifr_flags & IFF_UP) && (ifr.ifr_flags & IFF_RUNNING)){
		ioctl (fd_to_parent, SIOCGIFADDR, &ifr);
		close (fd_to_parent);
		IP = ( (struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
	}
	// If no real network connected to eth0 use loopback
	else
		print_string (std::string ("Error getting current_IP"), true, true);

	return IP;
}

/***********************************************************************/

/* 
 * set_member_socket
 * 
 * Assigns the input socket for that IP
 * 
 */

void set_member_socket (struct in_addr ip, int sock){
	print_string (std::string ("Received connection from ") + inet_ntoa (ip), true, true);
	for (unsigned i = 0; i< NUMBER_OF_NODES;i++){
		if (all_processes.at(i) -> IP.s_addr == ip.s_addr){
			all_processes.at(i) -> control_socket = sock;
			print_string (std::string ("sock: ") + std::to_string (sock), verbose, true);
		}
	}
}

/***********************************************************************/
