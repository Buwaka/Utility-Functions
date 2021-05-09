#Cheap Curve
more efficient but less accurate sin/cos curve

#Sutils
	bool IsPointInRect(const Point2f& p, const Rectf& r);
	bool IsPointInCircle(const Point2f& p, const Circlef& c);
	bool IsPointInPolygon( const Point2f& p, const std::vector<Point2f>& vertices );
	bool IsPointInPolygon( const Point2f& p, const Point2f* vertices, size_t nrVertices );

	bool IsOverlapping( const Point2f& a, const Point2f& b, const Circlef& c );
	bool IsOverlapping( const Point2f& a, const Point2f& b, const Rectf& r );

	bool IsOverlapping(const Rectf & r1, const Rectf & r2);
	bool IsOverlapping( const Rectf& r, const Circlef& c );
	bool IsOverlapping( const Circlef& c1, const Circlef& c2 );
	bool IsOverlapping( const std::vector<Point2f>& vertices, const Circlef& c );
	bool IsOverlapping( const Point2f* vertices, size_t nrVertices, const Circlef& c );
	bool Raycast( const Point2f* vertices, const size_t nrVertices, const Point2f& rayP1, const Point2f& rayP2, HitInfo& hitInfo );
	bool Raycast( const std::vector<Point2f>& vertices, const Point2f& rayP1, const Point2f& rayP2, HitInfo& hitInfo );

	bool IntersectLineSegments(const Point2f& p1, const Point2f& p2, const Point2f& q1, const Point2f& q2, float& outLambda1, float& outLambda2, float epsilon = 1e-6);
	float DistPointLineSegment(const Point2f& p, const Point2f& a, const Point2f& b);
	bool IsPointOnLineSegment(const Point2f& p, const Point2f& a, const Point2f& b);

	int sign(int x);
	double sign(double x);
	float sign(float x);

	float PolarToCartesianX(float r, float angle);
	float PolarToCartesianY(float r, float angle);

	float HueToRGB(float v1, float v2, float vH);
	float HSLToRGB(float h, float s, float l, char c);

	//generic C
	float lerpC(float v0, float v1, float t);

	//FMA optimized
	float lerp(float v0, float v1, float t);

	//generic C, sammy edition
	template <typename T>
	T lerpS(T x, T y, float t) 

	int DigitsAmount(int number);
	std::string IntToStringWidth(int value, int length);


	struct RGBA
	{
		Uint8 Red, Green, Blue, Alpha;
	};

	enum Direction
	{
		Down, Left, Right, Up, size
	};
	void ReverseRoute(std::vector<Direction>* route);

	Uint32 GetPixelPosition(Vector2f position, const SDL_Surface* surface);
	RGBA GetRGBA(Vector2f position, const SDL_Surface* surface);

	//heuristics
	enum Heuristic
	{
		manhatten, euclidean, chebyshev, octile, Heuristicsize
	};

	float Manhattanf(Vector2f start, Vector2f destination);
	float Euclideanf(Vector2f start, Vector2f destination);
	float Chebyshevf(Vector2f start, Vector2f destination);
	float Octilef(Vector2f start, Vector2f destination);


	float AnglePoints(Point2f p1, Point2f p2);
	float Angle(Vector2f in) ;
	
#Pathfinding
Dijkstra and A-Star pathfinding implementation (old)
Uses Sutils

std::vector<Sutils::Direction> Dijkstra(const Grid& map, Vector2f start, Vector2f destination);
std::vector<Sutils::Direction> Astar(const Grid& map, Vector2f start, Vector2f destination, Sutils::Heuristic type = Sutils::manhatten);

#SurfaceMath
Collection of SDL software rendering functions

#Subject and Observer
Observer pattern implementation

#Singleton
Singleton implementation 

#Threadpool
Threadpool implementation (https://github.com/progschj/ThreadPool)

#GameTime
Highly accurate windows specific gametime/deltatime implementation

#InputManager
Gamepad, keyboard and mouse input manager with unordered map

#ScriptLoader
Custom script parser (quick and dirty), was used to read in an obj based level for the rayman2 remake project