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
	dict_rect[(int)Tile::SOFT_BLOCK] =	{ 4*n , 3* n , n , n };
	dict_rect[(int)Tile::BOMB] =		{ 0, 3 * n , n ,  n };
	dict_rect[(int)Tile::BLOCK] =		{ 3*n , 3 * n , n , n };


	dict_rect[(int)Tile::DOOR] = {11 *n , 3*n ,n , n };


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
								/* a veces se peta, y al cargar la otra pantalla elimina al enemigo de la primera al volver a ella. revisar */
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
	return (tile == Tile::BLOCK || tile == Tile::SOFT_BLOCK || tile == Tile::BOMB);
}


bool TileMap::TestCollisionWallLeft(const AABB& box) const
{
	return CollisionX(box.pos, box.height);
}
bool TileMap::TestCollisionWallRight(const AABB& box) const
{
 
	return CollisionX(box.pos + Point(box.width - 1, 0), box.height);
}
bool TileMap::TestCollisionWallUp(const AABB& box) const
{
	return CollisionY(box.pos, box.height);
} 
bool TileMap::TestCollisionWallDown(const AABB& box) const
{

	return CollisionY(box.pos + Point(box.width - 1, 0), box.height);
}

bool TileMap::TestCollisionGround(const AABB& box, int* py) const
{
	Point p(box.pos.x, *py);
	int tile_y;

	if (CollisionY(p, box.width))
	{
		tile_y = p.y / TILE_SIZE;

		*py = tile_y * TILE_SIZE - 1;
		return true;
	}
	return false;
}

bool TileMap::CollisionX(const Point& p, int distance) const
{
	int x, y, y0, y1;

	x = p.x / TILE_SIZE;
	y0 = p.y / TILE_SIZE;
	y1 = (p.y + distance - 1) / TILE_SIZE;
	
	for (y = y0; y <= y1; ++y)
	{
		if (IsTileSolid(GetTileIndex(x, y)))
			return true;
	}
	return false;
}
bool TileMap::CollisionY(const Point& p, int distance) const
{
	int x, y, x0, x1;
	Tile tile;

	y = p.y / TILE_SIZE;
	x0 = p.x / TILE_SIZE;
	x1 = (p.x + distance - 1) / TILE_SIZE;

	for (x = x0; x <= x1; ++x)
	{
		tile = GetTileIndex(x, y);

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
			if (tile != Tile::AIR && tile != Tile::EMPTY)
			{
				rc = dict_rect[(int)tile];
				pos.x = (float)j * TILE_SIZE;
				pos.y = (float)i * TILE_SIZE;

				DrawTextureRec(*img_tiles, rc, pos, WHITE);
			}
			/*if (IsTileSolid(tile)) {
				DrawRectangleLines((int)pos.x, (int)pos.y, TILE_SIZE, TILE_SIZE, RED);
			}*/
		}
	}
}
void TileMap::Release()
{
	ResourceManager& data = ResourceManager::Instance(); 
	data.ReleaseTexture(Resource::IMG_TILES);



	dict_rect.clear();
}
Tile TileMap::GetObjectAtPosition(const AABB& box, int* px) const
{
	int left, right, bottom;
	int tx1, tx2, ty;
	Tile tile1;

	left = box.pos.x;
	right = box.pos.x + box.width;
	bottom = box.pos.y + box.height - 1;

	tx1 = left / TILE_SIZE;
	tx2 = right / TILE_SIZE;
	ty = bottom / TILE_SIZE;

	if (tx1 >= 0 && tx1 < LEVEL_WIDTH && ty >= 0 && ty < LEVEL_HEIGHT)
	{
		return GetTileIndex(tx1, ty);
	}
	else
	{
		// Si las coordenadas están fuera del mapa, devuelve un valor que indique "fuera de los límites"
		return Tile::EMPTY; // Asumiendo que Tile::EMPTY representa un espacio vacío o fuera de los límites
	}

}
bool TileMap::IsTileDoor(Tile tile) const
{
	return (tile == Tile::DOOR);
}
