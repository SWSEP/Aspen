#include "mud.h"
#include "conf.h"
#include "olcs.h"
#include "staticObject.h"
#include "room.h"
#include "entity.h"
#include "baseObject.h"
#include "npc.h"
#include "help/help.h"

BOOL InitializeOlcs()
{
	if (!InitializeRoomTileOlcs())
	{
		return false;
	}
	if (!InitializeHelpOlcs())
	{
		return false;
	}
    if (!InitializeBaseObjectOlcs())
        {
            return false;
        }
    if (!InitializeEntityOlcs())
        {
            return false;
        }
    if (!InitializeStaticObjectOlcs())
        {
            return  false;
        }
    if (!InitializeRoomOlcs())
        {
            return false;
        }
	if (!InitializeNPCOlcs())
	{
		return false;
	}
    return true;
}
