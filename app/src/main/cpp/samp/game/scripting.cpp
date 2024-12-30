#include "../main.h"
#include "scripting.h"

GAME_SCRIPT_THREAD* gst;
char ScriptBuf[0xFF];
uintptr_t *pdwParamVars[18];

uint8_t ExecuteScriptBuf()
{
    gst->dwScriptIP = (uintptr_t)ScriptBuf;
    (( void (*)(GAME_SCRIPT_THREAD*))(g_libGTASA + (VER_x32 ? 0x0032B708 + 1 : 0x3F445C)))(gst);

    return gst->condResult;
}

int ScriptCommand(const SCRIPT_COMMAND *pScriptCommand, ...)
{
    if (!pScriptCommand || !pScriptCommand->Params) {
        //FLog("Error: Invalid ScriptCommand structure!");
        return 0;
    }

    va_list ap;
    const char* p = pScriptCommand->Params;
    va_start(ap, pScriptCommand);
    memcpy(&ScriptBuf, &pScriptCommand->OpCode, 2);
    int buf_pos = 2;
    uint16_t var_pos = 0;

    for (int i = 0; i < 18; i++) {
        gst->dwLocalVar[i] = 0;
    }

    //FLog("ScriptCommand: OpCode=0x%04x, Params=%s", pScriptCommand->OpCode, pScriptCommand->Params);

    while (*p) {
        if (buf_pos >= sizeof(ScriptBuf)) {
            //FLog("Error: Buffer overflow detected in ScriptBuf!");
            va_end(ap);
            return 0;
        }

        switch (*p) {
            case 'i': {
                int i = va_arg(ap, int);
                if (buf_pos + 5 > sizeof(ScriptBuf)) {
                    //FLog("Error: Buffer overflow detected while processing 'i'!");
                    va_end(ap);
                    return 0;
                }
                ScriptBuf[buf_pos++] = 0x01;
                memcpy(&ScriptBuf[buf_pos], &i, 4);
                buf_pos += 4;
                break;
            }
            case 'f': {
                float f = (float)va_arg(ap, double);
                if (buf_pos + 5 > sizeof(ScriptBuf)) {
                    //FLog("Error: Buffer overflow detected while processing 'f'!");
                    va_end(ap);
                    return 0;
                }
                ScriptBuf[buf_pos++] = 0x06;
                memcpy(&ScriptBuf[buf_pos], &f, 4);
                buf_pos += 4;
                break;
            }
            case 'v': {
                if (var_pos >= sizeof(pdwParamVars) / sizeof(pdwParamVars[0])) {
                    //FLog("Error: Too many variables for ScriptCommand!");
                    va_end(ap);
                    return 0;
                }
                uintptr *v = va_arg(ap, uintptr*);
                ScriptBuf[buf_pos++] = 0x03;
                pdwParamVars[var_pos] = v;
                gst->dwLocalVar[var_pos] = *v;
                memcpy(&ScriptBuf[buf_pos], &var_pos, 2);
                buf_pos += 2;
                var_pos++;
                break;
            }
            case 's': {
                char* sz = va_arg(ap, char*);
                unsigned char aLen = strlen(sz);
                if (buf_pos + aLen + 2 > sizeof(ScriptBuf)) {
                    //FLog("Error: Buffer overflow detected while processing 's'!");
                    va_end(ap);
                    return 0;
                }
                ScriptBuf[buf_pos++] = 0x0E;
                ScriptBuf[buf_pos++] = aLen;
                memcpy(&ScriptBuf[buf_pos], sz, aLen);
                buf_pos += aLen;
                break;
            }
            case 'z': {
                if (buf_pos + 1 > sizeof(ScriptBuf)) {
                    //FLog("Error: Buffer overflow detected while processing 'z'!");
                    va_end(ap);
                    return 0;
                }
                ScriptBuf[buf_pos++] = 0x00;
                break;
            }
            default: {
                //FLog("Error: Invalid parameter type '%c'", *p);
                va_end(ap);
                return 0;
            }
        }
        ++p;
    }

    va_end(ap);

    //FLog("Executing ScriptBuf...");
    int result = ExecuteScriptBuf();
    if (var_pos) {
        for (int i = 0; i < var_pos; i++) {
            *pdwParamVars[i] = gst->dwLocalVar[i];
        }
    }

    //FLog("ScriptCommand execution finished with result: %d", result);
    return result;
}

void InitScripting()
{
    FLog("InitScripting");
    gst = new GAME_SCRIPT_THREAD;
    memset(gst, 0, sizeof(GAME_SCRIPT_THREAD));

}