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
        for (char *p = filter_copy; *p != '\0'; p++) {
            switch (*p) {
                case '|' : 
                    *p = '\0'; 
                    capture = p+1;
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
                default:
                    break;
            }
        }
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
        free(filter_copy);
        NSOpenPanel* openDlg = [NSOpenPanel openPanel];
        [openDlg setDirectoryURL:[NSURL fileURLWithPath: NSHomeDirectory()]];
        if ([naFileTypes count] != 0) {
            [openDlg setAllowedContentTypes:naFileTypes];
        }
        //[openDlg setPrompt: "Open"];
        [openDlg setMessage: nsTitle];
        NSInteger ret = [openDlg runModal];
        if (ret > 0) {
            NSString *path = [[openDlg URL] path];
            string_t result = String.Create([path cStringUsingEncoding:NSUTF8StringEncoding]);
            
            char *last_lf = strrchr(result.c_str,'\n');
            if (last_lf != NULL && last_lf[1] == '\0') {
                last_lf[0] = '\0';
            }
            return result;
        } else {
            return String.Create(NULL);
        }
    }
}

string_t OpenFileDialogUTF8(const char *title, const char *filter) {
    return OpenFileDialog(title, filter);
}

string_t SaveFileDialog(const char *title, const char *filter) {
    @autoreleasepool {
        NSString* nsTitle = (title == NULL) ? nil : [NSString stringWithUTF8String:title];
        NSString* nsFilter = (filter == NULL) ? nil : [NSString stringWithUTF8String:filter];
        NSMutableArray* naFileTypes = [@[] mutableCopy];
        
        char *filter_copy = strdup(filter);
        char *capture = NULL;
        for (char *p = filter_copy; *p != '\0'; p++) {
            switch (*p) {
                case '|' :
                    *p = '\0';
                    capture = p+1;
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
            while (*capture != '.' && *capture != '\0') {
                capture++;
            }
            if (*capture == '.') {
                capture++;
                [naFileTypes addObject: [NSString stringWithUTF8String: capture]];
            }
            capture = NULL;
        }
        free(filter_copy);
        NSSavePanel* openDlg = [NSSavePanel savePanel];
        [openDlg setDirectoryURL:[NSURL fileURLWithPath: NSHomeDirectory()]];
        if ([naFileTypes count] != 0) {
            [openDlg setAllowedContentTypes:naFileTypes];
        }
        [openDlg setMessage: nsTitle];
        NSInteger ret = [openDlg runModal];
        if (ret > 0) {
            NSString *path = [[openDlg URL] path];
            string_t result = String.Create([path cStringUsingEncoding:NSUTF8StringEncoding]);
            
            char *last_lf = strrchr(result.c_str,'\n');
            if (last_lf != NULL && last_lf[1] == '\0') {
                last_lf[0] = '\0';
            }
            return result;
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
