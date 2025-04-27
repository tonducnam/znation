#include "../modulos/zombies/nodes.inc"

stock IsValidZPoint(playerid, playerid2) {

	static
		Float:pPos[3][2]
		;
		
	GetPlayerPos(playerid, pPos[0][0],pPos[1][0],pPos[2][0]);

	GetPlayerPos(playerid2, pPos[0][1],pPos[1][1],pPos[2][1]);

	if (!IsZombieViewPlayer(playerid, playerid2) && ZombieInfo[playerid][zombie_attack] == INVALID_PLAYER_ID) {
	    return 0;
	}
	
	return 1;
}

stock connectZombieToServer(world=0) {

	new z_name[MAX_PLAYER_NAME];

	format(z_name, sizeof (z_name), "zombie_%d_%d", zombiecount++, gettime());

	new zo_id = FCNPC_Create(z_name);

    if (zo_id != INVALID_PLAYER_ID) {

	    // Vida do Zombie
	    ZombieInfo[zo_id][zombie_health] 	  = 100.0;
	    ZombieInfo[zo_id][zombie_id]     	  = zo_id;
	    ZombieInfo[zo_id][zombie_attack]      = INVALID_PLAYER_ID;
	    ZombieInfo[zo_id][zombie_pause_init]  = GetTickCount() + 10000;
	    ZombieInfo[zo_id][zombie_indobarulho] = 0;
	    ZombieInfo[zo_id][zombie_observando]  = 0;
	    ZombieInfo[zo_id][zombie_lastupdate]  = GetTickCount() + 10000;
	    
	    new random_spawn = Iter_Random(ZombieSpawnsIter);

	    if (random_spawn != -1)
	    {
	    	Iter_Remove(ZombieSpawnsIter, random_spawn);
		}
		else
		{
            random_spawn = random(sizeof(Zombies_Spawns));
		}
	    
	    ZombieInfo[zo_id][zombie_spawnx]      = Zombies_Spawns[random_spawn][0];
	    ZombieInfo[zo_id][zombie_spawny]      = Zombies_Spawns[random_spawn][1];
	    ZombieInfo[zo_id][zombie_spawnz]      = Zombies_Spawns[random_spawn][2];
	    
	    CA_RayCastLine(ZombieInfo[zo_id][zombie_spawnx], ZombieInfo[zo_id][zombie_spawny], ZombieInfo[zo_id][zombie_spawnz], ZombieInfo[zo_id][zombie_spawnx], ZombieInfo[zo_id][zombie_spawny], ZombieInfo[zo_id][zombie_spawnz] - 100.0,
	    ZombieInfo[zo_id][zombie_spawnx], ZombieInfo[zo_id][zombie_spawny], ZombieInfo[zo_id][zombie_spawnz]);
	    
	    ZombieInfo[zo_id][zombie_spawnz] += 1.0;
	    
//	    ZombieInfo[zo_id][zombie_velocity]    = MOVE_SPEED_RUN;
		ZombieInfo[zo_id][zombie_velocity]    = FCNPC_MOVE_SPEED_RUN;

	    FCNPC_Spawn(zo_id, 136, ZombieInfo[zo_id][zombie_spawnx], ZombieInfo[zo_id][zombie_spawny], ZombieInfo[zo_id][zombie_spawnz]);
	    FCNPC_SetVirtualWorld(zo_id, world);
	    respawnZombie(zo_id);
	}
	return zo_id;
}

stock IsZombieViewPlayer(zombie, playerid) {

	static
	    // player
		Float:x,
		Float:y,
		Float:z,
		// zombie
		Float:zox,
		Float:zoy,
		Float:zoz;
	
	FCNPC_GetPosition(zombie, zox, zoy, zoz);
	
	GetPlayerPos(playerid, x, y, z);

	if (!IsZombieFacingPlayer(zombie, x, y, zox, zoy, 80.0) && getPlayerOnfootSpeed(playerid) < 12)
	    return 0;

	if (CA_RayCastLine(x, y, z, zox, zoy, zoz, x, x, x))
	    return 0;
	    
	return 1;
}

stock MoveZombie(npcid, Float:x, Float:y, Float:z, Float:speed, use_zmap=0)
{
	if (!IsPlayerNPC(npcid)) return 0;

	FCNPC_GoTo(npcid, x, y, z, 1, speed, use_zmap);

	return 1;
}

stock StopZombie(npcid, time)
{
	if (!IsPlayerNPC(npcid)) return 0;

	FCNPC_Stop(npcid);
	
	//RNPC_StopPlayback(npcid);
	ZombieInfo[npcid][zombie_pause] = GetTickCount() + time;
	ClearAnimations(npcid, 1);

	return 1;
}

stock IsZombieFacingPlayer(zombieid, Float:pX, Float:pY, Float:X, Float:Y, Float:dOffset)
{
	static
		Float:pA,
		Float:ang
		;

	pA = FCNPC_GetAngle(zombieid);

	if( Y > pY )
		ang = (-acos((X - pX) / floatsqroot((X - pX) * (X - pX) + (Y - pY) * (Y - pY))) - 90.0);

	else if( Y < pY && X < pX )
		ang = (acos((X - pX) / floatsqroot((X - pX) * (X - pX) + (Y - pY) * (Y - pY))) - 450.0);

	else if( Y < pY )
		ang = (acos((X - pX) / floatsqroot((X - pX) * (X - pX) + (Y - pY) * (Y - pY))) - 90.0);

	if(AngleInRangeOfAngleEx(-ang, pA, dOffset))
		return 1;

	return 0;

}

