#pragma once

enum ePadKeys
{
	// KEY_ 						OnFoot 				InCar
	KEY_ACTION = 0, 		// TAB 			| 	ALT GR/LCTRL/NUM0	|	1
	KEY_CROUCH, 			// C 			|	H / CAPSLOCK		| 	2
	KEY_FIRE,				// LCTRL/LMB	|	LALT				| 	4
	KEY_SPRINT,				// SPACE		| 	W 					|	8
	KEY_SECONDARY_ATTACK, 	// ENTER		| 	ENTER 				|	16
	KEY_JUMP,				// LSHIFT		|	S 					| 	32
	KEY_LOOK_RIGHT,			// 				| 	E 					| 	64
	KEY_HANDBRAKE,			// RMB			|	SPACE 				| 	128
	KEY_LOOK_LEFT,			// 				|	Q 					| 	256
	//bool KEY_LOOK_BEHIND;	// KEY_LOOK_LEFT + KEY_LOOK_RIGHT 		| 	320
	KEY_SUBMISSION,			// NUM1/MMB		| 	2/NUMPAD+ 			| 	512
	KEY_WALK,				// LALT			| 	 					| 	1024
	KEY_ANALOG_UP,			// NUM8			| 	NUM8				| 	2048
	KEY_ANALOG_DOWN,		// NUM2 		| 	NUM2				|	4096
	KEY_ANALOG_LEFT,		// NUM4			| 	NUM4				|	8192
	KEY_ANALOG_RIGHT,		// NUM6			| 	NUM6				| 	16384
	SIZE,
	KEY_YES,				// Y 			| 	Y 					| 	65536
	KEY_NO,					// N 			| 	N 					| 	131072
	KEY_CTRL_BACK, 			// H 			|   H 					|	262144
};

typedef struct
{
	uint16_t wKeyLR;
	uint16_t wKeyUD;
	bool bKeys[ePadKeys::SIZE+4];
	bool bIgnoreJump;
} PAD_KEYS;

VALIDATE_SIZE(PAD_KEYS, 24);

extern PAD_KEYS LocalPlayerKeys;
extern PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

void HookCPad();