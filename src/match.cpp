#include <string>
#include <vector>
#include <list>
#include "mud.h"
#include "conf.h"
#include "match.h"
#include "world.h"
#include "entity.h"
#include "baseObject.h"
#include "player.h"
#include "npc.h"
#include "utils.h"

bool FullMatch(const std::string& a, const std::string& b, bool icase)
{
    if (icase)
        {
            return a == b? true : false;
        }
    else
        {
            return iequals(a,b);
        }
}

Entity* World::MatchKeyword(const std::string &name, Player* caller)
{
    if ((name=="me")||(FullMatch(name,caller->GetName())))
        {
            return caller;
        }
    if (name.length() < caller->GetName().length() && FindInString(name, caller->GetName()))
        {
            return caller;
        }
    return NULL;
}

Entity* World::MatchObject(const std::string &name,Player* caller)
{
    std::list<Entity*>* contents; //holds contents for the location and current caller.
    std::list<Entity*>* val;
    std::list<Entity*>::iterator it, itEnd;
	Room* room = NULL;
    Entity* obj = NULL;

    obj = MatchKeyword(name, caller);
    if (obj)
        {
            return obj;
        }

	room = (Room*)caller->GetLocation();
    contents = new std::list<Entity*>();
    val = room->GetContents();
	
	if (!val->empty())
	{
		contents->insert(contents->begin(), val->begin(), val->end());
	}
	
	val->clear();
    val = caller->GetContents();
	
	if (!val->empty())
	{
		contents->insert(contents->begin(), val->begin(), val->end());
	}
	
	if (contents->empty())
	{
		delete contents;
		return obj;
	}

	obj = MatchObjectInList(name, contents);
    delete contents;
    return obj;
}
Entity* World::MatchObjectInList(const std::string &name, std::list<Entity*> *olist)
{
    std::list<Entity*>::iterator it, itEnd;
    std::string sub; //used for holding the subpart of the number.
    std::string temp; //used for holding a temp copy of the name after it is trimmed.
    std::string alias; //holds a copy of the alias.
    Entity* obj = NULL; //the object we are currently examining/returning.
    std::vector<std::string>* aliases = NULL; //a list of aliases.
    std::vector<std::string>::iterator ait, aitEnd;
    int number = 0; //used for holding the actual number.
    int count = 0; //used for holding the number of objects found.
    size_t marker; //used for holding the position of the '.'.

    if (!olist->size())
        {
            return NULL;
        }

//we check to see if the string has a '.', if so, there's a number.
    marker = name.find_first_of(".");
//check to see if it is 1) at the beginning, or 2) at the end.
    if ((marker == 0) || (marker == name.length()))
        {
            return NULL;
        }

    if (marker != std::string::npos)   //we found something.
        {
            sub = name.substr(marker); //the subnumber.
            temp = name.substr(marker+1); //trim off the x. bit
            number = atoi(sub.c_str());
            if (number == 0)
                {
                    return NULL;
                }

            itEnd = olist->end();
            for (it = olist->begin(); it != itEnd && count < number; ++it)
                {
                    obj = *it;
                    if (obj->GetName().length() < temp.length())   //we check for a partial match
                        {
                            if (FullMatch(obj->GetName().substr(temp.length()), temp))
                                {
                                    count++; //we found a match, increase the counter.
                                    continue;
                                }
                        }
                    if (FullMatch(obj->GetName(), temp))   //full match
                        {
                            count++;
                            continue;
                        }

//now we check a list of aliases.
                    aliases = obj->GetAliases();
                    if (aliases->size())
                        {
                            aitEnd = aliases->end();
                            for (ait = aliases->begin(); ait != aitEnd; ++ait)
                                {
                                    alias = (*ait);
                                    if (alias.length() < temp.length())   //we check for a partial match
                                        {
                                            if (FullMatch(alias.substr(temp.length()),temp))
                                                {
                                                    count++; //we found a match, increase the counter.
                                                    continue;
                                                }
                                        }
                                    if (FullMatch(alias,temp))   //full match
                                        {
                                            count++;
                                            continue;
                                        }
                                }
                        }
                }
            if (count != (number -1))
                {
                    return NULL;
                }
            else
                {
                    return obj;
                }
        }
    else
        {
            itEnd = olist->end();
            for (it = olist->begin(); it != itEnd; ++it)
                {
                    obj = *it;

                    if (obj->GetName().length() < temp.length())   //we check for a partial match
                        {
                            if (FullMatch(obj->GetName().substr(temp.length()),temp))
                                {
                                    return obj;
                                }
                        }
                    if (FullMatch(obj->GetName(), temp))   //full match
                        {
                            return obj;
                        }

//now we check a list of aliases.
                    aliases = obj->GetAliases();
                    if (aliases->size())
                        {
                            aitEnd = aliases->end();
                            for (ait = aliases->begin(); ait != aitEnd; ++ait)
                                {
                                    alias = *ait;
                                    if (alias.length() < temp.length())   //we check for a partial match
                                        {
                                            if (FullMatch(alias.substr(temp.length()),temp))
                                                {
                                                    return obj;
                                                }
                                        }
                                    if (FullMatch(alias,temp))   //full match
                                        {
                                            return obj;
                                        }
                                }
                        }
                }
        }

    return NULL;
}
Entity* World::MatchObjectInVector(const std::string &name, std::vector<Entity*> *olist)
{
    std::vector<Entity*>::iterator it, itEnd;
    std::string sub; //used for holding the subpart of the number.
    std::string temp; //used for holding a temp copy of the name after it is trimmed.
    std::string alias; //holds a copy of the alias.
    Entity* obj = NULL; //the object we are currently examining/returning.
    std::vector<std::string>* aliases = NULL; //a list of aliases.
    std::vector<std::string>::iterator ait, aitEnd;
    int number = 0; //used for holding the actual number.
    int count = 0; //used for holding the number of objects found.
    size_t marker; //used for holding the position of the '.'.

    if (!olist->size())
        {
            return NULL;
        }

//we check to see if the string has a '.', if so, there's a number.
    marker = name.find_first_of(".");
//check to see if it is 1) at the beginning, or 2) at the end.
    if ((marker == 0) || (marker == name.length()))
        {
            return NULL;
        }

    if (marker != std::string::npos)   //we found something.
        {
            sub = name.substr(marker); //the subnumber.
            temp = name.substr(marker+1); //trim off the x. bit
            number = atoi(sub.c_str());
            if (number == 0)
                {
                    return NULL;
                }

            itEnd = olist->end();
            for (it = olist->begin(); it != itEnd && count < number; ++it)
                {
                    obj = *it;
                    if (obj->GetName().length() < temp.length())   //we check for a partial match
                        {
                            if (FullMatch(obj->GetName().substr(temp.length()), temp))
                                {
                                    count++; //we found a match, increase the counter.
                                    continue;
                                }
                        }
                    if (FullMatch(obj->GetName(),temp))   //full match
                        {
                            count++;
                            continue;
                        }

//now we check a list of aliases.
                    aliases = obj->GetAliases();
                    if (aliases->size())
                        {
                            aitEnd = aliases->end();
                            for (ait = aliases->begin(); ait != aitEnd; ++ait)
                                {
                                    alias = (*ait);
                                    if (alias.length() < temp.length())   //we check for a partial match
                                        {
                                            if (FullMatch(alias.substr(temp.length()), temp))
                                                {
                                                    count++; //we found a match, increase the counter.
                                                    continue;
                                                }
                                        }
                                    if (FullMatch(alias, temp))   //full match
                                        {
                                            count++;
                                            continue;
                                        }
                                }
                        }
                }
            if (count != (number -1))
                {
                    return NULL;
                }
            else
                {
                    return obj;
                }
        }
    else
        {
            itEnd = olist->end();
            for (it = olist->begin(); it != itEnd; ++it)
                {
                    obj = *it;

                    if (obj->GetName().length() < temp.length())   //we check for a partial match
                        {
                            if (FullMatch(obj->GetName().substr(temp.length()),temp))
                                {
                                    return obj;
                                }
                        }
                    if (FullMatch(obj->GetName(),temp))   //full match
                        {
                            return obj;
                        }

//now we check a list of aliases.
                    aliases = obj->GetAliases();
                    if (aliases->size())
                        {
                            aitEnd = aliases->end();
                            for (ait = aliases->begin(); ait != aitEnd; ++ait)
                                {
                                    alias = *ait;
                                    if (alias.length() < temp.length())   //we check for a partial match
                                        {
                                            if (FullMatch(alias.substr(temp.length()), temp))
                                                {
                                                    return obj;
                                                }
                                        }
                                    if (FullMatch(alias,temp))   //full match
                                        {
                                            return obj;
                                        }
                                }
                        }
                }
        }

    return NULL;
}

