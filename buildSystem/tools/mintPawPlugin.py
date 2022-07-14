bl_info = {
    'name': 'MintPaw plugin',
    'category': 'All'
}

import struct
import os
import mathutils
import bpy
import bmesh

poseMarkersOutString = ""

def writeString(ba, string):
    stringLength = len(string);
    ba.extend(struct.pack("<"+str(stringLength+1)+"s", string.encode("utf-8")))

def writeMatrix(ba, matrix):
    ba.extend(struct.pack("<1f", matrix[0][0]))
    ba.extend(struct.pack("<1f", matrix[0][1]))
    ba.extend(struct.pack("<1f", matrix[0][2]))
    ba.extend(struct.pack("<1f", matrix[0][3]))
    ba.extend(struct.pack("<1f", matrix[1][0]))
    ba.extend(struct.pack("<1f", matrix[1][1]))
    ba.extend(struct.pack("<1f", matrix[1][2]))
    ba.extend(struct.pack("<1f", matrix[1][3]))
    ba.extend(struct.pack("<1f", matrix[2][0]))
    ba.extend(struct.pack("<1f", matrix[2][1]))
    ba.extend(struct.pack("<1f", matrix[2][2]))
    ba.extend(struct.pack("<1f", matrix[2][3]))
    ba.extend(struct.pack("<1f", matrix[3][0]))
    ba.extend(struct.pack("<1f", matrix[3][1]))
    ba.extend(struct.pack("<1f", matrix[3][2]))
    ba.extend(struct.pack("<1f", matrix[3][3]))

def writeVec3(ba, vec):
    ba.extend(struct.pack("<1f", vec.x))
    ba.extend(struct.pack("<1f", vec.y))
    ba.extend(struct.pack("<1f", vec.z))

def findArmature(obj):
    if obj.type == "ARMATURE": return obj
    if obj.parent == None: return None
    return findArmature(obj.parent)

def saveSubMesh(obj, meshName, path, materialIndex):
    for thing in bpy.data.objects:
        thing.select_set(False)
    obj.select_set(True);
    bpy.context.view_layer.objects.active = obj

    mesh = obj.data
    armature = findArmature(obj);

    ba = bytearray()
    ba.append(1) # Version
    writeString(ba, meshName);

    boneNames = []
    if armature != None:
        ba.extend(struct.pack("<1i", len(armature.data.bones)))
        for bone in armature.data.bones:
            boneNames.append(bone.name)
            writeString(ba, bone.name)
    else:
        ba.extend(struct.pack("<1i", 0))

    vertsCount = 0
    for poly in mesh.polygons:
        if len(poly.vertices) > 3:
            self.report({'ERROR'}, mesh.name + " has a polygon with more than 3 vertices...")
            return
        if poly.material_index == materialIndex:
            vertsCount += 3

    ba.extend(struct.pack("<1I", vertsCount))

    indices = []
    vertCount = 0
    for i in range(0, len(mesh.polygons)):
        poly = mesh.polygons[i]
        if poly.material_index != materialIndex:
            continue

        for i in range(0, 3):
            indices.append(vertCount)
            vertCount += 1
            meshVertex = mesh.vertices[poly.vertices[i]]
            meshUvLoop = mesh.uv_layers[0].data[poly.loop_indices[i]]
            meshLoop = mesh.loops[poly.loop_indices[i]]
            ba.extend(struct.pack("<1f", meshVertex.co.x))
            ba.extend(struct.pack("<1f", meshVertex.co.y))
            ba.extend(struct.pack("<1f", meshVertex.co.z))
            ba.extend(struct.pack("<1f", meshUvLoop.uv.x))
            ba.extend(struct.pack("<1f", meshUvLoop.uv.y))
            ba.extend(struct.pack("<1f", meshLoop.normal.x))
            ba.extend(struct.pack("<1f", meshLoop.normal.y))
            ba.extend(struct.pack("<1f", meshLoop.normal.z))
            ba.extend(struct.pack("<1f", meshLoop.tangent.x))
            ba.extend(struct.pack("<1f", meshLoop.tangent.y))
            ba.extend(struct.pack("<1f", meshLoop.tangent.z))

            realMeshVertex = obj.data.vertices[poly.vertices[i]]

            boneIndexCount = 0
            for i in range(0, len(realMeshVertex.groups)):
                weight = realMeshVertex.groups[i].weight
                groupIndex = realMeshVertex.groups[i].group
                if weight < 0.0001:
                    continue

                if boneIndexCount == 4:
                    print("A vertex has more than 4 bones")
                    break

                vertexGroupName = obj.vertex_groups[groupIndex].name
                if vertexGroupName in boneNames:
                    boneIndex = boneNames.index(obj.vertex_groups[groupIndex].name)
                    # print("Bone: "+str(boneIndex))
                    ba.append(boneIndex)
                    boneIndexCount += 1
                    # print(str(boneIndexCount) + " Bone: "+str(boneIndex))

            for i in range(0, 4-boneIndexCount):
                ba.append(0)

            weightCount = 0
            for i in range(0, len(realMeshVertex.groups)):
                weight = realMeshVertex.groups[i].weight
                if weight < 0.0001:
                    continue

                ba.extend(struct.pack("<1f", weight))
                weightCount += 1
                # print(str(i) + " Weight: "+str(weight))

                if weightCount == 4:
                    break

            for i in range(0, 4-weightCount):
                ba.extend(struct.pack("<1f", 0))

    ba.extend(struct.pack("<1I", vertsCount))
    for i in range(0, vertsCount):
        # print(mesh.name+" ind: "+str(indices[i]))
        ba.extend(struct.pack("<1H", indices[i]))

    ba.extend(struct.pack("<1I", 0)) # Material type
    ba.append(0) # Back face culled

    ba.append(0) # Old diffuse texture
    ba.append(0) # Old normal texture
    ba.append(0) # Old specular texture

    outputFile = open(path+".mesh", "wb")
    outputFile.write(ba)
    outputFile.close()

