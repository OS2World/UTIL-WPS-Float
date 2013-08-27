/*** window floater, using tasklist name ***/

#define INCL_PM
#define INCL_DOSPROCESS

#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** private version of toupper ***/

char toupper(char c)
{
    if ((c >= 'a') && (c <= 'z'))
    {
        return c + 'A' - 'a';
    }
    else
    {
        return c;
    }
}

/*** check if the given string matches task name (for subsytem exe) ***/

BOOL FloatMatchTaskName(PSZ pszTitle, PSZ pszTaskName)
{
    for (; *pszTaskName; pszTitle ++, pszTaskName ++)
    {
        if ((!(*pszTitle)) || (toupper(*pszTitle) != toupper(*pszTaskName)))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*** find window handle of the task ***/

HWND FloatFindWindow(PSZ pszTitle)
{
    PSWBLOCK pswblock;
    ULONG cbSize;
    ULONG ulCount;
    HAB hab;
    PSWENTRY pswentry;
    PSWCNTRL pswcntrl;

    hab = WinQueryAnchorBlock(HWND_DESKTOP);

    ulCount = WinQuerySwitchList(hab, NULL, 0L);

    cbSize = sizeof(SWENTRY) * ulCount + sizeof(ULONG);
    pswblock = (PSWBLOCK)malloc(cbSize);

    ulCount = WinQuerySwitchList(hab, pswblock, cbSize);

    for (ulCount = 0; ulCount < pswblock->cswentry; ulCount ++)
    {
        pswentry = pswblock->aswentry + ulCount;
        pswcntrl = &pswentry->swctl;

        if (FloatMatchTaskName(pswcntrl->szSwtitle, pszTitle))
        {
            return pswcntrl->hwnd;
        }
    }

    return (HWND)NULLHANDLE;
}

/*** main routine ***/

#define MAIN_OK 0
#define MAIN_ERROR -1

int main(int lArgc, PSZ pszArgv[])
{
    LONG lInterval;
    HWND hwnd;
    PSZ pszTaskName;
    PSZ pszInterval;
    SWP swpSaved;

    if (lArgc < 3)
    {
        printf("usage: float interval-in-ms partial-taskname\n");
        exit(MAIN_ERROR);
    }

    pszInterval = pszArgv[1];
    pszTaskName = pszArgv[2];

    lInterval = atol(pszInterval);

    if (*pszTaskName == '0')
    {
        pszTaskName ++;
        if (toupper(*pszTaskName) == 'X')
        {
            sscanf(++pszTaskName, "%lX", &hwnd);
        }
        else
        {
            sscanf(pszTaskName, "%ld", &hwnd);
        }
    }
    else
    {
        hwnd = FloatFindWindow(pszTaskName);
    }

    if (!hwnd)
    {
        printf("error: no window found for [%s]\n", pszTaskName);
        exit(MAIN_ERROR);
    }

    if (WinIsWindowShowing(hwnd))
    {
        if (!WinQueryWindowPos(hwnd, &swpSaved))
        {
            printf("error: cannot save position\n");
            exit(MAIN_ERROR);
        }
    }
    else
    {
        printf("error: the window must be showing\n");
        exit(MAIN_ERROR);
    }

    for (;;)
    {
        if (!WinIsWindowShowing(hwnd))
        {
            if (!WinSetWindowPos(hwnd, HWND_TOP, swpSaved.x, swpSaved.y, 0L, 0L, SWP_MOVE))
            {
                printf("error: cannot move to saved position\n");
                exit(MAIN_ERROR);
            }
        }
        else
        {
            if (!WinQueryWindowPos(hwnd, &swpSaved))
            {
                printf("error: cannot update saved position\n");
                exit(MAIN_ERROR);
            }
        }

        if (WinSetWindowPos(hwnd, HWND_TOP, 0L, 0L, 0L, 0L, SWP_ZORDER))
        {
            DosSleep(lInterval);
        }
        else
        {
            printf("error: cannot change z-order\n");
            exit(MAIN_ERROR);
        }
    }

    exit(MAIN_OK);
}
