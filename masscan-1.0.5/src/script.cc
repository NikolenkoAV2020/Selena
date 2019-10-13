#include "script.h"
#include "string_s.h"

extern struct MassScript script_ntp_monlist;

struct MassScript * script_lookup(const char *name)
{
    return(strcmp(name, script_ntp_monlist.name) ? 0 : &script_ntp_monlist);
}

