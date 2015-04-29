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

struct reliable_state {
    rel_t *next;			/* Linked list for traversing all connections */
    rel_t **prev;

    conn_t *c;			/* This is the connection object */

    /* Add your own data fields below this */

};
rel_t *rel_list;





/* Creates a new reliable protocol session, returns NULL on failure.
* ss is always NULL */
rel_t *
rel_create (conn_t *c, const struct sockaddr_storage *ss,
const struct config_common *cc)
{
    rel_t *r;

    r = xmalloc (sizeof (*r));
    memset (r, 0, sizeof (*r));

    if (!c) {
        c = conn_create (r, ss);
        if (!c) {
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
    //char buf[conn_bufspace(r->c)];
    printf("->rel_recvpkt\n");
    //conn_output(r->c, buf, pkt->len);
}


void
rel_read (rel_t *s)
{
    printf("->rel_read\n");
    packet_t pkt;
    int input;
    char buf[conn_bufspace(s->c)];

    input = conn_input(s->c, buf, conn_bufspace(s->c));

    if(input == 1)
        return;

    if(input == -1)
        rel_destroy(s);

    pkt.len = input;
    //pkt.data = buf;
    //printf("%d\t%s\n", input, buf);

    conn_sendpkt(s->c, &pkt, input);
}

void
rel_output (rel_t *r)
{
    printf("->rel_output\n");
}

void
rel_timer ()
{
    //printf("->rel_timer\n");
    /* Retransmit any packets that need to be retransmitted */

}
