#pragma once

#include <process.h>
#include <string>
#include "GameInstance.h"

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_CUSTOMROOM, LEVEL_GAMEPLAY, LEVEL_BOSS, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum OBJECT_TYPE { OBJECT_ITEM, OBJECT_DECO, OBJECT_END };

	enum ANIM_TYPE { ANIM_PLAYER, ANIM_MONSTER, ANIM_END };
	enum NONANIM_TYPE { NONANIM_DECO, NONANIM_ITEM, NONANIM_MAP, NONANIM_END};

	const _ulonglong PLAYER_STATE_IDLE			= 0x0000000000000001; // Naruto : 135 / Custom : 139
	const _ulonglong PLAYER_STATE_RUN			= 0x0000000000000002; // Naruto : 169 / Custom : 177
	const _ulonglong PLAYER_STATE_RUN_END		= 0x0000000000000004; // Naruto : 168 / Custom : 176
	
	const _ulonglong PLAYER_STATE_MOVE			= 0x0000000020408007;
			
	
	const _ulonglong PLAYER_STATE_DASH_FRONT	= 0x0000000000000008; // Naruto : 101 / Custom : 118
	const _ulonglong PLAYER_STATE_DASH_BACK		= 0x0000000000000010; // Naruto : 102 / Custom : 117
	const _ulonglong PLAYER_STATE_DASH_LEFT		= 0x0000000000000020; // Naruto : 104 / Custom : 119
	const _ulonglong PLAYER_STATE_DASH_RIGHT	= 0x0000000000000040; // Naruto : 105 / Custom : 120
				
	const _ulonglong PLAYER_STATE_DASH			= 0x0000000000000078;
			

	const _ulonglong PLAYER_STATE_COMBO_1		 = 0x0000000000000080; // Naruto :  32 / Custom : 28
	const _ulonglong PLAYER_STATE_COMBO_2		 = 0x0000000000000100; // Naruto :  33 / Custom : 19
	const _ulonglong PLAYER_STATE_COMBO_3		 = 0x0000000000000200; // Naruto :  23 / Custom : 185
	const _ulonglong PLAYER_STATE_COMBO_4		 = 0x0000000000000400; // Naruto :  25 / Custom : 20
				
	const _ulonglong PLAYER_STATE_COMBO_ATTACK	 = 0x0000000000000780;
				
				
	const _ulonglong PLAYER_STATE_COMBO_STRONG_1		= 0x0000000000000800; // Naruto : 31 / Custom : 32
	const _ulonglong PLAYER_STATE_COMBO_STRONG_2		= 0x0000000000001000; // Naruto : 29 / Custom : 33
	const _ulonglong PLAYER_STATE_COMBO_STRONG_3		= 0x0000000000002000; // Naruto : 30 / Custom : 34
				
	const _ulonglong PLAYER_STATE_COMBO_STRONG_ATTACK	= 0x0000000000003800;
				
				
	const _ulonglong PLAYER_STATE_RASENGUN_CHARGE		= 0x0000000000004000; // Naruto : 152 
	const _ulonglong PLAYER_STATE_RASENGUN_RUN_LOOP		= 0x0000000000008000; // Naruto : 155 
	const _ulonglong PLAYER_STATE_RASENGUN_ATTACK		= 0x0000000000010000; // Naruto : 150 

	const _ulonglong PLAYER_STATE_CHIDORI_CHARGE		= 0x0000000000004000;				// Custom : 162
	const _ulonglong PLAYER_STATE_CHIDORI_RUN_LOOP		= 0x0000000000008000;				// Custom : 164
	const _ulonglong PLAYER_STATE_CHIDORI_ATTACK		= 0x0000000000010000;				// Custom : 159 

				
	const _ulonglong PLAYER_STATE_RASENSHURIKEN		= 0x0000000000020000; // Naruto : 160 / 
	const _ulonglong PLAYER_STATE_FLAME_BOMB		= 0x0000000000020000; //			  / Custom : 165 

				
	const _ulonglong PLAYER_STATE_RASENSGUN_SUPER		= 0x0000000000040000; // Naruto : 149 / 
	const _ulonglong PLAYER_STATE_KAMUI					= 0x0000000000040000; //			  / Custom : 166 


	const _ulonglong PLAYER_STATE_HANDSEAL				= 0x0000000000080000; // Naruto : 157 / Custom : 
				
	const _ulonglong PLAYER_STATE_JUMP					= 0x0000000000100000; // Naruto : 136 / Custom : 140
	const _ulonglong PLAYER_STATE_JUMP_DOUBLE			= 0x0000000000200000; // Naruto : 108 / Custom : 123
				
	const _ulonglong PLAYER_STATE_FALL_LOOP			= 0x0000000000400000; // Naruto : 118 / Custom : 128
	const _ulonglong PLAYER_STATE_LAND				= 0x0000000000800000; // Naruto : 138 / Custom : 135
				
	const _ulonglong PLAYER_STATE_AERIAL_COMBO_1	= 0x0000000001000000; // Naruto : 21 / Custom : 9
	const _ulonglong PLAYER_STATE_AERIAL_COMBO_2	= 0x0000000002000000; // Naruto : 19 / Custom : 7
	const _ulonglong PLAYER_STATE_AERIAL_COMBO_3	= 0x0000000004000000; // Naruto : 20 / Custom : 13
				
	const _ulonglong PLAYER_STATE_AERIAL_ATTACK		= 0x0000000007000000;
				
	const _ulonglong PLAYER_STATE_AERIAL_RASENGUN_CHARGE	= 0x0000000008000000; // Naruto : 146 / 
	const _ulonglong PLAYER_STATE_AERIAL_RASENGUN_RUN_START = 0x0000000010000000; // Naruto : 144 / 
	const _ulonglong PLAYER_STATE_AERIAL_RASENGUN_RUN_LOOP  = 0x0000000020000000; // Naruto : 143 / 
	const _ulonglong PLAYER_STATE_AERIAL_RASENGUN_ATTACK	= 0x0000000040000000; // Naruto : 142 / 

	const _ulonglong PLAYER_STATE_AERIAL_CHIDORI_CHARGE		= 0x0000000008000000; //			  / Custom : 155
	const _ulonglong PLAYER_STATE_AERIAL_CHIDORI_RUN_LOOP	= 0x0000000020000000; //			  / Custom : 157
	const _ulonglong PLAYER_STATE_AERIAL_CHIDORI_ATTACK		= 0x0000000040000000; //			  / Custom : 152

				
	const _ulonglong PLAYER_STATE_AERIAL_RASENSHURIKEN		= 0x0000000080000000; // Naruto : 148 /
	const _ulonglong PLAYER_STATE_AERIAL_FLAME_BOMB			= 0x0000000080000000; //			  / Custom : 158

				
	const _ulonglong PLAYER_STATE_AERIAL_RASENGUN_SUPER		= 0x0000000100000000; // Naruto : 141 / Custom : 

	const _ulonglong PLAYER_STATE_AERIAL_DASH_START			= 0x0000000200000000; // Naruto : 2 / Custom : 2
	const _ulonglong PLAYER_STATE_AERIAL_DASH_END			= 0x0000000400000000; // Naruto : 0 / Custom : 0

	const _ulonglong PLAYER_STATE_CHAKRAJUMP_START			= 0x0000000800000000; // Naruto : 99 / Custom : 115
	const _ulonglong PLAYER_STATE_CHAKRAJUMP_LOOP			= 0x0000001000000000; // Naruto : 98 / Custom : 114
	const _ulonglong PLAYER_STATE_CHAKRAJUMP_END			= 0x0000002000000000; // Naruto : 97 / Custom : 113

	const _ulonglong PLAYER_STATE_STRUCK_LEFT				= 0x0000004000000000; // Naruto : 73 / Custom : 80
	const _ulonglong PLAYER_STATE_STRUCK_RIGHT				= 0x0000008000000000; // Naruto : 81 / Custom : 88

	const _ulonglong PLAYER_STATE_STRUCK					= 0x000000C000000000; // 

	const _ulonglong PLAYER_THROW							= 0x0000010000000000; // Naruto : 34 / Custom : 38

	const _ulonglong PLAYER_BEATEN_START					= 0x0000020000000000; // Naruto : 88 / Custom : 95
	const _ulonglong PLAYER_BEATEN_END						= 0x0000040000000000; // Naruto : 86 / Custom : 93
	
	const _ulonglong PLAYER_BEATEN							= 0x0000060000000000; // 

	const _ulonglong PLAYER_HIT								= 0x000007C000000000; // 피격전체

	const _ulonglong PLAYER_GET_UP							= 0x0000080000000000; // Naruto : 120 / Custom : 130

	const _ulonglong PLAYER_APPEAR							= 0x0000100000000000; // Naruto : 112 / Custom : 127
	const _ulonglong PLAYER_EXIT							= 0x0000200000000000; // Naruto : 113 / Custom : 169

	const _ulonglong PLAYER_WOOD_HAND						= 0x0000400000000000; //			/Custom : 168


	const _ulonglong PLAYER_STRUCK_STRONG_LEFT				= 0x0000800000000000; ///Naruto : 76	/Custom : 83
	const _ulonglong PLAYER_STRUCK_STRONG_RIGHT				= 0x0001000000000000; ///Naruto : 74 	/Custom : 81

	const _ulonglong PLAYER_STRUCK_STRONG					= 0x0001800000000000; //

	const _ulonglong PLAYER_ELECTRICSHOCK					= 0x0002000000000000; ///Naruto : 63

	const _ulonglong PLAYER_DEAD							= 0x0004000000000000; ///Naruto : 77



	////////////////////////////////////////////////////////////////////////////////////

	const _uint MONSTER_IDLE = 0x00000001; // LeafNinja : 71 / Samurai : 103
	const _uint MONSTER_RUN = 0x00000002; // LeafNinja : 75 / Samurai : 110

	const _uint MONSTER_MOVE = 0x00000003; // 


	const _uint MONSTER_RUSH_ATTACK_PUNCH= 0x00000004; // LeafNinja : 74 / Samurai : 107
	const _uint MONSTER_RUSH_ATTACK_KICK = 0x00000008; // LeafNinja : 2 / Samurai : 16

	const _uint MONSTER_RUSH_ATTACK = 0x0000000C; // 


	const _uint MONSTER_COMBO_ATTACK_1 = 0x00000010; // LeafNinja : 1 / Samurai : 18
	const _uint MONSTER_COMBO_ATTACK_2 = 0x00000020; // LeafNinja : 0 / Samurai : 16

	const _uint MONSTER_COMBO_ATTACK = 0x00000030; // 

	
	const _uint MONSTER_HANDSEAL		= 0x00000040; //  LeafNinja : 72 / Samurai : 106
	const _uint MONSTER_NINJUTSU_ATTACK	= 0x00000080; //  LeafNinja : 73 / Samurai :  x

	const _uint MONSTER_NINJUTSU		= 0x000000C0; // 


	const _uint MONSTER_STRUCK_LEFT		= 0x00000100; // LeafNinja : 45 / Samurai : 68
	const _uint MONSTER_STRUCK_RIGHT	= 0x00000200; // LeafNinja : 53 / Samurai : 76

	const _uint MONSTER_STRUCK			= 0x00000300; // 


	const _uint MONSTER_STRUCK_STRONG_LEFT	= 0x00000400; // LeafNinja : 48 / Samurai : 71
	const _uint MONSTER_STRUCK_STRONG_RIGHT = 0x00000800; // LeafNinja : 46 / Samurai : 69 or 91

	const _uint MONSTER_STRUCK_STRONG		= 0x00000C00; // 


	const _uint MONSTER_THROW			= 0x00001000; // LeafNinja : 3 / Samurai : 20

	const _uint MONSTER_BEATEN_START	= 0x00002000; // LeafNinja : 60 / Samurai : 83
	const _uint MONSTER_BEATEN_END		= 0x00004000; // LeafNinja : 58 / Samurai : 81

	const _uint MONSTER_BEATEN			= 0x00006000; // 


	const _uint MONSTER_GET_UP			= 0x00008000; // LeafNinja : 70 / Samurai : 101 or 102
	const _uint MONSTER_ELECTRICSHOCK	= 0x00010000; // LeafNinja : 35 / Samurai : 101 or 57

	const _uint MONSTER_DEAD			= 0x00020000; // LeafNinja : 49 / Samurai : 72

	const _uint MONSTER_DASH_LEFT	= 0x00040000; // LeafNinja :  / Samurai : 95
	const _uint MONSTER_DASH_RIGHT	= 0x00080000; // LeafNinja :  / Samurai : 96

	const _uint MONSTER_DASH		= 0x000C0000; //






	enum MONSTER_STATE{ 
		MONSTER_STATE_IDLE, 
		MONSTER_STATE_RUSH_ATTACK,
		MONSTER_STATE_COMBO_ATTACK,
		MONSTER_STATE_BEATEN, 
		MONSTER_STATE_MOVE,
		MONSTER_STATE_NINJUTSU,
		MONSTER_STATE_NINJUTSU_2,
		MONSTER_STATE_NINJUTSU_3,
		MONSTER_STATE_STRUCK,
		MONSTER_STATE_END };


	const _uint BOSS_IDLE	= 0x00000001; // 21
	const _uint BOSS_RUN	= 0x00000002; // 31

	const _uint BOSS_MOVE	= 0x00000003; // 


	const _uint BOSS_DASH_LEFT	= 0x00000004; // 22 안씀
	const _uint BOSS_DASH		= 0x00000008; // 7


	const _uint BOSS_APPEAR		= 0x00000010; // 12
	const _uint BOSS_APPEAR2	= 0x00000020; // 18

	const _uint BOSS_DEAD		= 0x00000040; // 10
	const _uint BOSS_DEAD_LOOP  = 0x00000080; // 11

	const _uint BOSS_ATTACK_SCRATCH		= 0x00000100; // 0
	const _uint BOSS_ATTACK_KICK		= 0x00000200; // 1
	const _uint BOSS_ATTACK_BITE		= 0x00000400; // 3
	const _uint BOSS_RUSH_ATTACK		= 0x00000800; // 4

	const _uint BOSS_ATTACK_SCRATCH_FAR = 0x00001000; // 6
	const _uint BOSS_ATTACK_LASER		= 0x00002000; // 29
	const _uint BOSS_ATTACK_LASER_AERIAL= 0x00004000; // 26
	const _uint BOSS_ATTACK_FIREBALL	= 0x00008000; // 27

	const _uint BOSS_ATTACK				= 0x0000FF00;

	const _uint BOSS_PHASECHANGE		= 0x00010000; // 28

	enum BOSS_STATE {
		BOSS_STATE_APPEAR,
		BOSS_STATE_IDLE,
		BOSS_STATE_MOVE,
		BOSS_STATE_ATTACK,
		BOSS_STATE_DEAD,
		BOSS_STATE_END
	};
}

extern _int				g_iPartsIndex[4];
extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace std;
using namespace Client;
