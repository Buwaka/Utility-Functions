#pragma region CollisionFunctionality

bool utils::IsPointInRect( const Point2f& p, const Rectf& r )
{
	return ( p.x >= r.left && p.x <= r.left + r.width &&
		p.y >= r.bottom && p.y <= r.bottom + r.height );
}

bool utils::IsPointInCircle( const Point2f& p, const Circlef& c )
{
	float squaredDist = ( p.x - c.center.x )*( p.x - c.center.x ) + ( p.y - c.center.y ) * ( p.y - c.center.y );
	float squaredRadius = c.radius * c.radius;
	return ( squaredRadius >= squaredDist );
}


bool utils::IsOverlapping( const Point2f & a, const Point2f & b, const Rectf & r )
{
	// if one of the line segment end points is in the rect
	if ( utils::IsPointInRect( a, r ) || utils::IsPointInRect( b, r ) )
	{
		return true;
	}

	HitInfo hitInfo{};
	Point2f vertices[]{ Point2f {r.left, r.bottom},
		Point2f{ r.left + r.width, r.bottom },
		Point2f{ r.left + r.width, r.bottom + r.height },
		Point2f{ r.left, r.bottom + r.height } };

	return Raycast( vertices, 4, a, b, hitInfo );
}

bool utils::IsOverlapping( const Rectf & r1, const Rectf & r2 )
{
	// If one rectangle is on left side of the other
	if ( ( r1.left + r1.width ) <= r2.left || ( r2.left + r2.width ) <= r1.left )
	{
		return false;
	}

	// If one rectangle is under the other
	if ( r1.bottom >= ( r2.bottom + r2.height ) || r2.bottom >= ( r1.bottom + r1.height ) )
	{
		return false;
	}

	return true;
}

