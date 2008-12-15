/*
 * Copyright (C) 2006 Ondrej Palkovsky
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ipc/ipc.h>
#include <ipc/services.h>
#include <sysinfo.h>
#include <async.h>
#include <as.h>
#include <align.h>
#include <errno.h>

#include "fb.h"
#include "sysio.h"
#include "ega.h"
#include "main.h"

void receive_comm_area(ipc_callid_t callid, ipc_call_t *call, void **area)
{
	void *dest;

	dest = as_get_mappable_page(IPC_GET_ARG2(*call));
	if (ipc_answer_fast(callid, 0, (sysarg_t)dest, 0) == 0) {
		if (*area)
			as_area_destroy(*area);
		*area = dest;
	}
}

int main(int argc, char *argv[])
{
	ipcarg_t phonead;
	int initialized = 0;

#ifdef FB_ENABLED
	if (sysinfo_value("fb.kind") == 1) {
		if (fb_init() == 0)
			initialized = 1;
	} 
#endif
#ifdef EGA_ENABLED
	if (! initialized && sysinfo_value("fb.kind") == 2) {
		if (ega_init() == 0)
			initialized = 1;
	}
#endif

	if (!initialized)
		sysio_init();

	if (ipc_connect_to_me(PHONE_NS, SERVICE_VIDEO, 0, &phonead) != 0) 
		return -1;
	
	async_manager();
	/* Never reached */
	return 0;
}
