#include "../modulos/zombies/core.inc"


new ZombieNodeIndex[MAX_PLAYERS];

new Float:PlayerNodesX[MAX_PLAYERS][MAX_PLAYER_NODES];
new Float:PlayerNodesY[MAX_PLAYERS][MAX_PLAYER_NODES];
new Float:PlayerNodesZ[MAX_PLAYERS][MAX_PLAYER_NODES];


stock addPlayerNode(playerid) {

	if(IsPlayerInRangeOfPoint(playerid, NODES_MIN_DISTANCE, PlayerNodesX[playerid][0], PlayerNodesY[playerid][0], PlayerNodesZ[playerid][0]) || PlayerData[playerid][dServerState] != SERVER_STATE_SURVIVAL) {
	    return 0;
	}

	if(PlayerData[playerid][dCountZombies] == 0) {
	    return 0;
	}

	static Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);

	for(new player_node = MAX_PLAYER_NODES - 1; player_node > 0; player_node--) {
	    PlayerNodesX[playerid][player_node] = PlayerNodesX[playerid][player_node - 1];
	    PlayerNodesY[playerid][player_node] = PlayerNodesY[playerid][player_node - 1];
	    PlayerNodesZ[playerid][player_node] = PlayerNodesZ[playerid][player_node - 1];
	}
	PlayerNodesX[playerid][0] = x;
    PlayerNodesY[playerid][0] = y;
    PlayerNodesZ[playerid][0] = z;

    return 1;
}

stock IsPointZValid(Float:z1, Float:z2) {

	if ( (z2 < z1 - 1.3) || (z2 > z1 + 1.3) ) return 0;

	return 1;
}

stock CA_TraceLineEx(zombie, Float:x, Float:y, Float:z, Float:endx, Float:endy, Float:endz, Float:stepsize=1.45, maxnodes = 1)
{
	static Float:a;

	// Resetar o caminho
    ZombieNodeIndex[zombie] = 0;

	static Float:lastx;
	static Float:lasty;
	static Float:lastz;
	new Float:point_distance = GetPointDistanceToPoint(x, y, z, endx, endy, endz);
	new Float:point_angle    = GetPointAngleToPoint(x, y, endx, endy);

	lastx = x;
	lasty = y;
   	lastz = z;

	for (new Float:point; point < point_distance; point += stepsize)
	{
		x += (stepsize * floatsin(-point_angle, degrees));
		y += (stepsize * floatcos(-point_angle, degrees));

		if (CA_RayCastLine(x, y, z, x, y, z - 70.0, x, y, z))
			z += 1.1;

		if (!IsPointZValid(z, lastz) || CA_RayCastLine(lastx, lasty, lastz, x, y, z, a, a, a))
			return 0;

		lastx = x;
		lasty = y;
	   	lastz = z;

   		PlayerNodesX[zombie][ZombieNodeIndex[zombie]] = x;
   		PlayerNodesY[zombie][ZombieNodeIndex[zombie]] = y;
   		PlayerNodesZ[zombie][ZombieNodeIndex[zombie]] = z;

   		ZombieNodeIndex[zombie] ++;

		if (ZombieNodeIndex[zombie] >= maxnodes) return 1;
	}

	return 1;
}


stock CA_TraceLine(zombie, playerid, Float:x, Float:y, Float:z, Float:endx, Float:endy, Float:endz, Float:stepsize=1.45, nodeid = 999, maxnodes = 1)
{
	if (nodeid == 0) {

		static Float:px;
		static Float:py;
		static Float:pz;
		GetPlayerPos(playerid, px, py, pz);

		endx = px;
		endy = py;
		endz = pz;
	}

	static Float:tx;
	static Float:ty;
	static Float:tz;

	if (CA_RayCastLine(x, y, z, endx, endy, endz, tx, ty, tz))
		return 0;
		
    CA_RayCastLine(endx, endy, endz, endx, endy, endz - 50.0, tx, ty, tz);
    tz += 1.0;

	// Resetar o caminho
    ZombieNodeIndex[zombie] = 0;

	static Float:lastx;
	static Float:lasty;
	static Float:lastz;
	new Float:point_distance = GetPointDistanceToPoint(x, y, z, endx, endy, tz);
	new Float:point_angle    = GetPointAngleToPoint(x, y, endx, endy);

	lastx = x;
	lasty = y;
   	lastz = z;

	if (nodeid != 999)
	{
		if (PlayerNodesX[playerid][nodeid] == ZombieInfo[zombie][zombie_lastx] && point_distance < 1.0)
		{
        	return -2;
		}
	}

	for (new Float:point; point < point_distance; point += stepsize)
	{
		x += (stepsize * floatsin(-point_angle, degrees));
		y += (stepsize * floatcos(-point_angle, degrees));

		if (CA_RayCastLine(x, y, z, x, y, z - 70.0, x, y, z))
			z += 1.1;

		if (!IsPointZValid(z, lastz) || CA_RayCastLine(lastx, lasty, lastz, x, y, z, tz, tz, tz))
			return 0;

		lastx = x;
		lasty = y;
	   	lastz = z;

   		PlayerNodesX[zombie][ZombieNodeIndex[zombie]] = x;
   		PlayerNodesY[zombie][ZombieNodeIndex[zombie]] = y;
   		PlayerNodesZ[zombie][ZombieNodeIndex[zombie]] = z;

   		ZombieNodeIndex[zombie] ++;

		if (ZombieNodeIndex[zombie] >= maxnodes) return 1;
	}

	return 1;
}

stock MoveZombieToPlayer(zombie, playerid) {

	static sucess, nodeid, Float:x, Float:y, Float:z, Float:px, Float:py, Float:pz;

    nodeid = -1;
    
    sucess = false;

	FCNPC_GetPosition(zombie, x, y, z);

	GetPlayerPos(playerid, px, py, pz);

    sucess = CA_TraceLine(zombie, playerid, x, y, z, px, py, pz, .maxnodes = 1);
    
	if (!sucess)
	{
		for (new node; node != MAX_PLAYER_NODES; node++)
		{
		    new result = CA_TraceLine(zombie,playerid,x, y, z,PlayerNodesX[playerid][node],PlayerNodesY[playerid][node],PlayerNodesZ[playerid][node], .nodeid = node, .maxnodes = 1);

			if(result == -2)
			    break;

			if (result)
			{
				nodeid = node;
				
				ZombieInfo[zombie][zombie_lastx] = PlayerNodesX[playerid][node];
				ZombieInfo[zombie][zombie_lasty] = PlayerNodesY[playerid][node];
				ZombieInfo[zombie][zombie_lastz] = PlayerNodesZ[playerid][node];
				
				break;
		    }
		}
	}

	if ( nodeid != -1 || sucess )
	{
	    FCNPC_CreateMovement(zombie);

	    for(new nodes; nodes < ZombieNodeIndex[zombie]; nodes++)
		{
	        // Add movimentos
            FCNPC_AddMovement(zombie, PlayerNodesX[zombie][nodes], PlayerNodesY[zombie][nodes], PlayerNodesZ[zombie][nodes]);
		}

		// Aplicar o movimento
		FCNPC_PlayMovement(zombie, ZombieInfo[zombie][zombie_movtype], ZombieInfo[zombie][zombie_velocity]);
		
		return 1;
	}
	else
	{
		FCNPC_Stop(zombie);
	}

	return 0;
}

