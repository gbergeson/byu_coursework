/* $begin proxymain */
/*
 * proxy.c - A simple proxy webserver
 */
#define __MAC_OS_X
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define NTHREADS 4
#define SBUFSIZE 16
#include "csapp.h"
#include "sbuf.h"
#include <errno.h>

struct Header {
    char value[1024];
    struct Header *next;
};
struct Header *first_header;

struct cache_entry {
    char * request;
    int object_size;
    int response_size;
    char * response;
    struct cache_entry *next;
    sem_t mutex;
    int read_since_last_write;
} ;

struct cache {
    int size;
    int readcnt;
    sem_t mutex, w; //Protect accessess to the cache
    struct cache_entry *first;
}; 
struct cache *my_cache;

void        *run_log(void *vargp);
void        *thread(void *vargp);
void		doit      (int fd);
void		read_requesthdrs(rio_t * rp);
int		    parse_uri  (char *uri, char *filename, char *cgiargs);
void		serve_static(int fd, char *filename, int filesize);
void		get_filetype(char *filename, char *filetype);
void		serve_dynamic(int fd, char *filename, char *cgiargs);
void        clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void        serve_proxy_request(int fd, char *webpage, char *filename, struct cache_entry * current_entry);
int         cache_serve_success(int fd, struct cache_entry * current_entry);
void        write_cache_entry(struct cache_entry * entry);
void        free_cache_entry(struct cache_entry * entry);
void        build_request(char * request, char * webpage, char * filename);
struct Header * create_header();
sbuf_t thread_buf, log_queue;

int main(int argc, char **argv){
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    my_cache = Malloc(sizeof(struct cache));
    my_cache->size = 0;
    my_cache->readcnt = 0;
    Sem_init(&my_cache->mutex, 0, 1);
    Sem_init(&my_cache->w, 0, 1);
    my_cache->first = NULL;

    listenfd = Open_listenfd(argv[1]);

    sbuf_init(&thread_buf, SBUFSIZE);
    sbuf_init(&log_queue, SBUFSIZE);
    Pthread_create(&tid, NULL, run_log, NULL);

    for (int i = 0; i < NTHREADS; i++)
        Pthread_create(&tid, NULL, thread, NULL);

    while(1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        sbuf_insert(&thread_buf, connfdp);
    }
}
/* $end proxymain */
void *run_log(void *vargp) {
    Pthread_detach(pthread_self());
    Free(vargp);
    
    char * log_text;
    char *log_file_path = "log.txt";
    FILE *log_file = fopen(log_file_path, "a");

    while (1) {
          log_text = (char *) sbuf_remove(&log_queue);
          fputs("Request received for: ", log_file);
          fputs(log_text, log_file);
          fputs("\n", log_file);
          fflush(log_file); //Flush so it's written in case of interrupt
    }
}

void *thread(void *vargp) {
    Pthread_detach(pthread_self());
    Free(vargp);
    while (1) {
        int *connfd = sbuf_remove(&thread_buf);
        doit(*connfd);
        Close(*connfd);
    }
}


/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
void doit(int fd){
	int		is_direct;
	struct stat	sbuf;
	char		buf       [MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char		filename  [MAXLINE], webpage[MAXLINE];
	rio_t		rio;

	/* Read request line and headers */
	Rio_readinitb(&rio, fd);
	if (!Rio_readlineb(&rio, buf, MAXLINE))
			return;
	printf("%s", buf);
	sscanf(buf, "%s %s %s", method, uri, version);
   
    /* Add log message for this request */
        sbuf_insert(&log_queue, uri);

		if (strcasecmp(method, "GET")) {
			clienterror(fd, method, "501", "Not Implemented",
				    "Proxy does not implement this method");
            return;
        }   
		read_requesthdrs(&rio);
        struct Header * ptr = first_header;
        while(ptr != NULL) {
            printf("%s", ptr->value);
            ptr = ptr->next;
        }
        
	/* Parse URI from GET request */
		is_direct = parse_uri(uri, filename, webpage);

		if (is_direct) {			/* Serve static content */
            if (stat(filename, &sbuf) < 0) {
                clienterror(fd, filename, "404", "Not found",
                        "Proxy couldn't find this file");
                return;
            } else if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
				clienterror(fd, filename, "403", "Forbidden",
					    "Proxy couldn't read the file");
                return;
            }
            serve_static(fd, filename, sbuf.st_size);
        } else {

        /* Start cache entry for this request */
            struct cache_entry * current_entry = Malloc(sizeof(struct cache_entry));

            current_entry->request = Malloc(strlen(uri));
            strncpy(current_entry->request, uri, strlen(uri));

            current_entry->response = NULL;
            
            Sem_init(&current_entry->mutex, 0, 1);

            current_entry->read_since_last_write = 0;

            serve_proxy_request(fd, webpage, filename, current_entry);
        }
}
/* $end doit */

