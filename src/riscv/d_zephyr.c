#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <stdio.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/fs_sys.h>
#ifdef SDCARD_READ
#include <ff.h>
#endif
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "zephyr.h"
#include "console.h"

enum
{
	FS_FILEFS = FS_TYPE_EXTERNAL_BASE
};

/* Flash "filesystem" */
static struct
{
	const char *name; /* Filename */
	size_t len;		  /* Length */
	void *addr;		  /* Address in flash */
} fs[] = {
	//	{ "doomu.wad", 12408292, (void*)0x40500000 },
	{"/SD:/doom1.wad", 4196020, (void *)0x40200000},
	{NULL}};

#define NUM_FDS 16

static struct
{
	enum
	{
		FD_NONE = 0,
		FD_STDIO = 1,
		FD_FLASH = 2,
	} type;
	size_t offset;
	size_t len;
	char *data;
	struct fs_file_t *zfp;
} fds[NUM_FDS] = {
	[0] = {
		.type = FD_STDIO,
	},
	[1] = {
		.type = FD_STDIO,
	},
	[2] = {
		.type = FD_STDIO,
	},
};

static int
get_fd(struct fs_file_t *zfp)
{
	for (int i = 0; i < NUM_FDS; i++)
		if (fds[i].zfp == zfp)
			return i;
	return -1;
}

static int
filefs_open(struct fs_file_t *zfp, const char *pathname, fs_mode_t flags)
{
	int fn, fd;

	// printf("%s(%p): opening '%s(%p)'\n", __FUNCTION__, filefs_open, pathname, zfp);
	/* Try to find file */
	for (fn = 0; fs[fn].name; fn++)
		if (!strcmp(pathname, fs[fn].name))
			break;
	// printf("%s: found '%s'\n", __FUNCTION__, fs[fn].name);

	if (!fs[fn].name)
	{
		errno = ENOENT;
		return -1;
	}

	/* Find free FD */
	for (fd = 3; (fd < NUM_FDS) && (fds[fd].type != FD_NONE); fd++)
		;
	if (fd == NUM_FDS)
	{
		errno = ENOMEM;
		return -1;
	}
	// printf("%s: found free fs '%d'\n", __FUNCTION__, fd);

	/* "Open" file */
	fds[fd].type = FD_FLASH;
	fds[fd].offset = 0;
	fds[fd].len = fs[fn].len;
	fds[fd].data = fs[fn].addr;
	fds[fd].zfp = zfp;

	printf("Opened: %s as fd=%d, len=%d, addr=%p\n", pathname, fd, fs[fn].len, fs[fn].addr);

	return fd;
}

static ssize_t
filefs_read(struct fs_file_t *zfp, void *buf, size_t nbyte)
{
	int fd;
	fd = get_fd(zfp);
	// printf("%s: fd = %d, buf = %p, len=%d, offs=0x%08x\n", __FUNCTION__, fd, buf, nbyte, fds[fd].offset);
	if ((fd < 0) || (fd >= NUM_FDS) || (fds[fd].type != FD_FLASH))
	{
		errno = EINVAL;
		return -1;
	}

	if ((fds[fd].offset + nbyte) > fds[fd].len)
		nbyte = fds[fd].len - fds[fd].offset;

	memcpy(buf, fds[fd].data + fds[fd].offset, nbyte);
	fds[fd].offset += nbyte;
	// printf("%s, read ret=%d, new offs=0x%08x\n", __FUNCTION__, nbyte, fds[fd].offset);
	return nbyte;
}

static ssize_t
filefs_write(struct fs_file_t *zfp, const void *buf, size_t nbyte)
{
	// int fd = get_fd(zfp); // not yet used
	const unsigned char *c = buf;
	for (int i = 0; i < nbyte; i++)
		console_putchar(*c++);
	return nbyte;
}

static int
filefs_close(struct fs_file_t *zfp)
{
	int fd = get_fd(zfp);
	printf("closing: %p as fd=%d...\n", zfp, fd);
	if ((fd < 0) || (fd >= NUM_FDS))
	{
		errno = EINVAL;
		return -1;
	}

	fds[fd].type = FD_NONE;
	printf("done\n");
	return 0;
}

