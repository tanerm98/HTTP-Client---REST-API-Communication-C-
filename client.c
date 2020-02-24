#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <sys/types.h>
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"
#include "helpers.h"

// folosit pentru a parsa raspunsurile in format JSON ale task-urilor 1-3
void parseData (char* response, char* url, char* params, char* method, char* content_type,
	char* username, char* password, char* token, char* id) {

	char *dataString;
	
	dataString = strchr (response, '{');
	
	JSON_Value *value = json_parse_string (dataString);
	JSON_Object *object = json_value_get_object (value);
	
	if (json_object_get_string(object, "url") != NULL) {
		strcpy (url, json_object_get_string(object, "url"));
	}
	
	if (json_object_get_string(object, "method") != NULL) {
		strcpy (method, json_object_get_string(object, "method"));
	}
	
	if (json_object_get_string(object, "type") != NULL) {
		strcpy (content_type, json_object_get_string(object, "type"));
	}
	
	
	JSON_Object *data = json_object_get_object (object, "data");
	
	if (json_object_get_string(data, "username") != NULL) {
		strcpy (username, json_object_get_string(data, "username"));
	}
	
	if (json_object_get_string(data, "password") != NULL) {
		strcpy (password, json_object_get_string(data, "password"));
	}
	
	if (json_object_get_string(data, "token") != NULL) {
		strcpy (token, json_object_get_string(data, "token"));
	}
	
	
	JSON_Object *queryParams = json_object_get_object (data, "queryParams");
	
	if (json_object_get_string(queryParams, "id") != NULL) {
		strcpy (id, json_object_get_string(queryParams, "id"));
	}
	
}

// folosit pentru a parsa raspunsurile in format JSON ale task-ului 4
void parseMoreData (char* response, char* url, char* params, char* method, 
	char* content_type, char* url2, char* q, char* APPID, char* method2) {

	char *dataString;
	
	dataString = strchr (response, '{');
	
	JSON_Value *value = json_parse_string (dataString);
	JSON_Object *object = json_value_get_object (value);
	
	if (json_object_get_string(object, "url") != NULL) {
		strcpy (url, json_object_get_string(object, "url"));
	}
	
	if (json_object_get_string(object, "method") != NULL) {
		strcpy (method, json_object_get_string(object, "method"));
	}
	
	if (json_object_get_string(object, "type") != NULL) {
		strcpy (content_type, json_object_get_string(object, "type"));
	}
	
	
	JSON_Object *data = json_object_get_object (object, "data");
	
	if (json_object_get_string(data, "url") != NULL) {
		strcpy (url2, json_object_get_string(data, "url"));
	}
	
	if (json_object_get_string(data, "method") != NULL) {
		strcpy (method2, json_object_get_string(data, "method"));
	}
	
	JSON_Object *queryParams = json_object_get_object (data, "queryParams");
	
	if (json_object_get_string(queryParams, "q") != NULL) {
		strcpy (q, json_object_get_string(queryParams, "q"));
	}
	
	if (json_object_get_string(queryParams, "APPID") != NULL) {
		strcpy (APPID, json_object_get_string(queryParams, "APPID"));
	}
	
}

// salveaza cookie-urile din mesajul <message> in <cookies>
void parseCookies (char *message, char *cookies) {
	
	char *cookie, aux[1000];
	
	memset (cookies, 0, 5000);
	
	cookie = strstr (message, "Set-Cookie: ");
	
	strcpy (cookies, "Cookie: ");
	
	while (cookie != NULL) {
		char *p = cookie;
		while (1) {
			if ((*p == 0) || (*p == '\n') || (*p == '\r')) {
				int len = p - cookie;
				strncpy (aux, cookie + 12, len - 12);
				aux[len - 12] = 0;

				strcat (cookies, aux);
				strcat (cookies, "; ");
				break;
			} else {
				p++;
			}
		}
		
		cookie = strstr (cookie + 1, "Set-Cookie: ");
	}
	
	strcat (cookies, "\r\n");
	
}


