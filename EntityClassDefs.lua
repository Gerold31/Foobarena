-- Let Foobarena maps be up to 131072 (65536*2) units big in each direction. This corresponds to a side length of 3329,23 km.
-- Note: If this gets any bigger than 65536, CaWE needs revision, especially cmap brush loading (starting digging from LoadSave_cmap.cpp)!
Mapsize={ -65536, 65536 }


-- Returns a clone of the given argument "node".
-- This is especially interesting when node is a table, because then a deep copy is made.
function clone(node)
    if type(node)~="table" then return node end

    -- node is a table, make a deep copy.
    local b={};
    table.foreach(node, function(k,v) b[k]=clone(v) end);

    return b;
end


-- This function takes a variable number of arguments, all of which must be of type "table".
-- It deep-copies all tables into one common table, and returns the combined result.
function newEntClassDef(...)
    local ResultTable={};

    for i, SubTable in ipairs{...} do
        -- Put a clone of each element of SubTable into ResultTable.
        for k, v in pairs(SubTable) do
            ResultTable[k]=clone(v);
        end
    end

    return ResultTable;
end


-- A base class with common definitions.
Common=
{
    name=
    {
        type       ="string";
        description="The name of this entity. It must be unique and non-empty.";
     -- editor_useredit=true;
        uniqueValue=true;
    };

    -- origin={ ... };
}


-- A base class for orientable entities.
Angles=
{
    angles=
    {
        type       ="string";
        description="Bank Heading Pitch (Y Z X)";
        value      ="0 0 0";
    };
}


-- This table contains the definitions of all entity classes in the game.
-- * Connection to C++ classes (CppClass key).
-- * Defines supported variables (properties, key/value pairs) by that (C++) entity class.
-- * Default values for instantiated entities.
-- * Auxiliary information for the Ca3DE World Editor CaWE.
EntityClassDefs={}


EntityClassDefs["HumanPlayer"]=newEntClassDef(Common,
{
    isPoint    =true;
    CppClass   ="EntHumanPlayerT";
    description="A human player entity. Should never be placed in a map or even shown in the editor, it just doesn't make sense.";
})

-- This is the "complement" to the texture area lights.
EntityClassDefs["PointLight"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
 -- CppClass   =...;    -- Cannot instantiate this in a map, it's for CaLight only.
    description="Radiosity Point Light";
 -- iconsprite("sprites/lightbulb.spr")

    opening_angle=
    {
        type       ="integer";
        description="Opening angle [0..180]";
        value      =180;
    };

    intensity_r=
    {
        type       ="integer";
        description="Intensity [W/sr] red";
    };

    intensity_g=
    {
        type       ="integer";
        description="Intensity [W/sr] green";
    };

    intensity_b=
    {
        type       ="integer";
        description="Intensity [W/sr] blue";
    };
})


-- An entity for a dynamic light source (like Doom3 lights).
EntityClassDefs["PointLightSource"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
    CppClass   ="EntPointLightSourceT";
    description="Dynamic Light Source";
 -- iconsprite("sprites/lightbulb.spr")

    light_radius=
    {
        type       ="integer";
        description="Dyn. light radius";
        value      ="500";
    };

    light_color_diff=
    {
        type       ="color";
        description="Dyn. light diffuse color";
        value      ="255 255 128";      -- Provide users with a reasonable default (warm yellow-white).
    };

    light_color_spec=
    {
        type       ="color";
        description="Dyn. light specular color";
        value      ="255 255 128";      -- Provide users with a reasonable default (warm yellow-white).
    };

    light_casts_shadows=
    {
        type       ="flags";    -- This should actually be "bool".
        description="Flags about whether the light casts shadows.";
        flags      ={ "Light casts shadows", true };
    };
})


-- Rename this to "generic"?   "A generic entity with no special purposes."
EntityClassDefs["func_wall"]=newEntClassDef(Common,
{
    isSolid    =true;
    description="A wall.";
})

EntityClassDefs["func_water"]=newEntClassDef(Common,
{
    isSolid    =true;
    description="A body of water.";
})


