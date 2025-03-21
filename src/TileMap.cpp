#include "TileMap.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <cstring>

TileMap::TileMap()
{
	map = nullptr;
	width = 0;
	height = 0;
	
	img_tiles = nullptr;

	InitTileDictionary();
}
TileMap::~TileMap()
{
	if (map != nullptr)
	{
		delete[] map;
		map = nullptr;
	}
	
}
void TileMap::InitTileDictionary()
{
	const int n = TILE_SIZE;

	dict_rect[(int)Tile::GREEN_BLOCK] =	{ 12 * n, 3 * n , n , n };
	dict_rect[(int)Tile::SOFT_BLOCK] =	{ 4 * n ,  0 , n , n };
	dict_rect[(int)Tile::BOMB] =		{ 0, 3 * n , n , 4 * n };
	dict_rect[(int)Tile::BLOCK] =		{ 3 * n , 3 * n , n , n };


	dict_rect[(int)Tile::DOOR] = { 11*n , 3*n , 12*n , 4*n };


	dict_rect[(int)Tile::ITEM_BOMB_UP] = { 0 , 0 , n , n };
	dict_rect[(int)Tile::ITEM_FIRE_UP] = { n , 0 , 2*n , n };
	dict_rect[(int)Tile::ITEM_SPEED_UP] = { 2*n , 0 , 3*n , n };
	dict_rect[(int)Tile::ITEM_REMOTE_CONTROL] = { 3*n , 0 , 4*n , n };
	dict_rect[(int)Tile::ITEM_WALL_PASS] = { 4*n , 0 , n , 5*n };
	dict_rect[(int)Tile::ITEM_BOMB_PASS] = { 5*n , 0 , n , 6*n };
	dict_rect[(int)Tile::ITEM_FLAME_PASS] = { 6*n , 0 , 7*n , n };
	dict_rect[(int)Tile::ITEM_INVINCIBLE] = { 7*n , 0 , 8*n , n };

	dict_rect[(int)Tile::ITEM_B_PANEL] = { 0 , n , n , 2*n };
	dict_rect[(int)Tile::ITEM_GODDESS] = { n , n , 2*n , 2*n };
	dict_rect[(int)Tile::ITEM_COLA] = { 2*n ,  n, 3*n , 2*n };
	dict_rect[(int)Tile::ITEM_FAMICOM] = { 3*n , n , 4*n , 2*n };
	dict_rect[(int)Tile::ITEM_PROGRAMMER_NAKAMOTO] = { 4*n , n , 5*n , 2*n };
	dict_rect[(int)Tile::ITEM_DEZENIMAN] = { 5*n , n , 6*n , 2*n };

	

}
AppStatus TileMap::Initialise()
{
	ResourceManager& data = ResourceManager::Instance();

	if (data.LoadTexture(Resource::IMG_TILES, "resources/Sprites/General.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	img_tiles = data.GetTexture(Resource::IMG_TILES);

	
	return AppStatus::OK;
}
AppStatus TileMap::Load(int data[], int w, int h)
{
	size = w*h;
	width = w;
	height = h;

	if (map != nullptr)	delete[] map;

	map = new Tile[size];
	if (map == nullptr)	
	{
		LOG("Failed to allocate memory for tile map");
		return AppStatus::ERROR;
	}
	memcpy(map, data, size * sizeof(int));

	return AppStatus::OK;
}
void TileMap::Update()
{
	
}
Tile TileMap::GetTileIndex(int x, int y) const
{
	int idx = x + y*width;
	if(idx < 0 || idx >= size)
	{
		LOG("Error: Index out of bounds. Tile map dimensions: %dx%d. Given index: (%d, %d)", width, height, x, y)
		return Tile::AIR;
	}
	return map[x + y * width];
}
bool TileMap::IsTileSolid(Tile tile) const
{
	return (Tile::BLOCK <= tile && tile <= Tile::BOMB);//creo
}


bool TileMap::TestCollisionWallLeft(const AABB& box) const
{
	return CollisionX(box.pos, box.height);
}
bool TileMap::TestCollisionWallRight(const AABB& box) const
{
	//comprobar que funcione  
	return CollisionX(box.pos + Point(box.width - 1, 0), box.height);
}
bool TileMap::TestCollisionWallUp(const AABB& box) const
{
	return CollisionY(box.pos, box.height);
} 
bool TileMap::TestCollisionWallDown(const AABB& box) const
{
	//comprobar que funcione 1
	//return CollisionY(box.pos + Point(box.width - 1, 0), box.height + Point(box.height - 1, 0));
	return CollisionY(box.pos + Point(box.width - 1, 0), box.height);
}

bool TileMap::CollisionX(const Point& p, int distance) const
{
	int x, y, y0, y1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y0 = p.y / TILE_SIZE;
	y1 = (p.y + distance - 1) / TILE_SIZE;
	
	//Iterate over the tiles within the vertical range
	for (y = y0; y <= y1; ++y)
	{
		//One solid tile is sufficient
		if (IsTileSolid(GetTileIndex(x, y)))
			return true;
	}
	return false;
}
bool TileMap::CollisionY(const Point& p, int distance) const
{
	int x, y, x0, x1;
	Tile tile;

	//Calculate the tile coordinates and the range of tiles to check for collision
	y = p.y / TILE_SIZE;
	x0 = p.x / TILE_SIZE;
	x1 = (p.x + distance - 1) / TILE_SIZE;

	//Iterate over the tiles within the horizontal range
	for (x = x0; x <= x1; ++x)
	{
		tile = GetTileIndex(x, y);

		//One solid or laddertop tile is sufficient
		if (IsTileSolid(tile)  )
			return true;
	}
	return false;
}
void TileMap::Render()
{
	Tile tile;
	Rectangle rc;
	Vector2 pos;

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			tile = map[i * width + j];
			if (tile != Tile::AIR)
			{
				pos.x = (float)j * TILE_SIZE;
				pos.y = (float)i * TILE_SIZE;

				
			}
		}
	}
}
void TileMap::Release()
{
	ResourceManager& data = ResourceManager::Instance(); 
	data.ReleaseTexture(Resource::IMG_TILES);



	dict_rect.clear();
}