bool utils::IsOverlapping( const Rectf& r, const Circlef& c )
{
	// Is center of circle in the rectangle?
	if ( IsPointInRect( c.center, r ) )return true;

	// Check line segments
	if ( utils::DistPointLineSegment( c.center, { r.left, r.bottom }, { r.left, r.bottom + r.height } ) <= c.radius ) return true;
	if ( utils::DistPointLineSegment( c.center, { r.left, r.bottom }, { r.left + r.width, r.bottom } ) <= c.radius ) return true;
	if ( utils::DistPointLineSegment( c.center, { r.left + r.width, r.bottom + r.height }, { r.left, r.bottom + r.height } ) <= c.radius ) return true;
	if ( utils::DistPointLineSegment( c.center, { r.left + r.width, r.bottom + r.height }, { r.left + r.width, r.bottom } ) <= c.radius ) return true;
	return false;
}
bool utils::IsOverlapping( const Circlef& c1, const Circlef& c2 )
{
	// squared distance between centers
	float xDistance{ c1.center.x - c2.center.x };
	float yDistance{ c1.center.y - c2.center.y };
	float squaredDistance = xDistance * xDistance + yDistance * yDistance;

	float squaredTouchingDistance = ( c1.radius + c2.radius ) * ( c1.radius + c2.radius );
	if ( squaredDistance < squaredTouchingDistance )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool utils::IsOverlapping( const Point2f& a, const Point2f& b, const Circlef& c )
{
	return utils::DistPointLineSegment( c.center, a, b ) <= c.radius;
}

bool utils::IsOverlapping( const std::vector<Point2f>& vertices, const Circlef& c )
{
	return IsOverlapping( vertices.data( ), vertices.size( ), c );
}

bool utils::IsOverlapping( const Point2f* vertices, size_t nrVertices, const Circlef& c )
{
	// Check points in circle
	for ( size_t i{ 0 }; i < nrVertices; ++i )
	{
		if ( IsPointInCircle( vertices[i], c ) )
		{
			return true;
		}
	}

	// Check overlapping line segments with circle
	for ( size_t i{ 0 }; i < nrVertices; ++i )
	{
		if ( DistPointLineSegment( c.center, vertices[i], vertices[( i + 1 ) % nrVertices] ) <= c.radius )
		{
			return true;
		}
	}

	// No overlapping line segments, verify whether circle is inside polygon
	if ( IsPointInPolygon( c.center, vertices, nrVertices ) )
	{
		return true;
	}
	return false;
}

bool utils::IsPointInPolygon( const Point2f& p, const std::vector<Point2f>& vertices )
{
	return IsPointInPolygon( p, vertices.data( ), vertices.size( ) );
}

bool utils::IsPointInPolygon( const Point2f& p, const Point2f* vertices, size_t nrVertices )
{
	if ( nrVertices < 2 )
	{
		return false;
	}
	// 1. First do a simple test with axis aligned bounding box around the polygon
	float xMin{ vertices[0].x };
	float xMax{ vertices[0].x };
	float yMin{ vertices[0].y };
	float yMax{ vertices[0].y };
	for ( size_t idx{ 1 }; idx < nrVertices; ++idx )
	{
		if ( xMin > vertices[idx].x )
			xMin = vertices[idx].x;
		if ( xMax < vertices[idx].x )
			xMax = vertices[idx].x;
		if ( yMin > vertices[idx].y )
			yMin = vertices[idx].y;
		if ( yMax < vertices[idx].y )
			yMax = vertices[idx].y;
	}
	if ( p.x < xMin || p.x > xMax || p.y < yMin || p.y > yMax )
		return false;

	// 2. Draw a virtual ray from anywhere outside the polygon to the point 
	//    and count how often it hits any side of the polygon. 
	//    If the number of hits is even, it's outside of the polygon, if it's odd, it's inside.
	int numberOfIntersectionPoints{0};
	Point2f p2{ xMax + 10.0f, p.y }; // Horizontal line from point to point outside polygon (p2)

	// Count the number of intersection points
	float lambda1{}, lambda2{};
	for ( size_t i{ 0 }; i < nrVertices; ++i )
	{
		if ( IntersectLineSegments( vertices[i], vertices[( i + 1 ) % nrVertices], p, p2, lambda1, lambda2 ) )
		{
			if ( lambda1 > 0 && lambda1 <= 1 && lambda2 > 0 && lambda2 <= 1 )
			{
				++numberOfIntersectionPoints;
			}
		}
	}
	if ( numberOfIntersectionPoints % 2 == 0 )
		return false;
	else
		return true;
}

bool utils::IntersectLineSegments( const Point2f& p1, const Point2f& p2, const Point2f& q1, const Point2f& q2, float& outLambda1, float& outLambda2, float epsilon )
{
	bool intersecting{ false };

	Vector2f p1p2{ p1, p2 };
	Vector2f q1q2{ q1, q2 };

	// cross product to determine if parallel
	float denom = p1p2.CrossProduct( q1q2 );

	// Don't divide by zero
	if ( std::abs( denom ) > epsilon )
	{
		intersecting = true;

		Vector2f p1q1{ p1, q1 };

		float num1 = p1q1.CrossProduct( q1q2 );
		float num2 = p1q1.CrossProduct( p1p2 );
		outLambda1 = num1 / denom;
		outLambda2 = num2 / denom;
	}
	else // are parallel
	{
		//connect start points
		Vector2f p1q1{ p1, q1 };

		// cross product to determine if segments and the line connecting their start points are parallel, 
		// if so, than they are on a line
		// if not, then there is no intersection
		float denom = p1q1.CrossProduct( q1q2 );
		if ( std::abs( denom ) > epsilon ) return false;

		// check the 4 conditions
		outLambda1 = 0;
		outLambda2 = 0;
		if ( utils::IsPointOnLineSegment( p1, q1, q2 ) )intersecting = true;
		if ( utils::IsPointOnLineSegment( p2, q1, q2 ) )intersecting = true;
		if ( utils::IsPointOnLineSegment( q1, p1, p2 ) )intersecting = true;
		if ( utils::IsPointOnLineSegment( q2, p1, p2 ) )intersecting = true;
	}
	return intersecting;
}
bool utils::Raycast( const std::vector<Point2f>& vertices, const Point2f& rayP1, const Point2f& rayP2, HitInfo& hitInfo )
{
	return Raycast( vertices.data( ), vertices.size( ), rayP1, rayP2, hitInfo );
}

bool utils::Raycast( const Point2f* vertices, const size_t nrVertices, const Point2f& rayP1, const Point2f& rayP2, HitInfo& hitInfo )
{
	if ( nrVertices == 0 )
	{
		return false;
	}

	std::vector<HitInfo> hits;

	Rectf r1, r2;
	// r1: minimal AABB rect enclosing the ray
	r1.left = std::min( rayP1.x, rayP2.x );
	r1.bottom = std::min( rayP1.y, rayP2.y );
	r1.width = std::max( rayP1.x, rayP2.x ) - r1.left;
	r1.height = std::max( rayP1.y, rayP2.y ) - r1.bottom;

	// Line-line intersections.
	for ( size_t idx{ 0 }; idx <= nrVertices; ++idx )
	{
		// Consider line segment between 2 consecutive vertices
		// (modulo to allow closed polygon, last - first vertice)
		Point2f q1 = vertices[( idx + 0 ) % nrVertices];
		Point2f q2 = vertices[( idx + 1 ) % nrVertices];

		// r2: minimal AABB rect enclosing the 2 vertices
		r2.left = std::min( q1.x, q2.x );
		r2.bottom = std::min( q1.y, q2.y );
		r2.width = std::max( q1.x, q2.x ) - r2.left;
		r2.height = std::max( q1.y, q2.y ) - r2.bottom;

		if ( IsOverlapping( r1, r2 ) )
		{
			float lambda1{};
			float lambda2{};
			if ( IntersectLineSegments( rayP1, rayP2, q1, q2, lambda1, lambda2 ) )
			{
				if ( lambda1 > 0 && lambda1 <= 1 && lambda2 > 0 && lambda2 <= 1 )
				{
					HitInfo hitInfo;
					hitInfo.lambda = lambda1;
					hitInfo.intersectPoint = { rayP1.x + ( ( rayP2.x - rayP1.x ) * lambda1 ), rayP1.y + ( ( rayP2.y - rayP1.y ) * lambda1 ) };
					hitInfo.normal = Vector2f{ q2 - q1 }.Orthogonal( ).Normalized( );
					hits.push_back( hitInfo );
				}
			}
		}
	}

	if ( hits.size( ) == 0 )
	{
		return false;
	}

	// Get closest intersection point and copy it into the hitInfo parameter
	hitInfo = *std::min_element( hits.begin( ), hits.end( ),
		[]( const HitInfo& first, const HitInfo& last ) {
		return first.lambda < last.lambda; } );
	return true;
}


bool  utils::IsPointOnLineSegment( const Point2f& p, const Point2f& a, const Point2f& b )
{
	Vector2f ap{ a, p }, bp{ b, p };
	// If not on same line, return false
	if ( abs( ap.CrossProduct( bp ) ) > 0.001f )
	{
		return false;
	}

	// Both vectors must point in opposite directions if p is between p1 and p2
	if ( ap.DotProduct( bp ) > 0 )
	{
		return false;
	}

	return true;
}

float  utils::DistPointLineSegment( const Point2f& p, const Point2f& a, const Point2f& b )
{
	Vector2f ba{ a, b };
	Vector2f pa{ a, p };
	Vector2f normba = ba.Normalized( );
	float distToA = normba.DotProduct( pa );
	// if distToA is negative, then the closest point is A
	// return the distance a, p
	if ( distToA < 0 )
	{
		return Vector2f{ a, p }.Length( );
	}
	// if distToA is > than dist(a,b) then the closest point is B
	// return the distance b, p
	float distAB = Vector2f{ a, b }.Length( );
	if ( distToA > distAB )
	{
		return Vector2f{ b, p }.Length( );
	}

	//closest point is between A and B, calc intersection point
	Vector2f intersection = normba.DotProduct( pa ) * normba + a;
	return Vector2f{ p - intersection }.Length( );
}
#pragma endregion CollisionFunctionality


int Sutils::sign(int x)
{
	return (x >= 0) ? 1 : -1;
}

double Sutils::sign(double x)
{
	return (x >= 0.0) ? 1.0 : -1.0;
}

float Sutils::sign(float x)
{
	return (x >= 0.0f) ? 1.0f : -1.0f;
}

float Sutils::PolarToCartesianX(float r, float angle)
{
	return r * cosf(angle * (float)utils::g_Pi / 180);
}

float Sutils::PolarToCartesianY(float r, float angle)
{
	return  r * sinf(angle * (float)utils::g_Pi / 180);
}

float Sutils::HueToRGB(float v1, float v2, float vH) {
	if (vH < 0)
		vH += 1;

	if (vH > 1)
		vH -= 1;

	if ((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);

	if ((2 * vH) < 1)
		return v2;

	if ((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

	return v1;
}

float Sutils::HSLToRGB(float h, float s, float l, char c)
{
	float r{ 0 };
	float g{ 0 };
	float b{ 0 };

	if (s == 0)
	{
		r = g = b = (float)(l * 255);
	}
	else
	{
		float v1, v2;
		float hue = h / 360;

		v2 = (l < 0.5) ? (l * (1 + s)) : ((l + s) - (l * s));
		v1 = 2 * l - v2;

		r = Sutils::HueToRGB(v1, v2, hue + (1.0f / 3));
		g = Sutils::HueToRGB(v1, v2, hue);
		b = Sutils::HueToRGB(v1, v2, hue - (1.0f / 3));
	}

	return (c == 'r' ? r : c == 'g' ? g : b);
}

//generic C
float Sutils::lerpC(float v0, float v1, float t) {
	return (1 - t)*v0 + t * v1;
}

//FMA optimized
float Sutils::lerp(float v0, float v1, float t) {
	return fma(t, v1, fma(-t, v0, v0));
}





int Sutils::DigitsAmount(int number)
{
	int digits = 0;
	if (number <= 0) digits = 1; // remove this line if '-' counts as a digit
	while (number) {
		number /= 10;
		digits++;
	}
	return digits;
}


std::string Sutils::IntToStringWidth(int value, int length)
{
	int digits = Sutils::DigitsAmount(value);
	std::string number{};

	for (int i = 0; i < length - digits; i++)
	{
		number += "0";
	}

	return number + std::to_string(value);
}


void Sutils::ReverseRoute(std::vector<Direction>* route)
{
	std::reverse(route->begin(), route->end());
	for (size_t i = 0; i < route->size(); i++)
	{
		switch (route->at(i))
		{
		case Up: route->at(i) = Down;
			break;

		case Down:  route->at(i) = Up;
			break;

		case Left: route->at(i) = Right;
			break;

		case Right: route->at(i) = Left;
			break;
		}
	}
}

Uint32 Sutils::GetPixelPosition(Vector2f position, const SDL_Surface* surface)
{

	int bpp = surface->format->BytesPerPixel;

	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + (  ( (surface->h - (int)position.y) * surface->pitch) + ((int)position.x * bpp) );


	//std::cout << (int) p << "  " << (m_pCollissionMapSurface->h - (int)position.y) << "   " << (int)position.x << "    " << (m_pCollissionMapSurface->h - (int)position.y) * m_pCollissionMapSurface->pitch + (int)position.x * bpp << "   ";


	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}

}

Sutils::RGBA Sutils::GetRGBA(Vector2f position, const SDL_Surface * surface)
{
	RGBA newrgba;
	SDL_GetRGBA(GetPixelPosition(position, surface), surface->format, &newrgba.Red, &newrgba.Green, &newrgba.Blue, &newrgba.Alpha);
	return newrgba;
}

SDL_Surface* Sutils::LoadSurface(std::string path)
{
	// Load image at specified path
	SDL_Surface* pLoadedSurface = IMG_Load(path.c_str());
	if (pLoadedSurface == nullptr)
	{
		std::cerr << "Texture::CreateFromImage, error when calling IMG_Load: " << SDL_GetError() << std::endl;

		return nullptr;
	}

	return pLoadedSurface;
}


int Sutils::Manhattan(Vector2I start, Vector2I destination)
{
	int x = std::abs(start.x - destination.x);
	int y = std::abs(start.y - destination.y);
	return x + y;
}

float Sutils::Manhattanf(Vector2f start, Vector2f destination)
{
	float x = std::abs(start.x - destination.x);
	float y = std::abs(start.y - destination.y);
	return x + y;
}

int Sutils::Euclidean(Vector2I start, Vector2I destination)
{
	int x = std::abs(start.x - destination.x);
	int y = std::abs(start.y - destination.y);
	return (int)std::sqrt((x * x) + (y * y));
}

float Sutils::Euclideanf(Vector2f start, Vector2f destination)
{
	float x = std::abs(start.x - destination.x);
	float y = std::abs(start.y - destination.y);
	return std::sqrt((x * x) + (y * y));
}

int Sutils::Chebyshev(Vector2I start, Vector2I destination)
{
	int x = std::abs(start.x - destination.x);
	int y = std::abs(start.y - destination.y);
	return (x + y) - std::min(x, y);
}

float Sutils::Chebyshevf(Vector2f start, Vector2f destination)
{
	float x = std::abs(start.x - destination.x);
	float y = std::abs(start.y - destination.y);
	return (x + y) - std::min(x, y);
}

int Sutils::Octile(Vector2I start, Vector2I destination)
{
	const float root2 = 1.4142135623730950488016887242097f;
	int x = std::abs(start.x - destination.x);
	int y = std::abs(start.y - destination.y);
	return int((x + y) + (root2 - 2) * std::min(x, y));
}

float Sutils::Octilef(Vector2f start, Vector2f destination)
{
	const float root2 = 1.4142135623730950488016887242097f;
	float x = std::abs(start.x - destination.x);
	float y = std::abs(start.y - destination.y);
	return (x + y) + (root2 - 2) * std::min(x, y);
}

float Sutils::AnglePoints(Point2f p1, Point2f p2)
{
	return atan((p2.y - p1.y) / (p2.x - p1.x));
}

float Sutils::Angle(Vector2f in) 
{
	float angle = atan(in.y / in.x);

	if (in.x < 0)
		angle += utils::g_Pi;
	else if (in.y < 0)
		angle += utils::g_Pi * 2;


	return angle;
}

float Sutils::Angle(Point2f in) 
{
	float angle = atan(in.y / in.x);

	if (in.x < 0)
		angle += utils::g_Pi;
	else if (in.y < 0)
		angle += utils::g_Pi * 2;


	return angle;
}
