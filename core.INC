// Zombies Enum

enum zombie_Enum {

	zombie_id,
	zombie_attack,
	zombie_pause,
	zombie_pause_init,
	zombie_dead,
	zombie_grito,
	zombie_indobarulho,
	zombie_observando,
	zombie_walktime,
	zombie_class,
	zombie_class_defalt,
	zombie_lastupdate,
	zombie_shootdelay,
	Float:zombie_health,
	Float:zombie_velocity,
	Float:zombie_detection,
	Float:zombie_alcance,
	Float:zombie_spawnx,
	Float:zombie_spawny,
	Float:zombie_spawnz,
	Float:zombie_lastx,
	Float:zombie_lasty,
	Float:zombie_lastz,
	zombie_movtype,
	Float:zombie_movspeed,
}
new ZombieInfo[MAX_PLAYERS][zombie_Enum];
new zombiecount;
new tickBarulho;
