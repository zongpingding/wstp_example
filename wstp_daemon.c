#include "wstp.h"
#include <stdio.h>
#include <string.h>

enum WSTP_TYPE {
    NEW = 1,
    OLD = 2,
    NET = 3
};

void eval_string(WSLINK link, const char* expr) {
    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "ToString", 1);
    WSPutFunction(link, "TeXForm", 1);
    WSPutFunction(link, "ToExpression", 1);
    WSPutString(link, expr);
    WSPutSymbol(link, "InputForm");
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
            /* launch a new kernal */
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
    /* NOTE: discard inital pkg only when launch new Kernel */
    if (wstp_type == NEW) {
        WSNextPacket(link);
        WSNewPacket(link);
    }

    // (Expr)Argument
    const char* expr = "Integrate[1/(x^5+1), x]";
    if (argc > 1) {
        expr = argv[1];
    }
    eval_string(link, expr);


    // wait for response
    while (WSNextPacket(link) != RETURNPKT) {
        WSNewPacket(link);
    }
    const char *result;
    if (WSGetString(link, &result)) {
        collapse_backslash(result);
        WSReleaseString(link, result);
    } else {
        printf("Failed to get string\n");
    }

    // disconnet
    WSClose(link);
    WSDeinitialize(env);

    return 0;
}
