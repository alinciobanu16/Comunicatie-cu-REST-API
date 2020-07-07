#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#define HOST "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com" 
#define LEN 100

int main(int argc, char *argv[]) {
	char *message;
	char *response;
	int sockfd, logged = 0;

	char command[BUFLEN], username[LEN], password[LEN];
	char *token = NULL;
	char **data = calloc(1, sizeof(char*));
	data[0] = calloc(1, sizeof(char));

	char **cookies = calloc(1, sizeof(char *));
	cookies[0] = calloc(1, sizeof(char));

	char *host_ip = "3.8.116.10";
	sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
  
	while (1) {
		memset(command, 0, BUFLEN);
		fgets(command, BUFLEN - 1, stdin);
		if (strcmp(command, "exit\n") == 0) {
			break;
		}

		if (strcmp(command, "register\n") == 0) {
			printf("username=");
			fgets(username, LEN - 1, stdin);
			username[strlen(username) - 1] = '\0';
			printf("password=");
			fgets(password, LEN - 1, stdin);
			password[strlen(password) - 1] = '\0';

			JSON_Value *root_value = json_value_init_object();
			JSON_Object *root_object = json_value_get_object(root_value);
			data[0] = NULL;
			json_object_set_string(root_object, "username", username);
			json_object_set_string(root_object, "password", password);
			data[0] = json_serialize_to_string_pretty(root_value);

			message = compute_post_request(HOST, "/api/v1/tema/auth/register",
											 "application/json", NULL, data, 1, NULL, 0);
			printf("%s\n", message);
			sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("\n%s\n", response);
			json_free_serialized_string(data[0]);
			json_value_free(root_value);
		}

		if (strcmp(command, "login\n") == 0) {
			if (logged == 1) {
				printf("A user is already logged\n\n");
				continue;
			}
			logged = 1;
			printf("username=");
			fgets(username, LEN - 1, stdin);
			username[strlen(username) - 1] = '\0';	
			printf("password=");
			fgets(password, LEN - 1, stdin);
			password[strlen(password) - 1] = '\0';
			/*creez obiectul json cu username si password si il pun sub forma de string in data[0]*/
			JSON_Value *root_value = json_value_init_object();
			JSON_Object *root_object = json_value_get_object(root_value);
			data[0] = NULL;
			json_object_set_string(root_object, "username", username);
			json_object_set_string(root_object, "password", password);
			data[0] = json_serialize_to_string_pretty(root_value);

			message = compute_post_request(HOST, "/api/v1/tema/auth/login",
											"application/json", NULL, data, 1, NULL, 0);
			sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("\n%s", response);
			json_free_serialized_string(data[0]);
			json_value_free(root_value);

			if (strstr(response, "200 OK") == NULL) {
				printf("\n\n");
				continue;
			}
			/*memorez cookie-ul in cookies[0]*/
			char *str;
			str = strstr(response, "connect.sid");
			cookies[0] = strtok(str, ";");
		}
		
		if (strcmp(command, "enter_library\n") == 0) {
			message = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, NULL, cookies, 1);
			sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("\n%s\n\n", response);
			if (strstr(response, "200 OK") == NULL) {
				continue;
			}
			/*extrag token-ul din raspunsul server-ului si il salvez in token*/
			token = strstr(basic_extract_json_response(response), ":");
			token = strstr(token, "\"");
			token = strtok(token, "\"");
		}

		if (strcmp(command, "get_books\n") == 0) {
			message = compute_get_request(HOST, "/api/v1/tema/library/books", NULL, token, NULL, 0);
			sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("\n%s\n\n", response);
		}

		if (strcmp(command, "get_book\n") == 0) {
			char id[LEN];
			printf("id=");
			fgets(id, LEN - 1, stdin);
			id[strlen(id) - 1] = '\0';
			char url[LEN];
			sprintf(url, "/api/v1/tema/library/books/%s", id);
			message = compute_get_request(HOST, url, NULL, token, NULL, 0);
			sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("\n%s\n\n", response);
		}

		if (strcmp(command, "add_book\n") == 0) {
			char title[LEN], author[LEN], genre[LEN], publisher[LEN], page[LEN];
			int page_count, ok = 1;
			printf("title=");
			fgets(title, LEN - 1, stdin);
			title[strlen(title) - 1] = '\0';
			printf("author=");
			fgets(author, LEN - 1, stdin);
			author[strlen(author) - 1] = '\0';
			printf("genre=");
			fgets(genre, LEN - 1, stdin);
			genre[strlen(genre) - 1] = '\0';
			printf("publisher=");
			fgets(publisher, LEN - 1, stdin);
			publisher[strlen(publisher) - 1] = '\0';
			printf("page_count=");
			fgets(page, LEN - 1, stdin);
			page[strlen(page) - 1] = '\0';
			for (int i = 0; page[i]; i++) {
				if (page[i] < '0' || page[i] > '9' || (page[0] == '0' && strlen(page) > 1)) {
					printf("page_count must be a positive number\n\n");
					ok = 0;
					break;
				}
			}
			if (ok == 0) {
				continue;
			}
			page_count = atoi(page);	
			JSON_Value *root_value = json_value_init_object();
			JSON_Object *root_object = json_value_get_object(root_value);
			data[0] = NULL;
			json_object_set_string(root_object, "title", title);
			json_object_set_string(root_object, "author", author);
			json_object_set_string(root_object, "genre", genre);
			json_object_set_string(root_object, "publisher", publisher);
			json_object_set_number(root_object, "page_count", page_count);
			data[0] = json_serialize_to_string_pretty(root_value);
			message = compute_post_request(HOST, "/api/v1/tema/library/books",
											"application/json", token, data, 1, NULL, 0);
			sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("\n%s", response);
			json_free_serialized_string(data[0]);
			json_value_free(root_value);
			if (strstr(response, "error") != NULL) {
				printf("\n\n");
				continue;
			}	
		}

		if (strcmp(command, "delete_book\n") == 0) {
			char id[LEN];
			int ok = 1;
			printf("id=");
			fgets(id, LEN - 1, stdin);
			id[strlen(id) - 1] = '\0';
			for (int i = 0; id[i]; i++) {
				if (id[i] < '0' || id[i] > '9') {
					printf("id must be a positive number\n\n");
					ok = 0;
					break;
				}
			}
			if (ok == 0) {
				continue;
			}
			char url[LEN];
			sprintf(url, "/api/v1/tema/library/books/%s", id);
			message = compute_delete_request(HOST, url, NULL, token, NULL, 0);
			sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("\n%s", response);
			if (strstr(response, "error") != NULL) {
				printf("\n\n");
				continue;
			}
		}

		if (strcmp(command, "logout\n") == 0) {
			message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, NULL, cookies, 1);
			sockfd = open_connection(host_ip, 8080, AF_INET, SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("\n%s", response);
			token = NULL;
			if (strstr(response, "error") != NULL) {
				printf("\n\n");
				continue;
			}
			logged = 0;
		}	
	}
	
	close_connection(sockfd);
	free(data[0]);
	free(data);

	return 0;
}
