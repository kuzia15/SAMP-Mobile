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
    va_list ap;
    const char* p = pScriptCommand->Params;
    va_start(ap, pScriptCommand);
    memcpy(&ScriptBuf, &pScriptCommand->OpCode, 2);
    int buf_pos = 2;
    uint16_t var_pos = 0;

    for(int i = 0; i < 18; i++)
        gst->dwLocalVar[i] = 0;

    while(*p)
    {
        switch(*p)
        {
            case 'i':
            {
                int i = va_arg(ap, int);
                ScriptBuf[buf_pos] = 0x01;
                buf_pos++;
                memcpy(&ScriptBuf[buf_pos], &i, 4);
                buf_pos += 4;
                break;
            }
            case 'f':
            {
                float f = (float)va_arg(ap, double);
                ScriptBuf[buf_pos] = 0x06;
                buf_pos++;
                memcpy(&ScriptBuf[buf_pos], &f, 4);
                buf_pos += 4;
                break;
            }
            case 'v':
            {
                uintptr *v = va_arg(ap, uintptr*);
                ScriptBuf[buf_pos] = 0x03;
                buf_pos++;
                pdwParamVars[var_pos] = v;
                gst->dwLocalVar[var_pos] = *v;
                memcpy(&ScriptBuf[buf_pos], &var_pos, 2);
                buf_pos += 2;
                var_pos++;
                break;
            }
            case 's':	// If string... Updated 13th Jan 06.. (kyeman) SA string support
            {
                char* sz = va_arg(ap, char*);
                unsigned char aLen = strlen(sz);
                ScriptBuf[buf_pos] = 0x0E;
                buf_pos++;
                ScriptBuf[buf_pos] = aLen;
                buf_pos++;
                memcpy(&ScriptBuf[buf_pos],sz,aLen);
                buf_pos += aLen;
                break;
            }
            case 'z':	// If the params need zero-terminating...
            {
                ScriptBuf[buf_pos] = 0x00;
                buf_pos++;
                break;
            }
            default:
            {
                return 0;
            }
        }
        ++p;
    }
    va_end(ap);

    int result = ExecuteScriptBuf();
    if (var_pos)
    {
        for (int i=0; i < var_pos; i++)
            *pdwParamVars[i] = gst->dwLocalVar[i];
    }

    return result;
}

void InitScripting()
{
    FLog("InitScripting");
    gst = new GAME_SCRIPT_THREAD;
    memset(gst, 0, sizeof(GAME_SCRIPT_THREAD));

}