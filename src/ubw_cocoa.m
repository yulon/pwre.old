#include "ubw.h"

#ifdef UBWINDOW_COCOA

#import <Cocoa/Cocoa.h>

static NSAutoreleasePool* pool;
static NSApplication* app;
static NSUInteger uiStyle;
static NSBackingStoreType backingStoreStyle;

int ubwInit(void) {
	pool = [[NSAutoreleasePool alloc] init];
	app = [NSApplication sharedApplication];
	uiStyle = NSTitledWindowMask | NSResizableWindowMask | NSClosableWindowMask;
	backingStoreStyle = NSBackingStoreBuffered;
	return 1;
}

int ubwHandleEvent(void) {
	[NSApp run];
	[pool drain];
	return 0;
}

UBW ubwCreate(void) {
	NSWindow* win = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 500, 500) styleMask:uiStyle backing:backingStoreStyle defer:NO];
	[win makeKeyAndOrderFront:win];
	[win makeMainWindow];
	_UBWPVT *wnd = calloc(1, sizeof(_UBWPVT));
	wnd->ntvPtr = (void *)win;
	wndCount++;
	return (UBW)wnd;
}

#define _NSWND (NSWindow)((_UBWPVT *)wnd)->ntvPtr

int ubwGetTitle(UBW wnd, char *title) {
	return 0;
}

void ubwSetTitle(UBW wnd, const char *title) {

}

void ubwMove(UBW wnd, int x, int y) {

}

void ubwMoveToScreenCenter(UBW wnd) {

}

void ubwResize(UBW wnd, int width, int height) {

}

void ubwShow(UBW wnd) {

}

void ubwHide(UBW wnd) {

}

#undef _NSWND
#endif // UBWINDOW_COCOA
