#include "./Dialog.h"
#include "../String/String.h"
#import <Cocoa/Cocoa.h>

string_t OpenFileDialog(const char *prompt, const char *message) {
    @autoreleasepool {
        NSString* nsPrompt = (prompt == NULL) ? nil : [NSString stringWithUTF8String:prompt];
        NSString* nsMessage = (message == NULL) ? nil : [NSString stringWithUTF8String:message];
        NSOpenPanel* openDlg = [NSOpenPanel openPanel];
        [openDlg setDirectoryURL:[NSURL fileURLWithPath: NSHomeDirectory()]];
        [openDlg setPrompt: nsPrompt];
        [openDlg setMessage: nsMessage];
        NSInteger ret = [openDlg runModal];
        if (ret > 0) {
            NSString *path = [[openDlg URL] path];
            return String.Create([path cStringUsingEncoding:NSUTF8StringEncoding]);
        } else {
            return String.Create(NULL);
        }
    }
}

string_t OpenFileDialogUTF8(const utf8_t *prompt, const utf8_t *message) {
    return OpenFileDialog(prompt, message);
}

string_t SaveFileDialog(const char *prompt, const char *message) {
    @autoreleasepool {
        NSString* nsPrompt = (prompt == NULL) ? nil : [NSString stringWithUTF8String:prompt];
        NSString* nsMessage = (message == NULL) ? nil : [NSString stringWithUTF8String:message];
        NSSavePanel* openDlg = [NSSavePanel savePanel];
        [openDlg setDirectoryURL:[NSURL fileURLWithPath: NSHomeDirectory()]];
        [openDlg setCanCreateDirectories:YES];
        [openDlg setPrompt: nsPrompt];
        [openDlg setMessage: nsMessage];
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

string_t SaveFileDialogUTF8(const utf8_t *prompt, const utf8_t *message) {
    return SaveFileDialog(prompt,message);
}

const struct __tagDialogAPI Dialog = {
    OpenFileDialog,
    OpenFileDialogUTF8,
    SaveFileDialog,
    SaveFileDialogUTF8,
};
