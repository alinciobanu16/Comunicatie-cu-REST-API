#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params, char *token,
                            char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    // add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
    	char *buff = calloc(BUFLEN, sizeof(char));
       	sprintf(line, "Cookie:");
        for (int i = 0; i < cookies_count; i++) {
        	sprintf(buff, " %s", cookies[i]);
       		if (i > 0) {
       			strcat(line, ";");
       		}
       		strcat(line, buff);
       }
       compute_message(message, line);
       free(buff);
    }
    // add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *token, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    /* add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    char *buff = calloc(BUFLEN, sizeof(char));
    for (int i = 0; i < body_data_fields_count; i++) {
    	if (i > 0) {
    		sprintf(buff, "&%s", body_data[i]);
    	} else {
    		sprintf(buff, "%s", body_data[i]);
    	}
    	strcat(body_data_buffer, buff);
    }
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer) + 1);
    compute_message(message, line);
    // add cookies
    memset(line, 0, LINELEN);
    if (cookies != NULL) {
    	sprintf(line, "Cookie:");
        for (int i = 0; i < cookies_count; i++) {
        	sprintf(buff, " %s", cookies[i]);
       		if (i > 0) {
       			strcat(line, ";");
       		}
       		strcat(line, buff);
       }
       compute_message(message, line);
    }
    free(buff);
    // add new line at end of header
    compute_message(message, "");
    // add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}


char *compute_delete_request(char *host, char *url, char *query_params, char *token,
                            char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    // add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        char *buff = calloc(BUFLEN, sizeof(char));
        sprintf(line, "Cookie:");
        for (int i = 0; i < cookies_count; i++) {
            sprintf(buff, " %s", cookies[i]);
            if (i > 0) {
                strcat(line, ";");
            }
            strcat(line, buff);
       }
       compute_message(message, line);
       free(buff);
    }
    // add final new line
    compute_message(message, "");
    return message;
}