stock IsPlayerFacingZombie(playerid, zombieid, Float:dOffset)
{
	static
		Float:pX,
		Float:pY,
		Float:pZ,
		Float:pA,
		Float:X,
		Float:Y,
		Float:Z,
		Float:ang
		;

	GetPlayerPos(playerid, X, Y, Z);
	FCNPC_GetPosition(zombieid, pX, pY, pZ);
	GetPlayerFacingAngle(playerid, pA);

	if( Y > pY )
		ang = (-acos((X - pX) / floatsqroot((X - pX)*(X - pX) + (Y - pY)*(Y - pY))) - 90.0);
	else if( Y < pY && X < pX )
		ang = (acos((X - pX) / floatsqroot((X - pX)*(X - pX) + (Y - pY)*(Y - pY))) - 450.0);
	else if( Y < pY )
		ang = (acos((X - pX) / floatsqroot((X - pX)*(X - pX) + (Y - pY)*(Y - pY))) - 90.0);

	if(AngleInRangeOfAngleEx(-ang, pA, dOffset)) return true;

	return false;
}

stock AngleInRangeOfAngleEx(Float:a1, Float:a2, Float:range)
{
	a1 -= a2;
	if((a1 < range) && (a1 > -range)) return true;

	return false;
}

stock CountZombiesFollowPlayer(playerid) {

	new count;

	for (new z = GetPlayerPoolSize(), zombie; zombie <= z; zombie++) {

		if (!FCNPC_IsValid(zombie))
		    continue;

		if (ZombieInfo[zombie][zombie_attack] == playerid) {

			//ZombieInfo[zombie][zombie_velocity] = (0.51000) - (count * 0.025);
			ZombieInfo[zombie][zombie_velocity] = (ZombieInfo[zombie][zombie_movspeed]) - (count * 0.025);

			if (ZombieInfo[zombie][zombie_velocity] < 0.20000)
			    ZombieInfo[zombie][zombie_velocity] = 0.20000;
			
			count ++;
	 	}
	}
	return count;
}

stock FazerBarulho(Float:x, Float:y, Float:z, Float:distancia=100.0)
{
	if (tickBarulho > GetTickCount())
	{
		return 0;
	}
    tickBarulho = GetTickCount() + 5000;
    
    new
        countAttractZombies = 0;

	for (new zz = GetPlayerPoolSize(), zombie; zombie <= zz; zombie++)
	{
		if (!FCNPC_IsValid(zombie))
		    continue;
		    
	    if (ZombieInfo[zombie][zombie_dead])
			continue;
	
	    if (ZombieInfo[zombie][zombie_attack] != INVALID_PLAYER_ID)
	        continue;
	
	    if (IsPlayerInRangeOfPoint(zombie, distancia, x, y, z))
		{
		    countAttractZombies++;
			// Fazer o zombie se mover até o local do barulho
			static Float:zPos[3];

			FCNPC_GetPosition(zombie, zPos[0], zPos[1], zPos[2]);

			CA_TraceLineEx(zombie, zPos[0], zPos[1], zPos[2], x, y, z, .stepsize=2.0, .maxnodes = 30);

			if (ZombieNodeIndex[zombie] > 0)
			{
			    FCNPC_CreateMovement(zombie);

			    for(new nodes; nodes < ZombieNodeIndex[zombie]; nodes++)
				{
			        // Add movimentos
		            FCNPC_AddMovement(zombie, PlayerNodesX[zombie][nodes], PlayerNodesY[zombie][nodes], PlayerNodesZ[zombie][nodes]);
				}

				// Aplicar o movimento
			    FCNPC_PlayMovement(zombie, FCNPC_MOVE_TYPE_WALK, .speed = 0.1252086, .delaystop = 0);

			    FCNPC_ApplyAnimation(zombie, "PED","WALK_old", 4.1, 1, 1, 1, 1, 0);
			}

			ZombieInfo[zombie][zombie_indobarulho] = 1;
			ZombieInfo[zombie][zombie_observando]  = 0;
		}
		if (countAttractZombies > 4)
		{
		    return 1;
		}
	}
	return 1;
}

stock GetNodeRandom(Float:x, Float:y, Float:z, Float:max_distance)
{
	new Iterator:randomNodes<sizeof (LS_NodesPoints)>;
	
	for(new node; node != sizeof (LS_NodesPoints); node++)
	{
		new Float:distance_point = GetPointDistanceToPoint(x, y, z, LS_NodesPoints[node][0], LS_NodesPoints[node][1], LS_NodesPoints[node][2]);

		if (distance_point > 5.0 && distance_point < max_distance)
		{
  			Iter_Add(randomNodes, node);
		}
	}
	
	return (Iter_Count(randomNodes) ? (Iter_Random(randomNodes)) : (random(sizeof(LS_NodesPoints))));
}

stock z_Init()
{
	for(new index; index < sizeof (Zombies_Spawns); index++) {
	    Iter_Add(ZombieSpawnsIter, index);
	}
	for(new z; z != MAX_ZOMBIES; z++) {
	    connectZombieToServer();
	}
}