def writeMaterial(ba, obj, materialIndex):
    diffusePath = None

    if obj != None and len(obj.data.materials) > 0:
        materialNodeTree = obj.data.materials[materialIndex].node_tree
        if "MintMaterial" in materialNodeTree.nodes:
            mintMaterialNodes = materialNodeTree.nodes["MintMaterial"]
            image = mintMaterialNodes.node_tree.nodes["texture"].image
            if image != None:
                diffusePath = image.name

    if diffusePath != None:
        writeString(ba, diffusePath)
    else:
        ba.append(0)

    ba.append(0) # Specular texture
    ba.append(0) # Normal texture

def writeModel(ba, obj, modelPath, depth=0):
    ba.append(1) # Version

    modelName = obj.name
    writeString(ba, modelName)

    if depth == 0:
        writeMatrix(ba, mathutils.Matrix());
    else:
        writeMatrix(ba, obj.matrix_local.transposed());

    materialInds = []
    newObj = None
    for thing in bpy.data.objects:
        thing.select_set(False)
    bpy.context.view_layer.update()

    if obj.type == "MESH":
        bpy.context.view_layer.objects.active = obj
        obj.select_set(True)
        bpy.ops.object.duplicate() 
        newObj = bpy.context.active_object
        newObj.select_set(True)
        bpy.ops.object.duplicates_make_real(use_base_parent=False, use_hierarchy=False)

        for objModifier in newObj.modifiers:
            print("Applying: "+objModifier.name);
            bpy.ops.object.modifier_apply(modifier=objModifier.name)

        me = newObj.data
        bm = bmesh.new()
        bm.from_mesh(me)

        bmesh.ops.triangulate(bm, faces=bm.faces[:])
        bm.to_mesh(me)
        bm.free()
        me.calc_normals_split()

        for poly in newObj.data.polygons:
            if poly.material_index not in materialInds:
                materialInds.append(poly.material_index)

    ba.append(0) # meshPath for root model
    writeMaterial(ba, None, 0) # Null material for root model

    ba.extend(struct.pack("<1i", len(obj.children) + len(materialInds)))

    for ind in materialInds:
        ba.append(1) # Version
        writeString(ba, str(ind))
        writeMatrix(ba, mathutils.Matrix())
        meshName = None
        if (len(materialInds) > 1):
            meshName = obj.data.name+"."+str(ind)
        else:
            meshName = obj.data.name
        meshName = modelName + "." + meshName
        writeString(ba, meshName) # Actually meshPath, which is in the same dir
        writeMaterial(ba, newObj, ind)
        ba.extend(struct.pack("<1i", 0)) # childrenNum
        saveSubMesh(newObj, meshName, modelPath + "/" + meshName, ind)

    if newObj != None:
        for thing in bpy.data.objects:
            thing.select_set(False)
        newObj.select_set(True)
        bpy.context.view_layer.objects.active = newObj
        bpy.ops.object.delete()

    for child in obj.children:
        writeModel(ba, child, modelPath, depth+1)

