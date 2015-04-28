/*
*The basic room object
*/
#ifndef ROOM_H
#define ROOM_H
#include <list>
#include <string>
#include <tinyxml.h>
#include "mud.h"
#include "conf.h"
#include "entity.h"
#include "living.h"
#include "exit.h"
#include "player.h"
#include "event.h"
#include "zone.h"
#include "utils.h"
#include "roomtile.h"
#include "objectContainer.h"
#include <map>

EVENT(ROOM_POST_LOOK);

class RoomTile;
/*TileMap is designed to take pull a RoomTile out by a std::string. This string is a combination of the x,y coordinates separated by an x. eg: 2x1
I have added an operator for a tilemap to allow access to the map using the [][] operators. This will allow interaction as if it's a 2 dimensional
array. */
typedef std::map<std::string, RoomTile* > RoomTileMap;


class TileMap : public RoomTileMap
{
public:
	int xsize, ysize;
	RoomTile* get(point coords); 
	void add(RoomTile *tile);
	void remove(RoomTile *tile);
};
class Room:public ObjectContainer
{
    std::list <Living*> _mobiles;
    std::vector<Exit*> _exits;
    FLAG _rflag;
    point _coord;
	TileMap* _roomtiles;
public:
    Commandable commands;
    Room();
	Room(int, int);
    virtual ~Room();
    /*
    *Adds the specified exit.
    *Param: [in] a pointer to the Exit object to be added.
    *Return: True if the exit could be added, false otherwise.
    */
    virtual BOOL AddExit(Exit* exit);
    /*
    *Checks for the existance of an exit
    *Param: [in] the direction of the exit to check for.
    *Return: True if the exit exists, false otherwise.
    */
    virtual BOOL ExitExists(ExitDirection dir);
    /*
    *Finds the specified exit.
    *Param: [in] the name of the exit.
    *Return: a pointer to the exit object found, or NULL if none was found.
    */
    virtual Exit* GetExit(ExitDirection dir);
    /*
    *Returns a pointer to the list of exits.
    *Return: a pointer to a vector of pointers to Exit objects
    */
    std::vector<Exit*>* GetExits();
    /*
    *Sets the flags on the room object.
    *Param: [in] the new flags.
    */
    virtual void SetRoomFlag(FLAG flag);
    /*
    *Returns the flags set on the room.
    */
    virtual FLAG GetRoomFlag();
    /*
    *Sends a message to all players in the room.
    *Param: [in] the message to send.
    */
    virtual void TellAll(const std::string &message);
    /*
    *Tells everyone in the room, except for a specific group of people.
    *Param: [in] the message to send.
    *Param: [in] a pointer to the list of players to exclude.
    */
    virtual void TellAllBut(const std::string & message, std::list <Player*>* players);
    /*
    *Sends a message to everyone but the specified target.
    *Param: [in] The message to send.
    *Param: [in] the target to exclude.
    */
    virtual void TellAllBut(const std::string &message, Player* exclude);
    /*
    *Returns a point structure with the coordenates of a room.
    *Return: a pointer to the coords structure.
    */
    point* GetCoord();
    /*
    *Sets the coord structure.
    *Param: [in] the coord structure to copy.
    */
    void SetCoord(point& coord);
    BOOL IsRoom() const;
//serialization
    virtual void Serialize(TiXmlElement* root);
    virtual void Deserialize(TiXmlElement* node);
	
	void ObjectEnter(Entity* obj);
    void ObjectLeave(Entity* obj);
    std::string TellObviousExits();
    EVENT(PostLook);
	bool TileExists(std::string tilekey);
	bool TileExists(point coords);
	void AddTile(RoomTile* tile);
	void RemoveTile(point coords);
	void RemoveTile(std::string key);
	std::string ShowTileMap();
	void CreateTileMap(int xmax, int ymax);
	void DeleteTileMap();
	RoomTile* GetTile(point coords);
	RoomTile* GetTile(std::string key);
	TileMap* GetTileMap() const;
	void FormatTileMap(); // This is to set all adjacents to the respective tiles in the tilemap.
	RoomTile* GetFirstAvailable(); //Returns the first available tile, in case a player doesn't have a tile set;
	virtual std::list<Entity*>* GetContents();
	virtual std::list<Living*>* GetMobiles(); //You MUST 'delete' anything you assign to this.
};

bool InitializeRoomOlcs();
#endif
