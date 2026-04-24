#include "wstp.h"   // NOT 'mathlink.h' anymore
#include <stdio.h>
#include <string.h>


void eval_string(WSLINK link, const char* expr) {
    WSPutFunction(link, "EvaluatePacket", 1);

    WSPutFunction(link, "ToString", 1);
    WSPutFunction(link, "TeXForm", 1);
    WSPutFunction(link, "ToExpression", 1);
    WSPutString(link, expr);

    WSEndPacket(link);
}

void collapse_backslash(const char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '\\' && s[i + 1] == '\\') {
            putchar('\\');
            i++;
        } else {
            putchar(s[i]);
        }
    }
    printf("\n");
}


int main(int argc, char* argv[]) {
    WSENV env;
    WSLINK link;
    int err;

    env = WSInitialize((WSEnvironmentParameter)0);
    link = WSOpenString(env, "-linkmode launch -linkname 'wolfram -wstp'", &err);
    
    if (link == (WSLINK)0 || err != WSEOK) {
        printf("WSOpenString failed\n");
        return 1;
    }

    WSNextPacket(link);
    WSNewPacket(link);

    printf("Wolfram Kernel Started. Ready for input (type 'quit' to exit).\n");
    printf("My Own Wolfram Frontend ...");

    char input_buffer[1024];
    while (1) {
        printf("\nIn := ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            break; // EOF
        }
        input_buffer[strcspn(input_buffer, "\n")] = 0;
        if (strcmp(input_buffer, "quit") == 0 || strcmp(input_buffer, "exit") == 0) {
            break;
        }
        if (strlen(input_buffer) == 0) continue;
        eval_string(link, input_buffer);
        while (WSNextPacket(link) != RETURNPKT) {
            WSNewPacket(link);
        }
        const char *result;
        if (WSGetString(link, &result)) {
            printf("Out = ");
            collapse_backslash(result);
            WSReleaseString(link, result);
        } else {
            printf("Failed to get string\n");
        }
    }
    WSClose(link);
    WSDeinitialize(env);

    return 0;
}
