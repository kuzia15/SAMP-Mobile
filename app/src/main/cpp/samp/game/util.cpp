#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"
#include <cmath>
#include "../net/netgame.h"

extern CNetGame* pNetGame;
extern CGame* pGame;

const char g_szAnimBlockNames[][40] = {
"AIRPORT:thrw_barl_thrw",
"ATTRACTORS:stepsit_in",
"ATTRACTORS:stepsit_loop",
"ATTRACTORS:stepsit_out",
"BAR:barcustom_get",
"BAR:barcustom_loop",
"BAR:barcustom_order",
"BAR:barman_idle",
"BAR:barserve_bottle",
"BAR:barserve_give",
"BAR:barserve_glass",
"BAR:barserve_in",
"BAR:barserve_loop",
"BAR:barserve_order",
"BAR:dnk_stndf_loop",
"BAR:dnk_stndm_loop",
"BASEBALL:bat_1",
"BASEBALL:bat_2",
"BASEBALL:bat_3",
"BASEBALL:bat_4",
"BASEBALL:bat_block",
"BASEBALL:bat_hit_1",
"BASEBALL:bat_hit_2",
"BASEBALL:bat_hit_3",
"BASEBALL:bat_idle",
"BASEBALL:bat_m",
"BASEBALL:bat_part",
"BD_FIRE:bd_fire1",
"BD_FIRE:bd_fire2",
"BD_FIRE:bd_fire3",
"BD_FIRE:bd_gf_wave",
"BD_FIRE:bd_panic_01",
"BD_FIRE:bd_panic_02",
"BD_FIRE:bd_panic_03",
"BD_FIRE:bd_panic_04",
"BD_FIRE:bd_panic_loop",
"BD_FIRE:grlfrd_kiss_03",
"BD_FIRE:m_smklean_loop",
"BD_FIRE:playa_kiss_03",
"BD_FIRE:wash_up",
"BEACH:bather",
"BEACH:lay_bac_loop",
"BEACH:parksit_m_loop",
"BEACH:parksit_w_loop",
"BEACH:sitnwait_loop_w",
"BENCHPRESS:gym_bp_celebrate",
"BENCHPRESS:gym_bp_down",
"BENCHPRESS:gym_bp_getoff",
"BENCHPRESS:gym_bp_geton",
"BENCHPRESS:gym_bp_up_a",
"BENCHPRESS:gym_bp_up_b",
"BENCHPRESS:gym_bp_up_smooth",
"BF_INJECTION:bf_getin_lhs",
"BF_INJECTION:bf_getin_rhs",
"BF_INJECTION:bf_getout_lhs",
"BF_INJECTION:bf_getout_rhs",
"BIKED:biked_back",
"BIKED:biked_drivebyft",
"BIKED:biked_drivebylhs",
"BIKED:biked_drivebyrhs",
"BIKED:biked_fwd",
"BIKED:biked_getoffback",
"BIKED:biked_getofflhs",
"BIKED:biked_getoffrhs",
"BIKED:biked_hit",
"BIKED:biked_jumponl",
"BIKED:biked_jumponr",
"BIKED:biked_kick",
"BIKED:biked_left",
"BIKED:biked_passenger",
"BIKED:biked_pushes",
"BIKED:biked_ride",
"BIKED:biked_right",
"BIKED:biked_shuffle",
"BIKED:biked_still",
"BIKEH:bikeh_back",
"BIKEH:bikeh_drivebyft",
"BIKEH:bikeh_drivebylhs",
"BIKEH:bikeh_drivebyrhs",
"BIKEH:bikeh_fwd",
"BIKEH:bikeh_getoffback",
"BIKEH:bikeh_getofflhs",
"BIKEH:bikeh_getoffrhs",
"BIKEH:bikeh_hit",
"BIKEH:bikeh_jumponl",
"BIKEH:bikeh_jumponr",
"BIKEH:bikeh_kick",
"BIKEH:bikeh_left",
"BIKEH:bikeh_passenger",
"BIKEH:bikeh_pushes",
"BIKEH:bikeh_ride",
"BIKEH:bikeh_right",
"BIKEH:bikeh_still",
"BIKELEAP:bk_blnce_in",
"BIKELEAP:bk_blnce_out",
"BIKELEAP:bk_jmp",
"BIKELEAP:bk_rdy_in",
"BIKELEAP:bk_rdy_out",
"BIKELEAP:struggle_cesar",
"BIKELEAP:struggle_driver",
"BIKELEAP:truck_driver",
"BIKELEAP:truck_getin",
"BIKES:bikes_back",
"BIKES:bikes_drivebyft",
"BIKES:bikes_drivebylhs",
"BIKES:bikes_drivebyrhs",
"BIKES:bikes_fwd",
"BIKES:bikes_getoffback",
"BIKES:bikes_getofflhs",
"BIKES:bikes_getoffrhs",
"BIKES:bikes_hit",
"BIKES:bikes_jumponl",
"BIKES:bikes_jumponr",
"BIKES:bikes_kick",
"BIKES:bikes_left",
"BIKES:bikes_passenger",
"BIKES:bikes_pushes",
"BIKES:bikes_ride",
"BIKES:bikes_right",
"BIKES:bikes_snatch_l",
"BIKES:bikes_snatch_r",
"BIKES:bikes_still",
"BIKEV:bikev_back",
"BIKEV:bikev_drivebyft",
"BIKEV:bikev_drivebylhs",
"BIKEV:bikev_drivebyrhs",
"BIKEV:bikev_fwd",
"BIKEV:bikev_getoffback",
"BIKEV:bikev_getofflhs",
"BIKEV:bikev_getoffrhs",
"BIKEV:bikev_hit",
"BIKEV:bikev_jumponl",
"BIKEV:bikev_jumponr",
"BIKEV:bikev_kick",
"BIKEV:bikev_left",
"BIKEV:bikev_passenger",
"BIKEV:bikev_pushes",
"BIKEV:bikev_ride",
"BIKEV:bikev_right",
"BIKEV:bikev_still",
"BIKE_DBZ:pass_driveby_bwd",
"BIKE_DBZ:pass_driveby_fwd",
"BIKE_DBZ:pass_driveby_lhs",
"BIKE_DBZ:pass_driveby_rhs",
"BMX:bmx_back",
"BMX:bmx_bunnyhop",
"BMX:bmx_drivebyft",
"BMX:bmx_driveby_lhs",
"BMX:bmx_driveby_rhs",
"BMX:bmx_fwd",
"BMX:bmx_getoffback",
"BMX:bmx_getofflhs",
"BMX:bmx_getoffrhs",
"BMX:bmx_jumponl",
"BMX:bmx_jumponr",
"BMX:bmx_left",
"BMX:bmx_pedal",
"BMX:bmx_pushes",
"BMX:bmx_ride",
"BMX:bmx_right",
"BMX:bmx_sprint",
"BMX:bmx_still",
"BOMBER:bom_plant",
"BOMBER:bom_plant_2idle",
"BOMBER:bom_plant_crouch_in",
"BOMBER:bom_plant_crouch_out",
"BOMBER:bom_plant_in",
"BOMBER:bom_plant_loop",
"BOX:boxhipin",
"BOX:boxhipup",
"BOX:boxshdwn",
"BOX:boxshup",
"BOX:bxhipwlk",
"BOX:bxhwlki",
"BOX:bxshwlk",
"BOX:bxshwlki",
"BOX:bxwlko",
"BOX:catch_box",
"BSKTBALL:bball_def_jump_shot",
"BSKTBALL:bball_def_loop",
"BSKTBALL:bball_def_stepl",
"BSKTBALL:bball_def_stepr",
"BSKTBALL:bball_dnk",
"BSKTBALL:bball_dnk_gli",
"BSKTBALL:bball_dnk_gli_o",
"BSKTBALL:bball_dnk_lnch",
"BSKTBALL:bball_dnk_lnch_o",
"BSKTBALL:bball_dnk_lnd",
"BSKTBALL:bball_dnk_o",
"BSKTBALL:bball_idle",
"BSKTBALL:bball_idle2",
"BSKTBALL:bball_idle2_o",
"BSKTBALL:bball_idleloop",
"BSKTBALL:bball_idleloop_o",
"BSKTBALL:bball_idle_o",
"BSKTBALL:bball_jump_cancel",
"BSKTBALL:bball_jump_cancel_o",
"BSKTBALL:bball_jump_end",
"BSKTBALL:bball_jump_shot",
"BSKTBALL:bball_jump_shot_o",
"BSKTBALL:bball_net_dnk_o",
"BSKTBALL:bball_pickup",
"BSKTBALL:bball_pickup_o",
"BSKTBALL:bball_react_miss",
"BSKTBALL:bball_react_score",
"BSKTBALL:bball_run",
"BSKTBALL:bball_run_o",
"BSKTBALL:bball_skidstop_l",
"BSKTBALL:bball_skidstop_l_o",
"BSKTBALL:bball_skidstop_r",
"BSKTBALL:bball_skidstop_r_o",
"BSKTBALL:bball_walk",
"BSKTBALL:bball_walkstop_l",
"BSKTBALL:bball_walkstop_l_o",
"BSKTBALL:bball_walkstop_r",
"BSKTBALL:bball_walkstop_r_o",
"BSKTBALL:bball_walk_o",
"BSKTBALL:bball_walk_start",
"BSKTBALL:bball_walk_start_o",
"BUDDY:buddy_crouchfire",
"BUDDY:buddy_crouchreload",
"BUDDY:buddy_fire",
"BUDDY:buddy_fire_poor",
"BUDDY:buddy_reload",
"BUS:bus_close",
"BUS:bus_getin_lhs",
"BUS:bus_getin_rhs",
"BUS:bus_getout_lhs",
"BUS:bus_getout_rhs",
"BUS:bus_jacked_lhs",
"BUS:bus_open",
"BUS:bus_open_rhs",
"BUS:bus_pullout_lhs",
"CAMERA:camcrch_cmon",
"CAMERA:camcrch_idleloop",
"CAMERA:camcrch_stay",
"CAMERA:camcrch_to_camstnd",
"CAMERA:camstnd_cmon",
"CAMERA:camstnd_idleloop",
"CAMERA:camstnd_lkabt",
"CAMERA:camstnd_to_camcrch",
"CAMERA:piccrch_in",
"CAMERA:piccrch_out",
"CAMERA:piccrch_take",
"CAMERA:picstnd_in",
"CAMERA:picstnd_out",
"CAMERA:picstnd_take",
"CAR:fixn_car_loop",
"CAR:fixn_car_out",
"CAR:flag_drop",
"CAR:sit_relaxed",
"CAR:tap_hand",
"CAR:tyd2car_bump",
"CAR:tyd2car_high",
"CAR:tyd2car_low",
"CAR:tyd2car_med",
"CAR:tyd2car_turnl",
"CAR:tyd2car_turnr",
"CARRY:crry_prtial",
"CARRY:liftup",
"CARRY:liftup05",
"CARRY:liftup105",
"CARRY:putdwn",
"CARRY:putdwn05",
"CARRY:putdwn105",
"CAR_CHAT:carfone_in",
"CAR_CHAT:carfone_loopa",
"CAR_CHAT:carfone_loopa_to_b",
"CAR_CHAT:carfone_loopb",
"CAR_CHAT:carfone_loopb_to_a",
"CAR_CHAT:carfone_out",
"CAR_CHAT:car_sc1_bl",
"CAR_CHAT:car_sc1_br",
"CAR_CHAT:car_sc1_fl",
"CAR_CHAT:car_sc1_fr",
"CAR_CHAT:car_sc2_fl",
"CAR_CHAT:car_sc3_br",
"CAR_CHAT:car_sc3_fl",
"CAR_CHAT:car_sc3_fr",
"CAR_CHAT:car_sc4_bl",
"CAR_CHAT:car_sc4_br",
"CAR_CHAT:car_sc4_fl",
"CAR_CHAT:car_sc4_fr",
"CAR_CHAT:car_talkm_in",
"CAR_CHAT:car_talkm_loop",
"CAR_CHAT:car_talkm_out",
"CASINO:cards_in",
"CASINO:cards_loop",
"CASINO:cards_lose",
"CASINO:cards_out",
"CASINO:cards_pick_01",
"CASINO:cards_pick_02",
"CASINO:cards_raise",
"CASINO:cards_win",
"CASINO:dealone",
"CASINO:manwinb",
"CASINO:manwind",
"CASINO:roulette_bet",
"CASINO:roulette_in",
"CASINO:roulette_loop",
"CASINO:roulette_lose",
"CASINO:roulette_out",
"CASINO:roulette_win",
"CASINO:slot_bet_01",
"CASINO:slot_bet_02",
"CASINO:slot_in",
"CASINO:slot_lose_out",
"CASINO:slot_plyr",
"CASINO:slot_wait",
"CASINO:slot_win_out",
"CASINO:wof",
"CHAINSAW:csaw_1",
"CHAINSAW:csaw_2",
"CHAINSAW:csaw_3",
"CHAINSAW:csaw_g",
"CHAINSAW:csaw_hit_1",
"CHAINSAW:csaw_hit_2",
"CHAINSAW:csaw_hit_3",
"CHAINSAW:csaw_part",
"CHAINSAW:idle_csaw",
"CHAINSAW:weapon_csaw",
"CHAINSAW:weapon_csawlo",
"CHOPPA:choppa_back",
"CHOPPA:choppa_bunnyhop",
"CHOPPA:choppa_drivebyft",
"CHOPPA:choppa_driveby_lhs",
"CHOPPA:choppa_driveby_rhs",
"CHOPPA:choppa_fwd",
"CHOPPA:choppa_getoffback",
"CHOPPA:choppa_getofflhs",
"CHOPPA:choppa_getoffrhs",
"CHOPPA:choppa_jumponl",
"CHOPPA:choppa_jumponr",
"CHOPPA:choppa_left",
"CHOPPA:choppa_pedal",
"CHOPPA:choppa_pushes",
"CHOPPA:choppa_ride",
"CHOPPA:choppa_right",
"CHOPPA:choppa_sprint",
"CHOPPA:choppa_still",
"CLOTHES:clo_buy",
"CLOTHES:clo_in",
"CLOTHES:clo_out",
"CLOTHES:clo_pose_hat",
"CLOTHES:clo_pose_in",
"CLOTHES:clo_pose_in_o",
"CLOTHES:clo_pose_legs",
"CLOTHES:clo_pose_loop",
"CLOTHES:clo_pose_out",
"CLOTHES:clo_pose_out_o",
"CLOTHES:clo_pose_shoes",
"CLOTHES:clo_pose_torso",
"CLOTHES:clo_pose_watch",
"COACH:coach_inl",
"COACH:coach_inr",
"COACH:coach_opnl",
"COACH:coach_opnr",
"COACH:coach_outl",
"COACH:coach_outr",
"COLT45:2guns_crouchfire",
"COLT45:colt45_crouchfire",
"COLT45:colt45_crouchreload",
"COLT45:colt45_fire",
"COLT45:colt45_fire_2hands",
"COLT45:colt45_reload",
"COLT45:sawnoff_reload",
"COP_AMBIENT:copbrowse_in",
"COP_AMBIENT:copbrowse_loop",
"COP_AMBIENT:copbrowse_nod",
"COP_AMBIENT:copbrowse_out",
"COP_AMBIENT:copbrowse_shake",
"COP_AMBIENT:coplook_in",
"COP_AMBIENT:coplook_loop",
"COP_AMBIENT:coplook_nod",
"COP_AMBIENT:coplook_out",
"COP_AMBIENT:coplook_shake",
"COP_AMBIENT:coplook_think",
"COP_AMBIENT:coplook_watch",
"COP_DVBYZ:cop_dvby_b",
"COP_DVBYZ:cop_dvby_ft",
"COP_DVBYZ:cop_dvby_l",
"COP_DVBYZ:cop_dvby_r",
"CRACK:bbalbat_idle_01",
"CRACK:bbalbat_idle_02",
"CRACK:crckdeth1",
"CRACK:crckdeth2",
"CRACK:crckdeth3",
"CRACK:crckdeth4",
"CRACK:crckidle1",
"CRACK:crckidle2",
"CRACK:crckidle3",
"CRACK:crckidle4",
"CRIB:crib_console_loop",
"CRIB:crib_use_switch",
"CRIB:ped_console_loop",
"CRIB:ped_console_loose",
"CRIB:ped_console_win",
"DAM_JUMP:dam_dive_loop",
"DAM_JUMP:dam_land",
"DAM_JUMP:dam_launch",
"DAM_JUMP:jump_roll",
"DAM_JUMP:sf_jumpwall",
"DANCING:bd_clap",
"DANCING:bd_clap1",
"DANCING:dance_loop",
"DANCING:dan_down_a",
"DANCING:dan_left_a",
"DANCING:dan_loop_a",
"DANCING:dan_right_a",
"DANCING:dan_up_a",
"DANCING:dnce_m_a",
"DANCING:dnce_m_b",
"DANCING:dnce_m_c",
"DANCING:dnce_m_d",
"DANCING:dnce_m_e",
"DEALER:dealer_deal",
"DEALER:dealer_idle",
"DEALER:dealer_idle_01",
"DEALER:dealer_idle_02",
"DEALER:dealer_idle_03",
"DEALER:drugs_buy",
"DEALER:shop_pay",
"DILDO:dildo_1",
"DILDO:dildo_2",
"DILDO:dildo_3",
"DILDO:dildo_block",
"DILDO:dildo_g",
"DILDO:dildo_hit_1",
"DILDO:dildo_hit_2",
"DILDO:dildo_hit_3",
"DILDO:dildo_idle",
"DODGE:cover_dive_01",
"DODGE:cover_dive_02",
"DODGE:crushed",
"DODGE:crush_jump",
"DOZER:dozer_align_lhs",
"DOZER:dozer_align_rhs",
"DOZER:dozer_getin_lhs",
"DOZER:dozer_getin_rhs",
"DOZER:dozer_getout_lhs",
"DOZER:dozer_getout_rhs",
"DOZER:dozer_jacked_lhs",
"DOZER:dozer_jacked_rhs",
"DOZER:dozer_pullout_lhs",
"DOZER:dozer_pullout_rhs",
"DRIVEBYS:gang_drivebylhs",
"DRIVEBYS:gang_drivebylhs_bwd",
"DRIVEBYS:gang_drivebylhs_fwd",
"DRIVEBYS:gang_drivebyrhs",
"DRIVEBYS:gang_drivebyrhs_bwd",
"DRIVEBYS:gang_drivebyrhs_fwd",
"DRIVEBYS:gang_drivebytop_lhs",
"DRIVEBYS:gang_drivebytop_rhs",
"FAT:fatidle",
"FAT:fatidle_armed",
"FAT:fatidle_csaw",
"FAT:fatidle_rocket",
"FAT:fatrun",
"FAT:fatrun_armed",
"FAT:fatrun_csaw",
"FAT:fatrun_rocket",
"FAT:fatsprint",
"FAT:fatwalk",
"FAT:fatwalkstart",
"FAT:fatwalkstart_csaw",
"FAT:fatwalkst_armed",
"FAT:fatwalkst_rocket",
"FAT:fatwalk_armed",
"FAT:fatwalk_csaw",
"FAT:fatwalk_rocket",
"FAT:idle_tired",
"FIGHT_B:fightb_1",
"FIGHT_B:fightb_2",
"FIGHT_B:fightb_3",
"FIGHT_B:fightb_block",
"FIGHT_B:fightb_g",
"FIGHT_B:fightb_idle",
"FIGHT_B:fightb_m",
"FIGHT_B:hitb_1",
"FIGHT_B:hitb_2",
"FIGHT_B:hitb_3",
"FIGHT_C:fightc_1",
"FIGHT_C:fightc_2",
"FIGHT_C:fightc_3",
"FIGHT_C:fightc_block",
"FIGHT_C:fightc_blocking",
"FIGHT_C:fightc_g",
"FIGHT_C:fightc_idle",
"FIGHT_C:fightc_m",
"FIGHT_C:fightc_spar",
"FIGHT_C:hitc_1",
"FIGHT_C:hitc_2",
"FIGHT_C:hitc_3",
"FIGHT_D:fightd_1",
"FIGHT_D:fightd_2",
"FIGHT_D:fightd_3",
"FIGHT_D:fightd_block",
"FIGHT_D:fightd_g",
"FIGHT_D:fightd_idle",
"FIGHT_D:fightd_m",
"FIGHT_D:hitd_1",
"FIGHT_D:hitd_2",
"FIGHT_D:hitd_3",
"FIGHT_E:fightkick",
"FIGHT_E:fightkick_b",
"FIGHT_E:hit_fightkick",
"FIGHT_E:hit_fightkick_b",
"FINALE:fin_climb_in",
"FINALE:fin_cop1_climbout2",
"FINALE:fin_cop1_loop",
"FINALE:fin_cop1_stomp",
"FINALE:fin_hang_l",
"FINALE:fin_hang_loop",
"FINALE:fin_hang_r",
"FINALE:fin_hang_slip",
"FINALE:fin_jump_on",
"FINALE:fin_land_car",
"FINALE:fin_land_die",
"FINALE:fin_legsup",
"FINALE:fin_legsup_l",
"FINALE:fin_legsup_loop",
"FINALE:fin_legsup_r",
"FINALE:fin_let_go",
"FINALE2:fin_cop1_climbout",
"FINALE2:fin_cop1_fall",
"FINALE2:fin_cop1_loop",
"FINALE2:fin_cop1_shot",
"FINALE2:fin_cop1_swing",
"FINALE2:fin_cop2_climbout",
"FINALE2:fin_switch_p",
"FINALE2:fin_switch_s",
"FLAME:flame_fire",
"FLOWERS:flower_attack",
"FLOWERS:flower_attack_m",
"FLOWERS:flower_hit",
"FOOD:eat_burger",
"FOOD:eat_chicken",
"FOOD:eat_pizza",
"FOOD:eat_vomit_p",
"FOOD:eat_vomit_sk",
"FOOD:ff_dam_bkw",
"FOOD:ff_dam_fwd",
"FOOD:ff_dam_left",
"FOOD:ff_dam_right",
"FOOD:ff_die_bkw",
"FOOD:ff_die_fwd",
"FOOD:ff_die_left",
"FOOD:ff_die_right",
"FOOD:ff_sit_eat1",
"FOOD:ff_sit_eat2",
"FOOD:ff_sit_eat3",
"FOOD:ff_sit_in",
"FOOD:ff_sit_in_l",
"FOOD:ff_sit_in_r",
"FOOD:ff_sit_look",
"FOOD:ff_sit_loop",
"FOOD:ff_sit_out_180",
"FOOD:ff_sit_out_l_180",
"FOOD:ff_sit_out_r_180",
"FOOD:shp_thank",
"FOOD:shp_tray_in",
"FOOD:shp_tray_lift",
"FOOD:shp_tray_lift_in",
"FOOD:shp_tray_lift_loop",
"FOOD:shp_tray_lift_out",
"FOOD:shp_tray_out",
"FOOD:shp_tray_pose",
"FOOD:shp_tray_return",
"FREEWEIGHTS:gym_barbell",
"FREEWEIGHTS:gym_free_a",
"FREEWEIGHTS:gym_free_b",
"FREEWEIGHTS:gym_free_celebrate",
"FREEWEIGHTS:gym_free_down",
"FREEWEIGHTS:gym_free_loop",
"FREEWEIGHTS:gym_free_pickup",
"FREEWEIGHTS:gym_free_putdown",
"FREEWEIGHTS:gym_free_up_smooth",
"GANGS:DEALER_DEAL",
"GANGS:dealer_idle",
"GANGS:drnkbr_prtl",
"GANGS:drnkbr_prtl_f",
"GANGS:drugs_buy",
"GANGS:hndshkaa",
"GANGS:hndshkba",
"GANGS:hndshkca",
"GANGS:hndshkcb",
"GANGS:hndshkda",
"GANGS:hndshkea",
"GANGS:hndshkfa",
"GANGS:hndshkfa_swt",
"GANGS:invite_no",
"GANGS:invite_yes",
"GANGS:leanidle",
"GANGS:leanin",
"GANGS:leanout",
"GANGS:prtial_gngtlka",
"GANGS:prtial_gngtlkb",
"GANGS:prtial_gngtlkc",
"GANGS:prtial_gngtlkd",
"GANGS:prtial_gngtlke",
"GANGS:prtial_gngtlkf",
"GANGS:prtial_gngtlkg",
"GANGS:prtial_gngtlkh",
"GANGS:prtial_hndshk_01",
"GANGS:prtial_hndshk_biz_01",
"GANGS:shake_cara",
"GANGS:shake_cark",
"GANGS:shake_carsh",
"GANGS:smkcig_prtl",
"GANGS:smkcig_prtl_f",
"GHANDS:gsign1",
"GHANDS:gsign1lh",
"GHANDS:gsign2",
"GHANDS:gsign2lh",
"GHANDS:gsign3",
"GHANDS:gsign3lh",
"GHANDS:gsign4",
"GHANDS:gsign4lh",
"GHANDS:gsign5",
"GHANDS:gsign5lh",
"GHANDS:lhgsign1",
"GHANDS:lhgsign2",
"GHANDS:lhgsign3",
"GHANDS:lhgsign4",
"GHANDS:lhgsign5",
"GHANDS:rhgsign1",
"GHANDS:rhgsign2",
"GHANDS:rhgsign3",
"GHANDS:rhgsign4",
"GHANDS:rhgsign5",
"GHETTO_DB:gdb_car2_ply",
"GHETTO_DB:gdb_car2_smo",
"GHETTO_DB:gdb_car2_swe",
"GHETTO_DB:gdb_car_ply",
"GHETTO_DB:gdb_car_ryd",
"GHETTO_DB:gdb_car_smo",
"GHETTO_DB:gdb_car_swe",
"GOGGLES:goggles_put_on",
"GRAFFITI:graffiti_chkout",
"GRAFFITI:spraycan_fire",
"GRAVEYARD:mrnf_loop",
"GRAVEYARD:mrnm_loop",
"GRAVEYARD:prst_loopa",
"GRENADE:weapon_start_throw",
"GRENADE:weapon_throw",
"GRENADE:weapon_throwu",
"GYMNASIUM:gymshadowbox",
"GYMNASIUM:gym_bike_celebrate",
"GYMNASIUM:gym_bike_fast",
"GYMNASIUM:gym_bike_faster",
"GYMNASIUM:gym_bike_getoff",
"GYMNASIUM:gym_bike_geton",
"GYMNASIUM:gym_bike_pedal",
"GYMNASIUM:gym_bike_slow",
"GYMNASIUM:gym_bike_still",
"GYMNASIUM:gym_jog_falloff",
"GYMNASIUM:gym_shadowbox",
"GYMNASIUM:gym_tread_celebrate",
"GYMNASIUM:gym_tread_falloff",
"GYMNASIUM:gym_tread_getoff",
"GYMNASIUM:gym_tread_geton",
"GYMNASIUM:gym_tread_jog",
"GYMNASIUM:gym_tread_sprint",
"GYMNASIUM:gym_tread_tired",
"GYMNASIUM:gym_tread_walk",
"GYMNASIUM:gym_walk_falloff",
"GYMNASIUM:pedals_fast",
"GYMNASIUM:pedals_med",
"GYMNASIUM:pedals_slow",
"GYMNASIUM:pedals_still",
"HAIRCUTS:brb_beard_01",
"HAIRCUTS:brb_buy",
"HAIRCUTS:brb_cut",
"HAIRCUTS:brb_cut_in",
"HAIRCUTS:brb_cut_out",
"HAIRCUTS:brb_hair_01",
"HAIRCUTS:brb_hair_02",
"HAIRCUTS:brb_in",
"HAIRCUTS:brb_loop",
"HAIRCUTS:brb_out",
"HAIRCUTS:brb_sit_in",
"HAIRCUTS:brb_sit_loop",
"HAIRCUTS:brb_sit_out",
"HEIST9:cas_g2_gasko",
"HEIST9:swt_wllpk_l",
"HEIST9:swt_wllpk_l_back",
"HEIST9:swt_wllpk_r",
"HEIST9:swt_wllpk_r_back",
"HEIST9:swt_wllshoot_in_l",
"HEIST9:swt_wllshoot_in_r",
"HEIST9:swt_wllshoot_out_l",
"HEIST9:swt_wllshoot_out_r",
"HEIST9:use_swipecard",
"INT_HOUSE:bed_in_l",
"INT_HOUSE:bed_in_r",
"INT_HOUSE:bed_loop_l",
"INT_HOUSE:bed_loop_r",
"INT_HOUSE:bed_out_l",
"INT_HOUSE:bed_out_r",
"INT_HOUSE:lou_in",
"INT_HOUSE:lou_loop",
"INT_HOUSE:lou_out",
"INT_HOUSE:wash_up",
"INT_OFFICE:ff_dam_fwd",
"INT_OFFICE:off_sit_2idle_180",
"INT_OFFICE:off_sit_bored_loop",
"INT_OFFICE:off_sit_crash",
"INT_OFFICE:off_sit_drink",
"INT_OFFICE:off_sit_idle_loop",
"INT_OFFICE:off_sit_in",
"INT_OFFICE:off_sit_read",
"INT_OFFICE:off_sit_type_loop",
"INT_OFFICE:off_sit_watch",
"INT_SHOP:shop_cashier",
"INT_SHOP:shop_in",
"INT_SHOP:shop_looka",
"INT_SHOP:shop_lookb",
"INT_SHOP:shop_loop",
"INT_SHOP:shop_out",
"INT_SHOP:shop_pay",
"INT_SHOP:shop_shelf",
"JST_BUISNESS:girl_01",
"JST_BUISNESS:girl_02",
"JST_BUISNESS:player_01",
"JST_BUISNESS:smoke_01",
"KART:kart_getin_lhs",
"KART:kart_getin_rhs",
"KART:kart_getout_lhs",
"KART:kart_getout_rhs",
"KISSING:bd_gf_wave",
"KISSING:gfwave2",
"KISSING:gf_carargue_01",
"KISSING:gf_carargue_02",
"KISSING:gf_carspot",
"KISSING:gf_streetargue_01",
"KISSING:gf_streetargue_02",
"KISSING:gift_get",
"KISSING:gift_give",
"KISSING:grlfrd_kiss_01",
"KISSING:grlfrd_kiss_02",
"KISSING:grlfrd_kiss_03",
"KISSING:playa_kiss_01",
"KISSING:playa_kiss_02",
"KISSING:playa_kiss_03",
"KNIFE:kill_knife_ped_damage",
"KNIFE:kill_knife_ped_die",
"KNIFE:kill_knife_player",
"KNIFE:kill_partial",
"KNIFE:knife_1",
"KNIFE:knife_2",
"KNIFE:knife_3",
"KNIFE:knife_4",
"KNIFE:knife_block",
"KNIFE:knife_g",
"KNIFE:knife_hit_1",
"KNIFE:knife_hit_2",
"KNIFE:knife_hit_3",
"KNIFE:knife_idle",
"KNIFE:knife_part",
"KNIFE:weapon_knifeidle",
"LAPDAN1:lapdan_d",
"LAPDAN1:lapdan_p",
"LAPDAN2:lapdan_d",
"LAPDAN2:lapdan_p",
"LAPDAN3:lapdan_d",
"LAPDAN3:lapdan_p",
"LOWRIDER:f_smklean_loop",
"LOWRIDER:lrgirl_bdbnce",
"LOWRIDER:lrgirl_hair",
"LOWRIDER:lrgirl_hurry",
"LOWRIDER:lrgirl_idleloop",
"LOWRIDER:lrgirl_idle_to_l0",
"LOWRIDER:lrgirl_l0_bnce",
"LOWRIDER:lrgirl_l0_loop",
"LOWRIDER:lrgirl_l0_to_l1",
"LOWRIDER:lrgirl_l12_to_l0",
"LOWRIDER:lrgirl_l1_bnce",
"LOWRIDER:lrgirl_l1_loop",
"LOWRIDER:lrgirl_l1_to_l2",
"LOWRIDER:lrgirl_l2_bnce",
"LOWRIDER:lrgirl_l2_loop",
"LOWRIDER:lrgirl_l2_to_l3",
"LOWRIDER:lrgirl_l345_to_l1",
"LOWRIDER:lrgirl_l3_bnce",
"LOWRIDER:lrgirl_l3_loop",
"LOWRIDER:lrgirl_l3_to_l4",
"LOWRIDER:lrgirl_l4_bnce",
"LOWRIDER:lrgirl_l4_loop",
"LOWRIDER:lrgirl_l4_to_l5",
"LOWRIDER:lrgirl_l5_bnce",
"LOWRIDER:lrgirl_l5_loop",
"LOWRIDER:m_smklean_loop",
"LOWRIDER:m_smkstnd_loop",
"LOWRIDER:prtial_gngtlkb",
"LOWRIDER:prtial_gngtlkc",
"LOWRIDER:prtial_gngtlkd",
"LOWRIDER:prtial_gngtlke",
"LOWRIDER:prtial_gngtlkf",
"LOWRIDER:prtial_gngtlkg",
"LOWRIDER:prtial_gngtlkh",
"LOWRIDER:rap_a_loop",
"LOWRIDER:rap_b_loop",
"LOWRIDER:rap_c_loop",
"LOWRIDER:sit_relaxed",
"LOWRIDER:tap_hand",
"MD_CHASE:carhit_hangon",
"MD_CHASE:carhit_tumble",
"MD_CHASE:donutdrop",
"MD_CHASE:fen_choppa_l1",
"MD_CHASE:fen_choppa_l2",
"MD_CHASE:fen_choppa_l3",
"MD_CHASE:fen_choppa_r1",
"MD_CHASE:fen_choppa_r2",
"MD_CHASE:fen_choppa_r3",
"MD_CHASE:hangon_stun_loop",
"MD_CHASE:hangon_stun_turn",
"MD_CHASE:md_bike_2_hang",
"MD_CHASE:md_bike_jmp_bl",
"MD_CHASE:md_bike_jmp_f",
"MD_CHASE:md_bike_lnd_bl",
"MD_CHASE:md_bike_lnd_die_bl",
"MD_CHASE:md_bike_lnd_die_f",
"MD_CHASE:md_bike_lnd_f",
"MD_CHASE:md_bike_lnd_roll",
"MD_CHASE:md_bike_lnd_roll_f",
"MD_CHASE:md_bike_punch",
"MD_CHASE:md_bike_punch_f",
"MD_CHASE:md_bike_shot_f",
"MD_CHASE:md_hang_lnd_roll",
"MD_CHASE:md_hang_loop",
"MD_END:end_sc1_ply",
"MD_END:end_sc1_ryd",
"MD_END:end_sc1_smo",
"MD_END:end_sc1_swe",
"MD_END:end_sc2_ply",
"MD_END:end_sc2_ryd",
"MD_END:end_sc2_smo",
"MD_END:end_sc2_swe",
"MEDIC:cpr",
"MISC:bitchslap",
"MISC:bmx_celebrate",
"MISC:bmx_comeon",
"MISC:bmx_idleloop_01",
"MISC:bmx_idleloop_02",
"MISC:bmx_talkleft_in",
"MISC:bmx_talkleft_loop",
"MISC:bmx_talkleft_out",
"MISC:bmx_talkright_in",
"MISC:bmx_talkright_loop",
"MISC:bmx_talkright_out",
"MISC:bng_wndw",
"MISC:bng_wndw_02",
"MISC:case_pickup",
"MISC:door_jet",
"MISC:grab_l",
"MISC:grab_r",
"MISC:hiker_pose",
"MISC:hiker_pose_l",
"MISC:idle_chat_02",
"MISC:kat_throw_k",
"MISC:kat_throw_o",
"MISC:kat_throw_p",
"MISC:pass_rifle_o",
"MISC:pass_rifle_ped",
"MISC:pass_rifle_ply",
"MISC:pickup_box",
"MISC:plane_door",
"MISC:plane_exit",
"MISC:plane_hijack",
"MISC:plunger_01",
"MISC:plyrlean_loop",
"MISC:plyr_shkhead",
"MISC:run_dive",
"MISC:scratchballs_01",
"MISC:seat_lr",
"MISC:seat_talk_01",
"MISC:seat_talk_02",
"MISC:seat_watch",
"MISC:smalplane_door",
"MISC:smlplane_door",
"MTB:mtb_back",
"MTB:mtb_bunnyhop",
"MTB:mtb_drivebyft",
"MTB:mtb_driveby_lhs",
"MTB:mtb_driveby_rhs",
"MTB:mtb_fwd",
"MTB:mtb_getoffback",
"MTB:mtb_getofflhs",
"MTB:mtb_getoffrhs",
"MTB:mtb_jumponl",
"MTB:mtb_jumponr",
"MTB:mtb_left",
"MTB:mtb_pedal",
"MTB:mtb_pushes",
"MTB:mtb_ride",
"MTB:mtb_right",
"MTB:mtb_sprint",
"MTB:mtb_still",
"MUSCULAR:msclewalkst_armed",
"MUSCULAR:msclewalkst_csaw",
"MUSCULAR:mscle_rckt_run",
"MUSCULAR:mscle_rckt_walkst",
"MUSCULAR:mscle_run_csaw",
"MUSCULAR:muscleidle",
"MUSCULAR:muscleidle_armed",
"MUSCULAR:muscleidle_csaw",
"MUSCULAR:muscleidle_rocket",
"MUSCULAR:musclerun",
"MUSCULAR:musclerun_armed",
"MUSCULAR:musclesprint",
"MUSCULAR:musclewalk",
"MUSCULAR:musclewalkstart",
"MUSCULAR:musclewalk_armed",
"MUSCULAR:musclewalk_csaw",
"MUSCULAR:musclewalk_rocket",
"NEVADA:nevada_getin",
"NEVADA:nevada_getout",
"ON_LOOKERS:lkaround_in",
"ON_LOOKERS:lkaround_loop",
"ON_LOOKERS:lkaround_out",
"ON_LOOKERS:lkup_in",
"ON_LOOKERS:lkup_loop",
"ON_LOOKERS:lkup_out",
"ON_LOOKERS:lkup_point",
"ON_LOOKERS:panic_cower",
"ON_LOOKERS:panic_hide",
"ON_LOOKERS:panic_in",
"ON_LOOKERS:panic_loop",
"ON_LOOKERS:panic_out",
"ON_LOOKERS:panic_point",
"ON_LOOKERS:panic_shout",
"ON_LOOKERS:pointup_in",
"ON_LOOKERS:pointup_loop",
"ON_LOOKERS:pointup_out",
"ON_LOOKERS:pointup_shout",
"ON_LOOKERS:point_in",
"ON_LOOKERS:point_loop",
"ON_LOOKERS:point_out",
"ON_LOOKERS:shout_01",
"ON_LOOKERS:shout_02",
"ON_LOOKERS:shout_in",
"ON_LOOKERS:shout_loop",
"ON_LOOKERS:shout_out",
"ON_LOOKERS:wave_in",
"ON_LOOKERS:wave_loop",
"ON_LOOKERS:wave_out",
"OTB:betslp_in",
"OTB:betslp_lkabt",
"OTB:betslp_loop",
"OTB:betslp_out",
"OTB:betslp_tnk",
"OTB:wtchrace_cmon",
"OTB:wtchrace_in",
"OTB:wtchrace_loop",
"OTB:wtchrace_lose",
"OTB:wtchrace_out",
"OTB:wtchrace_win",
"PARACHUTE:fall_skydive",
"PARACHUTE:fall_skydive_accel",
"PARACHUTE:fall_skydive_die",
"PARACHUTE:fall_skydive_l",
"PARACHUTE:fall_skydive_r",
"PARACHUTE:para_decel",
"PARACHUTE:para_decel_o",
"PARACHUTE:para_float",
"PARACHUTE:para_float_o",
"PARACHUTE:para_land",
"PARACHUTE:para_land_o",
"PARACHUTE:para_land_water",
"PARACHUTE:para_land_water_o",
"PARACHUTE:para_open",
"PARACHUTE:para_open_o",
"PARACHUTE:para_rip_land_o",
"PARACHUTE:para_rip_loop_o",
"PARACHUTE:para_rip_o",
"PARACHUTE:para_steerl",
"PARACHUTE:para_steerl_o",
"PARACHUTE:para_steerr",
"PARACHUTE:para_steerr_o",
"PARK:tai_chi_in",
"PARK:tai_chi_loop",
"PARK:tai_chi_out",
"PAULNMAC:piss_in",
"PAULNMAC:piss_loop",
"PAULNMAC:piss_out",
"PAULNMAC:pnm_argue1_a",
"PAULNMAC:pnm_argue1_b",
"PAULNMAC:pnm_argue2_a",
"PAULNMAC:pnm_argue2_b",
"PAULNMAC:pnm_loop_a",
"PAULNMAC:pnm_loop_b",
"PAULNMAC:wank_in",
"PAULNMAC:wank_loop",
"PAULNMAC:wank_out",
"PED:abseil",
"PED:arrestgun",
"PED:atm",
"PED:bike_elbowl",
"PED:bike_elbowr",
"PED:bike_fallr",
"PED:bike_fall_off",
"PED:bike_pickupl",
"PED:bike_pickupr",
"PED:bike_pullupl",
"PED:bike_pullupr",
"PED:bomber",
"PED:car_alignhi_lhs",
"PED:car_alignhi_rhs",
"PED:car_align_lhs",
"PED:car_align_rhs",
"PED:car_closedoorl_lhs",
"PED:car_closedoorl_rhs",
"PED:car_closedoor_lhs",
"PED:car_closedoor_rhs",
"PED:car_close_lhs",
"PED:car_close_rhs",
"PED:car_crawloutrhs",
"PED:car_dead_lhs",
"PED:car_dead_rhs",
"PED:car_doorlocked_lhs",
"PED:car_doorlocked_rhs",
"PED:car_fallout_lhs",
"PED:car_fallout_rhs",
"PED:car_getinl_lhs",
"PED:car_getinl_rhs",
"PED:car_getin_lhs",
"PED:car_getin_rhs",
"PED:car_getoutl_lhs",
"PED:car_getoutl_rhs",
"PED:car_getout_lhs",
"PED:car_getout_rhs",
"PED:car_hookertalk",
"PED:car_jackedlhs",
"PED:car_jackedrhs",
"PED:car_jumpin_lhs",
"PED:car_lb",
"PED:car_lb_pro",
"PED:car_lb_weak",
"PED:car_ljackedlhs",
"PED:car_ljackedrhs",
"PED:car_lshuffle_rhs",
"PED:car_lsit",
"PED:car_open_lhs",
"PED:car_open_rhs",
"PED:car_pulloutl_lhs",
"PED:car_pulloutl_rhs",
"PED:car_pullout_lhs",
"PED:car_pullout_rhs",
"PED:car_qjacked",
"PED:car_rolldoor",
"PED:car_rolldoorlo",
"PED:car_rollout_lhs",
"PED:car_rollout_rhs",
"PED:car_shuffle_rhs",
"PED:car_sit",
"PED:car_sitp",
"PED:car_sitplo",
"PED:car_sit_pro",
"PED:car_sit_weak",
"PED:car_tune_radio",
"PED:climb_idle",
"PED:climb_jump",
"PED:climb_jump2fall",
"PED:climb_jump_b",
"PED:climb_pull",
"PED:climb_stand",
"PED:climb_stand_finish",
"PED:cower",
"PED:crouch_roll_l",
"PED:crouch_roll_r",
"PED:dam_arml_frmbk",
"PED:dam_arml_frmft",
"PED:dam_arml_frmlt",
"PED:dam_armr_frmbk",
"PED:dam_armr_frmft",
"PED:dam_armr_frmrt",
"PED:dam_legl_frmbk",
"PED:dam_legl_frmft",
"PED:dam_legl_frmlt",
"PED:dam_legr_frmbk",
"PED:dam_legr_frmft",
"PED:dam_legr_frmrt",
"PED:dam_stomach_frmbk",
"PED:dam_stomach_frmft",
"PED:dam_stomach_frmlt",
"PED:dam_stomach_frmrt",
"PED:door_lhinge_o",
"PED:door_rhinge_o",
"PED:drivebyl_l",
"PED:drivebyl_r",
"PED:driveby_l",
"PED:driveby_r",
"PED:drive_boat",
"PED:drive_boat_back",
"PED:drive_boat_l",
"PED:drive_boat_r",
"PED:drive_l",
"PED:drive_lo_l",
"PED:drive_lo_r",
"PED:drive_l_pro",
"PED:drive_l_pro_slow",
"PED:drive_l_slow",
"PED:drive_l_weak",
"PED:drive_l_weak_slow",
"PED:drive_r",
"PED:drive_r_pro",
"PED:drive_r_pro_slow",
"PED:drive_r_slow",
"PED:drive_r_weak",
"PED:drive_r_weak_slow",
"PED:drive_truck",
"PED:drive_truck_back",
"PED:drive_truck_l",
"PED:drive_truck_r",
"PED:drown",
"PED:duck_cower",
"PED:endchat_01",
"PED:endchat_02",
"PED:endchat_03",
"PED:ev_dive",
"PED:ev_step",
"PED:facanger",
"PED:facgum",
"PED:facsurp",
"PED:facsurpm",
"PED:factalk",
"PED:facurios",
"PED:fall_back",
"PED:fall_collapse",
"PED:fall_fall",
"PED:fall_front",
"PED:fall_glide",
"PED:fall_land",
"PED:fall_skydive",
"PED:fight2idle",
"PED:fighta_1",
"PED:fighta_2",
"PED:fighta_3",
"PED:fighta_block",
"PED:fighta_g",
"PED:fighta_m",
"PED:fightidle",
"PED:fightshb",
"PED:fightshf",
"PED:fightsh_bwd",
"PED:fightsh_fwd",
"PED:fightsh_left",
"PED:fightsh_right",
"PED:flee_lkaround_01",
"PED:floor_hit",
"PED:floor_hit_f",
"PED:fucku",
"PED:gang_gunstand",
"PED:gas_cwr",
"PED:getup",
"PED:getup_front",
"PED:gum_eat",
"PED:guncrouchbwd",
"PED:guncrouchfwd",
"PED:gunmove_bwd",
"PED:gunmove_fwd",
"PED:gunmove_l",
"PED:gunmove_r",
"PED:gun_2_idle",
"PED:gun_butt",
"PED:gun_butt_crouch",
"PED:gun_stand",
"PED:handscower",
"PED:handsup",
"PED:hita_1",
"PED:hita_2",
"PED:hita_3",
"PED:hit_back",
"PED:hit_behind",
"PED:hit_front",
"PED:hit_gun_butt",
"PED:hit_l",
"PED:hit_r",
"PED:hit_walk",
"PED:hit_wall",
"PED:idlestance_fat",
"PED:idlestance_old",
"PED:idle_armed",
"PED:idle_chat",
"PED:idle_csaw",
"PED:idle_gang1",
"PED:idle_hbhb",
"PED:idle_rocket",
"PED:idle_stance",
"PED:idle_taxi",
"PED:idle_tired",
"PED:jetpack_idle",
"PED:jog_femalea",
"PED:jog_malea",
"PED:jump_glide",
"PED:jump_land",
"PED:jump_launch",
"PED:jump_launch_r",
"PED:kart_drive",
"PED:kart_l",
"PED:kart_lb",
"PED:kart_r",
"PED:kd_left",
"PED:kd_right",
"PED:ko_shot_face",
"PED:ko_shot_front",
"PED:ko_shot_stom",
"PED:ko_skid_back",
"PED:ko_skid_front",
"PED:ko_spin_l",
"PED:ko_spin_r",
"PED:pass_smoke_in_car",
"PED:phone_in",
"PED:phone_out",
"PED:phone_talk",
"PED:player_sneak",
"PED:player_sneak_walkstart",
"PED:roadcross",
"PED:roadcross_female",
"PED:roadcross_gang",
"PED:roadcross_old",
"PED:run_1armed",
"PED:run_armed",
"PED:run_civi",
"PED:run_csaw",
"PED:run_fat",
"PED:run_fatold",
"PED:run_gang1",
"PED:run_left",
"PED:run_old",
"PED:run_player",
"PED:run_right",
"PED:run_rocket",
"PED:run_stop",
"PED:run_stopr",
"PED:run_wuzi",
"PED:seat_down",
"PED:seat_idle",
"PED:seat_up",
"PED:shot_leftp",
"PED:shot_partial",
"PED:shot_partial_b",
"PED:shot_rightp",
"PED:shove_partial",
"PED:smoke_in_car",
"PED:sprint_civi",
"PED:sprint_panic",
"PED:sprint_wuzi",
"PED:swat_run",
"PED:swim_tread",
"PED:tap_hand",
"PED:tap_handp",
"PED:turn_180",
"PED:turn_l",
"PED:turn_r",
"PED:walk_armed",
"PED:walk_civi",
"PED:walk_csaw",
"PED:walk_doorpartial",
"PED:walk_drunk",
"PED:walk_fat",
"PED:walk_fatold",
"PED:walk_gang1",
"PED:walk_gang2",
"PED:walk_old",
"PED:walk_player",
"PED:walk_rocket",
"PED:walk_shuffle",
"PED:walk_start",
"PED:walk_start_armed",
"PED:walk_start_csaw",
"PED:walk_start_rocket",
"PED:walk_wuzi",
"PED:weapon_crouch",
"PED:woman_idlestance",
"PED:woman_run",
"PED:woman_runbusy",
"PED:woman_runfatold",
"PED:woman_runpanic",
"PED:woman_runsexy",
"PED:woman_walkbusy",
"PED:woman_walkfatold",
"PED:woman_walknorm",
"PED:woman_walkold",
"PED:woman_walkpro",
"PED:woman_walksexy",
"PED:woman_walkshop",
"PED:xpressscratch",
"PLAYER_DVBYS:plyr_drivebybwd",
"PLAYER_DVBYS:plyr_drivebyfwd",
"PLAYER_DVBYS:plyr_drivebylhs",
"PLAYER_DVBYS:plyr_drivebyrhs",
"PLAYIDLES:shift",
"PLAYIDLES:shldr",
"PLAYIDLES:stretch",
"PLAYIDLES:strleg",
"PLAYIDLES:time",
"POLICE:coptraf_away",
"POLICE:coptraf_come",
"POLICE:coptraf_left",
"POLICE:coptraf_stop",
"POLICE:cop_getoutcar_lhs",
"POLICE:cop_move_fwd",
"POLICE:crm_drgbst_01",
"POLICE:door_kick",
"POLICE:plc_drgbst_01",
"POLICE:plc_drgbst_02",
"POOL:pool_chalkcue",
"POOL:pool_idle_stance",
"POOL:pool_long_shot",
"POOL:pool_long_shot_o",
"POOL:pool_long_start",
"POOL:pool_long_start_o",
"POOL:pool_med_shot",
"POOL:pool_med_shot_o",
"POOL:pool_med_start",
"POOL:pool_med_start_o",
"POOL:pool_place_white",
"POOL:pool_short_shot",
"POOL:pool_short_shot_o",
"POOL:pool_short_start",
"POOL:pool_short_start_o",
"POOL:pool_walk",
"POOL:pool_walk_start",
"POOL:pool_xlong_shot",
"POOL:pool_xlong_shot_o",
"POOL:pool_xlong_start",
"POOL:pool_xlong_start_o",
"POOR:winwash_start",
"POOR:winwash_wash2beg",
"PYTHON:python_crouchfire",
"PYTHON:python_crouchreload",
"PYTHON:python_fire",
"PYTHON:python_fire_poor",
"PYTHON:python_reload",
"QUAD:quad_back",
"QUAD:quad_driveby_ft",
"QUAD:quad_driveby_lhs",
"QUAD:quad_driveby_rhs",
"QUAD:quad_fwd",
"QUAD:quad_getoff_b",
"QUAD:quad_getoff_lhs",
"QUAD:quad_getoff_rhs",
"QUAD:quad_geton_lhs",
"QUAD:quad_geton_rhs",
"QUAD:quad_hit",
"QUAD:quad_kick",
"QUAD:quad_left",
"QUAD:quad_passenger",
"QUAD:quad_reverse",
"QUAD:quad_ride",
"QUAD:quad_right",
"QUAD_DBZ:pass_driveby_bwd",
"QUAD_DBZ:pass_driveby_fwd",
"QUAD_DBZ:pass_driveby_lhs",
"QUAD_DBZ:pass_driveby_rhs",
"RAPPING:laugh_01",
"RAPPING:rap_a_in",
"RAPPING:rap_a_loop",
"RAPPING:rap_a_out",
"RAPPING:rap_b_in",
"RAPPING:rap_b_loop",
"RAPPING:rap_b_out",
"RAPPING:rap_c_loop",
"RIFLE:rifle_crouchfire",
"RIFLE:rifle_crouchload",
"RIFLE:rifle_fire",
"RIFLE:rifle_fire_poor",
"RIFLE:rifle_load",
"RIOT:riot_angry",
"RIOT:riot_angry_b",
"RIOT:riot_challenge",
"RIOT:riot_chant",
"RIOT:riot_fuku",
"RIOT:riot_punches",
"RIOT:riot_shout",
"ROB_BANK:cat_safe_end",
"ROB_BANK:cat_safe_open",
"ROB_BANK:cat_safe_open_o",
"ROB_BANK:cat_safe_rob",
"ROB_BANK:shp_handsup_scr",
"ROCKET:idle_rocket",
"ROCKET:rocketfire",
"ROCKET:run_rocket",
"ROCKET:walk_rocket",
"ROCKET:walk_start_rocket",
"RUSTLER:plane_align_lhs",
"RUSTLER:plane_close",
"RUSTLER:plane_getin",
"RUSTLER:plane_getout",
"RUSTLER:plane_open",
"RYDER:ryd_beckon_01",
"RYDER:ryd_beckon_02",
"RYDER:ryd_beckon_03",
"RYDER:ryd_die_pt1",
"RYDER:ryd_die_pt2",
"RYDER:van_crate_l",
"RYDER:van_crate_r",
"RYDER:van_fall_l",
"RYDER:van_fall_r",
"RYDER:van_lean_l",
"RYDER:van_lean_r",
"RYDER:van_pickup_e",
"RYDER:van_pickup_s",
"RYDER:van_stand",
"RYDER:van_stand_crate",
"RYDER:van_throw",
"SCRATCHING:scdldlp",
"SCRATCHING:scdlulp",
"SCRATCHING:scdrdlp",
"SCRATCHING:scdrulp",
"SCRATCHING:sclng_l",
"SCRATCHING:sclng_r",
"SCRATCHING:scmid_l",
"SCRATCHING:scmid_r",
"SCRATCHING:scshrtl",
"SCRATCHING:scshrtr",
"SCRATCHING:sc_ltor",
"SCRATCHING:sc_rtol",
"SHAMAL:shamal_align",
"SHAMAL:shamal_getin_lhs",
"SHAMAL:shamal_getout_lhs",
"SHAMAL:shamal_open",
"SHOP:rob_2idle",
"SHOP:rob_loop",
"SHOP:rob_loop_threat",
"SHOP:rob_shifty",
"SHOP:rob_stickup_in",
"SHOP:shp_duck",
"SHOP:shp_duck_aim",
"SHOP:shp_duck_fire",
"SHOP:shp_gun_aim",
"SHOP:shp_gun_duck",
"SHOP:shp_gun_fire",
"SHOP:shp_gun_grab",
"SHOP:shp_gun_threat",
"SHOP:shp_handsup_scr",
"SHOP:shp_jump_glide",
"SHOP:shp_jump_land",
"SHOP:shp_jump_launch",
"SHOP:shp_rob_givecash",
"SHOP:shp_rob_handsup",
"SHOP:shp_rob_react",
"SHOP:shp_serve_end",
"SHOP:shp_serve_idle",
"SHOP:shp_serve_loop",
"SHOP:shp_serve_start",
"SHOP:smoke_ryd",
"SHOTGUN:shotgun_crouchfire",
"SHOTGUN:shotgun_fire",
"SHOTGUN:shotgun_fire_poor",
"SILENCED:crouchreload",
"SILENCED:silencecrouchfire",
"SILENCED:silence_fire",
"SILENCED:silence_reload",
"SKATE:skate_idle",
"SKATE:skate_run",
"SKATE:skate_sprint",
"SMOKING:f_smklean_loop",
"SMOKING:m_smklean_loop",
"SMOKING:m_smkstnd_loop",
"SMOKING:m_smk_drag",
"SMOKING:m_smk_in",
"SMOKING:m_smk_loop",
"SMOKING:m_smk_out",
"SMOKING:m_smk_tap",
"SNIPER:weapon_sniper",
"SPRAYCAN:spraycan_fire",
"SPRAYCAN:spraycan_full",
"STRIP:ply_cash",
"STRIP:pun_cash",
"STRIP:pun_holler",
"STRIP:pun_loop",
"STRIP:strip_a",
"STRIP:strip_b",
"STRIP:strip_c",
"STRIP:strip_d",
"STRIP:strip_e",
"STRIP:strip_f",
"STRIP:strip_g",
"STRIP:str_a2b",
"STRIP:str_b2a",
"STRIP:str_b2c",
"STRIP:str_c1",
"STRIP:str_c2",
"STRIP:str_c2b",
"STRIP:str_loop_a",
"STRIP:str_loop_b",
"STRIP:str_loop_c",
"SUNBATHE:batherdown",
"SUNBATHE:batherup",
"SUNBATHE:lay_bac_in",
"SUNBATHE:lay_bac_out",
"SUNBATHE:parksit_m_idlea",
"SUNBATHE:parksit_m_idleb",
"SUNBATHE:parksit_m_idlec",
"SUNBATHE:parksit_m_in",
"SUNBATHE:parksit_m_out",
"SUNBATHE:parksit_w_idlea",
"SUNBATHE:parksit_w_idleb",
"SUNBATHE:parksit_w_idlec",
"SUNBATHE:parksit_w_in",
"SUNBATHE:parksit_w_out",
"SUNBATHE:sbathe_f_lieb2sit",
"SUNBATHE:sbathe_f_out",
"SUNBATHE:sitnwait_in_w",
"SUNBATHE:sitnwait_out_w",
"SWAT:gnstwall_injurd",
"SWAT:jmp_wall1m_180",
"SWAT:rail_fall",
"SWAT:rail_fall_crawl",
"SWAT:swt_breach_01",
"SWAT:swt_breach_02",
"SWAT:swt_breach_03",
"SWAT:swt_go",
"SWAT:swt_lkt",
"SWAT:swt_sty",
"SWAT:swt_vent_01",
"SWAT:swt_vent_02",
"SWAT:swt_vnt_sht_die",
"SWAT:swt_vnt_sht_in",
"SWAT:swt_vnt_sht_loop",
"SWAT:swt_wllpk_l",
"SWAT:swt_wllpk_l_back",
"SWAT:swt_wllpk_r",
"SWAT:swt_wllpk_r_back",
"SWAT:swt_wllshoot_in_l",
"SWAT:swt_wllshoot_in_r",
"SWAT:swt_wllshoot_out_l",
"SWAT:swt_wllshoot_out_r",
"SWEET:ho_ass_slapped",
"SWEET:lafin_player",
"SWEET:lafin_sweet",
"SWEET:plyr_hndshldr_01",
"SWEET:sweet_ass_slap",
"SWEET:sweet_hndshldr_01",
"SWEET:sweet_injuredloop",
"SWIM:swim_breast",
"SWIM:swim_crawl",
"SWIM:swim_dive_under",
"SWIM:swim_glide",
"SWIM:swim_jumpout",
"SWIM:swim_tread",
"SWIM:swim_under",
"SWORD:sword_1",
"SWORD:sword_2",
"SWORD:sword_3",
"SWORD:sword_4",
"SWORD:sword_block",
"SWORD:sword_hit_1",
"SWORD:sword_hit_2",
"SWORD:sword_hit_3",
"SWORD:sword_idle",
"SWORD:sword_part",
"TANK:tank_align_lhs",
"TANK:tank_close_lhs",
"TANK:tank_doorlocked",
"TANK:tank_getin_lhs",
"TANK:tank_getout_lhs",
"TANK:tank_open_lhs",
"TATTOOS:tat_arml_in_o",
"TATTOOS:tat_arml_in_p",
"TATTOOS:tat_arml_in_t",
"TATTOOS:tat_arml_out_o",
"TATTOOS:tat_arml_out_p",
"TATTOOS:tat_arml_out_t",
"TATTOOS:tat_arml_pose_o",
"TATTOOS:tat_arml_pose_p",
"TATTOOS:tat_arml_pose_t",
"TATTOOS:tat_armr_in_o",
"TATTOOS:tat_armr_in_p",
"TATTOOS:tat_armr_in_t",
"TATTOOS:tat_armr_out_o",
"TATTOOS:tat_armr_out_p",
"TATTOOS:tat_armr_out_t",
"TATTOOS:tat_armr_pose_o",
"TATTOOS:tat_armr_pose_p",
"TATTOOS:tat_armr_pose_t",
"TATTOOS:tat_back_in_o",
"TATTOOS:tat_back_in_p",
"TATTOOS:tat_back_in_t",
"TATTOOS:tat_back_out_o",
"TATTOOS:tat_back_out_p",
"TATTOOS:tat_back_out_t",
"TATTOOS:tat_back_pose_o",
"TATTOOS:tat_back_pose_p",
"TATTOOS:tat_back_pose_t",
"TATTOOS:tat_back_sit_in_p",
"TATTOOS:tat_back_sit_loop_p",
"TATTOOS:tat_back_sit_out_p",
"TATTOOS:tat_bel_in_o",
"TATTOOS:tat_bel_in_t",
"TATTOOS:tat_bel_out_o",
"TATTOOS:tat_bel_out_t",
"TATTOOS:tat_bel_pose_o",
"TATTOOS:tat_bel_pose_t",
"TATTOOS:tat_che_in_o",
"TATTOOS:tat_che_in_p",
"TATTOOS:tat_che_in_t",
"TATTOOS:tat_che_out_o",
"TATTOOS:tat_che_out_p",
"TATTOOS:tat_che_out_t",
"TATTOOS:tat_che_pose_o",
"TATTOOS:tat_che_pose_p",
"TATTOOS:tat_che_pose_t",
"TATTOOS:tat_drop_o",
"TATTOOS:tat_idle_loop_o",
"TATTOOS:tat_idle_loop_t",
"TATTOOS:tat_sit_in_o",
"TATTOOS:tat_sit_in_p",
"TATTOOS:tat_sit_in_t",
"TATTOOS:tat_sit_loop_o",
"TATTOOS:tat_sit_loop_p",
"TATTOOS:tat_sit_loop_t",
"TATTOOS:tat_sit_out_o",
"TATTOOS:tat_sit_out_p",
"TATTOOS:tat_sit_out_t",
"TEC:tec_crouchfire",
"TEC:tec_crouchreload",
"TEC:tec_fire",
"TEC:tec_reload",
"TRAIN:tran_gtup",
"TRAIN:tran_hng",
"TRAIN:tran_ouch",
"TRAIN:tran_stmb",
"TRUCK:truck_align_lhs",
"TRUCK:truck_align_rhs",
"TRUCK:truck_closedoor_lhs",
"TRUCK:truck_closedoor_rhs",
"TRUCK:truck_close_lhs",
"TRUCK:truck_close_rhs",
"TRUCK:truck_getin_lhs",
"TRUCK:truck_getin_rhs",
"TRUCK:truck_getout_lhs",
"TRUCK:truck_getout_rhs",
"TRUCK:truck_jackedlhs",
"TRUCK:truck_jackedrhs",
"TRUCK:truck_open_lhs",
"TRUCK:truck_open_rhs",
"TRUCK:truck_pullout_lhs",
"TRUCK:truck_pullout_rhs",
"TRUCK:truck_shuffle",
"UZI:uzi_crouchfire",
"UZI:uzi_crouchreload",
"UZI:uzi_fire",
"UZI:uzi_fire_poor",
"UZI:uzi_reload",
"VAN:van_close_back_lhs",
"VAN:van_close_back_rhs",
"VAN:van_getin_back_lhs",
"VAN:van_getin_back_rhs",
"VAN:van_getout_back_lhs",
"VAN:van_getout_back_rhs",
"VAN:van_open_back_lhs",
"VAN:van_open_back_rhs",
"VENDING:vend_drink2_p",
"VENDING:vend_drink_p",
"VENDING:vend_eat1_p",
"VENDING:vend_eat_p",
"VENDING:vend_use",
"VENDING:vend_use_pt2",
"VORTEX:car_jumpin_lhs",
"VORTEX:car_jumpin_rhs",
"VORTEX:vortex_getout_lhs",
"VORTEX:vortex_getout_rhs",
"WAYFARER:wf_back",
"WAYFARER:wf_drivebyft",
"WAYFARER:wf_drivebylhs",
"WAYFARER:wf_drivebyrhs",
"WAYFARER:wf_fwd",
"WAYFARER:wf_getoffback",
"WAYFARER:wf_getofflhs",
"WAYFARER:wf_getoffrhs",
"WAYFARER:wf_hit",
"WAYFARER:wf_jumponl",
"WAYFARER:wf_jumponr",
"WAYFARER:wf_kick",
"WAYFARER:wf_left",
"WAYFARER:wf_passenger",
"WAYFARER:wf_pushes",
"WAYFARER:wf_ride",
"WAYFARER:wf_right",
"WAYFARER:wf_still",
"WEAPONS:shp_1h_lift",
"WEAPONS:shp_1h_lift_end",
"WEAPONS:shp_1h_ret",
"WEAPONS:shp_1h_ret_s",
"WEAPONS:shp_2h_lift",
"WEAPONS:shp_2h_lift_end",
"WEAPONS:shp_2h_ret",
"WEAPONS:shp_2h_ret_s",
"WEAPONS:shp_ar_lift",
"WEAPONS:shp_ar_lift_end",
"WEAPONS:shp_ar_ret",
"WEAPONS:shp_ar_ret_s",
"WEAPONS:shp_g_lift_in",
"WEAPONS:shp_g_lift_out",
"WEAPONS:shp_tray_in",
"WEAPONS:shp_tray_out",
"WEAPONS:shp_tray_pose",
"WUZI:cs_dead_guy",
"WUZI:cs_plyr_pt1",
"WUZI:cs_plyr_pt2",
"WUZI:cs_wuzi_pt1",
"WUZI:cs_wuzi_pt2",
"WUZI:walkstart_idle_01",
"WUZI:wuzi_follow",
"WUZI:wuzi_greet_plyr",
"WUZI:wuzi_greet_wuzi",
"WUZI:wuzi_grnd_chk",
"WUZI:wuzi_stand_loop",
"WUZI:wuzi_walk",
"WOP:DANCE_G1",
"WOP:DANCE_G2",
"WOP:DANCE_G3",
"WOP:DANCE_G4",
"WOP:DANCE_G5",
"WOP:DANCE_G6",
"WOP:DANCE_G7",
"WOP:DANCE_G8",
"WOP:DANCE_G9",
"WOP:DANCE_G10",
"WOP:DANCE_G11",
"WOP:DANCE_G12",
"WOP:DANCE_G13",
"WOP:DANCE_G14",
"WOP:DANCE_G15",
"WOP:DANCE_G16",
"WOP:DANCE_B1",
"WOP:DANCE_B2",
"WOP:DANCE_B3",
"WOP:DANCE_B4",
"WOP:DANCE_B5",
"WOP:DANCE_B6",
"WOP:DANCE_B7",
"WOP:DANCE_B8",
"WOP:DANCE_B9",
"WOP:DANCE_B10",
"WOP:DANCE_B11",
"WOP:DANCE_B12",
"WOP:DANCE_B13",
"WOP:DANCE_B14",
"WOP:DANCE_B15",
"WOP:DANCE_B16",
"WOP:DANCE_LOOP",
"GFUNK:DANCE_G1",
"GFUNK:DANCE_G2",
"GFUNK:DANCE_G3",
"GFUNK:DANCE_G4",
"GFUNK:DANCE_G5",
"GFUNK:DANCE_G6",
"GFUNK:DANCE_G7",
"GFUNK:DANCE_G8",
"GFUNK:DANCE_G9",
"GFUNK:DANCE_G10",
"GFUNK:DANCE_G11",
"GFUNK:DANCE_G12",
"GFUNK:DANCE_G13",
"GFUNK:DANCE_G14",
"GFUNK:DANCE_G15",
"GFUNK:DANCE_G16",
"GFUNK:DANCE_B1",
"GFUNK:DANCE_B2",
"GFUNK:DANCE_B3",
"GFUNK:DANCE_B4",
"GFUNK:DANCE_B5",
"GFUNK:DANCE_B6",
"GFUNK:DANCE_B7",
"GFUNK:DANCE_B8",
"GFUNK:DANCE_B9",
"GFUNK:DANCE_B10",
"GFUNK:DANCE_B11",
"GFUNK:DANCE_B12",
"GFUNK:DANCE_B13",
"GFUNK:DANCE_B14",
"GFUNK:DANCE_B15",
"GFUNK:DANCE_B16",
"GFUNK:DANCE_LOOP",

"RUNNINGMAN:DANCE_G1",
"RUNNINGMAN:DANCE_G2",
"RUNNINGMAN:DANCE_G3",
"RUNNINGMAN:DANCE_G4",
"RUNNINGMAN:DANCE_G5",
"RUNNINGMAN:DANCE_G6",
"RUNNINGMAN:DANCE_G7",
"RUNNINGMAN:DANCE_G8",
"RUNNINGMAN:DANCE_G9",
"RUNNINGMAN:DANCE_G10",
"RUNNINGMAN:DANCE_G11",
"RUNNINGMAN:DANCE_G12",
"RUNNINGMAN:DANCE_G13",
"RUNNINGMAN:DANCE_G14",
"RUNNINGMAN:DANCE_G15",
"RUNNINGMAN:DANCE_G16",
"RUNNINGMAN:DANCE_B1",
"RUNNINGMAN:DANCE_B2",
"RUNNINGMAN:DANCE_B3",
"RUNNINGMAN:DANCE_B4",
"RUNNINGMAN:DANCE_B5",
"RUNNINGMAN:DANCE_B6",
"RUNNINGMAN:DANCE_B7",
"RUNNINGMAN:DANCE_B8",
"RUNNINGMAN:DANCE_B9",
"RUNNINGMAN:DANCE_B10",
"RUNNINGMAN:DANCE_B11",
"RUNNINGMAN:DANCE_B12",
"RUNNINGMAN:DANCE_B13",
"RUNNINGMAN:DANCE_B14",
"RUNNINGMAN:DANCE_B15",
"RUNNINGMAN:DANCE_B16",
"RUNNINGMAN:DANCE_LOOP",

"CDANCESTA:DANCE_G1",
"CDANCESTA:DANCE_G2",
"CDANCESTA:DANCE_G3",
"CDANCESTA:DANCE_G4",
"CDANCESTA:DANCE_G5",
"CDANCESTA:DANCE_G6",
"CDANCESTA:DANCE_G7",
"CDANCESTA:DANCE_G8",
"CDANCESTA:DANCE_G9",
"CDANCESTA:DANCE_G10",
"CDANCESTA:DANCE_G11",
"CDANCESTA:DANCE_G12",
"CDANCESTA:DANCE_G13",
"CDANCESTA:DANCE_G14",
"CDANCESTA:DANCE_G15",
"CDANCESTA:DANCE_G16",
"CDANCESTA:DANCE_B1",
"CDANCESTA:DANCE_B2",
"CDANCESTA:DANCE_B3",
"CDANCESTA:DANCE_B4",
"CDANCESTA:DANCE_B5",
"CDANCESTA:DANCE_B6",
"CDANCESTA:DANCE_B7",
"CDANCESTA:DANCE_B8",
"CDANCESTA:DANCE_B9",
"CDANCESTA:DANCE_B10",
"CDANCESTA:DANCE_B11",
"CDANCESTA:DANCE_B12",
"CDANCESTA:DANCE_B13",
"CDANCESTA:DANCE_B14",
"CDANCESTA:DANCE_B15",
"CDANCESTA:DANCE_B16",
"SAMP:FISHINGIDLE"

};