/*
 * read_requesthdrs - read HTTP request headers
 */
/* $begin read_requesthdrs */
void read_requesthdrs(rio_t * rp){
    char buf[MAXLINE];
    struct Header *ptr;

    first_header = malloc(sizeof(struct Header));
    ptr = first_header;
	rio_readlineb(rp, buf, MAXLINE);
    memcpy(ptr->value, buf, strlen(buf));
	while (strcmp(buf, "\r\n"))
	{
        rio_readlineb(rp, buf, MAXLINE);
        ptr->next = malloc(sizeof(struct Header));
        ptr = ptr->next;
        memcpy(ptr->value, buf, strlen(buf));
	}
}
/* $end read_requesthdrs */

/*
 * parse_uri - parse URI into filename and CGI args return 0 if dynamic
 * content, 1 if static
 */
/* $begin parse_uri */
int parse_uri(char *uri, char *filename, char *webpage){
	char *ptr;
    char *temp;

	if (!strstr(uri, "http://")) {			/* Direct request */
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri) - 1] == '/')
            strcat(filename, "home.html");
        return 1;
	} else {                                /* Proxy request */
        ptr = strstr(uri, "/");
        ptr += 2; // Move pointer by two to get past double slash
        strcpy(webpage, ptr);
        temp = strstr(ptr, "/");
        if (temp == NULL) {
            strcpy(filename, "/");
        } else {
            strcpy(filename, temp);
            strcpy(webpage + strlen(webpage) - strlen(filename) , "\0");
        }
        return 0;
    }
}
/* $end parse_uri */

/*
 * serve_static - copy a file back to the client
 */
