#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Effects11\d3dx11effect.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\ScreenGrab.h>
#include <DirectXTK\PrimitiveBatch.h>
#include <DirectXTK\VertexTypes.h>
#include <DirectXTK\Effects.h>
#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>
#include <random>

using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
using namespace std;

namespace Engine
{
	const wstring		g_strTransformTag = TEXT("Com_Transform");

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum HIT_TYPE{ HIT_NORMAL, HIT_STRONG, HIT_THROW, HIT_BEATEN,  HIT_END};

	enum FONT_ORIGIN_OPTION { ORIGIN_CENTER, ORIGIN_LEFT_TOP, ORIGIN_RIGHT_TOP, ORIGIN_LEFT_BOTTOM, ORIGIN_RIGHT_BOTTOM, ORIGIN_END};

}

#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
using namespace Engine;


#include <iostream>
#include <filesystem>

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG