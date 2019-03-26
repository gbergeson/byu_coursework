#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
extern int errno;
int MAX_BUF_LEN = 1024;
int MAX_RDATA_SIZE = 256;

typedef unsigned int dns_rr_ttl;
typedef unsigned short dns_rr_type;
typedef unsigned short dns_rr_class;
typedef unsigned short dns_rdata_len;
typedef unsigned short dns_rr_count;
typedef unsigned short dns_query_id;
typedef unsigned short dns_flags;

typedef struct {
	char *name;
	dns_rr_type type;
	dns_rr_class class;
	dns_rr_ttl ttl;
	dns_rdata_len rdata_len;
	unsigned char * rdata;
} dns_rr;

struct dns_answer_entry;
struct dns_answer_entry {
	char *value;
	struct dns_answer_entry *next;
};
typedef struct dns_answer_entry dns_answer_entry;

void print_bytes(unsigned char *bytes, int byteslen) {
	int i, j, byteslen_adjusted;
	unsigned char c;

	if (byteslen % 8) {
		byteslen_adjusted = ((byteslen / 8) + 1) * 8;
	} else {
		byteslen_adjusted = byteslen;
	}
	for (i = 0; i < byteslen_adjusted + 1; i++) {
		if (!(i % 8)) {
			if (i > 0) {
				for (j = i - 8; j < i; j++) {
					if (j >= byteslen_adjusted) {
						printf("  ");
					} else if (j >= byteslen) {
						printf("  ");
					} else if (bytes[j] >= '!' && bytes[j] <= '~') {
						printf(" %c", bytes[j]);
					} else {
						printf(" .");
					}
				}
			}
			if (i < byteslen_adjusted) {
				printf("\n%02X: ", i);
			}
		} else if (!(i % 4)) {
			printf(" ");
		}
		if (i >= byteslen_adjusted) {
			continue;
		} else if (i >= byteslen) {
			printf("   ");
		} else {
			printf("%02X ", bytes[i]);
		}
	}
	printf("\n");
}

void canonicalize_name(char *name) {
	/*
	 * Canonicalize name in place.  Change all upper-case characters to
	 * lower case and remove the trailing dot if there is any.  If the name
	 * passed is a single dot, "." (representing the root zone), then it
	 * should stay the same.
	 *
	 * INPUT:  name: the domain name that should be canonicalized in place
	 */
	
	int namelen, i;

	// leave the root zone alone
	if (strcmp(name, ".") == 0) {
		return;
	}

	namelen = strlen(name);
	// remove the trailing dot, if any
	if (name[namelen - 1] == '.') {
		name[namelen - 1] = '\0';
	}

	// make all upper-case letters lower case
	for (i = 0; i < namelen; i++) {
		if (name[i] >= 'A' && name[i] <= 'Z') {
			name[i] += 32;
		}
	}
}

int name_ascii_to_wire(char *name, unsigned char *wire) {
	/* 
	 * Convert a DNS name from string representation (dot-separated labels)
	 * to DNS wire format, using the provided byte array (wire).  Return
	 * the number of bytes used by the name in wire format.
	 *
	 * INPUT:  name: the string containing the domain name
	 * INPUT:  wire: a pointer to the array of bytes where the
	 *              wire-formatted name should be constructed
	 * OUTPUT: the length of the wire-formatted name.
	 */
     int indexpt = 0;
     char * delim = ".";
     char * copy = malloc(strlen(name)*sizeof(char));
     strcpy(copy, name);
     unsigned char * str = strtok(copy, delim);

     while (str != NULL) {
        int length = strlen(str);

        memcpy(wire + indexpt, &length, 1);
        indexpt += 1;
        memcpy(wire + indexpt, str, length);
        indexpt += length;

        str = strtok(NULL, delim);
     }
     wire[indexpt++] = 0x00;

     return indexpt;
}

char *name_ascii_from_wire(unsigned char *wire, int *indexp) {
	/* 
	 * Extract the wire-formatted DNS name at the offset specified by
	 * *indexp in the array of bytes provided (wire) and return its string
	 * representation (dot-separated labels) in a char array allocated for
	 * that purpose.  Update the value pointed to by indexp to the next
	 * value beyond the name.
	 *
	 * INPUT:  wire: a pointer to an array of bytes
	 * INPUT:  indexp, a pointer to the index in the wire where the
	 *              wire-formatted name begins
	 * OUTPUT: a string containing the string representation of the name,
	 *              allocated on the heap.
	 */
     int original_index = 0;
     if ((unsigned char)wire[*indexp] == 0xc0) {
        original_index = *indexp;
        *indexp = wire[*indexp + 1];
     }

     int oldindex = 0;
     unsigned char buildStr[MAX_BUF_LEN];

     int currLen = 0;
     while ((unsigned char)wire[*indexp] != '\0') {
        if ((unsigned char)wire[*indexp] == 0xc0) {
           oldindex = *indexp;
           *indexp = wire[*indexp + 1];
        }
        int length = wire[*indexp];
        *indexp += 1;
        memcpy(buildStr + currLen, wire + *indexp, length);
        *indexp += length;
        currLen += length;
        buildStr[currLen++] = '.';
     }

     buildStr[currLen - 1] = '\0';
     unsigned char * result = malloc(currLen*sizeof(char));
     memcpy(result, buildStr, currLen);

     *indexp += 1;

     if (original_index)
        *indexp = original_index + 2;

     return result;
}

