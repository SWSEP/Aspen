#include <sstream>
#include <string>
#include <list>
#include <functional>
#include <tinyxml.h>
#include <map>
#include "room.h"
#include "living.h"
#include "exit.h"
#include "player.h"
#include "event.h"
#include "eventargs.h"
#include "zone.h"
#include "olc.h"
#include "olcManager.h"
#include "olcGroup.h"
#include "world.h"
#include "roomtile.h"


	
void TileMap::add(RoomTile* tile)
{
	std::string add = std::to_string(tile->GetCoords().x) + "x" + std::to_string(tile->GetCoords().y);
	insert(std::pair<std::string, RoomTile*>(add, tile));
}
void TileMap::remove(RoomTile* tile)
{
	erase(std::to_string(tile->GetCoords().x) + "x" + std::to_string(tile->GetCoords().y));
	delete tile;
}

RoomTile* TileMap::get(point coords)
{ 
	std::string get;
	get = std::to_string(coords.x) + "x" + std::to_string(coords.y);
	RoomTile* tile = find(get)->second;
	return tile;
}
Room::Room()
{
    _rflag = 0;
    SetOnum(ROOM_NOWHERE);
    SetPersistent(false);
    events.AddCallback("PostLook", std::bind(&Room::PostLook, this, std::placeholders::_1, std::placeholders::_2));
	_roomtiles = new TileMap();
}
Room::Room(int x, int y)
{
	_rflag = 0;
	SetOnum(ROOM_NOWHERE);
	SetPersistent(false);
	events.AddCallback("PostLook", std::bind(&Room::PostLook, this, std::placeholders::_1, std::placeholders::_2));
	_roomtiles = new TileMap();
	CreateTileMap(x, y);
}
Room::~Room()
{
    for (auto it: _exits)
        {
            delete it;
        }
	DeleteTileMap();
}

BOOL Room::AddExit(Exit* exit)
{
    if (exit==nullptr)
        {
            return false;
        }

    if (_exits.size())
        {
            for (auto it: _exits)
                {
                    if (exit == it)
                        {
                            return false;
                        }
                }
        }

    if (ExitExists(exit->GetDirection()))
        {
            return false;
        }

    _exits.push_back(exit);
    return true;
}

BOOL Room::ExitExists(ExitDirection dir)
{
    if (_exits.size())
        {
            for (auto it: _exits)
                {
                    if (it->GetDirection() == dir)
                        {
                            return true;
                        }
                }
        }

    return false;
}

Exit* Room::GetExit(ExitDirection dir)
{
    if (_exits.size())
        {
            for (auto it: _exits)
                {
                    if (it->GetDirection() == dir)
                        {
                            return it;
                        }
                }
        }

    return nullptr;
}

std::vector<Exit*>* Room::GetExits()
{
    return &_exits;
}

void Room::SetRoomFlag(FLAG flag)
{
    _rflag = flag;
}
FLAG Room::GetRoomFlag()
{
    return _rflag;
}