// creeaza un request de tip POST
char* compute_post_request(char* url, char* url_params, char* host, 
	char* content_type, char* cookies, char* body, char* authorization) {

	char *message = (char*) calloc (BUFLEN, sizeof(char));
    char *line = (char*) calloc (LINELEN, sizeof(char));
    
    //metoda, tipul de http si URL-ul
    sprintf(line, "POST %s?%s HTTP/1.1", url, url_params);
    compute_message (message, line);
    memset (line, 0, LINELEN*sizeof(char) );
    
    //serverul
    sprintf (line, "Host: %s", host);
    compute_message (message, line);
    memset (line, 0, LINELEN*sizeof(char) );
    
    //content-type-ul
    sprintf (line, "Content-Type: %s", content_type);
    compute_message (message, line);
    memset (line, 0, LINELEN*sizeof(char) );
    
    //content-length-ul
    sprintf (line, "Content-Length: %ld", strlen(body));
    compute_message (message, line);
    memset (line, 0, LINELEN*sizeof(char) );
    
    //autorizatia
    if (authorization[0] != 0) {
		sprintf (line, "Authorization: %s", authorization);
		compute_message (message, line);
		memset (line, 0, LINELEN*sizeof(char) );
	}
    
    //cookie-urile
    if (cookies[0] != 0) {
		sprintf(line, "%s", cookies);
		compute_message (message, line);
		memset (line, 0, LINELEN*sizeof(char) );
	} else {
		strcat (message, "\r\n");
	}
    
    //fields & values
    sprintf (line, "%s", body);
    compute_message (message, line);
    memset (line, 0, LINELEN*sizeof(char) );
    
    return message;
    
}


// creeaza un request de tip GET
char* compute_get_request(char* url, char* url_params, char* host, char* cookies,
	char* authorization) {

	char *message = (char*) calloc (BUFLEN, sizeof(char));
    char *line = (char*) calloc (LINELEN, sizeof(char));
    
    //metoda, tipul de http si URL-ul
    sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    compute_message (message, line);
    memset (line, 0, LINELEN*sizeof(char) );
    
    //serverul
    sprintf (line, "Host: %s", host);
    compute_message (message, line);
    memset (line, 0, LINELEN*sizeof(char) );
    
    //autorizatia
    if (authorization[0] != 0) {
		sprintf (line, "Authorization: %s", authorization);
		compute_message (message, line);
		memset (line, 0, LINELEN*sizeof(char) );
	}
    
    //cookie-urile
    if (cookies[0] != 0) {
		sprintf(line, "%s", cookies);
		compute_message (message, line);
		memset (line, 0, LINELEN*sizeof(char) );
	} else {
		strcat (message, "\r\n");
	}
    
    return message;
    
}


// obtine ip-ul din numele unui server
void get_ip (char* host, char ip[100]) {

	struct addrinfo hints, *res;

	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo (host, NULL, &hints, &res);

	//while (res) {
		inet_ntop (res->ai_family, &((struct sockaddr_in *) res->ai_addr)->sin_addr, ip, 100);
		//res = res->ai_next;
	//}

	freeaddrinfo(res);		

	return;

}


