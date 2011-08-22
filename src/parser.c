/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

irc_user_t *parse_user(char *hostmask)
{
    irc_user_t *user = mowgli_alloc(sizeof(irc_user_t));
    char *ptr;

    if((ptr = strchr(hostmask, '!')) != NULL)
    {
        ptr = '\0';
        user->nick = hostmask;
        hostmask = ++ptr;
    }
    if ((ptr = strchr(hostmask, '@')) != NULL)
    {
        ptr = '\0';
        user->user = hostmask;
        hostmask = ++ptr;
    }

    user->host = hostmask;

    return user;
}

irc_event_t *parse(char line[])
{
    char *token;
    irc_event_t *event = mowgli_alloc(sizeof(irc_event_t));

    strip(line);

    token = strtok(line, " ");
    if((strncmp(token, ":", 1)) == 0)
    {
       event->origin = token + 1;
    }
    else
    {
        event->command = token;
    }
    
    if(token != NULL)
    {
        if(event->command != NULL)
        {
            token = strtok(NULL, "\0");
            event->data = token;
        }
        else
        {
            token = strtok(NULL, " ");
            event->command = token;
        }
    }
    
    if(token != NULL)
    {
        if (event->data != NULL)
            return event;

        token = strtok(NULL, "\0");
        event->data = token;
    }

    return event;
}


#if 0
irc_event_t *parse(char *text)
{
    /*irc_event_t *event = mowgli_alloc(sizeof(irc_event_t));*/
    irc_event_t *event = malloc(sizeof(irc_event_t));
    char *user = NULL, *ptr, *cmd, *data = NULL;

    if (text != NULL)
    {
        if (*text == '\n')
            return NULL;
        if (*text == '\0')
            return NULL;

        logger(LOG_RAW, ">> %s", text);

            if (text[0] == ':') {
                user = &text[1];
                ptr = strchr(user, ' ');
                if (ptr == NULL)
                    return NULL;
                ptr = '\0';
                //*ptr++ = '\0';
                //ptr++;
                //ptr = '\0';
                cmd = ++ptr;
                if ((ptr = strchr(user, ' ')))
                    //ptr++;
                    ptr ='\0';
            } else
                cmd = text;
            ptr = strchr(cmd, ' ');
            if (ptr) {
                ptr = '\0';
                //ptr = '\0';
                data = ptr;
                if (*data == ':')
                    data++;
            }

            if (user != NULL)
            {
                irc_user_t *origin = parse_user(user);

                /* If the origin is a server */
                if (!origin->nick)
                    origin->nick = origin->host;

                event->origin = origin;
            }

            event->command = cmd;
            return event;
    }
    return NULL;
}
#endif