void Room::TellAll(const std::string &message)
{
    std::list<Living*>::iterator it, itEnd;
	TileMap::iterator tit, titEnd;

	titEnd = _roomtiles->end();
	
	for (tit = _roomtiles->begin(); tit != titEnd; ++tit)
	{
		itEnd = tit->second->GetMobiles()->end();
		if (!tit->second->GetMobiles()->empty())
		{
			for (it = tit->second->GetMobiles()->begin(); it != itEnd; ++it)
			{
				if ((*it)->IsPlayer())
				{
					((Player*)(*it))->Message(MSG_INFO, message);
				}
			}
		}
	}
}
void Room::TellAllBut(const std::string &message, std::list <Player*>* players)
{
    std::list<Player*> left;
    std::list<Player*>::iterator pit, pitEnd;
    std::list <Living*>::iterator lit, litEnd;
	TileMap::iterator tit, titEnd;

    BOOL found = false;


    pitEnd = players->end();
	titEnd = _roomtiles->end();
	for (tit = _roomtiles->begin(); tit != titEnd; ++tit)
	{
		litEnd = tit->second->GetMobiles()->end();
		if (!tit->second->GetMobiles()->empty())
		{
			for (lit = tit->second->GetMobiles()->begin(); lit != litEnd; ++lit)
			{
				for (pit = players->begin(); pit != pitEnd; ++pit)
				{
					if ((*lit) == (*pit))
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					left.push_back((Player*)(*lit));
				}
				found = false;
			}
		}
	}
    pitEnd = left.end();
	
	if (!left.empty())
	{
		for (pit = left.begin(); pit != pitEnd; ++pit)
		{
			(*pit)->Message(MSG_INFO, message);
		}
	}
}
void Room::TellAllBut(const std::string &message, Player* exclude)
{
    std::list<Living*>::iterator it, itEnd;
	TileMap::iterator tit, titEnd;
	
	titEnd = _roomtiles->end();
	for (tit = _roomtiles->begin(); tit != titEnd; ++tit)
	{
		itEnd = tit->second->GetMobiles()->end();
		if (!tit->second->GetMobiles()->empty())
		{
			for (it = tit->second->GetMobiles()->begin(); it != itEnd; ++it)
			{   
				
				if ((*it)->IsPlayer() && (*it) != exclude)
				{
					((Player*)(*it))->Message(MSG_INFO, message);
				}
			}
		}
	}
}

point* Room::GetCoord()
{
    return &_coord;
}
void Room::SetCoord(point& coord)
{
    _coord = coord;
}

BOOL Room::IsRoom() const
{
    return true;
}

void Room::Serialize(TiXmlElement* root)
{
    TiXmlElement* room = new TiXmlElement("room");
    TiXmlElement* exits = new TiXmlElement("exits");
	TiXmlElement* tiles = new TiXmlElement("tiles");
    ObjectContainer::Serialize(room);

    room->SetAttribute("rflag", _rflag);
    room->SetAttribute("x", _coord.x);
    room->SetAttribute("y", _coord.y);
    room->SetAttribute("z", _coord.z);
	room->SetAttribute("xmax", _roomtiles->xsize);
	room->SetAttribute("ymax", _roomtiles->ysize);

    if (_exits.size())
        {
            for (auto it: _exits)
                {
                    it->Serialize(exits);
                }
        }
    room->LinkEndChild(exits);
	TileMap::iterator it, itEnd;
	itEnd = _roomtiles->end();

	if (_roomtiles->size())
	{
		for (it = _roomtiles->begin(); it != itEnd; ++it)
		{
			it->second->Serialize(tiles);
		}
	}
	room->LinkEndChild(tiles);
    root->LinkEndChild(room);
}
void Room::Deserialize(TiXmlElement* room)
{
    TiXmlElement* exit = NULL;
    TiXmlElement* exits = NULL;
	TiXmlElement* tile = NULL;
	TiXmlElement* tiles = NULL;
    TiXmlNode* node = NULL;
    Exit* ex = NULL;
	RoomTile* t;

	ObjectContainer::Deserialize(room->FirstChild("objc")->ToElement());
    room->Attribute("rflag", &_rflag);
    room->Attribute("x", &_coord.x);
    room->Attribute("y", &_coord.y);
    room->Attribute("z", &_coord.z);
	room->Attribute("xmax", &_roomtiles->xsize);
	room->Attribute("ymax", &_roomtiles->ysize);
    exits = room->FirstChild("exits")->ToElement();
    for (node = exits->FirstChild(); node; node = node->NextSibling())
        {
            exit = node->ToElement();
            ex = new Exit();
            ex->Deserialize(exit);
            _exits.push_back(ex);
            ex = NULL;
        }
	node = NULL;
	tiles = room->FirstChild("tiles")->ToElement();
	for (node = tiles->FirstChild(); node; node = node->NextSibling())
	{
		tile = node->ToElement();
		t = new RoomTile();
		t->SetParent(this);
		t->Deserialize(tile);
		_roomtiles->add(t);
		t = NULL;
	}
	FormatTileMap();
    
}

