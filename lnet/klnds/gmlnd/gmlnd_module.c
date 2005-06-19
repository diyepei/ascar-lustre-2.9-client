/* -*- mode: c; c-basic-offset: 8; indent-tabs-mode: nil; -*-
 * vim:expandtab:shiftwidth=8:tabstop=8:
 *
 *  Copyright (c) 2003 Los Alamos National Laboratory (LANL)
 *
 *   This file is part of Lustre, http://www.lustre.org/
 *
 *   Lustre is free software; you can redistribute it and/or
 *   modify it under the terms of version 2 of the GNU General Public
 *   License as published by the Free Software Foundation.
 *
 *   Lustre is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Lustre; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "gmnal.h"


int gmnal_small_msg_size = sizeof(gmnal_msghdr_t) + sizeof(ptl_hdr_t) + PTL_MTU;
/*
 *      -1 indicates default value.
 *      This is 1 thread per cpu
 *      See start_kernel_threads
 */
int num_rx_threads = -1;
int num_stxds = 5;
int gm_port_id = 4;

int
gmnal_cmd(struct portals_cfg *pcfg, void *private)
{
	gmnal_data_t	*nal_data = NULL;
	char		*name = NULL;
	int		nid = -2;
	int		gnid;
	gm_status_t	gm_status;


	CDEBUG(D_TRACE, "gmnal_cmd [%d] private [%p]\n",
	       pcfg->pcfg_command, private);
	nal_data = (gmnal_data_t*)private;
	switch(pcfg->pcfg_command) {
	/*
	 * just reuse already defined GET_NID. Should define GMNAL version
	 */
	case(GMNAL_IOC_GET_GNID):

		PORTAL_ALLOC(name, pcfg->pcfg_plen1);
		copy_from_user(name, PCFG_PBUF(pcfg, 1), pcfg->pcfg_plen1);

		GMNAL_GM_LOCK(nal_data);
		//nid = gm_host_name_to_node_id(nal_data->gm_port, name);
                gm_status = gm_host_name_to_node_id_ex(nal_data->gm_port, 0,
                                                       name, &nid);
		GMNAL_GM_UNLOCK(nal_data);
                if (gm_status != GM_SUCCESS) {
                        CDEBUG(D_INFO, "gm_host_name_to_node_id_ex(...host %s) "
                               "failed[%d]\n", name, gm_status);
                        return (-1);
                } else
		        CDEBUG(D_INFO, "Local node %s id is [%d]\n", name, nid);
		GMNAL_GM_LOCK(nal_data);
		gm_status = gm_node_id_to_global_id(nal_data->gm_port,
						    nid, &gnid);
		GMNAL_GM_UNLOCK(nal_data);
		if (gm_status != GM_SUCCESS) {
			CDEBUG(D_INFO, "gm_node_id_to_global_id failed[%d]\n",
			       gm_status);
			return(-1);
		}
		CDEBUG(D_INFO, "Global node is is [%u][%x]\n", gnid, gnid);
		copy_to_user(PCFG_PBUF(pcfg, 2), &gnid, pcfg->pcfg_plen2);
	break;
	default:
		CDEBUG(D_INFO, "gmnal_cmd UNKNOWN[%d]\n", pcfg->pcfg_command);
		pcfg->pcfg_nid2 = -1;
	}


	return(0);
}


static int __init
gmnal_load(void)
{
	int	status;
	CDEBUG(D_TRACE, "This is the gmnal module initialisation routine\n");


	CDEBUG(D_INFO, "Calling gmnal_init\n");
        status = gmnal_init();
	if (status == PTL_OK) {
		CDEBUG(D_INFO, "Portals GMNAL initialised ok\n");
	} else {
		CDEBUG(D_INFO, "Portals GMNAL Failed to initialise\n");
		return(-ENODEV);
	}

	CDEBUG(D_INFO, "This is the end of the gmnal init routine");

	return(0);
}


static void __exit
gmnal_unload(void)
{
	gmnal_fini();
	return;
}


module_init(gmnal_load);

module_exit(gmnal_unload);

MODULE_PARM(gmnal_small_msg_size, "i");
MODULE_PARM(num_rx_threads, "i");
MODULE_PARM(num_stxds, "i");
MODULE_PARM(gm_port_id, "i");

MODULE_AUTHOR("Morgan Doyle");

MODULE_DESCRIPTION("A Portals kernel NAL for Myrinet GM.");

MODULE_LICENSE("GPL");