#define PI 3.14159265

extern char* WORLD_PLAYERS;

PED_TYPE* GamePool_FindPlayerPed()
{
	return *(PED_TYPE**)WORLD_PLAYERS;
}

PED_TYPE* GamePool_Ped_GetAt(int iID)
{
	// GetPoolPed
	return ((PED_TYPE* (*)(int))(g_libGTASA + /*0x41DD7C*/0x483DB8 + 1))(iID);
}

int GamePool_Ped_GetIndex(PED_TYPE* pActor)
{
	// GettPoolPedRef
	return ((int (*)(PED_TYPE*))(g_libGTASA + 0x483DAA + 1))(pActor);
}

ENTITY_TYPE *GamePool_Object_GetAt(int iID)
{
	// GetPoolObj
	return ((ENTITY_TYPE* (*)(int))(g_libGTASA + /*0x41DDB4*/0x483DD2 + 1))(iID);
}

uintptr_t GamePool_Vehicle_GetIndex(VEHICLE_TYPE* pGtaVehicle)
{
	// GettPoolVehicleRef
	return ((uintptr_t (*)(VEHICLE_TYPE*))(g_libGTASA + 0x483D90 + 1))(pGtaVehicle);
}

VEHICLE_TYPE* GamePool_Vehicle_GetAt(int iID)
{
	// GetPoolVehicle
	return ((VEHICLE_TYPE* (*)(int))(g_libGTASA + 0x483D9E + 1))(iID);
}