void Room::ObjectEnter(Entity* obj)
{
    if (obj->IsLiving())
        {
            _mobiles.push_back((Living*)obj);
        }
    else
        {
            ObjectContainer::ObjectEnter(obj);
        }
}
void Room::ObjectLeave(Entity* obj)
{
    if (obj->IsLiving())
        {
            std::list<Living*>::iterator it, itEnd;
            itEnd = _mobiles.end();
            for (it = _mobiles.begin(); it != itEnd; ++it)
                {
                    if ((*it) == obj)
                        {
                            _mobiles.erase(it);
                            break;
                        }
                }
        }
    else
        {
            ObjectContainer::ObjectLeave(obj);
        }
}

//events
CEVENT(Room, PostLook)
{
	std::stringstream st;
	LookArgs* largs = (LookArgs*)args;
	std::list<Living*> tiles_living;
	std::list<Entity*>* tiles_entity;
	std::list<Living*>::iterator Lit, LitEnd;
	std::string mobiles, objects;
	TileMap::iterator it, itEnd;

	itEnd = _roomtiles->end();

	for (it = _roomtiles->begin(); it != itEnd; ++it)
	{
		if (it->second->GetMobiles()->size() >= 1)
		{
			tiles_living = *(it->second->GetMobiles());
			LitEnd = tiles_living.end();
			for (Lit = tiles_living.begin(); Lit != LitEnd; ++Lit)
			{
				if ((*Lit) == largs->_caller)
				{
					continue;
				}
				mobiles += (*Lit)->GetShort() + " [" + it->first + "]" + "\n\r";
			}
		}
		tiles_entity = it->second->GetContents();
		if (tiles_entity->size())
		{
			std::list<Entity*>::iterator Eit, EitEnd;
			std::map<std::string, int> counts;
			std::map<std::string, int>::iterator mit, mitEnd;

			//we need to try to combine the objects. First, we go through the list of everything and see how many of x there are.
			//after that, we can add (x) foobars to the string.
			//this is a slightly slow process...
			std::list<Entity*>::iterator cit, citEnd;
			citEnd = tiles_entity->end();
			for (cit = tiles_entity->begin(); cit != EitEnd; ++cit)
			{
				if (counts.count((*cit)->GetShort()))
				{
					counts[(*cit)->GetShort()]++;
				}
				else
				{
					counts[(*cit)->GetShort()] = 1;
				}
			}

			//now we iterate:
			mitEnd = counts.end();
			for (mit = counts.begin(); mit != mitEnd; ++mit)
			{
				if ((*mit).second > 1)
				{
					st << "(" << (*mit).second << ") " << (*mit).first << " [" << it->first << "]" << "\r\n";
				}
				else
				{
					st << (*mit).first << " [" << it->first << "]" << "\r\n";
				}
			}
			objects = st.str();
		}
	}

	largs->_desc += mobiles + objects;
	//   largs->_desc += TellObviousExits();
	largs->_desc += "map: " + std::to_string(_roomtiles->size()) + ShowTileMap();
}
std::string Room::TellObviousExits()
{
    std::stringstream st;
    size_t count = 0;
    size_t i = 0;
    std::vector<Exit*> *exits = GetExits();

    if (!exits->size())
        {
            return "You see no obvious exits.";
        }
    else
        {
            st << "Obvious exits: [";
            count = exits->size();
            for (i = 0; i < count-1; i++)
                {
                    st << exits->at(i)->GetName() << ", ";
                }
            st << exits->at(exits->size()-1)->GetName();
            st << "].";
        }

    return st.str();
}