public FCNPC_OnUpdate(npcid)
{
	new
	    currentTick = GetTickCount();
	if (ZombieInfo[npcid][zombie_lastupdate] < currentTick)
	{
        ZombieInfo[npcid][zombie_lastupdate] = currentTick + (190 + random(70));

		if ( ZombieInfo[npcid][zombie_dead] )
	 	    return 1;

		if ( FCNPC_IsDead(npcid) )
		    return 1;

	 	if ( ZombieInfo[npcid][zombie_pause] > currentTick )
	 	    return 1;

		if ( ZombieInfo[npcid][zombie_pause_init] > currentTick )
	 	    return 1;
	 	    
		if (!FCNPC_IsStreamedInForAnyone(npcid))
		    return 1;
		    
//		printf("%d", ZombieInfo[npcid][zombie_attack]);
		    
		// update zombie follow player
		if (ZombieInfo[npcid][zombie_attack] != INVALID_PLAYER_ID) {
        	UpdateZombieFolowPlayer(npcid, ZombieInfo[npcid][zombie_attack]);
		// update iddle zombie
		} else {
		    UpdateZombieIddleMovements(npcid);
		}
        
        // sound effect in zombie
		if(ZombieInfo[npcid][zombie_grito] < GetTickCount() && ZombieInfo[npcid][zombie_class] != zombie_class_bandido) {
		    // get a position
	 	    static Float:pos[3];
	 	    FCNPC_GetPosition(npcid, pos[0], pos[1], pos[2]);
			// sound effct audio
		    Play3DSound(3 + random(18), pos[0], pos[1], pos[2]);
		    ZombieInfo[npcid][zombie_grito] = GetTickCount() + (8000 + random(9500));
		}
	}
	return 1;
}

stock UpdateZombieFolowPlayer(zombie, playerid) {
	// get a current zombie clossest player
	new Float:currentDistanceBetween;
	new currentClossestPlayer = z_GetClosestPlayer(zombie, currentDistanceBetween);
	new zombieFollowPlayer = ZombieInfo[zombie][zombie_attack];

	if (currentClossestPlayer != playerid && IsZombieViewPlayer(zombie, currentClossestPlayer)) {
	
        PlayerData[playerid][dCountZombies] --;
        if (PlayerData[playerid][dCountZombies] < 0)
            PlayerData[playerid][dCountZombies] = 0;
	
	    ZombieInfo[zombie][zombie_attack] = currentClossestPlayer;
	    zombieFollowPlayer = currentClossestPlayer;
	}
	
	if (currentDistanceBetween < ZombieInfo[zombie][zombie_detection]) {
	    // count zombies follow player
        new countPlayerCurrentFollow = CountZombiesFollowPlayer(zombieFollowPlayer);
		// check if zombies amount if much
		if (countPlayerCurrentFollow >= MAX_ZOMBIES_FOLLOW_PLAYER && ZombieInfo[zombie][zombie_attack] != zombieFollowPlayer)
		    return 0;

        PlayerData[currentClossestPlayer][dCountZombies] ++;

 	    new Float:pos[6];
 	    // get a player position
 	    GetPlayerPos(zombieFollowPlayer, pos[0], pos[1], pos[2]);

 	    FCNPC_GetPosition(zombie, pos[3], pos[4], pos[5]);

		// check if distance is better than zombie radius attack
		if ( currentDistanceBetween < ZombieInfo[zombie][zombie_alcance] ) {
			// check if is a bandido
			if (ZombieInfo[zombie][zombie_class] == zombie_class_bandido) {

				if (IsZombieViewPlayer(zombie, currentClossestPlayer)) {
					// stop zombie
					if (FCNPC_IsMoving(zombie))
                    	FCNPC_Stop(zombie);

                    if (!FCNPC_IsAiming(zombie))
                    	FCNPC_ClearAnimations(zombie);
					// aim at player
					if (!FCNPC_IsAiming(zombie))
						FCNPC_AimAtPlayer(zombie, zombieFollowPlayer, true);
					// delay in weapon fire
//					ZombieInfo[zombie][zombie_pause] = GetTickCount() + ZOMBIE_UPDATE_TIME;
					
					return 1;
				} else {
				    // move zombie to player
				    MoveZombieToPlayer(zombie, zombieFollowPlayer);

				    FCNPC_StopAim(zombie);
				}
			} else {
			    // check if explosive zombie
				if (ZombieInfo[zombie][zombie_class] == zombie_class_bomber) {
					#if defined onZombieDeath
				    	onZombieDeath(zombie, INVALID_PLAYER_ID, 255);
					#endif
				   	FCNPC_Kill(zombie);
				    return 1;
				}
			
			    // stop zombie
			    FCNPC_Stop(zombie);

                if (!FCNPC_IsAttacking(zombie))
                   	FCNPC_ClearAnimations(zombie);

				// attack melee player
				FCNPC_AimAtPlayer(zombie, playerid, false, -1, false);

				ZombieInfo[zombie][zombie_pause] = GetTickCount() + ZOMBIE_UPDATE_TIME;

				FCNPC_MeleeAttack(zombie);
				// set a zombie angle
				setZombieAngleToPlayer(zombie, zombieFollowPlayer);
			}
			return 1;
		// stop aim and attack
 	    } else {
 	        FCNPC_StopAttack(zombie);
 	        FCNPC_StopAim(zombie);
 	    }

		if (!FCNPC_IsAttacking(zombie)) {
		    // move zombie to player
        	if (MoveZombieToPlayer(zombie, zombieFollowPlayer))
        	{
        	    FCNPC_ApplyAnimation(zombie, "PED","sprint_civi", 4.1, 1, 1, 1, 1, 0);
        	}
        	else
        	{
        	    if (FCNPC_IsMoving(zombie))
        	        FCNPC_Stop(zombie);
				// apply anim
        	    FCNPC_ApplyAnimation(zombie, "HAIRCUTS","BRB_Buy", 4.1, 1, 1, 1, 1, 0);
        	}
		}
	// stop following
	} else {
        StopZombieFollow(zombie);
	}
	return 1;
}