dns_rr * rr_from_wire(unsigned char *wire, int *indexp, int query_only) {
	/* 
	 * Extract the wire-formatted resource record at the offset specified by
	 * *indexp in the array of bytes provided (wire) and return a 
	 * dns_rr (struct) populated with its contents. Update the value
	 * pointed to by indexp to the next value beyond the resource record.
	 *
	 * INPUT:  wire: a pointer to an array of bytes
	 * INPUT:  indexp: a pointer to the index in the wire where the
	 *              wire-formatted resource record begins
	 * INPUT:  query_only: a boolean value (1 or 0) which indicates whether
	 *              we are extracting a full resource record or only a
	 *              query (i.e., in the question section of the DNS
	 *              message).  In the case of the latter, the ttl,
	 *              rdata_len, and rdata are skipped.
	 * OUTPUT: the resource record (struct)
	 */

     dns_rr * record = malloc(sizeof(dns_rr));
     record->rdata = malloc(MAX_RDATA_SIZE*sizeof(char));

     record->name = name_ascii_from_wire(wire, indexp);
     memcpy(&record->type, wire + *indexp, sizeof(dns_rr_type));
     record->type = ntohs(record->type);
     *indexp += sizeof(dns_rr_type);
     memcpy(&record->class, wire + *indexp, sizeof(dns_rr_class));
     record->class = ntohs(record->class);
     *indexp += sizeof(dns_rr_class);
     
     if (!query_only) {
        memcpy(&record->ttl, wire + *indexp, sizeof(dns_rr_ttl));
        record->ttl = ntohl(record->ttl);
        *indexp += sizeof(dns_rr_ttl);
        memcpy(&record->rdata_len, wire + *indexp, sizeof(dns_rdata_len));
        record->rdata_len = ntohs(record->rdata_len);
        *indexp += sizeof(dns_rdata_len);
        memcpy(record->rdata, wire + *indexp, record->rdata_len);
        *indexp += record->rdata_len;
     }

     return record;
}


int rr_to_wire(dns_rr rr, unsigned char *wire, int query_only) {
	/* 
	 * Convert a DNS resource record struct to DNS wire format, using the
	 * provided byte array (wire).  Return the number of bytes used by the
	 * name in wire format.
	 *
	 * INPUT:  rr: the dns_rr struct containing the rr record
	 * INPUT:  wire: a pointer to the array of bytes where the
	 *             wire-formatted resource record should be constructed
	 * INPUT:  query_only: a boolean value (1 or 0) which indicates whether
	 *              we are constructing a full resource record or only a
	 *              query (i.e., in the question section of the DNS
	 *              message).  In the case of the latter, the ttl,
	 *              rdata_len, and rdata are skipped.
	 * OUTPUT: the length of the wire-formatted resource record.
	 *
	 */
}

unsigned short create_dns_query(char *qname, dns_rr_type qtype, unsigned char *wire) {
	/* 
	 * Create a wire-formatted DNS (query) message using the provided byte
	 * array (wire).  Create the header and question sections, including
	 * the qname and qtype.
	 *
	 * INPUT:  qname: the string containing the name to be queried
	 * INPUT:  qtype: the integer representation of type of the query (type A == 1)
	 * INPUT:  wire: the pointer to the array of bytes where the DNS wire
	 *               message should be constructed
	 * OUTPUT: the length of the DNS wire message
	 */
     memset(wire, 0, MAX_BUF_LEN);

     unsigned short ID = rand();
     int headerlen = 12;
     unsigned char header[12] = {
        0x00, 0x00, 0x01, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
     };
     memcpy(header, &ID, sizeof(ID));
     memcpy(wire, header, headerlen);
     
     unsigned char name[MAX_BUF_LEN];
     int namelen = name_ascii_to_wire(qname, name); 
     memcpy(wire + headerlen, name, namelen);

     unsigned char type_class[4] = {
        0x00, 0x01, 0x00, 0x01
     };
     unsigned char * current = wire + headerlen + namelen;
     memcpy(current, type_class, 4);
     current += 4;
    
     int querylen = headerlen + namelen + 4;
     return querylen;
}

