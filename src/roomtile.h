#ifndef ROOMTILE_H
#define ROOMTILE_H
//#include "world.h"
#include "conf.h"
//#include "baseObject.h"
//#include "staticObject.h"
#include "living.h"
//#include "player.h"
#include "exit.h"
#include "room.h"
#include "utils.h"
#include "entity.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <array>


class point; 

class Terrain {
	TerrainType _type;
	int _movecost;
	std::string _symbol;
	std::string _description; // Will be used in future overland module.
public:
			Terrain(TerrainType type, int movecost, std::string symbol, std::string description);
			Terrain();
			~Terrain();
			TerrainType GetType();
			int GetMoveCost();
			std::string GetSymbol();
			std::string GetDescription();

};

extern Terrain TerrainList[]; 

enum class LightLevel { LIGHT_DARK, LIGHT_LOW, LIGHT_NORMAL };

class RoomTile : public ObjectContainer {
	Terrain _terrain;
	Room* _parent;
	point _coords;
	LightLevel _light;
	std::map<ExitDirection, RoomTile*> _adjacents;
	std::list<Living*> _mobiles;
	std::vector<Exit*> _exits;
public:
	RoomTile(Terrain terrain, Room* parent, point _coords, LightLevel light);
	RoomTile();
	~RoomTile();
	std::string GetKey();
	Terrain GetTerrain();
	void SetTerrain(Terrain terrain);
	std::string GetTerrainString() const;
	void SetTerrainByString(const std::string terrain);
	Room* GetParent();
	bool SetParent(Room* parent);
	point GetCoords();
	bool SetCoords(point coords);
	LightLevel GetLighting();
	void SetLighting(LightLevel level);
	std::string GetLightingString() const;
	void SetLightingByString(const std::string lighting);
	std::map<ExitDirection, RoomTile*> GetAdjacentTiles();
	RoomTile* GetAdjacentTile(const ExitDirection direction);
	bool AdjacentExists(const ExitDirection direction);
	bool AddAdjacentTile(ExitDirection direction, RoomTile* tile);
	void RemoveAdjacentTile(ExitDirection direction);
	virtual std::list<Living*>* GetMobiles();
	Living* GetMobile(const std::string name);
	bool AddMobile(Living* mobile);
	bool RemoveMobile(Living* mobile);
	std::vector<Exit*> GetExits();
	Exit* GetExit(ExitDirection direction);
	bool AddExit(Exit*);
	bool RemoveExit(Exit* exit);
	bool MoveTo(Living* mobile, RoomTile* tile);
	std::string ShowTile();
	virtual void Serialize(TiXmlElement* root);
	virtual void Deserialize(TiXmlElement* node);
	void ObjectEnter(Entity *obj);
	void ObjectLeave(Entity *obj);
	virtual BOOL IsTile() const;
	BOOL CanEnter(Player* mobile);
	BOOL CanAddAdjacent(ExitDirection direction);
};
/*Utilites*/
std::string LightToString(LightLevel level);
LightLevel StringToLight(std::string light);
std::string TerrainToString(Terrain terrain);
Terrain StringToTerrain(std::string terrain);
std::string CoordToKey(point coords);

bool InitializeRoomTileOlcs();

#endif