int main(int argc, char *argv[]) {

	// mesajele si raspunsurile query-urilor HTTP
    char *message = (char*) calloc (BUFLEN, sizeof(char));
    char *response = (char*) calloc (BUFLEN, sizeof(char));;
    
    // socketul folosit si hostul cerut
    int sockfd;
	char *host ="185.118.200.35";
	
	// parametri care vor fi primiti in body-ul query-urilor
	char url[256], params[256], method[256], content_type[256];
	char fieldValues[5000], username[256], password[256];
	char token[1000], id[100], authorization[500];
	char host2[100], url2[100], method2[100], q[100], APPID[100];
	char empty[2];
	
	// empty e folosit in loc de NULL
	memset (empty, 0, 2);
	char *cookies = (char*) malloc (5000 * sizeof(char));


	// TASK 1
	memset (message, 0, BUFLEN*sizeof(char));
    memset (response, 0, BUFLEN*sizeof(char));
    memset (cookies, 0, 5000);
    memset (params, 0, 256);
    
    strcpy (url, "/task1/start");

    sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(url, params, host, cookies, authorization);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    printf("%s\n\n",response);
    
    parseCookies (response, cookies);
    parseData (response, url, params, method, content_type, username, password, token, id);
    
    sprintf (fieldValues, "username=%s&password=%s", username, password);
    

    // TASK 2
    memset (message, 0, BUFLEN*sizeof(char));
    memset (response, 0, BUFLEN*sizeof(char));
    
    if (strcmp(method, "POST") == 0) {
		sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
		message = compute_post_request(url, params, host, content_type, 
					cookies, fieldValues, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	} else {
		sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
		message = compute_get_request(url, params, host, cookies, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	}
	
	parseCookies (response, cookies);
    parseData (response, url, params, method, content_type, username, password, token, id);
    
    sprintf (params, "raspuns1=omul&raspuns2=numele&id=%s", id);
    
    sprintf (authorization, "Token: %s", token);
    
    
    // TASK 3
    memset (message, 0, BUFLEN*sizeof(char));
    memset (response, 0, BUFLEN*sizeof(char));
    memset (fieldValues, 0, 5000);
    
    if (strcmp(method, "POST") == 0) {
		sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
		message = compute_post_request(url, params, host, content_type, 
					cookies, fieldValues, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	} else {
		sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
		message = compute_get_request(url, params, host, cookies, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	}
	
	parseCookies (response, cookies);
    parseData (response, url, params, method, content_type, username, password, token, id);
    
    
    // TASK 4
    memset (message, 0, BUFLEN*sizeof(char));
    memset (response, 0, BUFLEN*sizeof(char));
    memset (params, 0, 256);
    
    if (strcmp(method, "POST") == 0) {
		sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
		message = compute_post_request(url, params, host, content_type, 
					cookies, fieldValues, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	} else {
		sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
		message = compute_get_request(url, params, host, cookies, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	}
	
	parseCookies (response, cookies);
    parseMoreData (response, url, params, method, content_type, url2, q, APPID, method2);
    
    sprintf (params, "q=%s&APPID=%s", q, APPID);
    
    // se apeleaza serverul DNS pentru a obtine IP-ul hostului cu numele <host2>
    char url3[100];
    strcpy (url3, url2);
    char *end = strchr (url3, '/');
    *end = 0;
    get_ip (url3, host2);
    *end = '/';
	
	
	// TASK 5
    memset (message, 0, BUFLEN*sizeof(char));
    memset (response, 0, BUFLEN*sizeof(char));
    
    //weather http server
    if (strcmp(method2, "POST") == 0) {
		sockfd =  open_connection(host2, 80, AF_INET, SOCK_STREAM, 0);
		message = compute_post_request(end, params, host2, content_type, 
					empty, fieldValues, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	} else {
		sockfd =  open_connection(host2, 80, AF_INET, SOCK_STREAM, 0);
		message = compute_get_request(end, params, host2, empty, empty);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	}
	
	// raspunsul primit de la serverul de vreme
	char *weatherInfo;
	weatherInfo = strchr (response, '{');
	
	//host server
	if (strcmp(method, "POST") == 0) {
		sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
		message = compute_post_request(url, params, host, content_type, 
					cookies, weatherInfo, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	} else {
		sockfd =  open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
		message = compute_get_request(url, params, host, cookies, authorization);
		send_to_server(sockfd, message);
		response = receive_from_server(sockfd);
		close_connection(sockfd);
		printf("%s\n\n",response);
	}


    free (message);
    free (response);
    return 0;
}