// 0.3.7
int GetVehicleSubtype(VEHICLE_TYPE* pGtaVehicle)
{
	if (pGtaVehicle)
	{
		uintptr_t vtable = pGtaVehicle->entity.vtable;

		if (vtable == (g_libGTASA + 0x0066D678)) {		// CAutomobile
			return VEHICLE_SUBTYPE_CAR;
		}
		else if (vtable == (g_libGTASA + 0x0066D7F0)) {	// CBike
			return VEHICLE_SUBTYPE_BIKE;
		}
		else if (vtable == (g_libGTASA + 0x0066DB34)) {	// CHeli
			return VEHICLE_SUBTYPE_HELI;
		}
		else if (vtable == (g_libGTASA + 0x0066DA20)) {	// CBoat
			return VEHICLE_SUBTYPE_BOAT;
		}
		else if (vtable == (g_libGTASA + 0x0066DD84)) {	// CPlane
			return VEHICLE_SUBTYPE_PLANE;
		}
		else if (vtable == (g_libGTASA + 0x0066D908)) {	// CBmx
			return VEHICLE_SUBTYPE_PUSHBIKE;
		}
		else if (vtable == (g_libGTASA + 0x0066E0FC)) {	// CTrain
			return VEHICLE_SUBTYPE_TRAIN;
		}
	}

	return 0;
}

