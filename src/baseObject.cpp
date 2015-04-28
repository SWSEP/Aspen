#include <string>
#include <sstream>
#include <list>
#include <tinyxml.h>
#include "entity.h"
#include "baseObject.h"
#include "world.h"
#include "utils.h"
#include "olc.h"
#include "olcManager.h"
#include "editor.h"
#include "component.h"
#include "eventargs.h"
#include "match.h"

BaseObject::BaseObject()
{
    events.RegisterEvent("PreLook", new Event());
    events.RegisterEvent("PostLook", new Event());
    events.RegisterEvent("OnEnter", new Event());
    events.RegisterEvent("OnExit", new Event());
    events.RegisterEvent("OnLook", new Event());

    _name="A blank object";
    _desc="You see nothing special.";
    _script="";
    _onum=0;
    _components=new std::vector<Component*>();
    _aliases = new std::vector<std::string>();
    variables.SetObject(this);
    _persistent = true;
    _zone = NULL;
}
BaseObject::~BaseObject()
{
    std::vector<Component*>::iterator it, itEnd;

    itEnd = _components->end();
    for (it=_components->begin(); it != itEnd; ++it)
        {
            delete (*it);
        }

    delete _components;
    delete _aliases;
}

std::string BaseObject::GetName() const
{
    return _name;
}
void BaseObject::SetName(const std::string &s)
{
    _name=s;
}


std::string BaseObject::GetDescription() const
{
    return _desc;
}
void BaseObject::SetDescription(const std::string &s)
{
    _desc=s;
}

std::string BaseObject::GetScript() const
{
    return _script;
}
void BaseObject::SetScript(const std::string &script)
{
    _script=script;
}
BOOL BaseObject::GetPersistent() const
{
    return _persistent;
}
void BaseObject::SetPersistent(BOOL s)
{
    _persistent = s;
}

Zone* BaseObject::GetZone() const
{
    return _zone;
}
void BaseObject::SetZone(Zone* s)
{
    _zone = s;
}

VNUM BaseObject::GetOnum() const
{
    return _onum;
}
void BaseObject::SetOnum(VNUM num)
{
    _onum=num;
}

BOOL BaseObject::AddComponent(Component* component)
{
    if (component==NULL)
        {
            return false;
        }

    if (HasComponent(component->GetMeta()->GetName()))
        {
            return false;
        }
    _components->push_back(component);
    component->Attach(this);
    return true;
}
BOOL BaseObject::RemoveComponent(Component* component)
{
    std::vector<Component*>::iterator it, itEnd;

    if (!HasComponent(component->GetMeta()->GetName()))
        {
            return false;
        }

    itEnd = _components->end();
    for (it = _components->begin(); it != itEnd; ++it)
        {
            if ((*it) == component)
                {
                    component->Detach();
                    _components->erase(it);
                    delete (*it);
                    return true;
                }
        }

    return false; //should never happen.
}
bool BaseObject::HasComponent(const std::string &name)
{
    std::vector <Component*>::iterator it, itEnd;

    if (_components->size())
        {
            itEnd = _components->end();
            for (it = _components->begin(); it != itEnd; ++it)
                {
                    if (FullMatch((*it)->GetMeta()->GetName(),name))
                        {
                            return true;
                        }
                }
        }

    return false;
}
Component* BaseObject::GetComponent(const std::string &name)
{
    std::vector <Component*>::iterator it, itEnd;

    if (_components->size())
        {
            itEnd = _components->end();
            for (it = _components->begin(); it != itEnd; ++it)
                {
                    if (FullMatch((*it)->GetMeta()->GetName(),name))
                        {
                            return (*it);
                        }
                }
        }

    return NULL;
}

void BaseObject::Attach()
{
    Attach(this);
}
void BaseObject::Attach(BaseObject* obj)
{
    std::vector <Component*>::iterator it, itEnd;
    if (_components->size())
        {
            itEnd = _components->end();
            for (it = _components->begin(); it != itEnd; ++it)
                {
                    (*it)->Attach(obj);
                }
        }
}

BOOL BaseObject::AddAlias(const std::string &alias)
{
    if (AliasExists(alias) && alias != "")
        {
            return false;
        }

    _aliases->push_back(alias);
    return true;
}
BOOL BaseObject::AliasExists(const std::string & name)
{
    std::vector<std::string>::iterator it, itEnd;

    if (_aliases->size())
        {
            itEnd = _aliases->end();
            for (it = _aliases->begin(); it != itEnd; ++it)
                {
                    if (FullMatch((*it),name))
                        {
                            return true;
                        }
                }
        }

    return false;
}
std::vector<std::string>* BaseObject::GetAliases()
{
    return _aliases;
}

