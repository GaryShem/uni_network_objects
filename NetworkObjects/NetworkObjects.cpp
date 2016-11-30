// NetworkObjects.cpp : Defines the entry point for the console application.
//

#define WIN32_LEAN_AND_MEAN

#include <vector>
#include <thread>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include "NetObject.h"
#include "NoneObject.h"
#include "PrintObject.h"
#include "WSAObject.h"
#include "CommandType.h"
#include "NetObjectType.h"
#include "ProcessObject.h"
#include "BeepObject.h"

#pragma comment(lib, "Ws2_32.lib")

//defines
#define PORT 9999
#define BUFFER_SIZE 128

//variables
std::vector<NetObject*> net_objects;
SOCKET listener = INVALID_SOCKET;
SOCKET sender = INVALID_SOCKET;
bool stop_program = false;

//function forward declarations
void init_network_objects();
void listen_to_connections();
CommandType compare_command(char* buf);
NetObjectType current_object_type;
void choose_object();
int send_command();
bool input_ip(in_addr& ip);
void send_type(in_addr ip);
void send_call(in_addr ip);
void send_swap(in_addr ip);
std::string obj_type_to_string(NetObjectType net_obj_type);
void kill_socket(SOCKET sock);

int main()
{
	WSAObject wsa;
	choose_object();

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener == INVALID_SOCKET)
	{
		std::cout << "cannot create listener socket" << std::endl;
		return 153;
	}

	sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(listener, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cout << "can't bind listener socket" << std::endl;
		return 154;
	}

	std::thread listen_thread (listen_to_connections);
	listen_thread.detach();

	init_network_objects();
	int a = 1;
	while (a != 0)
	{
		a = send_command();
	}

    return 0;
}

void listen_to_connections()
{
	listen(listener, SOMAXCONN);

	while (stop_program == false)
	{
		char buf[BUFFER_SIZE];
		sockaddr_in client_addr;
		int addrSize = sizeof(client_addr);
		char* message = nullptr;
		SOCKET client_socket = accept(listener, (sockaddr*)&client_addr, &addrSize);

		int bytes_read = recv(client_socket, buf, BUFFER_SIZE - 1, 0);
		if (bytes_read == SOCKET_ERROR)
			continue;
		buf[bytes_read] = '\0';
		if (bytes_read < 4)
		{
			std::cout << std::endl << "received malformed command, ignoring" << std::endl;
		}
		if (bytes_read > 5)
		{
			std::cout << std::endl << "command MIGHT have a message with it" << std::endl;
			message = buf + 5;
		}
		CommandType type = compare_command(buf);
		int bytes_sent;
		switch (type)
		{
			case TYPE: 
				buf[0] = (char)current_object_type;
				bytes_sent = send(client_socket, buf, 1, 0);
				if (bytes_sent == SOCKET_ERROR)
				{
					std::cout << std::endl << "could not respond to TYPE command" << std::endl;
				}
				break;
			case CALL:
				net_objects[current_object_type]->call(message == nullptr ? "" : std::string(message));
				break;
			case SWAP:
				buf[0] = (char)current_object_type;
				bytes_sent = send(client_socket, buf, 1, 0);
				if (bytes_sent == SOCKET_ERROR)
				{
					std::cout << std::endl << "could not respond to SWAP command" << std::endl;
				}
				else
				{	
					std::cout << std::endl << "swapping objects" << std::endl;
					current_object_type = (NetObjectType)buf[4];
				}
				break;
			default:
				std::cout << std::endl << "received malformed command, ignoring" << std::endl;
				break;
		}
		closesocket(client_socket);
	}
}

void init_network_objects()
{
	net_objects.push_back(new NoneObject());
	net_objects.push_back(new PrintObject());
	net_objects.push_back(new ProcessObject());
	net_objects.push_back(new BeepObject());
}

CommandType compare_command(char* buf)
{
	char command[5];
	strncpy(command, buf, 4);
	command[4] = '\0';

	if (strcmp(command, "TYPE") == 0)
		return TYPE;
	if (strcmp(command, "SWAP") == 0)
		return SWAP;
	if (strcmp(command, "CALL") == 0)
		return CALL;

	return ERROR_COMMAND;
}

void choose_object()
{
	int choice;
	bool correct_choice = false;
	while (correct_choice == false)
	{
		correct_choice = true;
		std::cout << "Choose object type for this machine" << std::endl;
		std::cout << "0. None\n" << "1. Print\n" << "2. Process\n" << "3. Beep\n" << "-> ";
		std::cin >> choice;
		std::cin.ignore();
		std::cout << "Your choice is " << choice << std::endl;
		switch (choice)
		{
		case 0:
			current_object_type = NO_OBJECT;
			break;
		case 1:
			current_object_type = PRINT;
			break;
		case 2:
			current_object_type = PROCESSES;
			break;
		case 3:
			current_object_type = BEEP;
			break;
		default:
			std::cout << "incorrect choice, try again" << std::endl;
			correct_choice = false;
			break;
		}
	}
	std::cout << "object chosen: " << obj_type_to_string(current_object_type) << std::endl;
}

int send_command()
{
	int choice;
	in_addr ip;
	bool correct_choice = false;
	while (correct_choice == false)
	{
		correct_choice = true;
		std::cout << "Choose command" << std::endl;
		std::cout << "1. TYPE\n" << "2. CALL\n" << "3. SWAP\n" << "0. exit\n" << "-> ";
		std::cin >> choice;
		std::cin.ignore();
		std::cout << "Your choice is " << choice << ": ";
		switch (choice)
		{
		case 0:
			std::cout << "exit" << std::endl;
			stop_program = true;
			break;
		case 1:
			std::cout << "TYPE" << std::endl;
			if (input_ip(ip) == false)
				break;
			send_type(ip);
			break;
		case 2:
			std::cout << "CALL" << std::endl;
			if (input_ip(ip) == false)
				break;
			send_call(ip);
			break;
		case 3:
			std::cout << "SWAP" << std::endl;
			if (input_ip(ip) == false)
				break;
			send_swap(ip);
			break;
		default:
			std::cout << "incorrect choice, try again" << std::endl;
			correct_choice = false;
			break;
		}
	}
	return choice;
}

