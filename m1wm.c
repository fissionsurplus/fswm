/* TinyWM is written by Nick Welch <nick@incise.org> in 2005 & 2011.
 *
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct Client
{
    Window win;               
    int x, y;        
    int width, height;        
    int border_width;       

    Bool is_floating;        
    Bool is_fullscreen;       
    Bool is_hidden;         
    Bool is_sticky;           /*This is here, simply lying around, unused for now. I *will* get back to this once I have support for multiple desktops*/
    
    struct Client *prev; 
    struct Client *next; 

    Bool is_urgent;           /*Urgent*/
    Bool has_focus;           

    int old_x, old_y, old_width, old_height;

    int monitor;             
    int desktop;             

    char *window_name;      
    char *class_name;         
} Client;

void findClients
(Display* dpy, Window root)
{
    /*We use this function to find client who were already running before the Window Manager was started*/
    Window root_ret, parent_ret;
    Window *children;
    unsigned int nchildren;
    if(XQueryTree(dpy, root, &root_ret, &parent_ret, &children, &nchildren))
    {
        printf("Direct child windows of the root window: \n");
        if(nchildren == 0)
        {
            printf("No child windows available. \n");
        } else {
            for(unsigned int i = 0; i < nchildren; i++)
            {
                Window child = children[i];
                XWindowAttributes attr;
                printf("Window ID: 0x%lx\n", children[i]);

                if(XGetWindowAttributes(dpy, child, &attr)) 
                {
                    printf("Window ID: 0x%lx, x: %d, y: %d, width: %d, height: %d\n", 
                                child, attr.x, attr.y, attr.width, attr.height);
                    Client *cl;
                    cl->win = child;
                    cl->width = attr.width;
                    cl->height = attr.height;
                    cl->x = attr.x;
                    cl->y = attr.y;
                    cl->border_width = attr.border_width;
                }
            }
        }
        XFree(children); /*free the kids*/
    } else {
        fprintf(stderr, "Failed to query window tree\n");
        printf("go away :(\n");
    }
    

}

int main(void)
{
    Display * dpy;
    Window root;
    XWindowAttributes attr;
    XButtonEvent start;
    XEvent ev;

    if(!(dpy = XOpenDisplay(0x0))) return 1;
    if(!(root = DefaultRootWindow(dpy))) return 1;

    XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("F1")), Mod1Mask,
            root, True, GrabModeAsync, GrabModeAsync);
    XGrabButton(dpy, 1, Mod1Mask, root, True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, 3, Mod1Mask, root, True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

    start.subwindow = None;
    findClients(dpy, root);
    for(;;)
    {
        XNextEvent(dpy, &ev);
        if(ev.type == KeyPress && ev.xkey.subwindow != None)
            XRaiseWindow(dpy, ev.xkey.subwindow);
        else if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
        {
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
            start = ev.xbutton;
        }
        else if(ev.type == MotionNotify && start.subwindow != None)
        {
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;
            XMoveResizeWindow(dpy, start.subwindow,
                attr.x + (start.button==1 ? xdiff : 0),
                attr.y + (start.button==1 ? ydiff : 0),
                MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
                MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
        }
        else if(ev.type == ButtonRelease)
            start.subwindow = None;
    }
}