bool Room::TileExists(std::string tilekey)
{
	if (_roomtiles->count(tilekey) == 1)
	{
		return true;
	}
	return false;
}
bool Room::TileExists(point coords){
	if (_roomtiles->count(CoordToKey(coords)) == 1)
	{
		return true;
	}
	return false;
}

void Room::AddTile(RoomTile* tile){
	if (!tile)
	{
		return;
	}
	if (TileExists(tile->GetCoords()))
	{
		return;
	}
	if (tile->GetCoords().x > _roomtiles->xsize)
	{
		_roomtiles->xsize = tile->GetCoords().x;
	}
	if (tile->GetCoords().y > _roomtiles->ysize)
	{
		_roomtiles->ysize = tile->GetCoords().y;
	}
	_roomtiles->add(tile);
}

void Room::RemoveTile(point coords){
	if (!TileExists(coords))
	{
		return;
	}
	_roomtiles->remove(_roomtiles->get(coords));
}

void Room::RemoveTile(std::string key)
{
	if (!TileExists(key))
	{
		return;
	}
	_roomtiles->remove((*_roomtiles)[key]);
}
std::string Room::ShowTileMap(){
	point showcoords = point(0, 0, 1);
	showcoords.y = _roomtiles->ysize;

	std::string output; 
	output = "\n\r";
	output += C_RESET;

	
	for (; showcoords.y >= 0; showcoords.y--)
	{

	  for (; showcoords.x <= _roomtiles->xsize; showcoords.x++)
		{ 
			if (!TileExists(showcoords))
			{
				output += " ";
				continue;
			}
				output += _roomtiles->get(showcoords)->ShowTile();
		}
		showcoords.x = 0;
		output += "\n\r";
	}
	
	return output;
}

void Room::CreateTileMap(int xmax, int ymax){
	int x = 0;
	int y = 0;
	--xmax;
	--ymax;
	_roomtiles->xsize = xmax;
	_roomtiles->ysize = ymax;
	while (y <= ymax )
	{
		while (x <= xmax )
		{
			point coords = point(x, y, 1);
			RoomTile* tile = new RoomTile(TerrainList[(int)TerrainType::DEFAULT], this, coords, LightLevel::LIGHT_NORMAL);
			AddTile(tile);
			x++;
		}
		x = 0;
		y++;
	}
	FormatTileMap();
}
void Room::DeleteTileMap(){
	TileMap::iterator it, itEnd;
	
	it = _roomtiles->begin();
	itEnd = _roomtiles->end();
	for (; it != itEnd; ++it)
	{
		delete it->second;
	}
	_roomtiles->clear();
	delete _roomtiles;
}
RoomTile* Room::GetTile(point coords) {
	if (TileExists(coords))
	{
		return _roomtiles->get(coords);
	}
	return nullptr;
}

RoomTile* Room::GetTile(std::string key)
{
	if (_roomtiles->count(key))
	{
		return (*_roomtiles)[key];
	}
	return nullptr;
}

