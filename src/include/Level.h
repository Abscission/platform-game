
#include "Types.h"
#include "Renderer.h"

struct GridSquare {
	u16 Texture;
};

struct Chunk {
	u16 X;
	u16 Y;
	GridSquare Grid[16*16];

	Chunk* Collission;
};

class Level {
private:
	Sprite* Sprites;
	u32 LookupLocation(u16 X, u16 Y);
	Chunk* Chunks;
public:
	//Constructor and destructor for the level
	Level();
	~Level();

	//The width and height of the level
	u16 Width;
	u16 Height;

	void SetChunk(u16 X, u16 Y, Chunk C);
	Chunk& GetChunk(u16 X, u16 Y);
};
