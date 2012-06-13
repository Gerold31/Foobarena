wait=coroutine.yield;
Console.Print("Arena.lua script loaded.\n");

function Game()
	wait(5);
	
	local timer = 5;
	
	while true do
		if countRobots() == 0 then
			if timer > 0 then
				--GUI
			end
			-- spawn robots
			spawnRobot1();
			wait(0.1);
			spawnRobot2();
			wait(0.1);
			spawnRobot3();
			wait(0.1);
			spawnRobot4();
		else
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

function countEntities()
	local entities = 0
	for k, v in pairs(_G) do                      -- Iterate over all global variables.
		if type(v)=="table" and v.GetName then    -- Implement a very simple type check.
			entities=entities+1
		end
	end
	Console.Print("entities: " .. entities .. "\n");
	return entities
end

function spawnRobot1()
	RobotSpawner_001:spawnRobot(-1, -1, -1, -1);
end
function spawnRobot2()
	RobotSpawner_002:spawnRobot(-1, -1, -1, -1);
end
function spawnRobot3()
	RobotSpawner_003:spawnRobot(-1, -1, -1, -1);
end
function spawnRobot4()
	RobotSpawner_004:spawnRobot(-1, -1, -1, -1);
end


thread(Game);

Console.Print("Game thread started.\n");