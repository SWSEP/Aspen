#include "../mud.h"
#include "../conf.h"
#include "../world.h"
#include "../player.h"
#include "../utils.h"
#include "../inputHandlers.h"
#include "../olc.h"
#include "../editor.h"
#include "../event.h"
#include "../eventargs.h"
#include "../olcManager.h"
#include "../olcGroup.h"
#include "../olc.h"
#include "help.h"
#include "HelpEntry.h"
#include "HelpTable.h"

#ifdef MODULE_HELP

CMDHelp::CMDHelp()
{
    SetName("help");
    AddAlias("h");
    SetType(CommandType::Information);
}
BOOL CMDHelp::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    World* world = World::GetPtr();
    HelpTable* table = NULL;

    if (!args.size())
        {
            mobile->Message(MSG_ERROR, "Syntax: help <topic>.\nYou can also use help <index> to get a listing of help categories.");
            return false;
        }

    table = (HelpTable*)world->GetProperty("help");
    if (args.size() == 1)
        {
            if (!table->ShowEntry(args[0], mobile))
                {
                    mobile->Message(MSG_ERROR, "That topic does not exist.");
                    return false;
                }
        }
    else
        {
            std::string arg;
            arg = Explode(args);
            if (!table->ShowEntry(arg, mobile))
                {
                    mobile->Message(MSG_ERROR, "That topic does not exist.");
                    return false;
                }
        }

    return true;
}

CMDHedit::CMDHedit()
{
    SetName("hedit");
    AddAlias("hed");
    SetAccess(RANK_ADMIN);
	SetType(CommandType::Admin);
}
BOOL CMDHedit::Execute(const std::string &verb, Player* mobile, std::vector<std::string> &args, int subcmd)
{

	World* world = World::GetPtr();
	HelpTable* table = (HelpTable*)world->GetProperty("help");
	HelpEntry* entry = nullptr;
	OlcGroup* help = world->GetOlcManager()->GetGroup(OLCGROUP::HELP);

	if (args.empty())
	{
		mobile->Message(MSG_ERROR, "Syntax: hedit <name> [field][value]");
		return false;
	}

	if (args.size() == 1 && table->EntryExists(args[0]))
	{
		ShowGroup(mobile, world->GetOlcManager()->GetGroup(OLCGROUP::HELP));
		return false;
	}

	if (!table->EntryExists(args[0]))
	{
		mobile->Message(MSG_ERROR, "That help file doesn't exist.");
		return false;
	}
	
	entry = table->GetEntry(args[0]);

	if (!entry)
	{
		mobile->Message(MSG_ERROR, "There was a problem retrieving the help file.");
		return false;
	}
	if (HandleEntry(mobile, entry, help, args, OlcEditType::Help))
	{
		table->Save();
		return true;
	}
	return false;
}

CMDHcreate::CMDHcreate()
{
	SetName("hcreate");
	AddAlias("hcr");
	SetAccess(RANK_ADMIN);
	SetType(CommandType::Admin);
}

BOOL CMDHcreate::Execute(const std::string &verb, Player *mobile, std::vector<std::string> &args, int subcmd)
{
	World* world = World::GetPtr();
	HelpTable* table = NULL;

	if (!args.size())
	{
		mobile->Message(MSG_ERROR, "Syntax: hcreate <name of topic> ");
		return false;
	}

	table = (HelpTable*)world->GetProperty("help");
	
	if (table->EntryExists(args[0]))
	{
		mobile->Message(MSG_ERROR, "That entry already exists. Use 'hedit'.");
		return false;
	}

	HelpEntry* entry = new HelpEntry;
	entry->SetName(args[0]);

	
	if (table->AddEntry(entry))
	{

		std::string output;
		output = "The helpfile for '" + entry->GetName() + "' has been created.";
		mobile->Message(MSG_INFO, output);
		table->Save();
		return true;
	}
	return false;
}
CMDHSave::CMDHSave()
{
	SetName("hsave");
	AddAlias("hs");
	SetAccess(RANK_ADMIN);
	SetType(CommandType::Admin);
}

BOOL CMDHSave::Execute(const std::string &verb, Player* mobile, std::vector<std::string> &args, int subcmd)
{
	World* world = World::GetPtr();
	HelpTable* table = (HelpTable*)world->GetProperty("help");

	if (!table)
	{
		mobile->Message(MSG_ERROR, "There's an error accessing the help file table.");
		return false;
	}

	table->Save();
	mobile->Message(MSG_INFO, "OK.");
	return true;
}
#endif

EVENT(CleanupHelp)
{
    World* world = World::GetPtr();

    HelpTable* table = (HelpTable*)world->GetProperty("help");
    if (table)
        {
            delete table;
        }
}

BOOL InitializeHelp()
{
#ifdef MODULE_HELP
    World* world = World::GetPtr();

    world->WriteLog("Initializing help.");
    HelpTable* table = new HelpTable();

	table->Load();
    world->AddProperty("help", (void*)table);
    world->events.AddCallback("Shutdown", CleanupHelp);
    world->commands.AddCommand(new CMDHelp());
    world->commands.AddCommand(new CMDHedit());
	world->commands.AddCommand(new CMDHcreate());
	world->commands.AddCommand(new CMDHSave());
#endif

    return true;
}
bool InitializeHelpOlcs()
{
	World* world = World::GetPtr();
	OlcManager* omanager = world->GetOlcManager();
	OlcGroup* group = new OlcGroup();

	group->AddEntry(new OlcStringEntry<HelpEntry>("name", "the name of the help file", OF_NORMAL, OLCDT::STRING,
		std::bind(&HelpEntry::GetName, std::placeholders::_1),
		std::bind(&HelpEntry::SetName, std::placeholders::_1, std::placeholders::_2)));
	group->AddEntry(new OlcEditorEntry<HelpEntry>("data", "the information in the help file", OF_NORMAL, OLCDT::EDITOR,
		std::bind(&HelpEntry::GetData, std::placeholders::_1),
		std::bind(&HelpEntry::SetData, std::placeholders::_1, std::placeholders::_2)));
	omanager->AddGroup(OLCGROUP::HELP, group);
	return true;
}