static int
filefs_lseek(struct fs_file_t *zfp, off_t offset, int whence)
{
	size_t new_offset;
	int fd = get_fd(zfp);
	// printf("%s: fd = %d, ofs=%ld, whence=%d\n", __FUNCTION__, fd, offset, whence);
	if ((fd < 0) || (fd >= NUM_FDS) || (fds[fd].type != FD_FLASH))
	{
		errno = EINVAL;
		return -1;
	}

	switch (whence)
	{
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset = fds[fd].offset + offset;
		break;
	case SEEK_END:
		new_offset = fds[fd].len - offset;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	if ((new_offset < 0) || (new_offset > fds[fd].len))
	{
		errno = EINVAL;
		return -1;
	}

	fds[fd].offset = new_offset;

	return new_offset;
}

static int
filefs_stat(struct fs_mount_t *mnt, const char *filename, struct fs_dirent *dirent)
{
	/* Not implemented */
	console_printf("[1] Unimplemented _stat(filename=\"%s\")\n", filename);
	return -1;
}

#if 0
static int
filefs_fstat(int fd, struct stat *statbuf)
{
	/* Not implemented */
	console_printf("[1] Unimplemented _fstat(fd=%d)\n", fd);
	return -1;
}

static int
filefs_isatty(int fd)
{
	/* Only stdout and stderr are TTY */
	errno = 0;
	return (fd == 1) || (fd == 2);
}

static int
filefs_access(const char *pathname, int mode)
{
	int fn;

	/* Try to find file */
	for (fn=0; fs[fn].name; fn++)
		if (!strcmp(pathname, fs[fn].name))
			break;

	if (!fs[fn].name) {
		errno = ENOENT;
		return -1;
	}
#if 0
	/* Check requested access */
	if (mode & ~(R_OK | F_OK)) {
		errno = EACCES;
		return -1;
	}
#endif

	return 0;
}
#endif

static int
filefs_mount(struct fs_mount_t *mountp)
{
	ARG_UNUSED(mountp);
	return 0;
}

static int
filefs_unmount(struct fs_mount_t *mountp)
{
	ARG_UNUSED(mountp);
	return 0;
}



#ifndef SDCARD_READ
static struct fs_file_system_t ops_fs = {
	.open = filefs_open,
	.close = filefs_close,
	.read = filefs_read,
	.write = filefs_write,
	.lseek = filefs_lseek,
	.tell = NULL,
	.truncate = NULL,
	.sync = NULL,
	.opendir = NULL,
	.readdir = NULL,
	.closedir = NULL,
	.mount = filefs_mount,
	.unmount = filefs_unmount,
	.unlink = NULL,
	.rename = NULL,
	.mkdir = NULL,
	.stat = filefs_stat,
	.statvfs = NULL,
#if defined(CONFIG_FILE_SYSTEM_MKFS)
	.mkfs = NULL,
#endif
};/* mounting info */
static struct fs_mount_t mp = {
	.type = FS_FILEFS,
	.flags = 0,
	.fs_data = &ops_fs,
};
#define FSTYPE FS_FILEFS
#else
static FATFS ops_fs;
// sdcard read
static struct fs_mount_t mp = {
	.type = FS_FATFS,
	.fs_data = &ops_fs,
};
#define FSTYPE FS_FATFS

#endif

static int filefs_init(const struct device *dev)
{
	ARG_UNUSED(dev);
	printf("registering filefs...\n");
	return fs_register(FSTYPE, &ops_fs);
}

int mountsd(const char *mp_literal)
{
#if 0
    printf("trying to mount SD card at mountpoint '%s'...\n", mp_literal);
    if (disk_access_init("SD") != 0)
    {
		printf("Storage init ERROR!\n");
		return -1;
	}
#endif
	mp.mnt_point = mp_literal;
	return fs_mount(&mp);
}
#define STACK_SIZE (1024 * 256)

K_THREAD_STACK_DEFINE(stack, STACK_SIZE);

void init_zephyr(void *fn)
{
#if 0
	printf("sleeping...4s\n");
	for (int i = 0; i < 8; i++)
	{
		k_sleep(K_MSEC(500));
		printf(".");
		fflush(stdout);
	}
	printf("\n");
#endif
	filefs_init(NULL);
	int res;
	if ((res = mountsd("/SD:")) < 0)
	{
		printf("%s: mount sd card failed: %d\n", __FUNCTION__, res);
	}
	fcntl(0, F_SETFL, O_NONBLOCK);
	struct k_thread th;
	k_tid_t main_id;
	main_id = k_thread_create(&th, stack, STACK_SIZE, fn, NULL, NULL, NULL, 0, 0, K_NO_WAIT);
}

void console_init(void) 
{
	printf("initializing console...\n");
}

void console_putchar(char c)
{
	putchar(c);
}

char console_getchar(void)
{
	return getchar();
}

int console_getchar_nowait(void)
{
	char c;
	int ret = read(0, &c, 1);
	//printf("%s: ret = %d, c = %c\n", __FUNCTION__, ret, c);
	if (ret < 0)
		perror("read failed.\n");
	if (ret == 0)
		return -1;
	return (int)c;
}

void console_puts(const char *p)
{
	puts(p);
}

int console_printf(const char *fmt, ...)
{
	static char _printf_buf[128];
	va_list va;
	int l;

	va_start(va, fmt);
	l = vsnprintf(_printf_buf, 128, fmt, va);
	va_end(va);

	console_puts(_printf_buf);

	return l;
}

void hexdump(const char *d, int len)
{
	for (int i = 0; i < len; i += 16)
	{
		printf("%p: ", &d[i]);
		for (int j = 0; (j < 16) && ((i + j) < len); j++)
			printf("%02x ", d[i + j]);

		for (int j = 0; (j < 16) && ((i + j) < len); j++)
		{
			char c = d[i + j];
			printf("%c", isprint(c) ? c : '.');
		}
		printf("\n");
	}
}