#include "wstp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void parse_expr(WSLINK link, const char* expr) {
    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "ToExpression", 1);
    WSPutString(link, expr);
}

void sync_directory_to_server(WSLINK link) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        WSPutFunction(link, "EvaluatePacket", 1);
        WSPutFunction(link, "SetDirectory", 1);
        WSPutString(link, cwd);
        WSEndPacket(link);
        while (WSNextPacket(link) != RETURNPKT) {
            WSNewPacket(link);
        }
        WSNewPacket(link);
    } else {
        printf("Warning: Could not get current working directory.\n");
    }
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

    enum WSTP_TYPE {
        NEW = 1,
        OLD = 2,
        NET = 3
    };

    /* Init Env */
    env = WSInitialize((WSEnvironmentParameter)0);
    if (env == (WSENV)0) {
        printf("WSInitialize failed\n");
        return 1;
    }

    /* Launch(Connect to) Wolfram Kernel */
    enum WSTP_TYPE wstp_type = NET;
    switch (wstp_type) {
        case NEW:
            /* launch a new kernel */
            link = WSOpenString(env, "-linkmode launch -linkname 'wolfram -wstp'", &err);
            break;
        case OLD:
            /* use command "wolfram -wstp -linkmode listen -linkname mylink" */
            link = WSOpenString(env, "-linkmode connect -linkname mylink", &err);
            break;
        case NET:
            /* connect to local port 5000, use TCPIP */
            link = WSOpenString(env, "-linkmode connect -linkname 5000 -linkprotocol TCPIP", &err);
            break;
        default:
            printf("ERROR: Invalid WSTP connection type.\n");
            return 1;
    }
    if (link == (WSLINK)0 || err != WSEOK) {
        printf("WSOpenString failed. Is the Wolfram Server running?\n");
        return 1;
    }

    /* Send Package */
    // NOTE: discard initial pkg only when launch new Kernel
    if (wstp_type == NEW) {
        WSNextPacket(link);
        WSNewPacket(link);
    }

    // sycn working directory
    sync_directory_to_server(link);

    // parse expr
    const char* expr = "1+1";
    if (argc > 1) {
        expr = argv[1];
    }
    parse_expr(link, expr);
    WSEndPacket(link);

    /* Wait for response */
    while (WSNextPacket(link) != RETURNPKT) {
        WSNewPacket(link);
    }
    const char *result;
    if (WSGetString(link, &result)) {
        collapse_backslash(result);
        WSReleaseString(link, result);
    } else {
        printf("Failed to get result(string)\n");
    }

    /* Disconnet */
    WSClose(link);
    WSDeinitialize(env);

    return 0;
}
