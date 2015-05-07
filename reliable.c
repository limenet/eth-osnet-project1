#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <poll.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>

#include "rlib.h"

struct reliable_state
{
	rel_t *next;            /* Linked list for traversing all connections */
	rel_t **prev;

	conn_t *c;          /* This is the connection object */

	/* Add your own data fields below this */

	uint32_t current_seq_no;
	uint32_t ackno;
	uint32_t window_size;

};
rel_t *rel_list;

void build_packet(packet_t *pkt, rel_t *s, int length);





/* Creates a new reliable protocol session, returns NULL on failure.
* ss is always NULL */
rel_t *
rel_create (conn_t *c, const struct sockaddr_storage *ss,
            const struct config_common *cc)
{
	rel_t *r;

	r = xmalloc (sizeof (*r));
	memset (r, 0, sizeof (*r));

	if (!c)
	{
		c = conn_create (r, ss);
		if (!c)
		{
			free (r);
			return NULL;
		}
	}

	r->c = c;
	r->next = rel_list;
	r->prev = &rel_list;
	if (rel_list)
		rel_list->prev = &r->next;
	rel_list = r;

	printf("->rel_create\n");
	/* Do any other initialization you need here */

	r->current_seq_no = 1;
	r->ackno = 1;
	r->window_size = cc->window;

	return r;
}

void
rel_destroy (rel_t *r)
{
	if (r->next)
		r->next->prev = r->prev;
	*r->prev = r->next;
	conn_destroy (r->c);

	printf("->rel_destroy\n");

	/* Free any other allocated memory here */
}


void
rel_recvpkt (rel_t *r, packet_t *pkt, size_t n)
{
	printf("->rel_recvpkt\n");
	int pkt_length = ntohs(pkt->len);
	int ackno = ntohs(pkt->ackno);
	uint32_t seqno = ntohs(pkt->seqno);

	if (pkt_length == 8)
	{
		// Do nothing for now.
		printf("ACK packet received.\n");
		if(ackno == r->current_seq_no)
		{
			printf("Finally ackno correct");
		}
		else
		{
			printf("Wrong again");
		}
	}
	else
	{
		if(!(seqno==r->ackno))
		{
			printf("Don't want this specific package!");
			return;
		}
		(r->ackno)++;
		
		// We need to subtract 12 due to the packet overhead.
		printf("Normal packet received.\n");
		char buf[r->window_size];
		strncpy(buf, pkt->data, pkt_length);
		conn_output(r->c, buf, pkt_length );

		packet_t *pkt;
		pkt = malloc(sizeof(*pkt));
		build_packet(pkt, r, 8);
		conn_sendpkt(r->c, pkt, 8);
		free(pkt);

	}
}

void
rel_read (rel_t *s)
{
	printf("->rel_read\n");
	int input;
	char buf[s->window_size];

	input = conn_input(s->c, buf, conn_bufspace(s->c));

	if (input == 1 || input == 0) // no input
		return;
	if (input == -1)
		rel_destroy(s);

	packet_t *pkt;
	pkt = malloc(sizeof(*pkt));

	// Since we need to be able to send more data than packet size,
	// we need to split them into multiple packets.
	int offset = 0;
	while (input > 1)
	{

		// Fit into one packet.
		if (input < 500)
		{
			build_packet(pkt, s, input + 12);
			strncpy(pkt->data, &buf[offset], input);
			conn_sendpkt(s->c, pkt, input + 12);
			input = 0;
		}
		else
		{
			build_packet(pkt, s, 512);
			strncpy(pkt->data, &buf[offset], 500);
			conn_sendpkt(s->c, pkt, 512);
			offset += 500;
			input -= 500;
		}

	}
	// Unsure if needed:
	free(pkt);
}

void
rel_output (rel_t *r)
{
	printf("->rel_output\n");
}

void
build_packet(packet_t *pkt, rel_t *s, int length)
{
	// len and seqno need to be in network order.
	pkt->len = htons(length);
	if (length)
	{
		pkt->seqno = htons(s->current_seq_no);
		++s->current_seq_no;
	}
	else
	{
	}
	pkt->ackno = htons(s->ackno);
	pkt->cksum = 0;
	cksum(pkt, pkt->cksum);
	return;
}
void
rel_timer ()
{
	//printf("->rel_timer\n");
	/* Retransmit any packets that need to be retransmitted */

}
