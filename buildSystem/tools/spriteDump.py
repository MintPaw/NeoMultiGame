bl_info = {
    "name": "spriteDump",
    "blender": (2, 80, 0),
    "category": "All"
}

import bpy
import bpy_extras
import os
import shutil

poseMarkersOutString = ""
totalFrameCount = 0

def setupFileFunc():
    scn = bpy.data.scenes[0]
    world = bpy.data.worlds["World"]

    if world.get("subSteps") is None:
        world["subSteps"] = 1

    scn.render.engine = "BLENDER_EEVEE"
    scn.render.image_settings.compression = 100

    bpy.context.scene.render.film_transparent = True
    bpy.context.scene.render.image_settings.compression = 0
    bpy.data.scenes["Scene"].render.use_overwrite = True

    global totalFrameCount
    totalFrameCount = 0

def dumpPoseMarkers(armature):
    scn = bpy.data.scenes[0]
    world = bpy.data.worlds["World"]
    unitName = armature.name[4:]
    outPath = "C:/bin/frames/spriteDump/"+unitName+"/params.vars"

    subSteps = world["subSteps"]

    global poseMarkersOutString;
    f = open(outPath, "w")
    f.write(poseMarkersOutString)
    f.close()
    poseMarkersOutString = "";

    f = open("C:/bin/frames/spriteDump/"+unitName+"/needsReprocess.dummy", "w")
    f.write("dummy file")
    f.close()

def exportImage(outPath, animFileName):
    scn = bpy.data.scenes[0]
    world = bpy.data.worlds["World"]

    path = outPath
    path += "/"
    path += animFileName
    path += ".png"

    scn.render.filepath = path
    print("["+totalFrameCount+"] Saving to: "+path)

    scn.view_layers[0].update()
    bpy.ops.render.render(write_still=True)
    #bpy.ops.render.opengl(animation=False, render_keyed_only=False, sequencer=False, write_still=True, view_context=False)

def exportAction(armature, action, outPath, altName=None):
    scn = bpy.data.scenes[0]
    world = bpy.data.worlds["World"]
    camera = bpy.context.scene.objects["Camera"]
    viewLayer = scn.view_layers[0]

    dopeSheetArea = None
    for a in bpy.context.screen.areas:
        if a.type == 'DOPESHEET_EDITOR':
            dopeSheetArea = a.spaces[0]
            break
    
    if dopeSheetArea is None:
        print("No dopesheet")
        raise Exception("Bad")

    realAction = action if action != None else dopeSheetArea.action # I don't think I need to do this...

    animName = realAction.name
    if altName is not None: animName = altName

    files = os.listdir(outPath)
    filtered_files = [file for file in files if (animName in file)]
    for file in filtered_files:
        path_to_file = os.path.join(outPath, file)
        os.remove(path_to_file)

    print("Exporting action "+animName+"("+realAction.name+") to "+outPath)
    dopeSheetArea.action = realAction
    subSteps = world["subSteps"]

    for i in range(int(realAction.frame_range.x), int(realAction.frame_range.y)):
        frameIndex = i
        for i in range(0, subSteps):
            scn.frame_set(frameIndex, subframe=i/subSteps)

            animFileName = animName
            animFileName += "_"
            animFileName += str(frameIndex*subSteps + i).zfill(3)
            exportImage(outPath, animFileName)
            totalFrameCount += 1

    global poseMarkersOutString
    for i in range(0, len(realAction.pose_markers)):
        marker = realAction.pose_markers[i]
        poseMarkersOutString += animName + " "
        poseMarkersOutString += str(marker.frame*subSteps) + " "
        poseMarkersOutString += marker.name
        poseMarkersOutString += "\n"

class MESH_OP_generate_all(bpy.types.Operator):
    bl_idname = "mesh.generate_all"
    bl_label = "generate_all"
    bl_options = {"REGISTER"}

    def execute(self, context):
        print("Generating all sprite sheets")
        setupFileFunc()
        scn = bpy.data.scenes[0]
        world = bpy.data.worlds["World"]

        unitName = None
        for obj in scn.objects:
            if "ARM_" in obj.name:
                unitName = obj.name[4:]

        print(unitName)
        outPath = "C:/bin/frames/spriteDump/"+unitName

        shutil.rmtree(outPath, ignore_errors=True)
        os.makedirs(outPath)

        armature = scn.objects["ARM_"+unitName]

        if armature == None:
            log("No arm")
            raise Exception("Bad")

        if len(armature.children) != 1:
            log("I don't know how to handle more than one child")
            raise Exception("Bad")

        for action in bpy.data.actions:
            exportAction(armature, action, outPath)

        print("Done generating all sprite sheets")
        dumpPoseMarkers(armature)
        return {"FINISHED"}