stock UpdateZombieIddleMovements(zombie) {

	// Verificar o tempo da ultima movimentada
	if (gettime() > ZombieInfo[zombie][zombie_walktime] && !ZombieInfo[zombie][zombie_indobarulho]) {
	    // Fazer ele se mover
	    updateZombiesMovements(zombie);
	    // Setar o tempo do ultimo movimento
	    ZombieInfo[zombie][zombie_walktime] = gettime() + (10 + random(15));
	}
	
	// check if pursuir a invalid player
	new Float:currentDistanceBetween;
	new currentClossestPlayer = z_GetClosestPlayer(zombie, currentDistanceBetween);
//	printf("current closest: %d %f detection: %f", currentClossestPlayer, currentDistanceBetween, ZombieInfo[zombie][zombie_detection]);
	// check if zombie view player
	if (currentClossestPlayer != INVALID_PLAYER_ID && currentDistanceBetween < ZombieInfo[zombie][zombie_detection] && IsZombieViewPlayer(zombie, currentClossestPlayer)) {
	
	    ZombieInfo[zombie][zombie_attack] = currentClossestPlayer;
	    ZombieInfo[zombie][zombie_indobarulho] = 0;
	    ZombieInfo[zombie][zombie_observando]  = 0;
	    PlayerData[currentClossestPlayer][dCountZombies] ++;
	}
}

stock SetZombieFollowingPlayer(zombie, playerid) {

	if (currentClossestPlayer != playerid) {
	    ZombieInfo[zombie][zombie_attack] = currentClossestPlayer;
	}
}

stock StopZombieFollow(zombie) {

	if (ZombieInfo[zombie][zombie_attack] != INVALID_PLAYER_ID) {

        PlayerData[ZombieInfo[zombie][zombie_attack]][dCountZombies] --;
        if (PlayerData[ZombieInfo[zombie][zombie_attack]][dCountZombies] < 0)
            PlayerData[ZombieInfo[zombie][zombie_attack]][dCountZombies] = 0;

	    ZombieInfo[zombie][zombie_attack] = INVALID_PLAYER_ID;

		// Parar o zombie
		FCNPC_Stop(zombie);

		// Limpar as animações
		ClearAnimations(zombie);

 	    // Fazer parar de bater
	    FCNPC_StopAttack(zombie);

	    // Parar de mirar
	    FCNPC_StopAim(zombie);

	    FCNPC_ApplyAnimation(zombie, "PAULNMAC","PnM_Loop_B", 3.1, 1, 1, 1, 1, 0);

	    ZombieInfo[zombie][zombie_walktime] = gettime() + (5 + random(15));
	}
}



callback updateZombiesMovements(zombie) {

	new
		Float:x,
		Float:y,
		Float:z,
		Float:tox,
		Float:toy,
		Float:toz
		;

	FCNPC_GetPosition(zombie, x, y, z);

	tox = x + frandom(5.0) - frandom(5.0);
	toy = y + frandom(5.0) - frandom(5.0);
	toz = z;

	if (CA_TraceLine(zombie, -1, x, y, z, tox, toy, toz, .stepsize = 1.5, .maxnodes = 2)) {

		FCNPC_CreateMovement(zombie);

	    for(new nodes; nodes < ZombieNodeIndex[zombie]; nodes++) {
	        FCNPC_AddMovement(zombie, PlayerNodesX[zombie][nodes], PlayerNodesY[zombie][nodes], PlayerNodesZ[zombie][nodes]);
	    }

	    FCNPC_PlayMovement(zombie, FCNPC_MOVE_TYPE_WALK, .speed = 0.1252086, .delaystop = 0);

	    FCNPC_ApplyAnimation(zombie, "PED","WALK_old", 3.1, 1, 1, 1, 1, 0);
	}

	return 1;
}

public FCNPC_OnMovementEnd(npcid) {

	if (ZombieInfo[npcid][zombie_dead]) return 0;

	if (ZombieInfo[npcid][zombie_attack] == INVALID_PLAYER_ID) {

		FCNPC_ClearAnimations(npcid);
		
		FCNPC_ApplyAnimation(npcid, "PAULNMAC","PnM_Loop_B", 4.1, 1, 1, 1, 1, 0);

		if(ZombieInfo[npcid][zombie_indobarulho]) {

			FCNPC_Stop(npcid);

			// Quando chegar ao local do barulho
	   		ZombieInfo[npcid][zombie_observando] = 1;
	   		
	   		ZombieInfo[npcid][zombie_walktime] = gettime() + (5 + random(12));
		}
	}
	return 1;
}

callback zombieAndar(zombie) {

	ZombieInfo[zombie][zombie_indobarulho] = 0;
	ZombieInfo[zombie][zombie_observando]  = 0;
	ZombieInfo[zombie][zombie_walktime]    = 0;
	
	FCNPC_ClearAnimations(zombie);
}

