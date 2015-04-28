#include <string>
#include "com_movement.h"
#include "player.h"
#include "world.h"
#include "objectManager.h"
#include "room.h"
#include "exit.h"

void InitializeMovementCommands()
{
    World* world = World::GetPtr();

    world->WriteLog("Initializing movement commands.");
    world->commands.AddCommand(new CMDNorth());
    world->commands.AddCommand(new CMDSouth());
    world->commands.AddCommand(new CMDEast());
    world->commands.AddCommand(new CMDWest());
    world->commands.AddCommand(new CMDNortheast());
    world->commands.AddCommand(new CMDNorthwest());
    world->commands.AddCommand(new CMDSoutheast());
    world->commands.AddCommand(new CMDSouthwest());
    world->commands.AddCommand(new CMDUp());
    world->commands.AddCommand(new CMDDown());
}

std::string DirectionString[] = { " ", "north", "south", "east", "west", "northeast", "northwest", "southeast", "southwest", "up", "down" };

std::string OppositeDirectionString[] = { " ", "south", "north", "west", "east", "southwest", "southeast", "northwest", "northeast", "down", "up" };

BOOL DoMove(const std::string &verb, Player* mobile, std::vector<std::string> &args, int subcmd)
{

	ExitDirection direction = (ExitDirection)subcmd;
	Room* room = NULL;
	RoomTile* dest;
	room = (Room*)mobile->GetLocation();

	if (!room)
	{
		if (mobile->GetTile() && mobile->GetTile()->GetParent())
		{
			room = mobile->GetTile()->GetParent();
		}
		mobile->Message(MSG_ERROR, "You're player isn't in a room!");
		return false;
	}


	if (!mobile->GetTile()->AdjacentExists(direction))
	{
		mobile->Message(MSG_ERROR, "You cannot go that way.");
		return false;
	}

	if (!mobile->GetTile()->CanEnter(mobile))
	{
		mobile->Message(MSG_ERROR, "Something is preventing you from entering that room.");
		return false;
	}

	dest = mobile->GetTile()->GetAdjacentTile(direction);
	mobile->MoveTo(dest);

	dest->events.CallEvent("OnEnter", NULL, (void*)dest);
	
	if (dest->GetParent() != room)
	{
		mobile->Message(MSG_INFO, "You move " + DirectionString[subcmd] + " to " + dest->GetParent()->GetName() + "[" + dest->GetKey() + "].");
		room->TellAllBut(mobile->GetName() + " moves " + DirectionString[subcmd] + " to " + dest->GetParent()->GetName() + "[" + dest->GetKey() + "].", mobile);
		mobile->Message(MSG_INFO, room->DoLook(mobile));
		room->TellAllBut(mobile->GetName() + " arrives " + OppositeDirectionString[subcmd] + " from " + room->GetName() + " to " + "[" + dest->GetKey() + "].", mobile);
		return true;
	}
	mobile->Message(MSG_INFO, "You move " + DirectionString[subcmd] + " to " + "[" + dest->GetKey() + "].");
	dest->GetParent()->TellAllBut(mobile->GetName() + " moves " + DirectionString[subcmd] + " to " + "[" + dest->GetKey() + "].", mobile);
	return true;
}


CMDNorth::CMDNorth()
{
    SetName("north");
    AddAlias("n");
    SetSubcmd(DIR_NORTH);
	SetType(CommandType::Movement);
}
BOOL CMDNorth::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDSouth::CMDSouth()
{
    SetName("south");
    AddAlias("s");
    SetSubcmd(DIR_SOUTH);
	SetType(CommandType::Movement);
}
BOOL CMDSouth::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDEast::CMDEast()
{
    SetName("east");
    AddAlias("e");
    SetSubcmd(DIR_EAST);
	SetType(CommandType::Movement);
}
BOOL CMDEast::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDWest::CMDWest()
{
    SetName("west");
    AddAlias("w");
    SetSubcmd(DIR_WEST);
	SetType(CommandType::Movement);
}
BOOL CMDWest::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDNortheast::CMDNortheast()
{
    SetName("northeast");
    AddAlias("ne");
    SetSubcmd(DIR_NORTHEAST);
	SetType(CommandType::Movement);
}
BOOL CMDNortheast::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDSoutheast::CMDSoutheast()
{
    SetName("southeast");
    AddAlias("se");
    SetSubcmd(DIR_SOUTHEAST);
	SetType(CommandType::Movement);
}
BOOL CMDSoutheast::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDSouthwest::CMDSouthwest()
{
    SetName("southwest");
    AddAlias("sw");
    SetSubcmd(DIR_SOUTHWEST);
	SetType(CommandType::Movement);
}
BOOL CMDSouthwest::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDNorthwest::CMDNorthwest()
{
    SetName("northwest");
    AddAlias("nw");
    SetSubcmd(DIR_NORTHWEST);
	SetType(CommandType::Movement);
}
BOOL CMDNorthwest::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDUp::CMDUp()
{
    SetName("up");
    AddAlias("u");
    SetSubcmd(DIR_UP);
	SetType(CommandType::Movement);
}
BOOL CMDUp::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}

CMDDown::CMDDown()
{
    SetName("down");
    AddAlias("d");
    SetSubcmd(DIR_DOWN);
	SetType(CommandType::Movement);
}
BOOL CMDDown::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return DoMove(verb, mobile, args, subcmd);
}