class MESH_OP_generate_concrete_jungle(bpy.types.Operator):
    bl_idname = "mesh.generate_concrete_jungle"
    bl_label = "generate_concrete_jungle"
    bl_options = {"REGISTER"}

    def execute(self, context):
        setupFileFunc()
        scn = bpy.data.scenes[0]
        world = bpy.data.worlds["World"]

        unitName = "Unit"
        outPath = "C:/bin/frames/spriteDump/"+unitName

        shutil.rmtree(outPath, ignore_errors=True)
        os.makedirs(outPath)

        armature = scn.objects["ARM_"+unitName]

        actions = bpy.data.actions
        bpy.data.scenes[0].objects["sword"].hide_render = True
        bpy.data.scenes[0].objects["knife"].hide_render = True

        exportAction(armature, actions["idle"], outPath, "idle");
        exportAction(armature, actions["walk"], outPath, "walk");
        exportAction(armature, actions["run"], outPath, "run");

        exportAction(armature, actions["startPickup"], outPath, "startPickup");
        exportAction(armature, actions["endPickup"], outPath, "endPickup");
        exportAction(armature, actions["toss"], outPath, "toss");

        exportAction(armature, actions["blockstun"], outPath, "blockstun");
        exportAction(armature, actions["hitstun"], outPath, "hitstun");
        exportAction(armature, actions["airHitstun"], outPath, "airHitstun");
        exportAction(armature, actions["knockdown"], outPath, "knockdown");
        exportAction(armature, actions["raising"], outPath, "raising");

        exportAction(armature, actions["jump"], outPath, "jump");
        exportAction(armature, actions["airKick"], outPath, "airKick");
        exportAction(armature, actions["airPunch"], outPath, "airPunch");

        exportAction(armature, actions["punch1"], outPath, "punch1");
        exportAction(armature, actions["punch2"], outPath, "punch2");
        exportAction(armature, actions["uppercut"], outPath, "uppercut");
        exportAction(armature, actions["kick"], outPath, "kick");

        exportAction(armature, actions["runningKick"], outPath, "runningKick");
        exportAction(armature, actions["runningPunch"], outPath, "runningPunch");

        exportAction(armature, actions["upTest"], outPath, "upTest");
        exportAction(armature, actions["duckTest"], outPath, "duckTest");

        bpy.data.scenes[0].objects["sword"].hide_render = False

        exportAction(armature, actions["endPickup_sword"], outPath, "endPickup_sword");
        exportAction(armature, actions["idle_sword"], outPath, "idle_sword");
        exportAction(armature, actions["attack1_sword"], outPath, "attack1_sword");

        exportAction(armature, actions["walk_sword"], outPath, "walk_sword");
        exportAction(armature, actions["run_sword"], outPath, "run_sword");

        exportAction(armature, actions["jump_sword"], outPath, "jump_sword");
        exportAction(armature, actions["airAttack1_sword"], outPath, "airAttack1_sword");

        exportAction(armature, actions["throw"], outPath, "throw_sword");
        exportAction(armature, actions["airThrow"], outPath, "airThrow_sword");
        exportAction(armature, actions["toss"], outPath, "toss_sword");

        bpy.data.scenes[0].objects["sword"].hide_render = True
        bpy.data.scenes[0].objects["knife"].hide_render = False

        exportAction(armature, actions["endPickup"], outPath, "endPickup_knife")
        exportAction(armature, actions["idle_knife"], outPath, "idle_knife");
        exportAction(armature, actions["attack1_knife"], outPath, "attack1_knife");

        exportAction(armature, actions["walk"], outPath, "walk_knife")
        exportAction(armature, actions["run"], outPath, "run_knife")

        exportAction(armature, actions["jump_knife"], outPath, "jump_knife")
        exportAction(armature, actions["airAttack1_knife"], outPath, "airAttack1_knife")

        exportAction(armature, actions["throw"], outPath, "throw_knife")
        exportAction(armature, actions["airThrow"], outPath, "airThrow_knife")
        exportAction(armature, actions["toss"], outPath, "toss_knife");

        dumpPoseMarkers(armature)
        return {"FINISHED"}

def register():
    bpy.utils.register_class(MESH_OP_generate_all)
    bpy.utils.register_class(MESH_OP_generate_concrete_jungle)

def unregister():
    bpy.utils.unregister_class(MESH_OP_generate_all)
    bpy.utils.unregister_class(MESH_OP_generate_concrete_jungle)

if __name__ == "__main__":
    register()
