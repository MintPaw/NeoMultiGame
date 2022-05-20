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
    print("Saving to: "+path)

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

        exportAction(armature, actions["idle"], outPath)
        exportAction(armature, actions["walk"], outPath)
        exportAction(armature, actions["run"], outPath)

        exportAction(armature, actions["startPickup"], outPath)
        exportAction(armature, actions["endPickup"], outPath)

        exportAction(armature, actions["blockstun"], outPath)
        exportAction(armature, actions["hitstun"], outPath)
        exportAction(armature, actions["airHitstun"], outPath)
        exportAction(armature, actions["knockdown"], outPath)
        exportAction(armature, actions["raising"], outPath)

        exportAction(armature, actions["jump"], outPath)
        exportAction(armature, actions["airKick"], outPath)
        exportAction(armature, actions["airPunch"], outPath)

        exportAction(armature, actions["punch1"], outPath)
        exportAction(armature, actions["punch2"], outPath)
        exportAction(armature, actions["uppercut"], outPath)
        exportAction(armature, actions["kick"], outPath)

        exportAction(armature, actions["runningKick"], outPath)
        exportAction(armature, actions["runningPunch"], outPath)

        exportAction(armature, actions["upTest"], outPath)
        exportAction(armature, actions["duckTest"], outPath)

        bpy.data.scenes[0].objects["sword"].hide_render = False

        exportAction(armature, actions["idle_sword"], outPath)
        exportAction(armature, actions["walk_sword"], outPath)
        exportAction(armature, actions["run_sword"], outPath)
        exportAction(armature, actions["pickupEnd_sword"], outPath)

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
