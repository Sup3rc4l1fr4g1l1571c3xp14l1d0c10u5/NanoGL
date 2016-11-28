#include "./Dialog.h"
#include "../String/String.h"

string_t OpenFileDialog(const char *prompt, const char *message) {
    return String.Create(NULL);
}

string_t OpenFileDialogUTF8(const utf8_t *prompt, const utf8_t *message) {
    return OpenFileDialog(prompt, message);
}

string_t SaveFileDialog(const char *prompt, const char *message) {
    return String.Create(NULL);
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
