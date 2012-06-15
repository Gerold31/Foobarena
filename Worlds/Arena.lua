
wait=coroutine.yield;
Console.Print("Arena.lua script loaded.\n");

maxRobots = 4;

function Game()
	wait(1);
	
	local timer = 5;
	
	local round = 0;
	
	local robotsPerRound = 2
	local robotsToSpawn = 0;
	
	while true do
		if countRobots() == 0 and robotsToSpawn == 0 then
			if timer == 5 then
				Player1:giveAmmo(30);
			end
			if timer > 0 then
				Player1:printHUD("Round " .. round+1 .. " starts in " .. timer);
				timer = timer - 1;
				
			elseif timer == 0 then
				round = round + 1;
				timer = timer - 1;
				robotsToSpawn = robotsPerRound * round;
							
			else 
				timer = 5;
								
			end
		else
			Player1:printHUD("Round " .. round .. ": " .. robotsToSpawn + countRobots() .. " robots left");
			
			
			local robots = math.min(maxRobots - countRobots(), robotsToSpawn);
			Console.Print("min(" .. maxRobots - countRobots() .. ", " .. robotsToSpawn .. ")==" .. robots .. "\n");
			spawnRobots(robots);
			robotsToSpawn = robotsToSpawn - robots;
		end
		wait(1);
	end
end

function countRobots()
	local robots = 0
	for k, v in pairs(_G) do                      -- Iterate over all global variables.
		if type(v)=="table" and v.GetName then    -- Implement a very simple type check.
			if(string.find(v:GetName(), "Robot_") == 1) then
				robots=robots+1
			end
		end
	end
	return robots
end

function spawnRobots(robots)	
	for i=1, robots, 1 do
		spawnRobot(math.random(4));
		wait(0.1);
	end	
end

function spawnRobot(i)
	if i == 1 then
		RobotSpawner_001:spawnRobot(-1, -1, -1, -1);
	elseif i == 2 then
		RobotSpawner_002:spawnRobot(-1, -1, -1, -1);
	elseif i == 3 then
		RobotSpawner_003:spawnRobot(-1, -1, -1, -1);
	else
		RobotSpawner_004:spawnRobot(-1, -1, -1, -1);
	end
end


thread(Game);

Console.Print("Game thread started.\n");