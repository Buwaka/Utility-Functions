enum class MeshDataTypeOBJ
{
	v,
	vt,
	vn,
	vp,
	f,
	o,
	g,
	s,
	mtllib,
	usemtl
};

MeshDataTypeOBJ GetOBJType(std::string type);
MeshFilter* OBJLoader(const std::wstring& assetFile);