callback respawnZombieWorld(npcid) {

	if (npcid < 0 || npcid >= MAX_PLAYERS) return 0;

	// Respawnar o zombie
	FCNPC_Respawn(npcid);
	
//	FCNPC_Spawn(npcid, 136, ZombieInfo[npcid][zombie_spawnx], ZombieInfo[npcid][zombie_spawny], ZombieInfo[npcid][zombie_spawnz]);

	return 1;
}

stock spawnServerBandido(Float:x, Float:y, Float:z, Float:ang, skinid = 0) {

	#pragma unused ang

	new z_name[MAX_PLAYER_NAME];

	format(z_name, sizeof (z_name), "bandido_%d", zombiecount++);

	new zo_id = FCNPC_Create(z_name);
	new randClass;

    if (zo_id != INVALID_PLAYER_ID) {

		for(new classid; classid < index_class; classid++) {
		
		    if (ZombieClassInfo[classid][zombie_class_skin] == skinid) {
		        randClass = classid; break;
		    }
		}

		if (skinid != 179) {
		    randClass = (index_class - index_bandidos) + random(index_bandidos - 1);
		}

	    // Vida do Zombie
	    ZombieInfo[zo_id][zombie_id]     	  = zo_id;
	    ZombieInfo[zo_id][zombie_attack]      = INVALID_PLAYER_ID;
	    ZombieInfo[zo_id][zombie_pause_init]  = GetTickCount() + 10000;
	    ZombieInfo[zo_id][zombie_indobarulho] = 0;
	    ZombieInfo[zo_id][zombie_observando]  = 0;
	    ZombieInfo[zo_id][zombie_lastupdate]  = GetTickCount() + 10000;
	    
	    ZombieInfo[zo_id][zombie_velocity]    = FCNPC_MOVE_SPEED_RUN;

		// Info da classe
	  	ZombieInfo[zo_id][zombie_health]    = ZombieClassInfo[randClass][zombie_class_resistence];
	  	ZombieInfo[zo_id][zombie_detection] = ZombieClassInfo[randClass][zombie_class_detection];
	  	ZombieInfo[zo_id][zombie_alcance]   = ZombieClassInfo[randClass][zombie_class_rangeattack];
	  	ZombieInfo[zo_id][zombie_shootdelay]= ZombieClassInfo[randClass][zombie_class_shootdelay];
	  	ZombieInfo[zo_id][zombie_class]     = ZombieClassInfo[randClass][zombie_class_id];
	  	ZombieInfo[zo_id][zombie_class_defalt]=randClass;
	  	ZombieInfo[zo_id][zombie_dead]      = 0;
	  	ZombieInfo[zo_id][zombie_walktime]  = gettime() + (5 + random(12));

		// Se a class tiver uma arma
		if (ZombieClassInfo[randClass][zombie_class_weapon]) {

		    FCNPC_SetWeapon(zo_id, ZombieClassInfo[randClass][zombie_class_weapon]);
//		    FCNPC_SetAmmo(zo_id, 999);
//		    FCNPC_SetWeaponClipSize(zo_id, ZombieClassInfo[randClass][zombie_class_weapon], 30);
//		    FCNPC_GiveAmmoInClip(zo_id, 30);
//	        FCNPC_ToggleInfiniteAmmo(zo_id, true);
		}

        if (ZombieClassInfo[randClass][zombie_class_weapon] == 33) {
            FCNPC_SetWeaponAccuracy(zo_id, ZombieClassInfo[randClass][zombie_class_weapon], 0.3);
        } else {
            FCNPC_SetWeaponAccuracy(zo_id, ZombieClassInfo[randClass][zombie_class_weapon], 0.5);
        }

	    ZombieInfo[zo_id][zombie_spawnx]      = x;
	    ZombieInfo[zo_id][zombie_spawny]      = y;
	    ZombieInfo[zo_id][zombie_spawnz]      = z;

	    CA_RayCastLine(ZombieInfo[zo_id][zombie_spawnx], ZombieInfo[zo_id][zombie_spawny], ZombieInfo[zo_id][zombie_spawnz] + 0.5, ZombieInfo[zo_id][zombie_spawnx], ZombieInfo[zo_id][zombie_spawny], ZombieInfo[zo_id][zombie_spawnz] - 100.0,
	    ZombieInfo[zo_id][zombie_spawnx], ZombieInfo[zo_id][zombie_spawny], ZombieInfo[zo_id][zombie_spawnz]);
	    
	    ZombieInfo[zo_id][zombie_spawnz] += 1.0;

	    FCNPC_Spawn(zo_id, 136, ZombieInfo[zo_id][zombie_spawnx], ZombieInfo[zo_id][zombie_spawny], ZombieInfo[zo_id][zombie_spawnz]);
	    FCNPC_SetVirtualWorld(zo_id, 0);
	    
	    respawnZombie(zo_id);
	}
}

public FCNPC_OnRespawn(npcid) {
    respawnZombie(npcid);
	return 1;
}