uintptr_t GetModelInfoByID(int iModelID)
{
	if (iModelID < 0 || iModelID > 20000) {
		return false;
	}

	uintptr_t *dwModelArray = (uintptr_t*)(g_libGTASA + /*0x87BF48*/0x91DCB8);
	return dwModelArray[iModelID];
}

bool IsExistInfoForModel(int iModelID)
{
	return GetModelInfoByID(iModelID) != 0;
}

bool IsValidModel(int iModelID)
{
	uintptr_t pModel = GetModelInfoByID(iModelID);

	if (pModel && *(uintptr_t*)pModel != 0xFFFFFFEC) {
		return true;
	}

	return false;
}
// 0.3.7
int GetModelRefCounts(int iModel)
{
	uint16_t* p = (uint16_t*)(GetModelInfoByID(iModel) + 30);
	return *p;
}
// 0.3.7
bool IsValidPedModel(uint modelID)
{
	if (modelID >= 0 && modelID <= 30000)
	{
		uintptr_t modelInfo = GetModelInfoByID(modelID);
		if (modelInfo)
		{
			if ( (*(uintptr_t*)modelInfo) == (g_libGTASA + 0x667658))
				return true;
		}
	}

	return false;
}

uintptr_t GetModelRWObject(uint uiModel)
{
	uintptr_t modelInfo = GetModelInfoByID(uiModel);
	return *(uintptr_t*)(modelInfo + 0x34); /* pRWObject */
}
/*
uintptr_t LoadTexture(const char* texname)
{
	static char* texdb[] = { "samp", "gta3", "gta_int", "player", "txd" };

	for (int i = 0; i < 5; i++)
	{
		uintptr_t texture = LoadTextureFromDB(texdb[i], texname);
		if (texture != 0) {
			FLog("%s loaded from %s", texname, texdb[i]);
			return texture;
		}
	}

	LOGW("Texture %s not found!", texname);
	return 0;
}*/