TileMap* Room::GetTileMap() const
{
	return _roomtiles;
}
void Room::FormatTileMap() 
{

	TileMap::iterator it, itEnd;

	itEnd = _roomtiles->end();
	
	if (_roomtiles->size() == 1)
	{
		return;
	}

	for (it = _roomtiles->begin(); it != itEnd; ++it)
	{
		point coords = point(it->second->GetCoords().x, it->second->GetCoords().y, it->second->GetCoords().z);
		coords.x += 1;
		
		if (TileExists(coords) && !it->second->AdjacentExists(ExitDirection::east))
		{
			RoomTile* e = GetTile(coords);
			it->second->AddAdjacentTile(ExitDirection::east, e);
		}
		
		coords.y += 1;
		
		if (TileExists(coords) && !it->second->AdjacentExists(ExitDirection::northeast))
		{
			RoomTile* ne = GetTile(coords);
			it->second->AddAdjacentTile(ExitDirection::northeast, ne);
		}
		
		coords.x -= 1;

		if (TileExists(coords) && !it->second->AdjacentExists(ExitDirection::north))
		{
			RoomTile* n = GetTile(coords);
			it->second->AddAdjacentTile(ExitDirection::north, n);
		}

		coords.x -= 1;

		if (TileExists(coords) && !it->second->AdjacentExists(ExitDirection::northwest))
		{
			RoomTile* nw = GetTile(coords);
			it->second->AddAdjacentTile(ExitDirection::northwest, nw);
		}

		coords.y -= 1;

		if (TileExists(coords) && !it->second->AdjacentExists(ExitDirection::west))
		{
			RoomTile* w = GetTile(coords);
			it->second->AddAdjacentTile(ExitDirection::west,w);
		}

		coords.y -= 1;

		if (TileExists(coords) && !it->second->AdjacentExists(ExitDirection::southwest))
		{
			RoomTile* sw = GetTile(coords);
			it->second->AddAdjacentTile(ExitDirection::southwest, sw);
		}

		coords.x += 1;

		if (TileExists(coords) && !it->second->AdjacentExists(ExitDirection::south))
		{
			RoomTile* s = GetTile(coords);
			it->second->AddAdjacentTile(ExitDirection::south, s);
		}
		
		coords.x += 1;

		if (TileExists(coords) && !it->second->AdjacentExists(ExitDirection::southeast))
		{
			RoomTile* se = GetTile(coords);
			it->second->AddAdjacentTile(ExitDirection::southeast, se);
		}
		
	}


}

RoomTile* Room::GetFirstAvailable() {
	TileMap::iterator it, itEnd;
	
	itEnd = _roomtiles->end();
	for (it = _roomtiles->begin(); it != itEnd; ++it)
	{
		if (it->second != nullptr)
			return it->second;
	}
	return nullptr;
}

std::list<Entity*>* Room::GetContents()
{
	TileMap::iterator it, itEnd;
	std::list<Entity*> contents;
	
	itEnd = _roomtiles->end();

	for (it = _roomtiles->begin(); it != itEnd; ++it)
	{
		if (!it->second->GetContents()->empty())
		{
			std::list<Entity*>::iterator eit, eitEnd;
			eitEnd = it->second->GetContents()->end();

			for (eit = it->second->GetContents()->begin(); eit != eitEnd; ++eit)
			{
				if ((*eit))
				{
					contents.push_back((*eit));
				}
			}
		}
	/*	if (!it->second->GetMobiles()->empty())
		{
			contents->insert(contents->begin(), it->second->GetMobiles()->begin(), it->second->GetMobiles()->end());
		}*/
	}
	return &contents;
}

std::list<Living*>* Room::GetMobiles()
{
	TileMap::iterator it, itEnd;
	itEnd = _roomtiles->end();
	std::list<Living*>* contents = new std::list<Living*>();

	for (it = _roomtiles->begin(); it != itEnd; ++it)
	{
		if (!it->second->GetMobiles()->empty() || it->second->GetMobiles()->size() == 0)
		{

			contents->insert(contents->end(), it->second->GetMobiles()->begin(), it->second->GetMobiles()->end());
		/*	std::list<Living*>::iterator lit, litEnd;
			litEnd = it->second->GetMobiles()->end();

			for (lit = it->second->GetMobiles()->begin(); lit != litEnd; ++lit)
			{
				if ((*lit))
				{
					contents.emplace((*lit));
				}
			}*/
		}
	}
	return contents;
}
bool InitializeRoomOlcs()
{
    World* world = World::GetPtr();
    OlcManager* omanager = world->GetOlcManager();
    OlcGroup* group = new OlcGroup();
    group->SetInheritance(omanager->GetGroup(OLCGROUP::BaseObject));
    omanager->AddGroup(OLCGROUP::ROOM, group);
    return true;
}
