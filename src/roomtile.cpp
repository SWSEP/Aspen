#include "roomtile.h"
#include "world.h"
//#include "conf.h"
#include "baseObject.h"
//#include "staticObject.h"
#include "entity.h"
//#include "player.h"
#include "exit.h"
//#include "olc.h"
#include "room.h"
#include "utils.h"
#include <string>
#include <vector>
#include <list>
#include <unordered_map>

Terrain::Terrain(TerrainType type, int movecost, std::string symbol, std::string description){
	_type = type;
	_movecost = movecost;
	_symbol = symbol;
	_description = description;
}
Terrain::Terrain()
{
}
Terrain::~Terrain()
{
}

TerrainType Terrain::GetType(){
	return _type;
}

int Terrain::GetMoveCost() {
	return _movecost;
}
std::string Terrain::GetSymbol(){
	return _symbol;
}

std::string Terrain::GetDescription(){
	return _description;
}


 Terrain TerrainList[] = {
	Terrain(TerrainType::OCEAN, 5, C_BLUE "~", "the rolling waves of the ocean."),
	Terrain(TerrainType::UNDERWATER, 4, C_BLUE "=", "the depths of the ocean."),
	Terrain(TerrainType::AIR, 1, C_CYAN "-", "the open expanse of the sky."),
	Terrain(TerrainType::PLAINS, 1, C_GREEN "#", "the open expanse of the plains."),
	Terrain(TerrainType::DESERT, 2, C_YELLOW ".", "the arid expanse of the desert."),
	Terrain(TerrainType::UNDERGROUND, 1, CB_YELLOW "=", "No description for underground yet."),
	Terrain(TerrainType::SWAMP, 3, CB_GREEN "_", "The humid expanse of swampland."),
	Terrain(TerrainType::HILLS, 3, C_GREEN "^", "The expanse of rolling hills."),
	Terrain(TerrainType::MOUNTAIN, 5, CB_YELLOW "M", "The rocky, jagged peaks of the mountains."),
	Terrain(TerrainType::FOREST, 2, CB_GREEN "+", "The wooded expanse of the forest."),
	Terrain(TerrainType::LIGHTFOREST, 1, C_GREEN  "+", "The light forest."),
	Terrain(TerrainType::CITY, 1, C_NORMAL "*", "The city."),
	Terrain(TerrainType::INSIDE, 1, C_WHITE  "*", "Inside."),
	Terrain(TerrainType::DEFAULT, 1, "X", "")
};

RoomTile::RoomTile(Terrain terrain, Room* parent, point coords, LightLevel light) {
	_terrain = terrain;
	_parent = parent;
	_coords = coords;
	_light = light;
	
}

RoomTile::RoomTile()
{}


RoomTile::~RoomTile() {

	if (!_mobiles.empty())
	{
		for (auto it : _mobiles)
		{
			if (it->IsNpc())
			{
				delete it;
			}
		}
	}
	if (!_exits.empty())
	{
		for (auto it : _exits)
		{
			delete it;
		}
		_exits.clear();
	}
	_adjacents.clear();
	_parent = nullptr;
}
std::string RoomTile::GetKey()
{
	return std::to_string(GetCoords().x) + "x" + std::to_string(GetCoords().y);
}
Terrain RoomTile::GetTerrain(){
	return _terrain;
}

void RoomTile::SetTerrain(Terrain terrain){
	_terrain = terrain;
}

std::string RoomTile::GetTerrainString() const
{
	return TerrainToString(_terrain);
}

void RoomTile::SetTerrainByString(const std::string terrain)
{
 _terrain =	StringToTerrain(terrain);
}
Room* RoomTile::GetParent(){
	return _parent;
}

bool RoomTile::SetParent(Room* parent){
	if (parent == nullptr)
	{
		return false;
	}
	_parent = parent;
	return true;
}

point RoomTile::GetCoords(){
	return _coords;
}

bool RoomTile::SetCoords(point coords){
	if (coords.x < 0 || coords.y < 0 || coords.z < 0)
	{
		return false;
	}
	_coords = coords;
	return true;
}