/* $begin serve_static */
void serve_static(int fd, char *filename, int filesize) {
	int		srcfd;
	char           *srcp, filetype[MAXLINE], buf[MAXBUF];

	/* Send response headers to client */
	get_filetype(filename, filetype);
//line: netp: servestatic:getfiletype
		sprintf(buf, "HTTP/1.0 200 OK\r\n");
//line: netp: servestatic:beginserve
		sprintf(buf, "%sServer: Proxy Web Server\r\n", buf);
	sprintf(buf, "%sConnection: close\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(fd, buf, strlen(buf));
//line: netp: servestatic:endserve
		printf("Response headers:\n");
	printf("%s", buf);

	/* Send response body to client */
	srcfd = Open(filename, O_RDONLY, 0);
//line: netp: servestatic:open
		srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
//line: netp: servestatic:mmap
		Close(srcfd);
//line: netp: servestatic:close
		Rio_writen(fd, srcp, filesize);
//line: netp: servestatic:write
		Munmap(srcp, filesize);
//line: netp: servestatic:munmap
}

/*
 * get_filetype - derive file type from file name
 */
void get_filetype(char *filename, char *filetype) {
	if (strstr(filename, ".html"))
		strcpy(filetype, "text/html");
	else if (strstr(filename, ".gif"))
		strcpy(filetype, "image/gif");
	else if (strstr(filename, ".png"))
		strcpy(filetype, "image/png");
	else if (strstr(filename, ".jpg"))
		strcpy(filetype, "image/jpeg");
	else
		strcpy(filetype, "text/plain");
}
/* $end serve_static */

void serve_proxy_request(int fd, char *webpage, char *filename, struct cache_entry * current_entry) {
    if (!cache_serve_success(fd, current_entry)) {
        //Build Request
        char port[8];
        strcpy(port, "80");  //Default to port 80
        char * temp_port = strstr(webpage, ":"); 
        if (temp_port) {
            strcpy(port, temp_port + 1); //Remove the preceding colon
            strcpy(temp_port, "\0");  //End the webpage string where the port was
        }
        char * request = malloc(2048*sizeof(char));
        build_request(request, webpage, filename);

        //Make connection, send request
        int endpoint_fd = open_clientfd(webpage, port);
        rio_writen(endpoint_fd, request, strlen(request));

        //Read response
        char buf[MAXLINE];
        char * response = NULL;
        rio_t * rio = malloc(sizeof(rio_t));
        rio_readinitb(rio, endpoint_fd);
        size_t n;
        int total = 0;

        while((n = rio_readnb(rio, buf, MAXLINE)) != 0) {
            if (n == -1) {
                strcpy(buf, "The server encountered an error\n");
                break;
            }
            total += n;

            response = realloc(response, total*sizeof(char));
            if (response == NULL) {
                strcpy(buf, "The server encountered an error\n");
                break;
            }
            memcpy(response + (total - n), buf, n);
        }

        //Send response to client
        rio_writen(fd, response, total);

        char * head_end = strstr(response, "\r\n\r\n");
        int object_size = strlen(head_end + 4);

        //Write response to cache if small enough
        if (object_size < MAX_OBJECT_SIZE) {
            current_entry->object_size = object_size;
            current_entry->response_size = total;
            current_entry->response = Malloc(total);
            memcpy(current_entry->response, response, total);

            write_cache_entry(current_entry);
        }
    }
}

int cache_serve_success(int fd, struct cache_entry * current_entry) {
    //make sure to free current entry if it's a success
    P(&my_cache->mutex);
    my_cache->readcnt++;
    if (my_cache->readcnt == 1)
        P(&my_cache->w);
    V(&my_cache->mutex);

    struct cache_entry * ptr = my_cache->first;
    while(ptr != NULL) {
        if (strcmp(current_entry->request, ptr->request) == 0) { //If found same request in cache
            ptr->read_since_last_write = 1;
            rio_writen(fd, ptr->response, ptr->response_size);
            free_cache_entry(current_entry);
            return 1;
        }

        ptr = ptr->next;
    }

    P(&my_cache->mutex);
    my_cache->readcnt--;
    if(my_cache->readcnt == 0)
        V(&my_cache->w);
    V(&my_cache->mutex);

    return 0;
}

void write_cache_entry(struct cache_entry * entry) {
    P(&my_cache->w);

    //Move those read since last write to the beginning of the cache
    struct cache_entry * ptr = my_cache->first;
    struct cache_entry * prev = NULL;
    while (ptr != NULL) {
        if (ptr->read_since_last_write) {
            if (ptr != my_cache->first) {
                prev->next = ptr->next;
                ptr->next = my_cache->first;
                my_cache->first = ptr;
            }

            ptr->read_since_last_write = 0;
        }

        prev = ptr;
        ptr = ptr->next;
    }

    //Add code here for removing items when cache gets too big
    while (my_cache->size + entry->object_size > MAX_CACHE_SIZE) {
        struct cache_entry * last = my_cache->first;
        struct cache_entry * penult = NULL;
        while (last->next != NULL) {
            penult = last;
            last = last->next;
        }

        my_cache->size -= last->object_size;
        penult->next = NULL;
        free_cache_entry(last);
    }
    
    //Always insert at the beginning as per our LRU policy
    entry->next = my_cache->first;
    my_cache->first = entry;

    my_cache->size += entry->object_size;

    V(&my_cache->w);
}

void free_cache_entry(struct cache_entry * entry) {
    if (entry->request != NULL)
        Free(entry->request);
    if (entry->response != NULL)
        Free(entry->response);

    Free(entry);
}

void build_request(char * request, char * webpage, char * filename) {
    //Build first line
    strcat(request, "GET ");
    strcat(request, filename);
    strcat(request, " HTTP/1.1\r\n");

    //Overwrite headers if necessary
    char * user_header = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
    char * conn_header = "Connection: close\r\n";
    char * proxy_header = "Proxy-Connection: close\r\n";

    struct Header *ptr = first_header;
    int add_host = 1;
    int add_user = 1;
    int add_conn = 1;
    int add_prox = 1;
    while (ptr) {
        if (strstr(ptr->value, "Host: ")) {  //If we find a Host header, no need to add one
            add_host = 0;
        } else if (strstr(ptr->value, "User-Agent: ")) {  //If we find a User-Agent header, replace it
            strcpy(ptr->value, user_header);
            add_user = 0;
        } else if (strstr(ptr->value, "Connection: ")) {  
            strcpy(ptr->value, conn_header);
            add_conn = 0;
        } else if (strstr(ptr->value, "Proxy-Connection: ")) {  
            strcpy(ptr->value, proxy_header);
            add_prox = 0;
        }
        ptr = ptr->next;
    }
    
    //Add headers
    if (add_host) {
        strcat(request, "Host: ");
        strcat(request, webpage);
        strcat(request, "\r\n");
    }
    if (add_user)
        strcat(request, user_header);
    if (add_conn)
        strcat(request, conn_header);
    if (add_prox)
        strcat(request, proxy_header);
    
    ptr = first_header;
    int old_size = 2048;  //Request initial size is 2048
    while (ptr) {
        strcat(request, ptr->value);
        ptr = ptr->next;
        if (strlen(request) >= old_size) {
            Realloc(request, old_size + 1024);
            old_size += 1024;
        }
    }

    //End request
    strcat(request, "\r\n");
}
/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
	char		buf       [MAXLINE], body[MAXBUF];

	/* Build the HTTP response body */
	sprintf(body, "<html><title>Proxy Error</title>");
	sprintf(body, "%s<body bgcolor=" "ffffff" ">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Proxy Web server</em>\r\n", body);

	/* Print the HTTP response */
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen(fd, buf, strlen(buf));
	Rio_writen(fd, body, strlen(body));
}
/* $end clienterror */
