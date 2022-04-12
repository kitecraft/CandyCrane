#pragma once
#include <Arduino.h>

enum ButtonMap {
	btn_Emergency_StopHome = 0,
	btn_Tower_Forward,
	btn_Tower_StopHome,
	btn_Tower_Backward,
	btn_Dolly_Outwards = 8,
	btn_Dolly_StopHome,
	btn_Dolly_Inwards,
	btn_Bucket_Up,
	btn_Bucket_StopHome,
	btn_Bucket_Down,
	btn_Bucket_Open,
	btn_Bucket_Close
};