/*
 *   Copyright (C) 2018 Samsung Electronics Co., Ltd.
 */

#include <linux/slab.h>

#include "../buffer_pool.h"
#include "../transport_ipc.h"

#include "file_notify.h"

/* Completion Filter flags for Notify */
#define FILE_NOTIFY_CHANGE_FILE_NAME	0x00000001
#define FILE_NOTIFY_CHANGE_DIR_NAME	0x00000002
#define FILE_NOTIFY_CHANGE_NAME		0x00000003
#define FILE_NOTIFY_CHANGE_ATTRIBUTES	0x00000004
#define FILE_NOTIFY_CHANGE_SIZE		0x00000008
#define FILE_NOTIFY_CHANGE_LAST_WRITE	0x00000010
#define FILE_NOTIFY_CHANGE_LAST_ACCESS	0x00000020
#define FILE_NOTIFY_CHANGE_CREATION	0x00000040
#define FILE_NOTIFY_CHANGE_EA		0x00000080
#define FILE_NOTIFY_CHANGE_SECURITY	0x00000100
#define FILE_NOTIFY_CHANGE_STREAM_NAME	0x00000200
#define FILE_NOTIFY_CHANGE_STREAM_SIZE	0x00000400
#define FILE_NOTIFY_CHANGE_STREAM_WRITE	0x00000800

#define MAX_NOTIFY	16

//TODO: consider to change to linked list
struct cifsd_file_notify noti_items[MAX_NOTIFY];

struct cifsd_file_notify_status *cifsd_file_notify_wait(unsigned long id, char *path, bool wtree, unsigned int filter)
{
	struct cifsd_file_notify *item;
	struct cifsd_file_notify_status *status = NULL;
	struct cifsd_notify_response *resp = NULL;

	cifsd_debug("notify manager for id %lu, path (%s), filter 0x%x\n", id, path, filter);

	item = noti_items + id - 1;
	item->notify_id = id;
	item->path = path;
	item->recursive = wtree;
	item->is_dir = filter & FILE_NOTIFY_CHANGE_DIR_NAME;

	resp = cifsd_ipc_notify_request(item);
	if (!resp)
		goto out;

	cifsd_debug("received ipc response. handle %u, status %u, outbuf_sz %u\n", resp->handle, resp->status, resp->outbuf_sz);

	status = cifsd_alloc(sizeof(struct cifsd_file_notify_status) + resp->outbuf_sz);
	if (!status)
		goto out1;

	status->ret = resp->status;
	status->outbuf_sz = resp->outbuf_sz;
	if (resp->outbuf_sz > 0)
		memcpy(status->outbuf, resp->outbuf, resp->outbuf_sz);

out1:
	cifsd_free(resp);
out:
	return status;
}

void cifsd_file_notify_cancel(unsigned long *id)
{
	struct cifsd_file_notify *item = noti_items + *id - 1;

	cifsd_debug("cancel notify id = %lu, handle = %u\n", item->notify_id, item->handle);

	cifsd_ipc_notify_cancel_request(item->handle);
}