LightLevel RoomTile::GetLighting(){
	return _light;
}

void RoomTile::SetLighting(LightLevel level){
	_light = level;
}

std::string RoomTile::GetLightingString() const
{
	return LightToString(_light);
}

void RoomTile::SetLightingByString(const std::string light)
{
	_light = StringToLight(light);
}

std::map<ExitDirection, RoomTile*> RoomTile::GetAdjacentTiles() {
	return _adjacents;
}

RoomTile* RoomTile::GetAdjacentTile(const ExitDirection direction) {
	return _adjacents[direction];
}

bool RoomTile::AdjacentExists(const ExitDirection direction) {
	if (_adjacents.count(direction) == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool RoomTile::AddAdjacentTile(ExitDirection direction, RoomTile* tile){
	if (AdjacentExists(direction))
	{
		return false;
	}
	_adjacents.insert(std::make_pair(direction,tile));
	return true;
}

void RoomTile::RemoveAdjacentTile(ExitDirection direction){
	_adjacents.erase(direction);
}

std::list<Living*>* RoomTile::GetMobiles(){
	return &_mobiles;
}

Living* RoomTile::GetMobile(const std::string name){
	for (auto it : _mobiles)
	{
		if (FullMatch(name, it->GetName()))
			return it;
	}
	return nullptr;
}

bool RoomTile::AddMobile(Living* mobile) {
	_mobiles.push_back(mobile);
	return true;
}

bool RoomTile::RemoveMobile(Living* mobile) {
	std::list<Living*>::iterator it, itEnd;
	itEnd = _mobiles.end();
	for (it = _mobiles.begin(); it != itEnd; ++it)
	{
		if (FullMatch(mobile->GetName(), (*it)->GetName()))
		{
			_mobiles.erase(it);
			return true;
		}
	}
	return false;
}

std::vector<Exit*> RoomTile::GetExits(){
	return _exits;
}

Exit* RoomTile::GetExit(ExitDirection direction){
	for (auto it : _exits) 
	{
		if (it->GetDirection() == direction)
			return it;
	}
	return nullptr;
}
bool RoomTile::AddExit(Exit* exit){
	if (GetExit(exit->GetDirection()) != nullptr)
		return false;
	else {
		_exits.push_back(exit);
		return true;
	}
}
bool RoomTile::RemoveExit(Exit* exit){
	if (GetExit(exit->GetDirection()) == nullptr)
		return false;
	else {
		std::vector<Exit*>::iterator it, itEnd;
		it = _exits.begin();
		itEnd = _exits.end();
		for (; it != itEnd; ++it)
		{ 
			if ((*it)->GetDirection() == exit->GetDirection())
			{ 
				_exits.erase(it);
					return true;
			} 
		}
	}
	return false;
}

bool RoomTile::MoveTo(Living* mobile, RoomTile* tile){
	if (tile == nullptr)
	{
		return false;
	}
	RemoveMobile(mobile);
	tile->AddMobile(mobile);
	return true;
}
std::string RoomTile::ShowTile(){
	std::stringstream tile;
	
	if (!_mobiles.empty() && !_contents.empty())
	{
		
		tile << C_GREEN << std::to_string(_mobiles.size() + _contents.size()) << C_RESET;
		return tile.str();
	}
	if (!_mobiles.empty() && _contents.empty())
	{
		if (_mobiles.size() > 1)
		{
			tile << C_RED << std::to_string(_mobiles.size()) << C_RESET;
			return tile.str();
		}
		tile << C_RED  << "0" << C_RESET;
		return tile.str();
	}
	if (!_contents.empty() && _mobiles.empty())
	{
		if (_mobiles.size() > 1)
		{
			tile << C_YELLOW << std::to_string(_contents.size()) << C_RESET; 
			return tile.str();
		}
		tile << C_YELLOW << "0" << C_RESET;
		return tile.str();
	}
	tile << GetTerrain().GetSymbol();
	return tile.str();
}
void RoomTile::Serialize(TiXmlElement* root){
	TiXmlElement* tile = new TiXmlElement("tile");
	TiXmlElement* exits = new TiXmlElement("exits");
	TiXmlElement* mobiles = new TiXmlElement("mobiles");

	ObjectContainer::Serialize(tile);
	tile->SetAttribute("x", _coords.x);
	tile->SetAttribute("y", _coords.y);
	tile->SetAttribute("z", _coords.z);
	tile->SetAttribute("terrain", (int)_terrain.GetType());
	tile->SetAttribute("parent", _parent->GetOnum());
	tile->SetAttribute("light", (int)_light);

	
	if (_exits.size())
	{
		for (auto it : _exits)
		{	
			it->Serialize(exits);
		}
	}

	if (_mobiles.size())
	{
		std::list<Living*>::iterator it, itEnd;
		itEnd = _mobiles.end();

		for (it = _mobiles.begin(); it != itEnd; ++it)
		{
			if ((*it)->IsNpc())
			{
				(*it)->Serialize(mobiles);
			}
		}
	}
	tile->LinkEndChild(exits);
	tile->LinkEndChild(mobiles);
	root->LinkEndChild(tile);
}

void RoomTile::Deserialize(TiXmlElement* tile){
	TiXmlElement* exit = NULL;
	TiXmlElement* exits = NULL;
	TiXmlElement* mobile = NULL;
	TiXmlElement* mobiles = NULL;
	TiXmlNode* node = NULL;
	Exit* ex = NULL;
	Npc* npc = NULL;
	int terrain;

	int templight;
	

	tile->Attribute("x", &_coords.x);
	tile->Attribute("y", &_coords.y);
	tile->Attribute("z", &_coords.z);
	tile->Attribute("terrain", &terrain);
	_terrain = TerrainList[terrain];
	tile->Attribute("light", &templight);
	_light = (LightLevel)templight;
	/*tile->Attribute("parent", (unsigned int*)&parent);
	vnum = (VNUM)parent;
	_parent = omanager->GetRoom(vnum);*/
	exits = tile->FirstChild("exits")->ToElement();
	for (node = exits->FirstChild(); node; node = node->NextSibling())
	{
		exit = node->ToElement();
		ex = new Exit();
		ex->Deserialize(exit);
		_exits.push_back(ex);
		ex = NULL;
	}
	mobiles = tile->FirstChild("mobiles")->ToElement();
	for (node = mobiles->FirstChild(); node; node = node->NextSibling())
	{
		mobile = node->ToElement();
		npc = new Npc();
		npc->SetLocation(GetParent());
		npc->SetTile(this);
		npc->Deserialize(mobile);
		_mobiles.push_back(npc);
		npc = NULL;
	}
	ObjectContainer::Deserialize(tile->FirstChild("objc")->ToElement());
}

void RoomTile::ObjectEnter(Entity* obj)
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
void RoomTile::ObjectLeave(Entity* obj)
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
BOOL RoomTile::IsTile() const
{
	return true;
}
BOOL RoomTile::CanEnter(Player* mobile)
{
	return true;
}

BOOL RoomTile::CanAddAdjacent(ExitDirection direction)
{
	if (AdjacentExists(direction))
	{
		return false;
	}

	point newcoords = GetDirectionCoords(direction, _coords);

	if (newcoords.x < 0 || newcoords.y < 0 || newcoords.z < 0)
	{
		return false;
	}
	return true;
}
/* UTILITIES*/

std::string LightToString(LightLevel level)
{
	if (level == LightLevel::LIGHT_DARK)
	{
		return "dark";
	}
	if (level == LightLevel::LIGHT_LOW)
	{
		return "low";
	}
	if (level == LightLevel::LIGHT_NORMAL)
	{
		return "normal";
	}
	return "error";
}

LightLevel StringToLight(std::string level)
{
	if (FullMatch(level, "dark"))
		return LightLevel::LIGHT_DARK;
	if (FullMatch(level, "low"))
		return LightLevel::LIGHT_LOW;
	if (FullMatch(level, "normal"))
		return LightLevel::LIGHT_NORMAL;

	return LightLevel::LIGHT_NORMAL;
}

Terrain StringToTerrain(std::string terrain)
{

	if (FullMatch(terrain, "inside"))
	{
		return TerrainList[(int)TerrainType::INSIDE];

	}
	if (FullMatch(terrain, "air"))
	{
		return TerrainList[(int)TerrainType::AIR];

	}
	if (FullMatch(terrain, "city"))
	{
		return TerrainList[(int)TerrainType::CITY];

	}
	if (FullMatch(terrain, "desert"))
	{
		return TerrainList[(int)TerrainType::DESERT];

	}
	if (FullMatch(terrain, "forest"))
	{
		return TerrainList[(int)TerrainType::FOREST];

	}
	if (FullMatch(terrain, "woods"))
	{
		return TerrainList[(int)TerrainType::LIGHTFOREST];

	}
	if (FullMatch(terrain, "ocean"))
	{
		return TerrainList[(int)TerrainType::OCEAN];

	}
	if (FullMatch(terrain, "underwater"))
	{
		return TerrainList[(int)TerrainType::UNDERWATER];

	}
	if (FullMatch(terrain, "underground"))
	{
		return TerrainList[(int)TerrainType::UNDERGROUND];
	}
	if (FullMatch(terrain, "mountain"))
	{
		return TerrainList[(int)TerrainType::MOUNTAIN];
	}
	if (FullMatch(terrain, "hills"))
	{
		return TerrainList[(int)TerrainType::HILLS];
	}
	if (FullMatch(terrain, "swamp"))
	{
		return TerrainList[(int)TerrainType::SWAMP];
	}

	return TerrainList[(int)TerrainType::DEFAULT];
}

std::string TerrainToString(Terrain _terrain)
{
	if (_terrain.GetType() == TerrainType::AIR)
	{
		return "air";
	}
	if (_terrain.GetType() == TerrainType::CITY)
	{
		return "city";
	}
	if (_terrain.GetType() == TerrainType::DESERT)
	{
		return "desert";
	}
	if (_terrain.GetType() == TerrainType::DEFAULT)
	{
		return "default";
	}
	if (_terrain.GetType() == TerrainType::FOREST)
	{
		return "forest";
	}
	if (_terrain.GetType() == TerrainType::LIGHTFOREST)
	{
		return "woods";
	}
	if (_terrain.GetType() == TerrainType::INSIDE)
	{
		return "inside";
	}
	if (_terrain.GetType() == TerrainType::OCEAN)
	{
		return "ocean";
	}
	if (_terrain.GetType() == TerrainType::UNDERWATER)
	{
		return "underwater";
	}
	if (_terrain.GetType() == TerrainType::UNDERGROUND)
	{
		return "underground";
	}
	if (_terrain.GetType() == TerrainType::MOUNTAIN)
	{
		return "mountain";
	}
	if (_terrain.GetType() == TerrainType::HILLS)
	{
		return "hills";
	}
	if (_terrain.GetType() == TerrainType::PLAINS)
	{
		return "plains";
	}
	if (_terrain.GetType() == TerrainType::SWAMP)
	{
		return "swamp";
	}
	return "error";
}
std::string CoordToKey(point coords){
	return (std::to_string(coords.x) + "x" + std::to_string(coords.y));
}
bool InitializeRoomTileOlcs() {
	World* world = World::GetPtr();
	OlcManager* omanager = world->GetOlcManager();
	OlcGroup* group = new OlcGroup();
	group->AddEntry(new OlcStringEntry<RoomTile>("terrain", "the terrain of the tile", OF_NORMAL, OLCDT::STRING,
		std::bind(&RoomTile::GetTerrainString, std::placeholders::_1),
		std::bind(&RoomTile::SetTerrainByString, std::placeholders::_1, std::placeholders::_2)));
	group->AddEntry(new OlcStringEntry<RoomTile>("light", "the lighting level of the tile", OF_NORMAL, OLCDT::STRING,
		std::bind(&RoomTile::GetLightingString, std::placeholders::_1),
		std::bind(&RoomTile::SetLightingByString, std::placeholders::_1, std::placeholders::_2)));
	omanager->AddGroup(OLCGROUP::ROOMTILE, group);
	return true;
}