#pragma once

#include <cstdint>

constexpr int32_t EXTRA_DIR_SIZE = 550;

// used in CStreaming
constexpr uint32_t TOTAL_DFF_MODEL_IDS = 20000;
constexpr uint32_t TOTAL_TXD_MODEL_IDS = 5000;
constexpr uint32_t TOTAL_COL_MODEL_IDS = 255;
constexpr uint32_t TOTAL_IPL_MODEL_IDS = 256;
constexpr uint32_t TOTAL_DAT_MODEL_IDS = 64;
constexpr uint32_t TOTAL_IFP_MODEL_IDS = 180;
constexpr uint32_t TOTAL_RRR_MODEL_IDS = 475;
constexpr uint32_t TOTAL_SCM_MODEL_IDS = 82;
constexpr uint32_t TOTAL_INTERNAL_MODEL_IDS = 4; // internal use?

constexpr uint32_t TOTAL_IMG_ARCHIVES = 8;
constexpr uint32_t TOTAL_LOADED_PEDS = 8;

constexpr uint32_t TOTAL_SCRIPTED_TASKS = 128;
constexpr uint32_t TOTAL_SCRIPTED_BRAIN_TASK_ENTRIES = 48;
constexpr int32_t  TOTAL_DECISION_MAKERS = 20;
constexpr int32_t  TOTAL_PED_GROUPS = 8;
constexpr int32_t  TOTAL_SPECIAL_MODELS = 10;
constexpr int32_t  TOTAL_CLOTHES_MODELS = 10;
constexpr int32_t  TOTAL_CUTSCENE_MODELS = 20;

constexpr int32_t  SPECIAL_MODELS_RESOURCE_ID = 290;
constexpr int32_t  CLOTHES_MODELS_RESOURCE_ID = 384;
constexpr int32_t  CUTSCENE_MODELS_RESOURCE_ID = 300;

constexpr auto MAX_VEHICLES_LOADED = 50;

// used in CRenderer
constexpr int32_t TOTAL_ENTITY_SCAN_LISTS = 5;
constexpr float MAX_LOD_DISTANCE = 300.0f;
constexpr float MAX_LOWLOD_DISTANCE = 150.0f;
constexpr float MAX_FADING_DISTANCE = 20.0f;
constexpr float MAX_STREAMING_DISTANCE = 50.0f;
constexpr float MAX_INVISIBLE_ENTITY_DISTANCE = 30.0f;
constexpr float MAX_INVISIBLE_VEHICLE_DISTANCE = 200.0f;
constexpr float MAX_STREAMING_RADIUS_SQUARED = 10000.0f;
constexpr float STREAMING_ANGLE_THRESHOLD_RAD = 0.36f;
constexpr float MAX_BIGBUILDING_STREAMING_RADIUS_SQUARED = 80000.0f;
constexpr float BIGBUILDING_STREAMING_ANGLE_THRESHOLD_RAD = 0.7f;
constexpr float LOWLOD_CAMERA_HEIGHT_THRESHOLD = 80.0f;
