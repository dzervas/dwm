#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xlib.h>

#define LENGTH(item)    (sizeof(item)/sizeof(*item))

typedef struct {
	char *name;
	const char * display; /* only screens may change for now. xserver must be the same for all */
	const char * const * ptrs;
	const char * const * kbds;
} MasterDevice;

static void attachslaves(const XIDeviceInfo *devs, int ndev, int master, const char * const *slaves);
static void die(const char *errstr, ...);
static const XIDeviceInfo *getdevicebyname(const XIDeviceInfo *devs, int ndev, int use, const char * name);
static void hierarchychanged(const XIHierarchyEvent * const ev);
static void inittree(void);
static int strcatcmp(const char *lhs, const char *rhs1, const char *rhs2);

static const MasterDevice mdevs[] = {
	{ "raimundo", ":0.0",
		(const char *[]){ "MousePS2", NULL },
		(const char *[]){ "KeyboardPS2", NULL } },
	{ "carla", ":0.1",
		(const char *[]){ "MouseGIGABYTE", "KeyboardGIGABYTEMultimedia", NULL },
		(const char *[]){ "KeyboardGIGABYTE", NULL } },
};
static int xi2opcode;
static unsigned char hcmask[XIMaskLen(XI_HierarchyChanged)];
static Display *dpy;
static Window root;
static XIEventMask hcevm = { XIAllDevices, sizeof(hcmask), hcmask };

void attachslaves(const XIDeviceInfo * const devs, const int ndev, const int master, const char * const * const slaves) {
	int i;
	const XIDeviceInfo *slave;
	XIAttachSlaveInfo atts = { XIAttachSlave, 0, master };

	for(i = 0; slaves[i]; i++) {
		if(!(slave = getdevicebyname(devs, ndev, XIFloatingSlave, slaves[i])) || slave->attachment == master)
			continue;
		atts.deviceid = slave->deviceid;
		XIChangeHierarchy(dpy, (XIAnyHierarchyChangeInfo *)&atts, 1);
	}
}

void die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

const XIDeviceInfo *getdevicebyname(const XIDeviceInfo * const devs, const int ndev, const int use, const char * const name) {
	int i;
	const char *cmp;

	switch(use) {
		case XIMasterPointer: cmp = " pointer"; break;
		case XIMasterKeyboard: cmp = " keyboard"; break;
		default: cmp = ""; break;
	}
	for(i = ndev-1; i >= 0; i--)
		if(!strcatcmp(devs[i].name, name, cmp))
			return &devs[i];
	return NULL;
}

void hierarchychanged(const XIHierarchyEvent * const ev) {
	int i, j, k, did, ndev;
	XIDeviceInfo *changed, * const devs = XIQueryDevice(dpy, XIAllDevices, &ndev);

	if(!devs)
		return;
	for(i = 0; i < ev->num_info; i++) {
		if(!ev->info[i].flags)
			continue;
		did = ev->info[i].deviceid;
		for(changed = NULL, j = 0; j < ndev; j++)
			if(devs[i].deviceid == did) {
				changed = &devs[i];
				break;
			}
		if(!changed)
			continue;
		if(ev->info[i].flags & XIMasterAdded) {
			switch(changed->use) {
				case XIMasterPointer:
					for(j = 0; j < LENGTH(mdevs); j++) {
						if(strcatcmp(changed->name, mdevs[j].name, " pointer"))
							continue;
						attachslaves(devs, ndev, did, mdevs[j].ptrs);
						break;
					}
				break;
				case XIMasterKeyboard:
					for(j = 0; j < LENGTH(mdevs); j++) {
						if(strcatcmp(changed->name, mdevs[j].name, " keyboard"))
							continue;
						attachslaves(devs, ndev, did, mdevs[j].kbds);
						break;
					}
				break;
			}
		}
		if((ev->info[i].flags & XIDeviceEnabled)
		&& (changed->use & (XISlavePointer|XISlaveKeyboard|XIFloatingSlave))) {
			XIAttachSlaveInfo atts = { XIAttachSlave, did, 0 };
			const XIDeviceInfo *master = NULL;

			for(k = 0; k < LENGTH(mdevs); k++) {
				for(j = 0; mdevs[k].ptrs[j]; j++)
					if(!strcmp(mdevs[k].ptrs[j], changed->name)) {
						master = getdevicebyname(devs, ndev, XIMasterPointer, mdevs[k].name);
						goto breakcycle;
					}
				for(j = 0; mdevs[k].kbds[j]; j++)
					if(!strcmp(mdevs[k].kbds[j], changed->name)) {
						master = getdevicebyname(devs, ndev, XIMasterKeyboard, mdevs[k].name);
						goto breakcycle;
					}
			}
breakcycle:
			if(master) {
				atts.new_master = master->deviceid;
				XIChangeHierarchy(dpy, (XIAnyHierarchyChangeInfo *)&atts, 1);
			}
		}
	}
	XIFreeDeviceInfo(devs);
}

void inittree(void) {
	int i, ndev;
	const XIDeviceInfo *master;
	XIDeviceInfo * const devs = XIQueryDevice(dpy, XIAllDevices, &ndev);

	if(!devs)
		return;
	for(i = 0; i < LENGTH(mdevs); i++) {
		if(!(master = getdevicebyname(devs, ndev, XIMasterPointer, mdevs[i].name))
			continue;
		attachslaves(devs, ndev, master->deviceid, mdevs[i].ptrs);
		attachslaves(devs, ndev, master->attachment, mdevs[i].kbds);
	}
	XIFreeDeviceInfo(devs);
}

int strcatcmp(const char *lhs, const char *const rhs1, const char *const rhs2) {
	int i;
	
	for(i = 0; lhs[i] && rhs1[i] && lhs[i] == rhs1[i]; i++);
	if(rhs1[i])
		return -1;
	lhs = &lhs[i];
	for(i = 0; lhs[i] && rhs2[i] && lhs[i] == rhs2[i]; i++);
	if(!lhs[i] && !rhs2[i])
		return 0;
	else return 1;
}

int main(int argc, char **argv) {
	XEvent ev;
	int screen, event, error, major = 2, minor = 1;

	if(!(dpy = XOpenDisplay(NULL)))
		die("inputmanager: cannot open display\n");
	if (!XQueryExtension(dpy, "XInputExtension", &xi2opcode, &event, &error))
		die("inputmanager: XInputExtension is not available\n");
	if(XIQueryVersion(dpy, &major, &minor) == BadRequest)
		die("inputmanager: XInput2 2.1 is not available. Server supports up to %d.%d\n", major, minor);
	root = DefaultRootWindow(dpy);

	inittree();

	memset(hcmask, 0, sizeof(hcmask));
	XISetMask(hcmask, XI_HierarchyChanged);
	XISelectEvents(dpy, root, &hcevm, 1);

	screen = DefaultScreen(dpy);
	XIWarpPointer(dpy, 2, 0, root, 0, 0, 0, 0, DisplayWidth(dpy, screen), DisplayHeight(dpy, screen));
	XSync(dpy, False);
	while(1) {
		XNextEvent(dpy, &ev);
		if(ev.type != GenericEvent || ev.xcookie.extension != xi2opcode
		|| ev.xcookie.evtype != XI_HierarchyChanged || !XGetEventData(dpy, &ev.xcookie))
			continue;
		hierarchychanged((XIHierarchyEvent *)ev.xcookie.data);
		XFreeEventData(dpy, &ev.xcookie);
	}

	XCloseDisplay(dpy);
	return 0;
}

