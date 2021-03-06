/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

mowgli_heap_t *recvq_heap;
mowgli_list_t recvq;

mowgli_heap_t *sendq_heap;
mowgli_list_t sendq;

void queue_init(void)
{
    recvq_heap = mowgli_heap_create(sizeof(io_queue_t), 1024, BH_NOW);

    if (!recvq_heap)
        sigyn_fatal("queue_init(): block allocator failed.");

    sendq_heap = mowgli_heap_create(sizeof(io_queue_t), 256, BH_NOW);

    if (!sendq_heap)
        sigyn_fatal("queue_init(): block allocator failed.");
}

io_queue_t *recvq_add(mowgli_linebuf_t * line, char *string, bool complete)
{
    mowgli_node_t *n;
    io_queue_t *q, *tail;
    char tmp[1024];

    if (strlen(string) == 0)
        return NULL;

    n = recvq.tail;
    if (n != NULL)
    {
        tail = (io_queue_t *)n->data;
        if (tail->completed == false)
        {
            snprintf(tmp, BUFSIZE, "%s%s", tail->string, string);
            mowgli_free(tail->string);
            tail->string = mowgli_strdup(tmp);
            tail->completed = complete;
            return tail;
        }
    }

    q = mowgli_heap_alloc(sendq_heap);
    q->line = line;
    q->string = mowgli_strdup(string);
    if (complete)
    {
        q->len = strlen(q->string);
        q->completed = true;
    }
    else
        q->completed = false;
    mowgli_node_add(q, mowgli_node_create(), &recvq);
    return q;
}

void recvq_dump(mowgli_linebuf_t * line)
{
    mowgli_node_t *n, *tn;
    io_queue_t *q;

    MOWGLI_ITER_FOREACH_SAFE(n, tn, recvq.head)
    {
        q = (io_queue_t *)n->data;

        if ((q->line == line) && (q->completed == true))
        {
            logger(LOG_RAW, ">> %s\n", strip_colour_codes(q->string));
	    parse(q->string);
            mowgli_free(q->string);
            mowgli_node_delete(n, &recvq);
        }
    }
}

#if 0
void sendq_add(socket_t sock, char *string, size_t len)
{
    io_queue_t *q;
    q = mowgli_heap_alloc(sendq_heap);
    q->len = len;
    q->sock = sock;
    q->string = mowgli_strdup(string);
    q->completed = true;
    mowgli_node_add(q, mowgli_node_create(), &sendq);
}

void sendq_dump(socket_t sock)
{
    mowgli_node_t *n, *tn;
    io_queue_t *q;

    MOWGLI_ITER_FOREACH_SAFE(n, tn, sendq.head)
    {
        q = (io_queue_t *)n->data;
        if (q->sock == sock)
        {
            ssize_t bytes = write(q->sock, q->string, q->len);
            if(bytes == -1)
                sigyn_fatal("Error writing to socket: %s", strerror(ERRNO));
            me.stats.outB += bytes;
            mowgli_node_delete(n, &sendq);
        }
    }
}

void sendq_flush(socket_t sock)
{
    mowgli_node_t *n, *tn;
    io_queue_t *q;

    MOWGLI_ITER_FOREACH_SAFE(n, tn, sendq.head)
    {
        q = (io_queue_t *)n->data;
        if (q->sock == sock)
            mowgli_node_delete(n, &sendq);
    }
}
#endif