void send_type(in_addr ip)
{
	bool error_occured = false;
	char buf[BUFFER_SIZE];
	sender = socket(AF_INET, SOCK_STREAM, 0);
	if (sender == INVALID_SOCKET)
	{
		std::cout << "coult not create sender socket, try again later" << std::endl;
		error_occured = true;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr = ip;
	addr.sin_port = htons(PORT);
	std::thread killer_thread(kill_socket, sender);
	if (!error_occured)
	if (connect(sender, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "could not connect to target host" << std::endl;
		error_occured = true;
	}
	if (!error_occured)
	if (send(sender, "TYPE", 4, 0) == SOCKET_ERROR)
	{
		std::cout << "could not send message to target host" << std::endl;
		error_occured = true;
	}

	int bytes_received = recv(sender, buf, BUFFER_SIZE - 1, 0);
	if (!error_occured)
	if (bytes_received == SOCKET_ERROR)
	{
		std::cout << "could not receive answer from target host" << std::endl;
		error_occured = true;
	}

	if (!error_occured)
	{
		NetObjectType obj_type = (NetObjectType)buf[0];
		std::cout << "Object type of target host: " << obj_type_to_string(obj_type) << std::endl;
	}

	closesocket(sender);
	killer_thread.join();
}

bool input_ip(in_addr& ip)
{
	in_addr buffer;
	std::string ip_string;
	std::cout << "input ip" << std::endl;
	std::cin >> ip_string;
	std::cin.ignore();
	if (inet_pton(AF_INET, ip_string.c_str(), &buffer) == 1)
	{
		ip = buffer;
		return true;
	}
	else
	{
		std::cout << "incorrect ip" << std::endl;
		return false;
	}
}

std::string obj_type_to_string(NetObjectType net_obj_type)
{
	switch (net_obj_type)
	{
	case NO_OBJECT: return "no object";
	case PRINT: return "print object";
	case PROCESSES: return "process object";
	case BEEP: return "beep object";
	default: return "error; check object types";
	}
}

void send_call(in_addr ip)
{
	bool error_occured = false;
	sender = socket(AF_INET, SOCK_STREAM, 0);
	std::string message;
	bool attach_message = false;

	std::cout << "enter your message (or press ENTER to leave message blank)" << std::endl;
	std::getline(std::cin, message);
	std::cout << "message ready" << std::endl;

	if (message.empty() == false)
	{
		std::cout << "message is not empty" << std::endl;
		attach_message = true;
		message = std::string("CALL ") + message;
	}
	else
	{
		std::cout << "message is empty" << std::endl;
		message = std::string("CALL");
	}

	if (sender == INVALID_SOCKET)
	{
		std::cout << "NO CALL, coult not create sender socket, try again later" << std::endl;
		error_occured = true;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr = ip;
	addr.sin_port = htons(PORT);
	std::thread killer_thread(kill_socket, sender);
	if (!error_occured)
	if (connect(sender, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "NO CALL, could not connect to target host" << std::endl;
		error_occured = true;
	}
	
	if (!error_occured)
	if (send(sender, message.c_str(), message.length(), 0) == SOCKET_ERROR)
	{
		std::cout << "NO CALL, could not send message to target host" << std::endl;
		error_occured = true;
	}

	if (!error_occured)
	{
		std::cout << "target object called" << std::endl;
	}

	closesocket(sender);
	killer_thread.join();
}

void send_swap(in_addr ip)
{
	bool error_occured = false;
	char buf[BUFFER_SIZE];
	sender = socket(AF_INET, SOCK_STREAM, 0);
	if (sender == INVALID_SOCKET)
	{
		std::cout << "NO SWAP, coult not create sender socket, try again later" << std::endl;
		error_occured = true;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr = ip;
	addr.sin_port = htons(PORT);
	std::thread killer_thread(kill_socket, sender);

	if (!error_occured)
	if (connect(sender, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "NO SWAP, could not connect to target host" << std::endl;
		error_occured = true;
	}
	char swap_buf[10];
	strcpy(swap_buf, "SWAP");
	swap_buf[4] = current_object_type;
	swap_buf[5] = '\0';
	if (!error_occured)
	if (send(sender, swap_buf, 5, 0) == SOCKET_ERROR)
	{
		std::cout << "NO SWAP, could not send message to target host" << std::endl;
		error_occured = true;
	}

	int bytes_received = SOCKET_ERROR;
	if (!error_occured)
	{
		bytes_received = recv(sender, buf, BUFFER_SIZE - 1, 0);
	}
	if (!error_occured)
	if (bytes_received == SOCKET_ERROR)
	{
		std::cout << "NO SWAP: could not receive answer from target host" << std::endl;
		error_occured = true;
	}

	if (!error_occured)
	{
		NetObjectType obj_type = (NetObjectType)buf[0];
		std::cout << "SWAPPING, Object type of target host: " << obj_type_to_string(obj_type) << std::endl;
		current_object_type = obj_type;
	}

	closesocket(sender);
	killer_thread.join();
}

void kill_socket(SOCKET sock)
{
	Sleep(5000);
	closesocket(sock);
}