// Zombies Classes enum

#define max_classes 75

new index_class = 0;
new index_bandidos = 0;

enum {
	zombie_class_normal,
	zombie_class_shovel,
	zombie_class_knifer,
	zombie_class_serra,
	zombie_class_bomber,
	zombie_class_bandido,
}

enum zombieClassEnumInfo {

	// Class id
	zombie_class_id,
	
	// Skin da class
	zombie_class_skin,
	
	// Resistencia do zombie
	Float:zombie_class_resistence,
	
	// Visão do zombie
	Float:zombie_class_detection,
	
	// Alcance do zombie/bandido
	Float:zombie_class_rangeattack,
	
	// Delay tiro
	zombie_class_shootdelay,
	
	// Arma do zombie
	zombie_class_weapon,
	
	// type movement
	zombie_class_movtype,
	
	// Velocidade movimento
	Float:zombie_class_movspeed,
}
new ZombieClassInfo[max_classes][zombieClassEnumInfo];
	
stock AddZombieClass(classid, skinid, Float:resistence, weaponid, Float:detection, Float:rangeattack = 0.6, shootdelay = 100, movtype = FCNPC_MOVE_TYPE_RUN, Float:movspeed = 0.36444) {

    ZombieClassInfo[index_class][zombie_class_id] = classid;
    ZombieClassInfo[index_class][zombie_class_resistence] = resistence;
    ZombieClassInfo[index_class][zombie_class_skin] = skinid;
    ZombieClassInfo[index_class][zombie_class_weapon] = weaponid;
    ZombieClassInfo[index_class][zombie_class_detection] = detection;
    ZombieClassInfo[index_class][zombie_class_rangeattack] = rangeattack;
    ZombieClassInfo[index_class][zombie_class_shootdelay] = shootdelay;
    ZombieClassInfo[index_class][zombie_class_movtype] = movtype;
    ZombieClassInfo[index_class][zombie_class_movspeed] = movspeed;

	index_class ++;
	
	if (classid == zombie_class_bandido) {
	    index_bandidos ++;
	}
}

stock SetupZombiesClasses() {

	//                    classid           skin    vida   weap  detection
	// Zombies Normais
    AddZombieClass(zombie_class_normal,     054,  	75.0, 	0,   30.0);
    AddZombieClass(zombie_class_normal, 	077,  	75.0, 	0,   30.0);
    AddZombieClass(zombie_class_normal, 	078,  	75.0, 	0,   30.0);
    AddZombieClass(zombie_class_normal, 	079,  	75.0, 	0,   30.0);
    AddZombieClass(zombie_class_normal, 	197, 	75.0, 	0,   30.0);
    AddZombieClass(zombie_class_normal, 	200, 	75.0, 	0,   30.0);
    
    // Zombies com Shovel
//  AddZombieClass(zombie_class_shovel, 	129, 	100.0,  6, 	 35.0);
    AddZombieClass(zombie_class_shovel, 	130, 	100.0,  6, 	 35.0);
    AddZombieClass(zombie_class_shovel, 	136, 	100.0, 	6, 	 35.0);
    AddZombieClass(zombie_class_shovel, 	159, 	100.0, 	6, 	 35.0);
    AddZombieClass(zombie_class_shovel, 	160, 	100.0, 	6, 	 35.0);
    AddZombieClass(zombie_class_shovel, 	196, 	100.0, 	6, 	 35.0);
    AddZombieClass(zombie_class_shovel, 	239, 	100.0, 	6, 	 35.0);
    
    // Zombies com Knife
    //AddZombieClass(zombie_class_knifer, 	134, 	150.0, 	4, 	 40.0);
    AddZombieClass(zombie_class_knifer, 	137, 	150.0, 	4, 	 40.0);
    AddZombieClass(zombie_class_knifer, 	162, 	150.0, 	4, 	 40.0);
    
	// Zombies com Serra
    AddZombieClass(zombie_class_serra,  	135, 	400.0, 	9, 	 55.0);
    AddZombieClass(zombie_class_serra,  	168, 	500.0, 	9, 	 55.0);
    AddZombieClass(zombie_class_serra,  	213, 	300.0, 	9, 	 55.0);
    
    // Zombies que explodem
    AddZombieClass(zombie_class_bomber, 	129, 	425.0,  0, 	 45.0,  	 .movtype = FCNPC_MOVE_TYPE_RUN, .movspeed = 0.38444);
//                                                                           alcance do disparo    shoot delay
    // Bandidos
    AddZombieClass(zombie_class_bandido, 	29, 	70.0,   22,  42.0,     	 .rangeattack = 25.0,  .shootdelay = 150);
    AddZombieClass(zombie_class_bandido, 	100, 	70.0,   31,  45.0,     	 .rangeattack = 35.0,  .shootdelay = 100);
    AddZombieClass(zombie_class_bandido, 	29, 	70.0,   30,  45.0,     	 .rangeattack = 33.0,  .shootdelay = 100);
    AddZombieClass(zombie_class_bandido, 	128, 	70.0,   22,  45.0,     	 .rangeattack = 25.0,  .shootdelay = 150);
    AddZombieClass(zombie_class_bandido, 	179, 	70.0,   33,  70.0,     	 .rangeattack = 60.0,  .shootdelay = -1);
}