def writeBone(ba, arm, bone, transformList):
    writeString(ba, bone.name)
    if bone.parent == None:
        ba.append(255) # -1
    else:
        found = False
        for i in range(0, len(arm.bones)):
            otherBone = arm.bones[i]
            if otherBone.name == bone.parent.name:
                # parentIndex = arm.bones.items().index(bone.parent)
                ba.append(i)
                found = True
                break
        if found == False:
            print("Couldn't find parent for "+bone.name)

    writeVec3(ba, bone.matrix_local.to_translation())
    writeVec3(ba, bone.matrix_local.to_euler("XYZ"))
    writeVec3(ba, bone.matrix_local.to_scale())

    print(bone.name+" would add "+str(len(transformList))+" transforms")
    for vecs in transformList:
        writeVec3(ba, vecs[0]);
        writeVec3(ba, vecs[1]);
        writeVec3(ba, vecs[2]);

def writeSkeleton(ba, obj, skeleName):
    ba.append(2) # Version
    arm = obj.data
    arm.pose_position = "POSE"

    actions = bpy.data.actions

    frameCount = 0
    framesMap = {}
    for action in actions:
        obj.animation_data.action = action

        global poseMarkersOutString
        for i in range(0, len(action.pose_markers)):
            marker = action.pose_markers[i]
            poseMarkersOutString += action.name + " "
            poseMarkersOutString += str(marker.frame/(int(action.frame_range.y) - int(action.frame_range.x))) + " "
            poseMarkersOutString += marker.name
            poseMarkersOutString += "\n"

        for i in range(int(action.frame_range.x), int(action.frame_range.y)):
            # print("Action "+action.name+" frame "+str(i))
            bpy.context.scene.frame_set(i);
            bpy.context.view_layer.update()
            # bpy.context.evaluated_depsgraph_get().update()
            # obj.data.update_tag()
            # bpy.ops.wm.redraw_timer(type='DRAW_WIN_SWAP', iterations=1)
            # bpy.context.scene.update_tag()
            for poseBone in obj.pose.bones:
                if poseBone.bone.name not in framesMap: framesMap[poseBone.bone.name] = list()
                matrix = poseBone.matrix
                if poseBone.parent:
                    matrix = poseBone.parent.matrix.inverted() @ matrix

                vecs = list();
                vecs.append(matrix.to_translation());
                vecs.append(matrix.to_euler("XYZ"));
                vecs.append(matrix.to_scale());
                framesMap[poseBone.bone.name].append(vecs);
            frameCount += 1


    writeString(ba, skeleName) 
    ba.extend(struct.pack("<1I", frameCount))

    ba.extend(struct.pack("<1i", len(arm.bones)))
    for bone in arm.bones:
        transformList = framesMap[bone.name]
        writeBone(ba, arm, bone, transformList)

    ba.extend(struct.pack("<1i", len(actions)))
    firstFrameCount = 0
    for action in actions:
        writeString(ba, action.name)
        ba.extend(struct.pack("<1i", firstFrameCount))
        framesInAction = int(action.frame_range.y) - int(action.frame_range.x)
        ba.extend(struct.pack("<1i", framesInAction))
        firstFrameCount += framesInAction

def writeBaToFile(ba, path):
    outputFile = open(path, "wb")
    outputFile.write(ba)
    outputFile.close()


class ExportMeshesOp(bpy.types.Operator):
    bl_idname = "wm.export_meshes"
    bl_label = "Export meshes"

    def execute(self, context):
        self.report({'INFO'}, "Exporting...")
        for file in os.listdir(bpy.path.abspath("//")):
            if file.endswith(".mesh") or file.endswith(".model") or file.endswith(".skele"):
                os.remove(os.path.join(bpy.path.abspath("//"), file))

        arms = []
        for obj in bpy.data.objects:
            if (obj.type == 'ARMATURE'):
                obj.data.pose_position = "REST"
                arms.append(obj)

        for obj in bpy.context.scene.objects:
            if obj.name[0:2] == "M_":
                ba = bytearray()
                writeModel(ba, obj, "")
                writeBaToFile(ba, bpy.path.abspath("//")+"/"+obj.name[2:] + ".model")
            if obj.type == "ARMATURE":
                if obj.data.name.startswith("S_"):
                    ba = bytearray()
                    writeSkeleton(ba, obj, obj.data.name[2:])
                    writeBaToFile(ba, bpy.path.abspath("//")+"/"+obj.data.name[2:] + ".skele")

        for arm in arms:
            arm.data.pose_position = "POSE"

        return {'FINISHED'}

