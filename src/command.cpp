#include "command.h"
#include "world.h"
#include "utils.h"
#include "com_gen.h"
#include "com_wiz.h"
#include "com_builder.h"
#include "com_movement.h"
#include "match.h"

static bool chook_comp(CommandHook* a, CommandHook* b)
{
    return (a->id < b->id);
}

Command::Command()
{
    _name = "";
    _subcmd=0;
    _type = CommandType::Misc;
    _access = BitSet(0, RANK_PLAYER);
    _position = POSITION::any;
}
Command::~Command()
{
}

void Command::SetName(const std::string &name)
{
    _name = name;
}
std::string Command::GetName() const
{
    return _name;
}

BOOL Command::AddAlias(const std::string &name)
{
    if (HasAlias(name))
        {
            return false;
        }

    _aliases.push_back(name);
    return true;
}
BOOL Command::RemoveAlias(const std::string &name)
{
    std::vector <std::string>::iterator it, itEnd;

    itEnd=_aliases.end();
    for (it = _aliases.begin(); it != itEnd; ++it)
        {
            if (FullMatch((*it), name))
                {
                    _aliases.erase(it);
                    return true;
                }
        }

    return false;
}
BOOL Command::HasAlias(const std::string &name, BOOL partialMatch)
{
    for (auto it: _aliases)
        {
            if (FullMatch(it,name))
                {
                    return true;
                }
            else if ((partialMatch) && (name.length() < it.length()) && (it.substr(name.length()) == name))
                {
                    return true;
                }
        }

    return false;
}

void Command::SetSubcmd(int subcmd)
{
    _subcmd = subcmd;
}
int Command::GetSubcmd() const
{
    return _subcmd;
}
void Command::SetType(CommandType type)
{
    _type = type;
}
CommandType Command::GetType() const
{
    return _type;
}
void Command::SetAccess(FLAG access)
{
    _access = access;
}
FLAG Command::GetAccess() const
{
    return _access;
}
void Command::SetPosition(POSITION position)
{
    _position = position;
}
POSITION Command::GetPosition() const
{
    return _position;
}
BOOL Command::CanExecute(Player* mobile, int subcmd)
{
    return true;
}
void Command::Syntax(Player* mobile, int subcmd) const
{
}
//hook operations.
bool Command::AddHook(CommandHook* hook, std::list<CommandHook*>* _hooks)
{
    if (!hook)
        {
            return false;
        }

    _hooks->push_back(hook);
    _hooks->sort(chook_comp);
    return true;
}
bool Command::RemoveHook(int id, std::list<CommandHook*>* _hooks)
{
    std::list<CommandHook*>::iterator it, itEnd;

    itEnd = _hooks->end();
    for (it = _hooks->begin(); it != itEnd; ++it)
        {
            if ((*it)->id == id)
                {
                    _hooks->erase(it);
                    return true;
                }
        }

    return false;
}
bool Command::ExecuteHooks(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd, std::list<CommandHook*>* _hooks)
{
    for (auto it: *_hooks)
        {
            if (!it->cb(verb, mobile, args, subcmd))
                {
                    return false;
                }
        }

    return true;
}

bool Command::AddPrehook(CommandHook* hook)
{
    return AddHook(hook, &_prehook);
}
bool Command::RemovePrehook(int id)
{
    return RemoveHook(id, &_prehook);
}
bool Command::ExecutePrehooks(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return ExecuteHooks(verb, mobile, args, subcmd, &_prehook);
}

bool Command::AddPosthook(CommandHook* hook)
{
    return AddHook(hook, &_posthook);
}
bool Command::RemovePosthook(int id)
{
    return RemoveHook(id, &_posthook);
}
bool Command::ExecutePosthooks(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
    return ExecuteHooks(verb, mobile, args, subcmd, &_posthook);
}

Commandable::Commandable()
{
}
Commandable::~Commandable()
{
    for (auto it: _commands)
        {
            delete it;
        }
}

BOOL Commandable::AddCommand(Command* com)
{
    if (CommandExists(com->GetName()))
        {
            return false;;
        }

    _commands.push_back(com);
    return true;
}
BOOL Commandable::RemoveCommand(const std::string &name)
{
    std::vector<Command*>::iterator it, itEnd;

    if (!CommandExists(name))
        {
            return false;
        }

    itEnd = _commands.end();
    for (it = _commands.begin(); it != itEnd; ++it)
        {
            if (FullMatch((*it)->GetName(),name))
                {
                    _commands.erase(it);
                    return true;
                }
        }

    return false;
}
BOOL Commandable::CommandExists(const std::string &name)
{
    for (auto it: _commands)
        {
            if (FullMatch(it->GetName(), name))
                {
                    return true;
                }
        }

    return false;
}
void Commandable::ListCommands(std::vector <std::string>* list, Player* mobile, CommandType filter)
{
    for (auto it: _commands)
        {
            if (mobile->HasAccess(it->GetAccess()))
                {
                    if ((it->GetType() == filter) || (filter == CommandType::All))
                        {
                            list->push_back(it->GetName());
                        }
                }
        }
}

std::vector <Command*> *Commandable::GetPtr()
{
    return &_commands;
}


BOOL InitializeCommands()
{
    World* world = World::GetPtr();

    world->WriteLog("Initializing commands.");
    InitializeGenCommands();
    InitializeMovementCommands();
    InitializeWizCommands();
    InitializeBuilderCommands();
    return true;
}