callback respawnZombie(npcid) {

	if (npcid < 0 || npcid >= MAX_PLAYERS) return 0;

	printf("spawned: %d", npcid);

	new randClass   = random(index_class - index_bandidos);

	if (ZombieInfo[npcid][zombie_class] == zombie_class_bandido) {
	    randClass = ZombieInfo[npcid][zombie_class_defalt];
	}

	// Respawnar o zombie
	if (ZombieInfo[npcid][zombie_dead]) {
//		FCNPC_Spawn(npcid);
		FCNPC_SetWeapon(npcid, 0);
		// remove attached
		for(new i; i < 10; i++)
		    RemovePlayerAttachedObject(npcid, i);
	}

	// Skin e vida do zombie
	FCNPC_SetSkin(npcid, ZombieClassInfo[randClass][zombie_class_skin]);
	FCNPC_SetHealth(npcid, ZombieClassInfo[randClass][zombie_class_resistence]);

	// Remover armas
	FCNPC_SetWeapon(npcid, 0);

	// Posição do zombie
	FCNPC_SetPosition(npcid, ZombieInfo[npcid][zombie_spawnx], ZombieInfo[npcid][zombie_spawny], ZombieInfo[npcid][zombie_spawnz]);
	
	// Informações do zombie
  	ZombieInfo[npcid][zombie_health]    = ZombieClassInfo[randClass][zombie_class_resistence];
  	ZombieInfo[npcid][zombie_detection] = ZombieClassInfo[randClass][zombie_class_detection];
  	ZombieInfo[npcid][zombie_alcance]   = ZombieClassInfo[randClass][zombie_class_rangeattack];
  	ZombieInfo[npcid][zombie_shootdelay]= ZombieClassInfo[randClass][zombie_class_shootdelay];
  	ZombieInfo[npcid][zombie_movtype]   = ZombieClassInfo[randClass][zombie_class_movtype];
  	ZombieInfo[npcid][zombie_movspeed]  = ZombieClassInfo[randClass][zombie_class_movspeed];
  	ZombieInfo[npcid][zombie_class]     = ZombieClassInfo[randClass][zombie_class_id];
  	ZombieInfo[npcid][zombie_class_defalt]= randClass;
  	ZombieInfo[npcid][zombie_dead]      = 0;
  	ZombieInfo[npcid][zombie_walktime]  = gettime() + (5 + random(12));

	// Se a class tiver uma arma
	if (ZombieClassInfo[randClass][zombie_class_weapon]) {
	
	    FCNPC_SetWeapon(npcid, ZombieClassInfo[randClass][zombie_class_weapon]);
	    FCNPC_SetAmmo(npcid, 9999);
//	    FCNPC_SetWeaponClipSize(npcid, ZombieClassInfo[randClass][zombie_class_weapon], 30);
//	    FCNPC_SetWeaponShootTime(npcid, ZombieClassInfo[randClass][zombie_class_weapon], ZombieInfo[npcid][zombie_shootdelay]);
//	    FCNPC_SetAmmoInClip(npcid, 30);
	    FCNPC_UseInfiniteAmmo(npcid, true);
	}

	// Se for bandido, retornar
	if (ZombieInfo[npcid][zombie_class] == zombie_class_bandido) {
	
	    FCNPC_SetWeaponAccuracy(npcid, ZombieClassInfo[randClass][zombie_class_weapon], 0.3);
	
	    return 0;
	}

  	// Attach objetos no zombie
	SetPlayerAttachedObject(npcid, 1, 19182, 2, 0.086999,0.065000,-0.026000,-84.499984,0.000000,0.000000,1.070000,1.887999,2.814999);// Olho 1
    SetPlayerAttachedObject(npcid, 2, 19182, 2, 0.086999,0.065000,0.026000,-84.499984,0.000000,0.000000,1.070000,1.887999,2.814999);// Olho 2

	switch (random(5)) {
	    case 0:SetPlayerAttachedObject(npcid, 3, 19583, 2, 0.170000,0.069999,0.157999,68.500000,0.000000,-32.799999,1.000000,1.000000,1.000000);
	    case 1:SetPlayerAttachedObject(npcid, 3, 19583, 1, 0.152999,0.300999,-0.018000,-6.999951,0.000000,-25.999998,1.000000,1.000000,1.000000);
	    case 2:SetPlayerAttachedObject(npcid, 3, 19583, 1, 0.400999,0.041000,0.204999,67.600059,0.000000,-56.899993,1.000000,1.000000,1.000000);
	    case 3:SetPlayerAttachedObject(npcid, 3, 19583, 8, 0.245000,0.128000,0.150999,49.800003,0.000000,0.000000,1.000000,1.000000,1.000000);
	    case 4:SetPlayerAttachedObject(npcid, 3, 19583, 12, 0.135000,0.252000,0.000000,0.000000,0.000000,0.000000,1.000000,1.000000,1.000000);
	}

	if (ZombieClassInfo[randClass][zombie_class_id] == zombie_class_bomber) {
	    SetPlayerAttachedObject(npcid, 4, 1654, 1, 0.051999,0.140000,0.057999,10.500000,-91.899986,-167.600158,0.944998,1.161000,0.783999);
		SetPlayerAttachedObject(npcid, 5, 1654, 1, 0.051999,0.140000,-0.055999,10.500000,-91.899986,-167.600158,0.944998,1.161000,0.783999);
		SetPlayerAttachedObject(npcid, 6, 1654, 1, 0.048999,-0.114999,-0.055999,10.500000,-91.899986,8.899861,0.944998,1.161000,0.783999);
		SetPlayerAttachedObject(npcid, 7, 1654, 1, 0.021999,-0.114999,-0.055999,10.500000,-91.899986,8.899861,0.944998,1.161000,0.783999);
	}

	return 1;
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if (tickBarulho < GetTickCount()) {
	
		static Float:x, Float:y, Float:z, Float:w;
		GetPlayerPos(playerid, x, y, z);

		if(!CA_RayCastLine(x, y, z, x, y, z + 100.0, w, w, w)) {
		    FazerBarulho(x, y, z);
		}
	}
	
	#if defined z_OnPlayerWeaponShot
	    return z_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, fX, fY, fZ);
	#else
    	return 1;
	#endif
}
#if defined _ALS_OnPlayerWeaponShot
	#undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif
