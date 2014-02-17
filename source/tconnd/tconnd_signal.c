#include "tconnd_signal.h"
#include "tcommon/terrno.h"
#include "tconnd/tconnd_reactor.h"
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

#include "tlog/tlog_instance.h"

static bool sig_term;

static void on_signal(int sig)
{
    switch(sig)
    {
        case SIGINT:
        case SIGTERM:
            sig_term = true;
            break;
    }
}

TERROR_CODE signal_processing_init()
{
    struct sigaction  sa;

	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = on_signal;
	if(sigemptyset(&sa.sa_mask) != 0)
	{
	    ERROR_LOG("sigemptyset errno[%d], %s.", errno, strerror(errno));
		goto ERROR_RET;
	}
	
	if((sigaction(SIGTERM, &sa, NULL) != 0)
	 ||(sigaction(SIGINT, &sa, NULL) != 0))
	{
        ERROR_LOG("sigaction error[%d], %s.", errno, strerror(errno));
	    goto ERROR_RET;
	}
    DEBUG_LOG("signal_processing_init succeed.");
    sig_term = false;
	return E_TS_NOERROR;
ERROR_RET:
	return E_TS_ERROR;
}

TERROR_CODE signal_processing_proc()
{
	if(sig_term)
	{
	    INFO_LOG("receive sig_term.");
		g_tconnd_reactor_switch = FALSE;
		return E_TS_NOERROR;
	}
	return E_TS_WOULD_BLOCK;
}