uintptr_t LoadTexture(const char* texname)
{
    static char* texdb[] = { "samp", "gta3", "gta_int", "player", "txd"};

    for (int i = 0; i < sizeof(texdb) / sizeof(const char*); i++)
    {
        uintptr_t texture = (uintptr_t)CUtil::LoadTextureFromDB(texdb[i], texname);
        if (texture != 0) {
            //Log("Texture: %s loaded from %s", texname, texdb[i]);
            return texture;
        }
    }

    //Log("Texture: %s not found!", texname);
    return 0;
}

// 0.3.7 
#include "sprite2d.h"
#include "armhook/patch.h"
#include <algorithm>

RwTexture* LoadTextureFromTxd(const char* txdname, const char* texturename)
{
    RwTexture* tex;
    if (strncmp(txdname, "none", 5u))
    {
        uintptr_t v10 = ((int (*)(const char*))(g_libGTASA + 0x5D3E60+1))(txdname);
        CHook::CallFunction<void>(g_libGTASA + 0x5D4184 + 1);
        CHook::CallFunction<void>(g_libGTASA + 0x5D4184 + 1, v10, 0);
        tex = CHook::CallFunction<RwTexture*>(g_libGTASA + 0x1DBABC + 1, texturename, 0);
        CHook::CallFunction<void>(g_libGTASA + 0x5D41C4 + 1);
    }

    if(!tex) tex = (RwTexture*)LoadTexture(std::string(std::string(texturename) + "_" + txdname).c_str());
    if(!tex) tex = (RwTexture*)LoadTexture(std::string(std::string(txdname) + "_" + texturename).c_str());
    if(!tex) tex = (RwTexture*)LoadTexture(texturename);
    if(!tex)
    {
        std::string str = texturename;
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        tex = (RwTexture*)LoadTexture(str.c_str());
    }
    if(!tex)
    {
        std::string str = texturename;
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        tex = (RwTexture*)LoadTexture(std::string(str + "_" + txdname).c_str());
    }

    if(!tex)
    {
        tex = (RwTexture*)CUtil::LoadTextureFromDB(txdname, texturename);
    }

    return tex;
}
// 0.3.7 
int FindTxdSlot(const char* txdname)
{
	// CTxdStore::FindTxdSlot 
	return ((int(*)(const char*))(g_libGTASA + 0x5D3E60 + 1))(txdname);
}
// 0.3.7 
void PushCurrentTxd()
{
	// CTxdStore::PushCurrentTxd 
	((void (*)())(g_libGTASA + 0x5D4184 + 1))();

}
// 0.3.7 
void SetCurrentTxd(int slot)
{
	// CTxdStore::SetCurrentTxd 
	((void (*)(int, int))(g_libGTASA + 0x5D40F4 + 1))(slot, 0);
}
// 0.3.7 
void PopCurrentTxd()
{
	// CTxdStore::PopCurrentTxd 
	((void (*)())(g_libGTASA + 0x5D41C4 + 1))();

}
// 0.3.7 
uintptr_t SetTexture(const char* texturename)
{
	uintptr_t thiz = 0;

	// CSprite2d::SetTexture 
	((void (*)(void*, const char*))(g_libGTASA + 0x5C8838 + 1))((void*)&thiz, texturename);

	return thiz;
}

