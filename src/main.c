/**
 * The runner function for SwitchCrypt
 */

#include "switchcrypt.h"

int main(int argc, char * argv[])
{
    int ret = -1;
    volatile CEXCEPTION_T e = EXCEPTION_NO_EXCEPTION;

    Try
    {
        ret = strongbox_main(argc, argv);
    }

    Catch(e)
    {
        CEXCEPTION_NO_CATCH_HANDLER(e);
    }

    return ret;
}
