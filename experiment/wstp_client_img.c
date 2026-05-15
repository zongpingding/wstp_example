#include "wstp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void wstp_send_text_cmd(WSLINK link, const char* expr) {
    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "ToString", 1); 
    WSPutFunction(link, "ToExpression", 1);
    WSPutString(link, expr);
}

void wstp_send_image_cmd(WSLINK link, const char* expr) {
    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "Normal", 1);             
    WSPutFunction(link, "ExportByteArray", 2);   
    WSPutFunction(link, "ToExpression", 1);     
    WSPutString(link, expr);
    WSPutString(link, "PNG");
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

    int is_image_mode = 0;
    const char* expr = "1+1";
    const char* output_file = "output.png";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--image") == 0) {
            is_image_mode = 1;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else {
            expr = argv[i];
        }
    }

    /* Init Env */
    env = WSInitialize((WSEnvironmentParameter)0);
    if (env == (WSENV)0) {
        printf("WSInitialize failed\n");
        return 1;
    }

    /* Connect to Wolfram Server */
    link = WSOpenString(env, "-linkmode connect -linkname 5000 -linkprotocol TCPIP", &err);
    if (link == (WSLINK)0 || err != WSEOK) {
        printf("WSOpenString failed. Is the Wolfram Server running?\n");
        return 1;
    }

    /* Sync working directory */
    wstp_sync_servercwd(link);

    if (!WSActivate(link)) {
        printf("ERROR: wstp client connects failed.\n");
        return 1;
    }

    /* Send Package based on mode */
    WSNewPacket(link);
    if (is_image_mode) {
        wstp_send_image_cmd(link, expr);
    } else {
        wstp_send_text_cmd(link, expr);
    }
    WSEndPacket(link);

    /* Wait for response & Intercept Print output */
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

    /* Process Final Result */
    int result_type = WSGetType(link);

    if (result_type == WSTKSTR) {
        const char *result;
        if (WSGetString(link, &result)) {
            if (strcmp(result, "Null") != 0) {
                wstp_print_str(result, 0);
            }
            WSReleaseString(link, result);
        }
        
    } else if (result_type == WSTKFUNC) {
        int *data;
        int length;
        
        if (WSGetInteger32List(link, &data, &length)) {
            printf("[Image Mode] Received Binary Array. Size: %d bytes\n", length);
            
            FILE *fp = fopen(output_file, "wb"); 
            if (fp != NULL) {
                for(int i = 0; i < length; i++) {
                    fputc((unsigned char)data[i], fp); 
                }
                fclose(fp);
                printf("[Image Mode] Success: Image saved to '%s'\n", output_file);
            } else {
                printf("[Image Mode] Error: Could not create '%s'\n", output_file);
            }
            WSReleaseInteger32List(link, data, length);
        } else {
            printf("Warning: Received a function, but it is not a valid byte array.\n");
            WSNewPacket(link);
        }
        
    } else if (result_type == WSTKSYM) {
        // In case 'ExportByteArray' return '$Failed'
        const char *sym;
        if (WSGetSymbol(link, &sym)) {
            printf("Evaluation Error (Symbol): %s\n", sym);
            WSReleaseSymbol(link, sym);
        }
    } else {
        printf("Received unsupported WSTP type code: %d\n", result_type);
        WSNewPacket(link);
    }

    /* Disconnect */
    WSClose(link);
    WSDeinitialize(env);

    return 0;
}
