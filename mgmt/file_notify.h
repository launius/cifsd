/*
 *   Copyright (C) 2018 Samsung Electronics Co., Ltd.
 */

#ifndef __FILE_NOTIFY_MANAGEMENT_H__
#define __FILE_NOTIFY_MANAGEMENT_H__

#include "../glob.h"  /* FIXME */

struct cifsd_file_notify {
	unsigned long	notify_id;
	char			*path;
	unsigned int	handle;
	bool			recursive;
	bool			is_dir;
};

struct cifsd_file_notify_status {
	unsigned int	ret;
	unsigned int	outbuf_sz;
	unsigned char	outbuf[0];
};

struct cifsd_file_notify_info {
	unsigned int	action;
	unsigned int	filename_len;
	unsigned char	filename[0];
};

struct cifsd_file_notify_status *cifsd_file_notify_wait(unsigned long id, char *path, bool wtree, unsigned int filter);
void cifsd_file_notify_cancel(unsigned long *id);

#endif /* __FILE_NOTIFY_MANAGEMENT_H__ */