float FloatOffset(float f1, float f2)
{
	if (f1 >= f2) return f1 - f2;
	else return (f2 - f1);
}

float DegToRad(float fDegrees)
{
	if (fDegrees > 360.0f || fDegrees < 0.0f) {
		return 0.0f;
	}

	if (fDegrees > 180.0f) return (float)(-(PI - (((fDegrees - 180.0f) * PI) / 180.0f)));
	else return (float)((fDegrees * PI) / 180.0f);
}
// 0.3.7
float fixAngle(float angle)
{
	if (angle > 180.0f)	angle -= 360.0f;
	if (angle < -180.0f)	angle += 360.0f;

	return angle;
}
// 0.3.7
float subAngle(float a1, float a2)
{
	return fixAngle(fixAngle(a2) - a1);
}

void HideEntity(ENTITY_TYPE *pEntity)
{
    pEntity->vPos.z -= 2000.0;

    RwMatrix *matrix = pEntity->mat;
    if(matrix) matrix->pos.y -= 2000.0;
}

/* =========== RemoveBuildings ============= */
int iBuildingToRemoveCount;
REMOVEBUILDING_DATA BuildingToRemove[1000];

void RemoveBuilding(uint32_t dwModel, RwV3d vecPos, float fRange)
{
    RemoveObjectInRange(dwModel, vecPos, fRange);

    BuildingToRemove[iBuildingToRemoveCount].dwModel = dwModel;
    BuildingToRemove[iBuildingToRemoveCount].vecPos.x = vecPos.x;
    BuildingToRemove[iBuildingToRemoveCount].vecPos.y = vecPos.y;
    BuildingToRemove[iBuildingToRemoveCount].vecPos.z = vecPos.z;
    BuildingToRemove[iBuildingToRemoveCount].fRange = fRange;
    iBuildingToRemoveCount++;
}

