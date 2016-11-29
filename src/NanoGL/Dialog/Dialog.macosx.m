#include "./Dialog.h"
#include "../String/String.h"
#import <Cocoa/Cocoa.h>

string_t OpenFileDialog(const char *title, const char *filter) {
    @autoreleasepool {
        NSString* nsTitle = (title == NULL) ? nil : [NSString stringWithUTF8String:title];
        NSString* nsFilter = (filter == NULL) ? nil : [NSString stringWithUTF8String:filter];
        NSMutableArray* naFileTypes = [@[] mutableCopy];

        char *filter_copy = strdup(filter);
        char *capture = NULL;
        for (char *p != filter_copy; *p != '\0'; p++) {
            switch (*p) {
                case '|' : 
                    *p = '\0'; 
                    capture = p+1
                    break;
                case ';' : 
                    *p = '\0'; 
                    if (capture != NULL) {
                        while (*capture != '.' && *capture != '\0') {
                            capture++;
                        }
                        if (*capture == '.') {
                            capture++;
                            [naFileTypes addObject: [NSString stringWithUTF8String: capture]];
                        }
                        capture = p+1;
                    }
                    break;
                case '\n' : 
                    *p = '\0'; 
                    if (capture != NULL) {
                        while (*capture != '.' && *capture != '\0') {
                            capture++;
                        }
                        if (*capture == '.') {
                            capture++;
                            [naFileTypes addObject: [NSString stringWithUTF8String: capture]];
                        }
                        capture = NULL;
                    }
                    break;
                default: break;
            }
        }
        if (capture != NULL) {
            [naFileTypes addObject: [NSString stringWithUTF8String: capture]];
            capture = NULL;
        }
        free(filter_copy);
        NSOpenPanel* openDlg = [NSOpenPanel openPanel];
        [openDlg setDirectoryURL:[NSURL fileURLWithPath: NSHomeDirectory()]];
        [openDlg setAllowedFileTypes:naFileTypes]
        //[openDlg setPrompt: "Open"];
        [openDlg setMessage: nsTitle];
        NSInteger ret = [openDlg runModal];
        if (ret > 0) {
            NSString *path = [[openDlg URL] path];
            return String.Create([path cStringUsingEncoding:NSUTF8StringEncoding]);
        } else {
            return String.Create(NULL);
        }
    }
}

string_t OpenFileDialogUTF8(const char *title, const char *filter) {
    return OpenFileDialog(title, filter);
}

string_t SaveFileDialog(const char *prompt, const char *message) {
    @autoreleasepool {
        NSString* nsPrompt = (prompt == NULL) ? nil : [NSString stringWithUTF8String:prompt];
        NSString* nsMessage = (message == NULL) ? nil : [NSString stringWithUTF8String:message];
        NSSavePanel* openDlg = [NSSavePanel savePanel];
        [openDlg setDirectoryURL:[NSURL fileURLWithPath: NSHomeDirectory()]];
        [openDlg setCanCreateDirectories:YES];
        //[openDlg setPrompt: "Save"];
        [openDlg setMessage: nsTitle];
        [openDlg setNameFieldStringValue:@"untitled.txt"];
        NSInteger ret = [openDlg runModal];
        if (ret > 0) {
            NSString *path = [[openDlg URL] path];
            return String.Create([path cStringUsingEncoding:NSUTF8StringEncoding]);
        } else {
            return String.Create(NULL);
        }
    }
}

string_t SaveFileDialogUTF8(const utf8_t *title, const utf8_t *filter) {
    return SaveFileDialog(title, filter);
}

const struct __tagDialogAPI Dialog = {
    OpenFileDialog,
    OpenFileDialogUTF8,
    SaveFileDialog,
    SaveFileDialogUTF8,
};