void BaseObject::Serialize(TiXmlElement* root)
{
    TiXmlElement*node = new TiXmlElement("BaseObject");
    TiXmlElement *components = new TiXmlElement("components");
    TiXmlElement* aliases = new TiXmlElement("aliases");
    TiXmlElement* alias = NULL;
    TiXmlElement* component = NULL;
    TiXmlElement* properties = new TiXmlElement("properties");
    std::vector<std::string>::iterator ait, aitEnd;
    std::vector <Component*>::iterator it, itEnd;
    itEnd = _components->end();

    if (_components->size())
        {
            for (it=_components->begin(); it != itEnd; ++it)
                {
                    component = new TiXmlElement("component");
                    component->SetAttribute("name", (*it)->GetMeta()->GetName().c_str());
                    (*it)->Serialize(component);
                    components->LinkEndChild(component);
                }
        }
    node->LinkEndChild(components);

    if (_aliases->size())
        {
            aitEnd = _aliases->end();
            for (ait = _aliases->begin(); ait != aitEnd; ++ait)
                {
                    alias = new TiXmlElement("alias");
                    alias->SetAttribute("name", (*ait).c_str());
                    aliases->LinkEndChild(alias);
                }
        }
    node->LinkEndChild(aliases);

    variables.Serialize(properties);
    node->LinkEndChild(properties);

    node->SetAttribute("name", _name.c_str());
    node->SetAttribute("desc", _desc.c_str());
    node ->SetAttribute("script", _script.c_str());
    node->SetAttribute("onum", _onum);
    root->LinkEndChild(node);
}
void BaseObject::Deserialize(TiXmlElement* root)
{
    World* world = World::GetPtr();

    TiXmlElement* components = NULL;
    TiXmlElement* component = NULL;
    TiXmlElement* alias = NULL;
    TiXmlElement*aliases = NULL;
    TiXmlElement* properties = NULL;
    TiXmlNode* node = NULL;
    Component* com = NULL;

    node = root->FirstChild("components");
    if (node)
        {
            components = node->ToElement();
            for (node = components->FirstChild(); node; node = node->NextSibling())
                {
                    component = node->ToElement();
                    com = world->CreateComponent(component->Attribute("name"));
                    com->Deserialize(component);
                    AddComponent(com);
                }
        }

    node = root->FirstChild("aliases");
    if (node)
        {
            aliases = node->ToElement();
            for (node = aliases->FirstChild(); node; node = node->NextSibling())
                {
                    alias=node->ToElement();
                    AddAlias(alias->Attribute("name"));
                }
        }

    node = root->FirstChild("properties");
    if (node)
        {
            properties = node->ToElement();
            variables.Deserialize(properties);
        }

    _name = root->Attribute("name");
    _desc = root->Attribute("desc");
    _script = root->Attribute("script");
    root->Attribute("onum", (unsigned int*)&_onum);
}

void BaseObject::Copy(BaseObject* obj) const
{
    std::vector<std::string>::iterator it, itEnd;
    it = _aliases->end();

    if (_aliases->size())
        {
            for (it = _aliases->begin(); it != itEnd; ++it)
                {
                    obj->AddAlias((*it));
                }
        }

    obj->SetName(_name);
    obj->SetOnum(_onum);
    obj->SetDescription(_desc);
    obj->SetScript(_script);
    obj->SetPersistent(_persistent);
}

std::string BaseObject::Identify(Player*mobile)
{
    std::stringstream st;
    st << GetName() << std::endl;
    st << "Persistent: " << (GetPersistent()? "yes." : "no.") << std::endl;
    st << "Originating zone: " << GetZone()->GetName() << std::endl;
    return st.str();
}
std::string BaseObject::DoLook(Player* mobile)
{
    std::string str;

    LookArgs* args=new LookArgs(mobile,this,str);
    events.CallEvent("PreLook", args, (void*)mobile);
    str+=Capitalize(this->GetName())+"\n";
    str+=this->GetDescription()+"\n";
    events.CallEvent("PostLook", args, (void*)mobile);
    delete args;
    return str;
}

BOOL BaseObject::IsNpc() const
{
    return false;
}
BOOL BaseObject::IsPlayer() const
{
    return false;
}
BOOL BaseObject::IsLiving() const
{
    if (IsPlayer() || IsNpc())
        {
            return true;
        }

    return false;
}
BOOL BaseObject::IsRoom() const
{
    return false;
}
BOOL BaseObject::IsObject() const
{
    return false;
}

BOOL BaseObject::IsTile() const
{
	return false;
}
bool InitializeBaseObjectOlcs()
{
    World* world = World::GetPtr();
    OlcManager* omanager = world->GetOlcManager();
    OlcGroup* group = new OlcGroup();

    group->AddEntry(new OlcStringEntry<BaseObject>("name", "the name of the object", OF_NORMAL, OLCDT::STRING,
                    std::bind(&BaseObject::GetName, std::placeholders::_1),
                    std::bind(&BaseObject::SetName, std::placeholders::_1, std::placeholders::_2)));
    group->AddEntry(new OlcEditorEntry<BaseObject>("description", "the description of the object", OF_NORMAL, OLCDT::EDITOR,
                    std::bind(&BaseObject::GetDescription, std::placeholders::_1),
                    std::bind(&BaseObject::SetDescription, std::placeholders::_1, std::placeholders::_2)));
    omanager->AddGroup(OLCGROUP::BaseObject, group);
    return true;
}
