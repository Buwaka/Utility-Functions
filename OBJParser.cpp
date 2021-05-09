
//v,
//vt,
//vn,
//vp,
//f,
//o,
//g,
//s,
//mtllib,
//usemtl

MeshFilterLoader::MeshDataTypeOBJ MeshFilterLoader::GetOBJType(std::string type)
{
	std::string token = ToUpper(type);
	if (token == "V")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::v;
	}
	else if (token == "VT")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::vt;
	}
	else if (token == "VN")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::vn;
	}
	else if (token == "VP")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::vp;
	}
	else if (token == "F")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::f;
	}
	else if (token == "O")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::o;
	}
	else if (token == "G")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::g;
	}
	else if (token == "S")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::s;
	}
	else if (token == "MTLLIB")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::mtllib;
	}
	else //(token == "USEMTL")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::usemtl;
	}
}

MeshFilter * MeshFilterLoader::OBJLoader(const std::wstring & assetFile)
{
	//NOTE: OBJ files should be internally sorted so that v, vt and vn entries are before the f lines, as f lines make reference to previous v, vt and vn entries. 
	//Henceforth this parser requires the OBJ files to be internally sorted, but since the rayman 2 static maps export are already sorted, I will omit the sorting task alltogether

	auto textReader = new TextReader();
	textReader->Open(To_stringSimple(assetFile));

	if (!textReader->Exists())
	{
		delete textReader;
		return nullptr;
	}


	auto pMesh = new MeshFilter();
	pMesh->m_Indices.reserve(200);
	pMesh->m_TexCoords.reserve(200);
	pMesh->m_Positions.reserve(200);
	std::vector<DirectX::XMFLOAT2> texcoordLUT;
	texcoordLUT.reserve(200);
	std::vector<DirectX::XMFLOAT3> vectorLUT;
	vectorLUT.reserve(200);
	int indexcounter = 0;




	std::string line = textReader->ReadLine();
	bool eof = false;
	while (!eof)
	{

		std::vector<std::string> tokens;
		tokens = tokenize(line,' ');


		MeshDataTypeOBJ type = GetOBJType(tokens[0]);

		switch (type)
		{
		case MeshFilterLoader::MeshDataTypeOBJ::v:
		{
			pMesh->m_HasElement |= ILSemantic::POSITION;
			DirectX::XMFLOAT3 pos;

			pos.x = - std::stof(tokens[1]);
			pos.y = std::stof(tokens[2]);
			pos.z = std::stof(tokens[3]);
			//a 4th W coordinate could be present, but is not used in rayman 2 as far as I can see
			vectorLUT.push_back(pos);
		}
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::vt:
		{
			pMesh->m_HasElement |= ILSemantic::TEXCOORD;
			DirectX::XMFLOAT2 pos;

			pos.x = std::stof(tokens[1]);
			pos.y = - std::stof(tokens[2]);
			//pos.z = std::stof(tokens[3]);
			//a 3th W coordinate could be present, but is not used in rayman 2 as far as I can see

			texcoordLUT.push_back(pos);
		}
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::vn:
			//omitted because not used

			//pMesh->m_HasElement |= ILSemantic::NORMAL;
			//DirectX::XMFLOAT3 pos;

			//pos.x = std::stof(tokens[1]);
			//pos.y = std::stof(tokens[2]);
			//pos.z = std::stof(tokens[3]);

			//pMesh->m_Normals.push_back(pos);
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::vp:
			//omitted
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::f:

			for (size_t i = 1; i < tokens.size(); i++)
			{
				std::vector<std::string> subtokens;
				subtokens = tokenize(tokens[i], '/');

				int index = std::stoi(subtokens[0]) - 1;

				pMesh->m_Positions.push_back(vectorLUT[index]);
				pMesh->m_Indices.push_back(indexcounter);
				indexcounter++;
				if (subtokens.size() > 1)
				{
					int texcoordindex = std::stoi(subtokens[1]) -1;
					pMesh->m_TexCoords.push_back(texcoordLUT[texcoordindex]);
				}
				else
				{
					Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::LoadContent() > failed to load texture ", assetFile.c_str());
				}

			}


			break;
		case MeshFilterLoader::MeshDataTypeOBJ::o:
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::g:
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::s:
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::mtllib:
			//just grabbing the texture filepath, since only that's used
		{
			//std::wstring path = GetPath(assetFile);
			std::filesystem::path path(GetPath(assetFile));
			auto filename = tokens[1].substr(0, tokens[1].find("."));
			pMesh->m_MeshName = To_wstringSimple(filename);


			auto subtextReader = new TextReader();
			subtextReader->Open(path.generic_string() + tokens[1]);

			if (!subtextReader->Exists())
				break;

			bool found = false;
			std::filesystem::path mtlpath;
			std::string temp = subtextReader->ReadLine();
			bool seof = false;
			while (!seof)
			{
				int i = temp.find("map_Kd");
				int j = temp.find((char)0, i);
				if (i != std::string::npos)
				{
					mtlpath = temp.substr(i + 7, j);
					found = true;
					break;
				}
				if (!subtextReader->eof())
					temp = subtextReader->ReadLine();
				else
					seof = true;
			}


			if (found)
			{
				auto rpath = (path / mtlpath);
				auto ddstest = rpath;
				ddstest.replace_extension(".dds");

				while (pMesh->m_SourceTexture == nullptr)
				{
					if (exists_test(ddstest.generic_string()))
					{
						pMesh->m_SourceTexture = ContentManager::Load<TextureData>(ddstest);
					}
					else
					{
						pMesh->m_SourceTexture = ContentManager::Load<TextureData>(rpath);
					}
					if (pMesh->m_SourceTexture == nullptr)
						Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::LoadContent() > failed to load texture ", assetFile.c_str());
				}
			}
			delete subtextReader;
		}
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::usemtl:
			//misc unused metadata
			break;
		default:
			break;
		}

		if (!textReader->eof())
			line = textReader->ReadLine();
		else
			eof = true;
	}


	std::reverse(pMesh->m_Indices.begin(), pMesh->m_Indices.end());
	//std::reverse(pMesh->m_Indices.begin(), pMesh->m_Indices.end());
	pMesh->m_VertexCount = pMesh->m_Positions.size();
	pMesh->m_IndexCount = pMesh->m_Indices.size();
	if(pMesh->m_VertexCount == 0)
		Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::LoadContent() > no vertices found ", assetFile.c_str());

	delete textReader;

	return pMesh;
}
