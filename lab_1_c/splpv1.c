/*
 * SPLPv1.c
 * The file is part of practical task for System programming course.
 * This file contains validation of SPLPv1 protocol.
 */


 /*
   Скридлевский Алексей Александрович
   14 группа
 */


 /*
 ---------------------------------------------------------------------------------------------------------------------------
 # |      STATE      |         DESCRIPTION       |           ALLOWED MESSAGES            | NEW STATE | EXAMPLE
 --+-----------------+---------------------------+---------------------------------------+-----------+----------------------
 1 | INIT            | initial state             | A->B     CONNECT                      |     2     |
 --+-----------------+---------------------------+---------------------------------------+-----------+----------------------
 2 | CONNECTING      | client is waiting for con-| A<-B     CONNECT_OK                   |     3     |
   |                 | nection approval from srv |                                       |           |
 --+-----------------+---------------------------+---------------------------------------+-----------+----------------------
 3 | CONNECTED       | Connection is established | A->B     GET_VER                      |     4     |
   |                 |                           |        -------------------------------+-----------+----------------------
   |                 |                           |          One of the following:        |     5     |
   |                 |                           |          - GET_DATA                   |           |
   |                 |                           |          - GET_FILE                   |           |
   |                 |                           |          - GET_COMMAND                |           |
   |                 |                           |        -------------------------------+-----------+----------------------
   |                 |                           |          GET_B64                      |     6     |
   |                 |                           |        ------------------------------------------------------------------
   |                 |                           |          DISCONNECT                   |     7     |
 --+-----------------+---------------------------+---------------------------------------+-----------+----------------------
 4 | WAITING_VER     | Client is waiting for     | A<-B     VERSION ver                  |     3     | VERSION 2
   |                 | server to provide version |          Where ver is an integer (>0) |           |
   |                 | information               |          value. Only a single space   |           |
   |                 |                           |          is allowed in the message    |           |
 --+-----------------+---------------------------+---------------------------------------+-----------+----------------------
 5 | WAITING_DATA    | Client is waiting for a   | A<-B     CMD data CMD                 |     3     | GET_DATA a GET_DATA
   |                 | response from server      |                                       |           |
   |                 |                           |          CMD - command sent by the    |           |
   |                 |                           |           client in previous message  |           |
   |                 |                           |          data - string which contains |           |
   |                 |                           |           the following allowed cha-  |           |
   |                 |                           |           racters: small latin letter,|           |
   |                 |                           |           digits and '.'              |           |
 --+-----------------+---------------------------+---------------------------------------+-----------+----------------------
 6 | WAITING_B64_DATA| Client is waiting for a   | A<-B     B64: data                    |     3     | B64: SGVsbG8=
   |                 | response from server.     |          where data is a base64 string|           |
   |                 |                           |          only 1 space is allowed      |           |
 --+-----------------+---------------------------+---------------------------------------+-----------+----------------------
 7 | DISCONNECTING   | Client is waiting for     | A<-B     DISCONNECT_OK                |     1     |
   |                 | server to close the       |                                       |           |
   |                 | connection                |                                       |           |
 ---------------------------------------------------------------------------------------------------------------------------

 IN CASE OF INVALID MESSAGE THE STATE SHOULD BE RESET TO 1 (INIT)

 */

 
#include "splpv1.h"

#include <string.h>


 /* FUNCTION:  validate_message
   *
   * PURPOSE:
   *    This function is called for each SPLPv1 message between client
   *    and server
   *
   * PARAMETERS:
   *    msg - pointer to a structure which stores information about
   *    message
   *
   * RETURN VALUE:
   *    MESSAGE_VALID if the message is correct
   *    MESSAGE_INVALID if the message is incorrect or out of protocol
   *    state
   */

const int base64_data[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 0, 0, 0, 0, 0
};

int validate_b64(char* message) {
	if (strncmp("B64: ", message, 5) != 0)
		return 0;
	message += 5;

	char* t = message;
	while (*t != '\0') {
		if (!base64_data[*t])
			break;
		t++;
	}

	if (*t == '=') {
		t++;
		if (*t == '=') {
			t++;
			if (*t != '\0')
				return 0;
		}
		else if (*t != '\0') 
			return 0;
	}
	else if (*t != '\0')
		return 0;

	if ((t - message) % 4 != 0) 
		return 0;

	return 1;
}

const int data[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 0, 0, 0, 0
};

const int numbers[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

int CurrentState = 1;
int Command = 0;

enum test_status get_return_value_and_update_state(enum test_status result, int state, int command) {
	CurrentState = state;
	Command = command;
	return result;
}


enum test_status validate_message(struct Message* msg) {
	char* message = msg->text_message;
	switch (msg->direction) {
	case A_TO_B: {
		switch (CurrentState) {
		case 1: {
			if (strcmp(message, "CONNECT") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 2, 0);
			}
			return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
		}
		case 3: {
			if (strcmp(message, "GET_VER") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 4, 0);
			}

			if (strcmp(message, "GET_DATA") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 5, 1);
			}

			if (strcmp(message, "GET_COMMAND") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 5, 2);

			}
			if (strcmp(message, "GET_FILE") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 5, 3);
			}

			if (strcmp(message, "GET_B64") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 6, 0);
			}

			if (strcmp(message, "DISCONNECT") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 7, 0);
			}
			return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
		}
		} //switch CUR
	}
			   break;
	case B_TO_A: {
		switch (CurrentState) {
		case 2: {
			if (strcmp(message, "CONNECT_OK") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 3, 0);
			}
			return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
		}

		case 4: {
			if (strncmp(message, "VERSION ", 8) == 0) {
				int num = 8;
				if (numbers[message[num]] != 1) {
					return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
				}
				num++;
				while (message[num] != '\0') {
					if (numbers[message[num]] != 1) {
						return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
					}
					num++;
				}
				return get_return_value_and_update_state(MESSAGE_VALID, 3, 0);
			}
			return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
		}

		case 5: {
			switch (Command) {
			case 1: {
				char* p = message;
				if (strncmp(p, "GET_DATA ", 9) != 0) {
					return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
				}
				p += 9;
				while (data[*p] != 0) {
					++p;
				}
				if (strncmp(p, " GET_DATA", 9) != 0) {
					return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
				}
				return get_return_value_and_update_state(MESSAGE_VALID, 3, 0);
			}

			case 2: {
				char* p = message;
				if (strncmp(p, "GET_COMMAND ", 12) != 0) {
					return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
				}
				p += 12;
				while (data[*p] != 0) {
					++p;
				}
				if (strncmp(p, " GET_COMMAND", 12) != 0) {
					return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
				}
				return get_return_value_and_update_state(MESSAGE_VALID, 3, 0);
			}

			case 3: {
				char* p = message;
				if (strncmp(p, "GET_FILE ", 9) != 0) {
					return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
				}
				p += 9;
				while (data[*p] != 0) {
					++p;
				}
				if (strncmp(p, " GET_FILE", 9) != 0) {
					return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
				}
				return get_return_value_and_update_state(MESSAGE_VALID, 3, 0);
			}

			} //switch Command
			return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
		}

		case 6: {
			if (validate_b64(message)) {
				return get_return_value_and_update_state(MESSAGE_VALID, 3, 0);
			}
			return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
		}

		case 7: {
			if (strcmp(message, "DISCONNECT_OK") == 0) {
				return get_return_value_and_update_state(MESSAGE_VALID, 1, 0);
			}
			return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
		}
		} //switch CUR
	}
			   break;
	} //switch direction
	return get_return_value_and_update_state(MESSAGE_INVALID, 1, 0);
}