void RemoveObjectInRange(int iModel, RwV3d vecPos, float fRange)
{
    RemoveOccludersInRadius(vecPos, 500.0);

    // CPools::ms_pBuildingPool
    uintptr_t *pBuildingPool = *(uintptr_t**)(g_libGTASA+0x95AC4C);
    for(int i = 0; i < 20000; i++)
    {
        ENTITY_TYPE *pEntity = (ENTITY_TYPE*)((i * 60) + *pBuildingPool);
        if(pEntity && !IsGameEntityArePlaceable(pEntity))
        {
            if(iModel == -1 || pEntity->nModelIndex == iModel)
            {
                CVector vecPool = CVector { 0.0f, 0.0f, 0.0f };
                memcpy(&vecPool, &pEntity->vPos, sizeof(CVector));

                RwMatrix *matrix = pEntity->mat;
                if(matrix) {
                    memcpy(&vecPool, &matrix->pos, sizeof(CVector));
                }

                float fDistance = GetDistance(vecPool, vecPos);
                if(fDistance <= fRange) HideEntity(pEntity);
            }
        }
    }

    // CPools::ms_pDummyPool
    uintptr_t *pDummyPool = *(uintptr_t**)(g_libGTASA+0x95AC54);
    for(int i = 0; i < 40000; i++)
    {
        ENTITY_TYPE *pEntity = (ENTITY_TYPE*)((i * 60) + *pDummyPool);
        if(pEntity && !IsGameEntityArePlaceable(pEntity))
        {
            if(iModel == -1 || pEntity->nModelIndex == iModel)
            {
                CVector vecPool = CVector { 0.0f, 0.0f, 0.0f };
                memcpy(&vecPool, &pEntity->vPos, sizeof(CVector));

                RwMatrix *matrix = pEntity->mat;
                if(matrix) {
                    memcpy(&vecPool, &matrix->pos, sizeof(CVector));
                }

                float fDistance = GetDistance(vecPool, vecPos);
                if(fDistance <= fRange) HideEntity(pEntity);
            }
        }
    }

    // CPools::ms_pObjectPool
    uintptr_t *pObjectPool = *(uintptr_t**)(g_libGTASA+0x95AC50);
    for(int i = 0; i < 3000; i++)
    {
        ENTITY_TYPE *pEntity = (ENTITY_TYPE*)((i * 420) + *pObjectPool);
        if(pEntity && !IsGameEntityArePlaceable(pEntity))
        {
            if(iModel == -1 || pEntity->nModelIndex == iModel)
            {
                CVector vecPool = CVector { 0.0f, 0.0f, 0.0f };
                memcpy(&vecPool, &pEntity->vPos, sizeof(CVector));

                RwMatrix *matrix = pEntity->mat;
                if(matrix) {
                    memcpy(&vecPool, &matrix->pos, sizeof(CVector));
                }

                float fDistance = GetDistance(vecPool, vecPos);
                if(fDistance <= fRange) HideEntity(pEntity);
            }
        }
    }
}

struct stOccluders {
    unsigned short fMidX;
    unsigned short fMidY;
    unsigned short fMidZ;
    unsigned short fWidthX;
    unsigned short fWidthY;
    unsigned short fHeight;
    char cRotX;
    char cRotY;
    char cRotZ;
    char cPad;
    unsigned short nFlags;
};

void RemoveOccludersInRadius(RwV3d vecPos, float fRadius)
{
    int iOccludersOnMap = *(uint32_t *)(g_libGTASA+0xA45790);
    if(iOccludersOnMap >= 1)
    {
        uint32_t dwOccluders = g_libGTASA+0xA41140;
        for(int i = 0; i <= iOccludersOnMap; i++)
        {
            stOccluders *aOccluders = (stOccluders*)((i * 18) + dwOccluders);

            CVector vecOccluderPos;
            vecOccluderPos.x = (float)aOccluders->fMidX * 0.25;
            vecOccluderPos.y = (float)aOccluders->fMidY * 0.25;
            vecOccluderPos.z = (float)aOccluders->fMidZ * 0.25;

            float fDistance = GetDistance(vecOccluderPos, vecPos);
            if(fDistance <= fRadius)
            {
                aOccluders->fMidX = 0;
                aOccluders->fMidY = 0;
                aOccluders->fMidZ = 0;
                aOccluders->fWidthX = 0;
                aOccluders->fWidthY = 0;
                aOccluders->fHeight = 0;
            }
        }
    }
}

/* =========== TexturedTextDraw ============= */

bool bTextDrawTextureSlotState[200];
uintptr_t TextDrawTexture[200];

// 0.3.7
void ResetTextDrawTextures()
{
	memset(bTextDrawTextureSlotState, 0, sizeof(bTextDrawTextureSlotState));
	memset(TextDrawTexture, 0, sizeof(TextDrawTexture));
}
// 0.3.7
int GetFreeTextDrawTextureSlot()
{
	for (int i = 0; i < 200; i++)
	{
		if (bTextDrawTextureSlotState[i] == false) {
			bTextDrawTextureSlotState[i] = true;
			return i;
		}
	}

	return -1;
}
// 0.3.7
void DestroyTextDrawTexture(int index)
{
	uintptr_t pTexture;

	if (index >= 0 && index < 200)
	{
		pTexture = TextDrawTexture[index];
		bTextDrawTextureSlotState[index] = false;
		if (pTexture)
			DeleteRwTexture(pTexture);

		TextDrawTexture[index] = 0;
	}
}

/* ========================================= */

void DeleteRwTexture(uintptr_t texture)
{
	// RwTextureDestroy
	((void(*)(uintptr_t))(g_libGTASA + 0x1DB764 + 1))(texture);
}

void DrawRaster(RwRaster* raster, CRect const& rect)
{
	uint32_t white = 0xFFFFFFFF;
	//CSprite2d::SetVertices(rect, color::White, color::White, color::White, color::White);
	((void(*)(RwRaster*, const CRect&, uint32_t, uint32_t, uint32_t, uint32_t))(g_libGTASA + 0x5C9014 + 1))(raster, rect, white, white, white, white);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)true);
	RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, (RwIm2DVertex*)(g_libGTASA + 0xA7C264), 4);
}

void DrawTexture(uintptr_t texture, CRect* rect, uint32_t dwColor)
{
	if (texture)
	{
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
		// CSprite2d::Draw(CRect const&, CRGBA const&)
		((void(*)(uintptr_t, CRect*, uint32_t*))(g_libGTASA + 0x5C9120 + 1))(texture, rect, &dwColor);
	}
}

void DrawTextureUV(uintptr_t texture, CRect* rect, uint32_t dwColor, float *uv)
{
	if (texture)
	{
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
		// CSprite2d::Draw(CRect  const& posn, CRGBA  const& color, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);
		((void(*)(uintptr_t, CRect*, uint32_t*, float, float, float, float, float, float, float, float))
			(g_libGTASA + 0x5C95C0 + 1))(texture, rect, &dwColor, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
	}
}

float GetDistance(RwV3d vec1, RwV3d vec2)
{
	return sqrt(
		(vec1.z - vec2.z) * (vec1.z - vec2.z) +
		(vec1.y - vec2.y) * (vec1.y - vec2.y) +
		(vec1.x - vec2.x) * (vec1.x - vec2.x));
}

PED_TYPE* dwPlayerPedPtrs[PLAYER_PED_SLOTS];
void SetPlayerPedPtrRecord(uint8_t bytePlayer, PED_TYPE* dwPedPtr)
{
	dwPlayerPedPtrs[bytePlayer] = dwPedPtr;
}
uint8_t FindPlayerNumFromPedPtr(PED_TYPE* dwPedPtr)
{
	uint8_t x = 0;
	while (x != PLAYER_PED_SLOTS)
	{
		if (dwPlayerPedPtrs[x] == dwPedPtr) return x;
		x++;
	}

	return 0;
}

int GameGetWeaponModelIDFromWeaponID(int iWeaponID)
{
	switch (iWeaponID)
	{
	case WEAPON_BRASSKNUCKLE:
		return WEAPON_MODEL_BRASSKNUCKLE;

	case WEAPON_GOLFCLUB:
		return WEAPON_MODEL_GOLFCLUB;

	case WEAPON_NITESTICK:
		return WEAPON_MODEL_NITESTICK;

	case WEAPON_KNIFE:
		return WEAPON_MODEL_KNIFE;

	case WEAPON_BAT:
		return WEAPON_MODEL_BAT;

	case WEAPON_SHOVEL:
		return WEAPON_MODEL_SHOVEL;

	case WEAPON_POOLSTICK:
		return WEAPON_MODEL_POOLSTICK;

	case WEAPON_KATANA:
		return WEAPON_MODEL_KATANA;

	case WEAPON_CHAINSAW:
		return WEAPON_MODEL_CHAINSAW;

	case WEAPON_DILDO:
		return WEAPON_MODEL_DILDO;

	case WEAPON_DILDO2:
		return WEAPON_MODEL_DILDO2;

	case WEAPON_VIBRATOR:
		return WEAPON_MODEL_VIBRATOR;

	case WEAPON_VIBRATOR2:
		return WEAPON_MODEL_VIBRATOR2;

	case WEAPON_FLOWER:
		return WEAPON_MODEL_FLOWER;

	case WEAPON_CANE:
		return WEAPON_MODEL_CANE;

	case WEAPON_GRENADE:
		return WEAPON_MODEL_GRENADE;

	case WEAPON_TEARGAS:
		return WEAPON_MODEL_TEARGAS;

	case WEAPON_MOLTOV:
		return WEAPON_MODEL_MOLOTOV;

	case WEAPON_COLT45:
		return WEAPON_MODEL_COLT45;

	case WEAPON_SILENCED:
		return WEAPON_MODEL_SILENCED;

	case WEAPON_DEAGLE:
		return WEAPON_MODEL_DEAGLE;

	case WEAPON_SHOTGUN:
		return WEAPON_MODEL_SHOTGUN;

	case WEAPON_SAWEDOFF:
		return WEAPON_MODEL_SAWEDOFF;

	case WEAPON_SHOTGSPA:
		return WEAPON_MODEL_SHOTGSPA;

	case WEAPON_UZI:
		return WEAPON_MODEL_UZI;

	case WEAPON_MP5:
		return WEAPON_MODEL_MP5;

	case WEAPON_AK47:
		return WEAPON_MODEL_AK47;

	case WEAPON_M4:
		return WEAPON_MODEL_M4;

	case WEAPON_TEC9:
		return WEAPON_MODEL_TEC9;

	case WEAPON_RIFLE:
		return WEAPON_MODEL_RIFLE;

	case WEAPON_SNIPER:
		return WEAPON_MODEL_SNIPER;

	case WEAPON_ROCKETLAUNCHER:
		return WEAPON_MODEL_ROCKETLAUNCHER;

	case WEAPON_HEATSEEKER:
		return WEAPON_MODEL_HEATSEEKER;

	case WEAPON_FLAMETHROWER:
		return WEAPON_MODEL_FLAMETHROWER;

	case WEAPON_MINIGUN:
		return WEAPON_MODEL_MINIGUN;

	case WEAPON_SATCHEL:
		return WEAPON_MODEL_SATCHEL;

	case WEAPON_BOMB:
		return WEAPON_MODEL_BOMB;

	case WEAPON_SPRAYCAN:
		return WEAPON_MODEL_SPRAYCAN;

	case WEAPON_FIREEXTINGUISHER:
		return WEAPON_MODEL_FIREEXTINGUISHER;

	case WEAPON_CAMERA:
		return WEAPON_MODEL_CAMERA;

	case WEAPON_NIGHTVISION:
		return WEAPON_MODEL_NIGHTVISION;

	case WEAPON_INFRARED:
		return WEAPON_MODEL_INFRARED;

	case WEAPON_PARACHUTE:
		return WEAPON_MODEL_PARACHUTE;

	}

	return -1;
}

bool IsPointInRect(float x, float y, CRect* rect)
{
	if (x >= rect->left && x <= rect->right &&
		y >= rect->top && y <= rect->bottom)
		return true;

	return false;
}

uintptr_t ModelInfoCreateInstance(int iModel)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);
	if (modelInfo)
	{
		return ((uintptr_t(*)(uintptr_t))*(uintptr_t*)(*(uintptr_t*)modelInfo + 0x2C))(modelInfo);
	}
	
	return 0;
}

void RenderClumpOrAtomic(uintptr_t rwObject)
{
	if (rwObject)
	{
		if (*(uint8_t*)rwObject == 1)
		{
			// Atomic
			FLog("Render Atomic!");
			((void(*)(uintptr_t))( *(uintptr_t*)(rwObject+0x48) ))(rwObject);
		} 
		else if (*(uint8_t*)rwObject == 2)
		{
			FLog("Render Clump!");
			// rpClumpRender
			((void(*)(uintptr_t))(g_libGTASA + 0x2142DC + 1))(rwObject);
		}
	}
}


float GetModelColSphereRadius(int iModel)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo)
	{
		uintptr_t colModel = *(uintptr_t*)(modelInfo + 0x2C);
		if (colModel != 0) {
			return *(float*)(colModel + 0x24);
		}
	}

	return 0.0f;
}


void GetModelColSphereVecCenter(int iModel, RwV3d* vec)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo)
	{
		uintptr_t colModel = *(uintptr_t*)(modelInfo + 0x2C);
		if (colModel != 0) {
            RwV3d* v = (RwV3d*)(colModel + 0x18);

			vec->x = v->x;
			vec->y = v->y;
			vec->z = v->z;
		}
	}
}

void DestroyAtomicOrClump(uintptr_t rwObject)
{
	if (rwObject)
	{
		int type = *(int*)(rwObject);

		if (type == 1)
		{
			// RpAtomicDestroy
			((void(*)(uintptr_t))(g_libGTASA + 0x2141EC + 1))(rwObject);

			uintptr_t parent = *(uintptr_t*)(rwObject + 4);
			if (parent)
			{
				// RwFrameDestroy
				((void(*)(uintptr_t))(g_libGTASA + 0x1D846C + 1))(parent);
			}

		}
		else if (type == 2)
		{
			// RpClumpDestroy
			((void(*)(uintptr_t))(g_libGTASA + 0x21460C + 1))(rwObject);
		}
	}
}

void GamePrepareTrain(VEHICLE_TYPE* pGtaVehicle)
{
	FLog("GamePrepareTrain");

	PED_TYPE* pDriver = pGtaVehicle->pDriver;

	if (pDriver)
	{
		if (pDriver->dwPedType != 0 && pDriver->dwPedType != 1)
		{
			// CPlayerPed::Destructor
			((void (*)(PED_TYPE*))(*(void**)(pDriver->entity.vtable + 0x4)))(pDriver);

			pGtaVehicle->pDriver = nullptr;
		}
	}
}

void GameResetStats()
{

}

