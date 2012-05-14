/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef SIGYN_TIMER_H
#define SIGYN_TIMER_H

#include "mowgli.h"

typedef struct timer_event_ timer_event_t;
typedef bool (*timer_function_t)(void *argument);

struct timer_event_
{
    char *name;
    timer_function_t function;
    void *arguments;
    unsigned int wait;
    time_t time;
};

extern void timer_init(void);
extern time_t timer_add(const char *name, timer_function_t function, void *arg,
        unsigned int wait);
extern void timer_del(const char *name, void *function);
extern void run_timers(time_t currtime);
extern time_t get_next_timer(void);

#endif /* SIGYN_TIMER_H */