Living* World::MatchMobile(const std::string &name, Player* caller)
{
	std::list<Living*>* contents; //holds contents for the location and current caller.
	std::list<Living*>* val;
	std::list<Living*>::iterator it, itEnd;
	Room* room = NULL;
	Living* mobile = NULL;

	mobile = (Living*)MatchKeyword(name, caller);
	if (mobile)
	{
		return mobile;
	}

	room = (Room*)caller->GetLocation();
	contents = new std::list<Living*>();
	val = room->GetMobiles();
	
	if (!val->empty())
	{
		itEnd = val->end();
	
		for (it = val->begin(); it != itEnd; ++it)
		{
			
			if ((*it) && (*it) != caller)
			{
				contents->push_back((*it));
			}
		}
		
	}
	delete val;
	/*val = caller->GetContents();

	if (!val->empty())
	{
		contents->insert(contents->begin(), val->begin(), val->end());
	}*/

	if (contents->empty())
	{
		delete contents;
		return mobile;
	}
	
	mobile = MatchMobileInList(name, contents);
	delete contents;
	return mobile;
}

Living* World::MatchMobileInList(const std::string &name, std::list<Living*>* olist)
{
	std::list<Living*>::iterator it, itEnd;
	std::string sub; //used for holding the subpart of the number.
	std::string temp; //used for holding a temp copy of the name after it is trimmed.
	std::vector<std::string>::iterator ait, aitEnd;
	int number = 0; //used for holding the actual number.
	int count = 0; //used for holding the number of objects found.
	size_t marker; //used for holding the position of the '.'.
	std::map<int, Living*> results;


	//we check to see if the string has a '.', if so, there's a number.
	marker = name.find_first_of(".");
	//check to see if it is 1) at the beginning, or 2) at the end.


	if (marker != std::string::npos)   //we found something.
	{
		sub.assign(name, 0, marker); //the subnumber.
		temp = name.substr(marker + 1); //trim off the x. bit
		number = tonum(sub.c_str());
		if (number == 0)
		{
			return NULL;
		}

		itEnd = olist->end();

		for (it = olist->begin(); it != itEnd; ++it)
		{
			if (FindInString(temp, (*it)->GetName()) || FindInString(temp, (*it)->GetShort()))
			{
				count++;
				results[count] = (*it);
			}
		}
		return results[number];
	}
	else
	{
		itEnd = olist->end();
		temp = name;
		for (it = olist->begin(); it != itEnd; ++it)
		{
			if (FindInString(temp, (*it)->GetName()) || FindInString(temp, (*it)->GetShort()))
			{
				return (*it);
			}
		}
	}
	results.clear();
	return nullptr;
}