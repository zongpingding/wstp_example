#include "wstp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void wstp_send_pkt(WSLINK link, const char* expr) {
    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "ToString", 1); 
    WSPutFunction(link, "ToExpression", 1);
    WSPutString(link, expr);
}

void wstp_sync_servercwd(WSLINK link) {
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

// WSTP will translate '\n' to '\012'
void wstp_print_str(const char *s, int is_text_pkt) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '\\' && s[i+1] == '0' && s[i+2] == '1' && s[i+3] == '2') {
            putchar('\n');
            i += 3;
        } 
        else if (s[i] == '\\' && s[i + 1] == '\\') {
            putchar('\\');
            i++;
        } 
        else {
            putchar(s[i]);
        }
    }
    if (!is_text_pkt) {
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    WSENV env;
    WSLINK link;
    int err;

    enum WSTP_TYPE {
        NEW = 1,
        OLD = 2,
        TCPIP = 3
    };

    /* Init Env */
    env = WSInitialize((WSEnvironmentParameter)0);
    if (env == (WSENV)0) {
        printf("WSInitialize failed\n");
        return 1;
    }

    /* Launch(Connect to) Wolfram Kernel */
    enum WSTP_TYPE wstp_type = TCPIP;
    switch (wstp_type) {
        case NEW:
            /* launch a new kernel */
            link = WSOpenString(env, "-linkmode launch -linkname 'wolfram -wstp'", &err);
            break;
        case OLD:
            /* use command 'wolfram -wstp -linkmode listen -linkname mylink' */
            link = WSOpenString(env, "-linkmode connect -linkname mylink", &err);
            break;
        case TCPIP:
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

    /* Sync working directory */
    wstp_sync_servercwd(link);

    /* Send Package */
    const char* expr = "1+1";
    if (argc > 1) {
        expr = argv[1];
    }
    if (!WSActivate(link)) {
        printf("ERROR: wstp client connects failed.\n");
        return 1;
    }
    WSNewPacket(link);
    wstp_send_pkt(link, expr);
    WSEndPacket(link);

    /* Wait for response */
    int pkt;
    while ((pkt = WSNextPacket(link)) != RETURNPKT && pkt != 0) {
        if (pkt == TEXTPKT) {
            const char *text;
            if (WSGetString(link, &text)) {
                wstp_print_str(text, 1); 
                WSReleaseString(link, text);
            }
        } else {
            WSNewPacket(link);
        }
    }
    
    const char *result;
    if (WSGetString(link, &result)) {
        if (strcmp(result, "Null") != 0) {
            wstp_print_str(result, 0);
        }
        WSReleaseString(link, result);
    } else {
        printf("Failed to get result(string)\n");
    }

    /* Disconnect */
    WSClose(link);
    WSDeinitialize(env);

    return 0;
}