dns_answer_entry *get_answer_address(char *qname, dns_rr_type qtype, unsigned char *wire) {
	/* 
	 * Extract the IPv4 address from the answer section, following any
	 * aliases that might be found, and return the string representation of
	 * the IP address.  If no address is found, then return NULL.
	 *
	 * INPUT:  qname: the string containing the name that was queried
	 * INPUT:  qtype: the integer representation of type of the query (type A == 1)
	 * INPUT:  wire: the pointer to the array of bytes representing the DNS wire message
	 * OUTPUT: a linked list of dns_answer_entrys the value member of each
	 * reflecting either the name or IP address.  If
	 */
     int index = 4;
     int *indexp = &index;
     short total_qs, total_as;
     dns_answer_entry answerList;

     memcpy(&total_qs, wire+*indexp, sizeof(short));
     total_qs = ntohs(total_qs);
     index += 2;
     memcpy(&total_as, wire+*indexp, sizeof(short));
     total_as = ntohs(total_as);
     index += 6;

     dns_answer_entry * first_ans = NULL;
     dns_answer_entry * current;

     for (int i = 0; i < total_qs; i++) {
        dns_rr * question = rr_from_wire(wire, indexp, 1);
     }
     for (int i = 0; i < total_as; i++) {
        dns_rr * a_rr = rr_from_wire(wire, indexp, 0);
        if (!strcmp(a_rr->name, qname) && a_rr->type == qtype) {
            int num0 = a_rr->rdata[0];
            int num1 = a_rr->rdata[1];
            int num2 = a_rr->rdata[2];
            int num3 = a_rr->rdata[3];
            char * ip = malloc(16*sizeof(char));
            sprintf(ip, "%d.%d.%d.%d", num0, num1, num2, num3);
            
            if (first_ans == NULL) {
                first_ans = malloc(sizeof(dns_answer_entry));
                current = first_ans;
            } else {
                current->next = malloc(sizeof(dns_answer_entry));
                current = current->next;
            }
            current->value = ip;

        } else if (a_rr->type == 5) {
            int index = *indexp - a_rr->rdata_len;
            char * next_name = name_ascii_from_wire(wire, &index);
            strcpy(qname, next_name);


            if (first_ans == NULL) {
                first_ans = malloc(sizeof(dns_answer_entry));
                current = first_ans;
            } else {
                current->next = malloc(sizeof(dns_answer_entry));
                current = current->next;
            }
            current->value = next_name;
        }
     }

     return first_ans;
}

int send_recv_message(unsigned char *request, int requestlen, unsigned char *response, char *server, unsigned short port) {
	/* 
	 * Send a message (request) over UDP to a server (server) and port
	 * (port) and wait for a response, which is placed in another byte
	 * array (response).  Create a socket, "connect()" it to the
	 * appropriate destination, and then use send() and recv();
	 *
	 * INPUT:  request: a pointer to an array of bytes that should be sent
	 * INPUT:  requestlen: the length of request, in bytes.
	 * INPUT:  response: a pointer to an array of bytes in which the
	 *             response should be received
	 * OUTPUT: the size (bytes) of the response received
	 */
     int check, sockfd;

     struct sockaddr_in * addr = malloc(sizeof(struct sockaddr_in));
     memset(addr, 0, sizeof(struct sockaddr_in));
     addr->sin_family = AF_INET;
     addr->sin_port = htons(port);
     inet_pton(AF_INET, server, &addr->sin_addr.s_addr);

     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
     if (sockfd == -1) fprintf(stderr, "socket error: %s\n", strerror(errno));

     check = connect(sockfd, (struct sockaddr *)addr, sizeof(struct sockaddr));
     if (check == -1) fprintf(stderr, "connect error: %s\n", strerror(errno));

     check = write(sockfd, request, requestlen);
     if (check == -1) fprintf(stderr, "write error: %s\n", strerror(errno));
     if (check != requestlen) fprintf(stderr, "write error: only %d of %d bytes written!", check, requestlen);

     int numbytes;
     numbytes = read(sockfd, response, MAX_BUF_LEN);
     if (numbytes == -1) fprintf(stderr, "recv error: %s\n", strerror(errno));

     return numbytes;
}

dns_answer_entry *resolve(char *qname, char *server) {
     unsigned short port = 53;
     unsigned short requestlen;
     dns_rr_type qtype = 1;
     unsigned char request[MAX_BUF_LEN];

     requestlen = create_dns_query(qname, qtype, request);

     unsigned char response[MAX_BUF_LEN];
     int responselen;

     responselen = send_recv_message(request, requestlen, response, server, port);

     return get_answer_address(qname, qtype, response);
}

int main(int argc, char *argv[]) {
	dns_answer_entry *ans, *prev;
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <domain name> <server>\n", argv[0]);
		exit(1);
	}
	ans = resolve(argv[1], argv[2]);
	while (ans != NULL) {
		printf("%s\n", ans->value);
        prev = ans;
        free(prev);
		ans = ans->next;
	}
}