def setAllArmsPoseMode(mode):
    for obj in bpy.data.objects:
        if (obj.type == 'ARMATURE'):
            obj.data.pose_position = mode

def saveModel(obj, name, path):
    setAllArmsPoseMode("REST")
    ba = bytearray()
    writeModel(ba, obj, path)
    writeBaToFile(ba, path + "/" + name + ".model")
    setAllArmsPoseMode("POSE")

def saveSkeleton(obj, name, path):
    setAllArmsPoseMode("REST")
    ba = bytearray()
    writeSkeleton(ba, obj, name);
    writeBaToFile(ba, path + "/" + name + ".skele")
    savePoseMarkers(path + "/" + name + ".markers")
    setAllArmsPoseMode("POSE")

def savePoseMarkers(path):
    scn = bpy.data.scenes[0]

    global poseMarkersOutString;
    f = open(path, "w")
    f.write(poseMarkersOutString)
    f.close()
    poseMarkersOutString = "";


class MESH_OP_export_content_for_concrete_jungle(bpy.types.Operator):
    bl_idname = "mesh.export_content_for_concrete_jungle"
    bl_label = "export_content_for_concrete_jungle"
    bl_options = {"REGISTER"}

    def execute(self, context):
        scn = bpy.data.scenes[0]
        world = bpy.data.worlds["World"]

        saveModel(bpy.data.objects["MESH_Unit"], "unit", "C:/Dropbox/concreteJungle/concreteJungleGameAssets/assets/models");
        saveSkeleton(bpy.data.objects["ARM_Unit"], "unit", "C:/Dropbox/concreteJungle/concreteJungleGameAssets/assets/skeletons")

        saveModel(bpy.data.objects["Sword"], "sword", "C:/Dropbox/concreteJungle/concreteJungleGameAssets/assets/models");
        saveModel(bpy.data.objects["Knife"], "knife", "C:/Dropbox/concreteJungle/concreteJungleGameAssets/assets/models");

        saveModel(bpy.data.objects["Road"], "road", "C:/Dropbox/concreteJungle/concreteJungleGameAssets/assets/models");
        saveModel(bpy.data.objects["Building1"], "building1", "C:/Dropbox/concreteJungle/concreteJungleGameAssets/assets/models");

        # saveModel(bpy.data.objects["M_Cube"], "simple", "C:/Dropbox/concreteJungle/concreteJungleGameAssets/assets/models");
        # saveSkeleton(bpy.data.objects["S_Cube"], "simple", "C:/Dropbox/concreteJungle/concreteJungleGameAssets/assets/skeletons")

        return {"FINISHED"}

class MESH_OP_export_content_for_models(bpy.types.Operator):
    bl_idname = "mesh.export_content_for_models"
    bl_label = "export_content_for_models"
    bl_options = {"REGISTER"}

    def execute(self, context):
        scn = bpy.data.scenes[0]
        world = bpy.data.worlds["World"]

        saveModel(bpy.data.objects["Cube"], "cube", "C:/Dropbox/MultiGame/multiGame/commonAssets/assets/common/models");
        saveModel(bpy.data.objects["Sphere"], "sphere", "C:/Dropbox/MultiGame/multiGame/commonAssets/assets/common/models");

        return {"FINISHED"}

class ExportPanel:
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "MintPaw Exporter"
    bl_options = {"DEFAULT_CLOSED"}

class ExportSubPanel(ExportPanel, bpy.types.Panel):
    bl_idname = "ExportSubPanel"
    bl_label = "Export panel"

    def draw(self, context):
        layout = self.layout
        layout.label(text="This is the export panel")
        layout.operator("wm.export_meshes")

def register():
    bpy.utils.register_class(ExportSubPanel)
    bpy.utils.register_class(ExportMeshesOp)
    bpy.utils.register_class(MESH_OP_export_content_for_concrete_jungle)
    bpy.utils.register_class(MESH_OP_export_content_for_models)

def unregister():
    bpy.utils.unregister_class(ExportSubPanel)
    bpy.utils.unregister_class(ExportMeshesOp)
    bpy.utils.unregister_class(MESH_OP_export_content_for_concrete_jungle)
    bpy.utils.unregister_class(MESH_OP_export_content_for_models)

register()