#define OnPlayerWeaponShot z_OnPlayerWeaponShot
#if defined z_OnPlayerWeaponShot
	forward z_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

public FCNPC_OnTakeDamage(npcid, issuerid, Float:amount, weaponid, bodypart) {

	//SendClientMessageToAllf(-1, "[Debug]: %d damaged %d, health loss: %f", damagerid, npcid, health_loss);

	if (ZombieInfo[npcid][zombie_dead])
	    return 0;

//	printf("damage: %f weapon: %d", health_loss, weaponid);

	// Matar o zombie

	// Retirar a vida do zombie
	amount = convertDamage(getWeaponZDamage(weaponid), bodypart) * 1.14524;

	// Head Shot de Sniper
	if ( weaponid == 34 && bodypart == 9) {
 		amount = 255.0;
	}

	// Retirar a vida do zombie
	ZombieInfo[npcid][zombie_health] -= amount;

	if (ZombieInfo[npcid][zombie_attack] == INVALID_PLAYER_ID) {
	    // stop a zombie
	    FCNPC_Stop(npcid);
	   	// set a zombie angle
		setZombieAngleToPlayer(npcid, issuerid);
	}

	// Dar os pontos
	static Float:pos[4];
	if (fireWeapons(weaponid)) {

		// Pegar a posição do ultimo tiro
       	GetPlayerLastShotVectors(issuerid, pos[3], pos[3], pos[3], pos[0], pos[1], pos[2]);

		// Dar os pontos
       	givePlayerPoints(issuerid, floatround(amount / 2.0), pos[0]+frandom(0.03)-frandom(0.03), pos[1]+frandom(0.03)-frandom(0.03), pos[2]+frandom(0.03)-frandom(0.03));
	} else {

	    // Pegar a posição do zombie
	    FCNPC_GetPosition(npcid, pos[0], pos[1], pos[2]);

		// Dar os pontos
       	givePlayerPoints(issuerid, floatround(amount / 2.0), pos[0]+frandom(0.03)-frandom(0.03), pos[1]+frandom(0.03)-frandom(0.03), pos[2]+frandom(0.05)-frandom(0.05));

		if (weaponid == 9)
		{
		    SetWeaponHealth(issuerid, 9, GetWeaponHealth(issuerid, 9) - 0.1);
		}
	}

	if ( ZombieInfo[npcid][zombie_health] <= 0.0) {

		// Pegar a posição do player
	   	FCNPC_GetPosition(npcid, pos[0], pos[1], pos[2]);

		// Dar os pontos
	   	givePlayerPoints(issuerid, 350, pos[0]+frandom(0.03)-frandom(0.03), pos[1]+frandom(0.03)-frandom(0.03), pos[2]+0.06, 0xFFAA00CC);

		#if defined onZombieDeath
	    	onZombieDeath(npcid, issuerid, weaponid);
		#endif

	    ZombieInfo[npcid][zombie_dead] = 1;

	   	FCNPC_Kill(npcid);

	    //SetTimerEx ("respawnZombie", ZOMBIE_RESPAWN, false, "i", npcid);
	}

	return 1;
}

public FCNPC_OnDeath(npcid, killerid, reason) {

	if (!ZombieInfo[npcid][zombie_dead]) {
        onZombieDeath(npcid, killerid, reason);
	}
	printf("death: %d", npcid);

	return 1;
}

public FCNPC_OnSpawn(npcid) {

	respawnZombie(npcid);

	return 1;
}


