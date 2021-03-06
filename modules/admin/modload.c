#include "sigyn.h"

DECLARE_MODULE("admin/modload", MODULE_UNLOAD_CAPABILITY_NEVER, _modinit, _moddeinit,
        "1.0", "Matthew <mattwb65@gmail.com>");

static void cmd_modload(const irc_event_t *event, int parc, char **parv);
static void cmd_modunload(const irc_event_t *event, int parc, char **parv);
static void cmd_modreload(const irc_event_t *event, int parc, char **parv);
static void cmd_modlist(const irc_event_t *event, int parc, char **parv);
extern mowgli_list_t modules;

void _modinit(UNUSED module_t *m)
{
    command_add("modload", cmd_modload, 1, AC_ADMIN, "Loads a module.", "<name>");
    command_add("modunload", cmd_modunload, 1, AC_ADMIN, "Unloads a module.", "<name>");
    command_add("modreload", cmd_modreload, 1, AC_ADMIN, "Reloads a module.", "<name>");
    command_add("modlist", cmd_modlist, 0, AC_ADMIN, "Lists loaded modules.", NULL);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("modload", cmd_modload);
    command_del("modunload", cmd_modunload);
    command_del("modlist", cmd_modlist);
    command_del("modreload", cmd_modreload);
}


static void cmd_modload(const irc_event_t *event, UNUSED int parc, char **parv)
{
    module_t *mod;

    if (module_find_published(parv[1]))
    {
        irc_notice(event->origin->nick, "Module \2%s\2 is already loaded.", parv[1]);
        return;
    }

    mod = module_load(parv[1]);

    if (mod == NULL)
        irc_notice(event->origin->nick, "Module \2%s\2 failed to load.", parv[1]);
    else
        irc_notice(event->origin->nick, "Module \2%s\2 successfully loaded.", parv[1]);
}

static void cmd_modunload(const irc_event_t *event, UNUSED int parc, char **parv)
{
    module_t *mod;

    mod = module_find_published(parv[1]);
    if (!mod)
    {
        irc_notice(event->origin->nick, "Module \2%s\2 is not loaded.", parv[1]);
        return;
    }
    if (mod->can_unload != MODULE_UNLOAD_CAPABILITY_OK)
    {
        irc_notice(event->origin->nick, "Module \2%s\2 is a permanent module and cannot be unloaded.", mod->name);
        return;
    }

    module_unload(mod, MODULE_UNLOAD_INTENT_PERM);

    irc_notice(event->origin->nick, "Module \2%s\2 successfully unloaded.", parv[1]);
}

static void cmd_modreload(const irc_event_t *event, UNUSED int parc, char **parv)
{
    module_t *mod;

    mod = module_find_published(parv[1]);
    if (!mod)
    {
      irc_notice(event->origin->nick, "Module \2%s\2 is not loaded.", parv[1]);
      return;
    }
    if (mod->can_unload != MODULE_UNLOAD_CAPABILITY_OK)
    {
      irc_notice(event->origin->nick, "Module \2%s\2 is a permanent module and cannot be unloaded.", mod->name);
      return;
    }
    
    module_unload(mod, MODULE_UNLOAD_INTENT_RELOAD);

    mod = module_load(parv[1]);
    
    if (mod == NULL)
      irc_notice(event->origin->nick, "Module \2%s\2 failed to load.", parv[1]);
    else
      irc_notice(event->origin->nick, "Module \2%s\2 successfully reloaded.", parv[1]);
    
}

static void cmd_modlist(const irc_event_t *event, UNUSED int parc, UNUSED char **parv)
{
    mowgli_node_t *n;
    int i = 0;
    irc_notice(event->origin->nick, "*** Sigyn Module List ***");
    MOWGLI_ITER_FOREACH(n, modules.head)
    {
        i++;
        module_t *m = n->data;
        irc_notice(event->origin->nick, "%s (version %s) by %s", m->name, m->header->version, m->header->vendor);
    }
    irc_notice(event->origin->nick, "*** End of Module List. Total of %i modules loaded. ***", i);
}
