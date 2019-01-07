
#include "ir.h"
#include <IRremote.h>
#include "settings.h"

IRrecv irrecv(RECV_PIN);

decode_results results;

void IRrecvInit()
{
	irrecv.enableIRIn();
}

unsigned long IRgetValue()
{
	if (irrecv.decode(&results))
    {
        DEBUG_PRINTLN(results.value, HEX);
        return results.value;
    }
    return 0;
}

void IRresume()
{
	irrecv.resume();
}
