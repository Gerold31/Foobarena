wait=coroutine.yield;
Console.Print("Arena.lua script loaded.\n");

function Game()
	while true do
		wait(1);
	end
end

function countRobots()
	local robots = 0
	for k, v in pairs(_G) do                      -- Iterate over all global variables.
		if type(v)=="table" and v.GetName then    -- Implement a very simple type check.
			if(v.GetName=="Robot") then
				robots=robots+1
			end
		end
	end
	return robots
end

function spawnRobot1()
	RobotSpawner_001:spawnRobot(1, -1, -1, -1);
end
function spawnRobot2()
	RobotSpawner_002:spawnRobot(1, -1, -1, -1);
end
function spawnRobot3()
	RobotSpawner_003:spawnRobot(1, -1, -1, -1);
end
function spawnRobot4()
	RobotSpawner_004:spawnRobot(1, -1, -1, -1);
end

thread(Game);


Console.Print("Game thread started.\n");