// 0.3.7
void ProjectMatrix(RwV3d* vecOut, RwMatrix* mat, RwV3d* vecPos)
{
	vecOut->x =	mat->at.x * vecPos->z + mat->up.x * vecPos->y + mat->right.x * vecPos->x + mat->pos.x;
	vecOut->y = mat->at.y * vecPos->z + mat->up.y * vecPos->y + mat->right.y * vecPos->x + mat->pos.y;
	vecOut->z = mat->at.z * vecPos->z + mat->up.z * vecPos->y + mat->right.z * vecPos->x + mat->pos.z;
}

static CVector _axis[3] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
};

// 0.3.7
void RwMatrixRotate(RwMatrix* mat, int axis, float angle)
{
	((void (*) (RwMatrix*, RwV3d*, float, int))(g_libGTASA + 0x1E3974 + 1))(mat, &_axis[axis], angle, 1);
}
// 0.3.7
void RwMatrixScale(RwMatrix* matrix, RwV3d* scale)
{
	matrix->right.x *= scale->x;
	matrix->right.y *= scale->x;
	matrix->right.z *= scale->x;

	matrix->up.x *= scale->y;
	matrix->up.y *= scale->y;
	matrix->up.z *= scale->y;

	matrix->at.x *= scale->z;
	matrix->at.y *= scale->z;
	matrix->at.z *= scale->z;

	matrix->flags &= 0xFFFDFFFC;
}

const char* getGameDataFolderDirectory()
{
	return (const char*)(g_libGTASA + /*0x63C4B8*/0x6D687C);
}
// 0.3.7
void RwFrameTranslate(uintptr_t parent, RwV3d* vec, int flag)
{
	// RwFrameTranslate
	((void(*)(uintptr_t, RwV3d*, int))(g_libGTASA + 0x1D8694 + 1))(parent, vec, flag);
}
// 0.3.7
void RwFrameRotate(uintptr_t frame, int axis, float angle)
{
	// RwFrameRotate
	((void(*)(uintptr_t, RwV3d*, float, int))(g_libGTASA + 0x1D87A8 + 1))(frame, &_axis[axis], angle, 1);
}
// 0.3.7
void RpWorldAddLight(uintptr_t light)
{
	uintptr_t pRwWorld = *(uintptr_t*)(g_libGTASA + 0x9FC938);
	if (pRwWorld) {
		// RpWorldAddLight
		((void(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x21E830 + 1))(pRwWorld, light);
	}
}
// 0.3.7
void RpWorldRemoveLight(uintptr_t light)
{
	uintptr_t pRwWorld = *(uintptr_t*)(g_libGTASA + 0x9FC938);
	if (pRwWorld) {
		// RpWorldRemoveLight
		((void(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x21E874 + 1))(pRwWorld, light);
	}
}

int LineOfSight(RwV3d* start, RwV3d* end, void* colpoint, uintptr_t ent, char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk)
{
	// CWorld::LineOfSight
	return (( int (*)(RwV3d*, RwV3d*, void*, uintptr_t, char, char, char, char, char, char, char, char))(g_libGTASA+0x424B44+1))(start, end, colpoint, ent, buildings, vehicles, peds, objects, dummies, seeThrough, camera, unk);
}

void RwMatrixInvert(RwMatrix *matOut, RwMatrix *matIn)
{
	// RwMatrixInvert
	(( void (*)(RwMatrix*, RwMatrix*))(g_libGTASA+0x1E3A28+1))(matOut, matIn);
}

int GetTaskTypeFromTask(uint32_t *task)
{
	if(!task || *task < g_libGTASA+0x6653F4 || *task > g_libGTASA+0x66D641) 
		return 0;

	uint32_t dwTaskVtbl = task[0];
	return (( int (*)(uint32_t))(*(void**)(dwTaskVtbl+0x14)))(dwTaskVtbl); // CTask*:GetTaskType
}

int Game_PedStatPrim(int model_id)
{
	int *pStat;
	uint32_t *d = (uint32_t *)(g_libGTASA+0x91DCB8+(model_id*4));
	pStat = (int *)((*d)+40);
	return *pStat;	
}

uintptr_t g_pWidgets[TYPE_SIZE];

eWidgetType GetWidgetTypeFromWidget(uintptr_t pWidget)
{	
	if(pWidget)
	{
		if(pWidget == *(uintptr_t*)(g_libGTASA+0x6F3798)) return TYPE_PUNCH;
		if(pWidget == *(uintptr_t*)(g_libGTASA+0x6F3810)) return TYPE_SPRINT;
		if(pWidget == g_pWidgets[TYPE_ACCELERATE]) return TYPE_ACCELERATE;
		if(pWidget == g_pWidgets[TYPE_ENTERCAR]) return TYPE_ENTERCAR;
		if(pWidget == g_pWidgets[TYPE_BRAKE]) return TYPE_BRAKE;
	}

	return TYPE_NONE;
}

void SetWidgetFromName(const char* name, uintptr_t pWidget)
{
	if(!strcmp("accelerate", name)) SetWidgetFromId(TYPE_ACCELERATE, pWidget);
	if(!strcmp("hud_car", name)) SetWidgetFromId(TYPE_ENTERCAR, pWidget);
	if(!strcmp("brake", name)) SetWidgetFromId(TYPE_BRAKE, pWidget);
}

void SetWidgetFromId(int idWidget, uintptr_t pWidget)
{
	g_pWidgets[idWidget] = pWidget;
}

eWidgetState ProcessFixedWidget(uintptr_t pWidget)
{
	eWidgetType widgetType = GetWidgetTypeFromWidget(pWidget);

	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	switch(widgetType)
	{
		case TYPE_NONE:
			return STATE_NONE;
		case TYPE_PUNCH:
		case TYPE_SPRINT:
			if(pPlayerPed->IsInVehicle() || 
				pPlayerPed->IsInJetpackMode()) 
			{
				return STATE_FIXED;
			}
			break;
		case TYPE_ACCELERATE:
		case TYPE_BRAKE:
			if(!pPlayerPed->IsInVehicle() &&
				!pPlayerPed->IsInJetpackMode()) 
			{
				return STATE_FIXED;
			}
			break;
		case TYPE_ENTERCAR:
			if(pPlayerPed->IsInJetpackMode()) return STATE_NONE;

			if(pNetGame)
			{
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				if(pVehiclePool)
				{
					VEHICLEID vehicleId = pVehiclePool->FindNearestToLocalPlayerPed();
					if(vehicleId == INVALID_VEHICLE_ID) return STATE_FIXED;

					if(vehicleId != INVALID_VEHICLE_ID)
					{
						CVehicle *pVehicle = pVehiclePool->GetAt(vehicleId);
						if(pVehicle)
						{
							if(!pPlayerPed->IsInVehicle() && 
								pVehicle->GetDistanceFromLocalPlayerPed() > 10.0f) 
							{
								return STATE_FIXED;
							}
						}
					}
				}
			}
			break;
	}

	return STATE_NONE;
}

eWidgetState ProcessFixedWidgetFromId(int iWidgetId)
{
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	switch(iWidgetId)
	{
		/*case 166: // Analog (onfoot)
			if(pPlayerPed->IsInVehicle() ||
				pPlayerPed->IsInJetpackMode()) 
			{
				return STATE_FIXED;
			}
			break;
		case 171: // Analog (incar)
			if(!pPlayerPed->IsInVehicle() && 
				!pPlayerPed->IsInJetpackMode()) 
			{
				return STATE_FIXED;
			}
			break;*/
	}

	return STATE_NONE;
}

uint32_t GetUpperCaseKey(const char* szUpper)
{
	// CKeyGen::GetUpperCaseKey
	return (( uint32_t (*)(const char*))(g_libGTASA+0x3F4E8C+1))(szUpper);
}

enum Flags {
    IDENTITY = 0x20000
};

enum Type {
    TYPENORMAL	= 1,
    TYPEORTHOGONAL	= 2,
    TYPEORTHONORMAL	= 3,
    TYPEMASK = 3
};

void mat_invertOrthonormal(RwMatrix *dst, const RwMatrix *src)
{
    dst->right.x = src->right.x;
    dst->right.y = src->up.x;
    dst->right.z = src->at.x;
    dst->up.x = src->right.y;
    dst->up.y = src->up.y;
    dst->up.z = src->at.y;
    dst->at.x = src->right.z;
    dst->at.y = src->up.z;
    dst->at.z = src->at.z;
    dst->pos.x = -(src->pos.x*src->right.x +
                   src->pos.y*src->right.y +
                   src->pos.z*src->right.z);
    dst->pos.y = -(src->pos.x*src->up.x +
                   src->pos.y*src->up.y +
                   src->pos.z*src->up.z);
    dst->pos.z = -(src->pos.x*src->at.x +
                   src->pos.y*src->at.y +
                   src->pos.z*src->at.z);
    dst->flags = TYPEORTHONORMAL;
}

RwMatrix* mat_invertGeneral(RwMatrix *dst, const RwMatrix *src)
{
    float det, invdet;
    // calculate a few cofactors
    dst->right.x = src->up.y*src->at.z - src->up.z*src->at.y;
    dst->right.y = src->at.y*src->right.z - src->at.z*src->right.y;
    dst->right.z = src->right.y*src->up.z - src->right.z*src->up.y;
    // get the determinant from that
    det = src->up.x * dst->right.y + src->at.x * dst->right.z + dst->right.x * src->right.x;
    invdet = 1.0;
    if(det != 0.0f)
        invdet = 1.0f/det;
    dst->right.x *= invdet;
    dst->right.y *= invdet;
    dst->right.z *= invdet;
    dst->up.x = invdet * (src->up.z*src->at.x - src->up.x*src->at.z);
    dst->up.y = invdet * (src->at.z*src->right.x - src->at.x*src->right.z);
    dst->up.z = invdet * (src->right.z*src->up.x - src->right.x*src->up.z);
    dst->at.x = invdet * (src->up.x*src->at.y - src->up.y*src->at.x);
    dst->at.y = invdet * (src->at.x*src->right.y - src->at.y*src->right.x);
    dst->at.z = invdet * (src->right.x*src->up.y - src->right.y*src->up.x);
    dst->pos.x = -(src->pos.x*dst->right.x + src->pos.y*dst->up.x + src->pos.z*dst->at.x);
    dst->pos.y = -(src->pos.x*dst->right.y + src->pos.y*dst->up.y + src->pos.z*dst->at.y);
    dst->pos.z = -(src->pos.x*dst->right.z + src->pos.y*dst->up.z + src->pos.z*dst->at.z);
    dst->flags &= ~IDENTITY;
    return dst;
}

RwMatrix* mat_invert(RwMatrix *dst, const RwMatrix *src)
{
    if(src->flags & IDENTITY)
        *dst = *src;
    else if((src->flags & TYPEMASK) == TYPEORTHONORMAL)
        mat_invertOrthonormal(dst, src);
    else
        return mat_invertGeneral(dst, src);
    return dst;
}

const char* GetAnimByIdx(int idx)
{
	if (idx < 0 || idx >= 1843)
	{
		return nullptr;
	}
	return &g_szAnimBlockNames[idx][0];
}

bool GetAnimationIndexFromName(const char* szName)
{
    for (int i = 0; i < 1843; i++)
    {
        if (!strcasecmp(szName, g_szAnimBlockNames[i]))
        {
            return true;
        }
    }
    return 0;
}

int GetAnimIdxByName(const char* szName)
{
	for (int i = 0; i < 1843; i++)
	{
        if (!strcasecmp(szName, g_szAnimBlockNames[i]))
        {
            return i;
        }
	}
	return -1;
}

bool IsGameEntityArePlaceable(ENTITY_TYPE *pEntity)
{
    if(pEntity)
    {
        if(pEntity->vtable)
            return (pEntity->vtable == g_libGTASA+0x667D14);
    }

    return false;
}

RpMaterial* ObjectMaterialCallBack(RpMaterial* material, void* data)
{
    CObject* pObject = (CObject*)data;
    RpAtomic* rpAtomic = (RpAtomic*)pObject->m_pEntity->pRpAtomic;
    for (int i = 0; i < rpAtomic->geometry->matList.numMaterials; i++)
    {
        if(i >= 16) break;
        if(rpAtomic->geometry->matList.materials[i] == material)
        {
            if(pObject->m_MaterialTexture[i])
                rpAtomic->geometry->matList.materials[i]->texture = reinterpret_cast<RwTexture *>(pObject->m_MaterialTexture[i]);

            if(pObject->m_dwMaterialColor[i]) {
                rpAtomic->geometry->flags = rpAtomic->geometry->flags & 0xFFFFFFF7 | 0x40;
                RwRGBA *r = (RwRGBA *) &pObject->m_dwMaterialColor[i];
                rpAtomic->geometry->matList.materials[i]->color = *r;
                rpAtomic->geometry->matList.materials[i]->surfaceProps.ambient = 1.0f;	// ambient
                rpAtomic->geometry->matList.materials[i]->surfaceProps.specular = 0.0f;	// specular
                rpAtomic->geometry->matList.materials[i]->surfaceProps.diffuse = 1.0f;	// diffuse
            }
            break;
        }
    }

    return material;
}

RpAtomic* ObjectMaterialTextCallBack(RpAtomic* rpAtomic, CObject* pObject)
{
    if(!pObject->m_MaterialTextTexture || rpAtomic->object.object.type != 1) return rpAtomic;

    int iTotalEntries = rpAtomic->geometry->matList.numMaterials;
    if (iTotalEntries > 16) iTotalEntries = 16; // fix fucking bug :|
    for (int i = 0; i < iTotalEntries; i++)
    {
        if(pObject->m_MaterialTextTexture[i])
        {
            rpAtomic->geometry->matList.materials[i]->texture = reinterpret_cast<RwTexture *>(pObject->m_MaterialTextTexture[i]);
        }
    }

    return rpAtomic;
}

void SetScissorRect(void* pRect)
{
    return ((void (*)(void*))(g_libGTASA + (VER_x32 ? 0x002B3EC4 + 1 : 0x373290)))(pRect);
}