-- Static detail models.
-- Note: There is no clipping (collision detection) for these models! If necessary, use explicit "clip" brushes.
EntityClassDefs["static_detail_model"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
    CppClass   ="EntStaticDetailModelT";
    description="A static detail model.";
    size       ={ { -16, -16, -24 }, { 16, 16, 24 } };
    color      ={ 255, 255, 0 };
    helpers    ={ model={} };

    model=
    {
        type       ="file_model";
        description="Model file name";
    };

    collisionModel=
    {
        type       ="file";
        description="Collision Model file name";
    };

    sequence=
    {
        type       ="integer";
        description="Model sequence";
        value      =0;
    };

    scale=
    {
        type       ="string";
        description="Model scale";
        value      ="1.0";
    };

    gui=
    {
        type       ="file";
        description="GUI file name";
    };
})


EntityClassDefs["speaker"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
    CppClass   ="EntSpeakerT";
    description="An invisible speaker that can playback a sound shader or file.";

    soundshader=
    {
        type       ="string";
        description="Name of sound shader or file for playback, e.g. Games/Foobarena/Sounds/jungle.wav";
        value      ="";
    };

    autoplay=
    {
        type       ="flags";    -- This should actually be "bool".
        description="Whether the sound starts playing automatically when the entity is created, or if it has to be triggered first.";
        flags      ={ "autoplay", true };
    };

    interval=
    {
        type       ="float";
        description="Time in seconds between two successive sound playbacks. 0 means that the sound is only played once.";
        value      =5;
    };

    innerCone=
    {
        type       ="integer";
        description="Apex angle of the speakers inner sound cone (where the sound is at its normal volume).";
        value      =360;
    };

    innerVolume=
    {
        type       ="float";
        description="This is the main volume control for the sound (inside the inner cone and before distance attenuation). Typically set between 0.0 and 1.0, but values larger than 1.0 are possible.";
        value      =1;
    };

    outerCone=
    {
        type       ="integer";
        description="Apex angle of the speakers outer sound cone (where the sound is at its outer volume).";
        value      =360;
    };

    outerVolume=
    {
        type       ="float";
        description="Sound volume beyond the outer cone. Relative to the innerVolume, thus the value must be between 0.0 and 1.0.";
        value      =0;
    };
})


-- This entity class represents rigid bodys such as crates, barrels, or arbitrarily complex objects.
-- Their motion is controlled by rigid body dynamics as implemented by the physics engine.
EntityClassDefs["Rigid Body"]=newEntClassDef(Common,
{
    isSolid    =true;
    CppClass   ="EntRigidBodyT";
    description="A rigid body whose motion is controlled by rigid body dynamics as implemented by the physics engine.";

    Mass=
    {
        type       ="float";
        description="The mass of the body, in kilograms [kg]. (1 pound are 0.454 kilograms.)";
        value      =10;
    };

    Shape=
    {
        type       ="choice";
        description="The shape of the body used for the physics computations. \"Visual Elements\" means that the physical shape matches the visual shape.";
        choices    ={ "Box", "Sphere", "Visual Elements" };    -- TODO: Need more default shapes!
    };

    -- TODO: friction, restitution (are these material properties??)
    -- TODO: does explode? can destroy?
    -- TODO: start active??
})


-- Info Player Start.
-- This is not a game-specific entity, but rather an engine entity, simply because it is sensible for each game to have at least one such.
-- Moreover, CaBSP even requires for its proper operation that at least one such entity exists, and that they all are *INSIDE* the actual map!
EntityClassDefs["info_player_start"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
    CppClass   ="EntInfoPlayerStartT";
    description="A starting location for the player.";
    size       ={ { -16, -16, -36 }, { 16, 16, 36 } };      -- Rename in editor_bbox? Rename color in editor_bbox_color?
    color      ={ 0, 255, 0 };
})



EntityClassDefs["Robot"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
    CppClass   ="EntRobotT";
    description="Robot";
    size       ={ { -16, -16, -36 }, { 16, 16, 36 } };

    TorsoID=
    {
        type       ="integer";
        description="TorsoID";
		value      =0;
    };
	
    HeadID=
    {
        type       ="integer";
        description="HeadID";
		value      =0;
    };
	
    WeaponID=
    {
        type       ="integer";
        description="WeaponID";
		value      =0;
    };
	
	MovementType=
    {
        type       ="integer";
        description="MovementType";
		value      =0;
    };
	
    MovementID=
    {
        type       ="integer";
        description="MovementID";
		value      =0;
    };
	
})

EntityClassDefs["RobotPart"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
    CppClass   ="EntRobotPartT";
    description="RobotPart";
    size       ={ { -16, -16, -36 }, { 16, 16, 36 } };
})