stock CreateServerBandidos() {

	// Bandidos
	spawnServerBandido(388.9094,2440.7532,16.5000,333.3851);
	spawnServerBandido(398.3870,2438.5962,16.5000,339.2185);
	spawnServerBandido(411.4981,2440.3665,16.5000,272.8558);
	spawnServerBandido(384.9623,2611.8062,16.4844,99.6143);
	spawnServerBandido(386.1747,2606.4480,16.4844,199.8175);
	spawnServerBandido(1149.2946,2335.7456,16.7188,76.1005);
	spawnServerBandido(1137.3976,2333.9402,16.7188,96.0722);
	spawnServerBandido(1126.4579,2335.8284,16.7188,75.5276);
	spawnServerBandido(1171.0029,2113.7922,10.8203,117.0584);
	spawnServerBandido(1168.3643,2113.9915,10.8203,117.0584);
	spawnServerBandido(1172.1573,2110.2693,10.8203,211.3868);
	spawnServerBandido(1069.3285,1226.1627,10.8275,100.9319);
	spawnServerBandido(1064.0323,1227.1565,10.8275,79.2828);
	spawnServerBandido(1092.6399,1078.0967,10.8359,174.2740);
	spawnServerBandido(1091.0881,1074.9298,10.8359,174.2740);
	spawnServerBandido(1288.9038,293.7601,25.7998,64.3918);
	spawnServerBandido(1286.2742,291.8633,25.7998,154.3919);
	spawnServerBandido(1320.6333,184.3171,20.1089,324.0505);
	spawnServerBandido(1321.7906,189.3620,19.5074,324.0505);
	spawnServerBandido(1541.1641,18.9019,24.1406,89.5272);
	spawnServerBandido(1545.7368,15.2188,24.1357,210.3647);
	spawnServerBandido(1912.5371,-572.6223,26.9702,183.4138);
	spawnServerBandido(1916.7297,-574.8104,26.5743,228.4138);
	spawnServerBandido(1919.9447,-578.3021,26.6931,228.4138);
	spawnServerBandido(2472.7034,-947.9433,80.1175,33.4987);
	spawnServerBandido(2472.5886,-943.8443,80.1041,33.4987);
	spawnServerBandido(2666.8125,-1435.2189,30.5138,198.5253);
	spawnServerBandido(2871.6140,-1590.0352,22.4522,336.1522);
	spawnServerBandido(2806.4849,-1182.2898,25.4082,78.5644);
	spawnServerBandido(2819.5901,-1180.9313,25.2404,238.8622);
	spawnServerBandido(2819.1348,-1184.5635,25.2343,238.8622);
	spawnServerBandido(2795.2505,-2013.9386,13.5547,98.0046);
	spawnServerBandido(2792.7520,-2011.5549,13.5547,98.0046);
	spawnServerBandido(2793.5750,-2017.5746,13.5547,181.2876);
	spawnServerBandido(216.9718,1815.7351,6.4141,299.0773);
	spawnServerBandido(252.2530,1802.5450,7.4141,78.7860);
	spawnServerBandido(2798.2698,-2557.5425,13.6320,23.4199);
	spawnServerBandido(2788.2974,-2553.1687,13.6331,57.8818);
	spawnServerBandido(1401.2252,-2199.2075,13.5391,330.8435);
	spawnServerBandido(1401.0386,-2196.6313,13.5391,330.8435);
	spawnServerBandido(1399.6201,-2193.2922,13.5391,60.8435);
	spawnServerBandido(1133.7809,-2038.9978,69.0078,164.6637);
	spawnServerBandido(1135.9255,-2042.8569,69.0078,209.6638);
	spawnServerBandido(371.6771,-1697.8158,28.5648,30.5718);
	spawnServerBandido(367.7953,-1696.0032,29.0110,30.5718);
	spawnServerBandido(-101.2332,-1568.5641,2.6107,77.4047);
	spawnServerBandido(-356.9950,-2156.8132,43.4339,291.2452);
	spawnServerBandido(27.6285,-2637.0957,40.4179,186.3135);
	spawnServerBandido(-2689.9243,232.2542,4.3359,87.3459);
	spawnServerBandido(-2690.0242,234.7666,4.3359,357.3459);
	spawnServerBandido(-2989.3005,465.5615,4.9141,77.7085);
	spawnServerBandido(-2989.0005,467.9530,4.9141,347.7084);
	spawnServerBandido(-2988.2781,471.2708,4.9141,347.7084);
	spawnServerBandido(-2696.3875,1269.0758,55.4297,350.4492);
	spawnServerBandido(-2695.3979,1274.9579,55.4297,350.4492);
	spawnServerBandido(-1848.0148,1298.4210,59.7344,75.6372);
	spawnServerBandido(-1847.4585,1301.4558,59.7344,345.6371);
	spawnServerBandido(-1844.0579,1302.8904,59.7344,255.6372);
	spawnServerBandido(-1345.6986,492.0405,11.2027,201.5550);
	spawnServerBandido(-1347.6953,492.9706,11.1953,21.5550);
	spawnServerBandido(-1384.5785,409.8881,7.1875,235.4540);

	// Snipers
	spawnServerBandido(111.5162,1814.0088,33.8984,15.2201, 179);
	spawnServerBandido(233.2907,1936.5044,33.8984,93.7810, 179);
	spawnServerBandido(71.7867,2221.1709,69.4954,297.6805, 179);
	spawnServerBandido(950.9907,1675.5441,15.7941,343.1872, 179);
	spawnServerBandido(1413.2395,229.7800,24.0027,198.1321, 179);
	spawnServerBandido(1768.8038,-173.6761,79.6720,245.2685, 179);
	spawnServerBandido(2702.4668,-1286.5637,72.9453,182.7509, 179);
	spawnServerBandido(2876.1707,-1590.9764,22.3652,246.1522, 179);
	spawnServerBandido(2725.5203,-2413.3013,29.6781,267.3040, 179);
	spawnServerBandido(2725.6609,-2394.3638,29.6781,220.6922, 179);
	spawnServerBandido(2725.6958,-2491.7463,29.7781,344.3189, 179);
	spawnServerBandido(1117.1115,-2020.5099,74.4297,144.4784, 179);
	spawnServerBandido(1119.3590,-2037.8889,78.2109,205.4496, 179);
	spawnServerBandido(153.8093,-1951.8235,51.3438,99.2748, 179);
	spawnServerBandido(-2718.4314,-319.2772,57.4873,97.0659, 179);
	spawnServerBandido(-2656.9382,1157.3969,55.5781,192.0570, 179);
	spawnServerBandido(-2659.0349,1364.7178,67.4726,167.3151, 179);
	spawnServerBandido(-2472.2703,1546.0845,36.7992,1.2737, 179);
	spawnServerBandido(-1746.1456,1306.8235,33.1094,210.6130, 179);
	spawnServerBandido(-1455.0417,771.7914,46.7422,142.1307, 179);
	spawnServerBandido(-1299.6462,733.3826,34.5781,303.1737, 179);
	spawnServerBandido(-1381.7216,493.5902,30.9525,269.2361, 179);
}

forward OnZombieAttackChange(zombieid, newplayer, oldplayer);
