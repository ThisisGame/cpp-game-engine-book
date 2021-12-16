# 导出骨骼权重，需要在Blender合并Mesh，然后选择Mesh，然后执行脚本。
import bpy
import os
import struct


# 顶点关联骨骼及权重,每个顶点最多可以关联4个骨骼。
class VertexRelateBoneInfo:
    bone_index=[-1,-1,-1,-1] #骨骼索引，一般骨骼少于128个，用char就行。注意-1表示无骨骼。
    bone_weight=[-1,-1,-1,-1]#骨骼权重，权重不会超过100，所以用char类型就可以。

    def __init__(self):# 一定要在 __init__ 里面初始化一次，不然每次实例化返回的对象实例都是相同指针。
        self.bone_index=[-1,-1,-1,-1]
        self.bone_weight=[-1,-1,-1,-1]

    def __str__(self):
        return "bone_index:{},bone_weight:{}".format(self.bone_index,self.bone_weight)

    def __repr__(self):
        return str(self)
    
    def write(self,f):
        for i in range(4):# 写入4个骨骼索引
            f.write(struct.pack('b',self.bone_index[i]))
        for i in range(4):# 写入4个骨骼权重，注意权重是乘以100的。
            if self.bone_weight[i]>=0:
                f.write(struct.pack('b',int(self.bone_weight[i]*100)))
            else:
                f.write(struct.pack('b',-1))

print("-----------EXPORT WEIGHT---------")

# 当前选中的Mesh
object=bpy.context.object
mesh = object.data

print("---------- COLLECT BONE ------------")
# 收集骨骼名字 [bone_name , bone_name , ...]
boneNames=[]
armature_obj = bpy.data.objects["Armature"]
for i in range(0, len(armature_obj.data.bones)):
    bone = armature_obj.data.bones[i]
    boneNames.append(bone.name)
print(boneNames)

print("---------- COLLECT WEIGHT ------------")
# 收集骨骼权重
armature_obj = bpy.data.objects["Armature"]#场景中的骨架对象。

obj_group_names = [g.name for g in object.vertex_groups]# 获取当前Mesh 关联的所有的顶点组名称。
print(obj_group_names)

vertex_relate_bone_infos=[]#顶点关联骨骼信息(4个骨骼索引、权重)，长度为顶点数。

for vertex in mesh.vertices:#遍历当前选中的物体的所有顶点。
    # print("Vertex index: %d" % vertex.index)
    vertex_relate_bone_info=VertexRelateBoneInfo()
    # print(vertex_relate_bone_info)

    for group_index in range(0, len(vertex.groups)):#遍历顶点关联的顶点组。
        group = vertex.groups[group_index]
        group_name = obj_group_names[group.group]#获取顶点组名字
        # print("Vertex group index: %d, group name: %s, weight: %f" % (group_index, group_name, group.weight))
        if group_name in boneNames:#判断顶点组名字是否在骨骼名字列表中。制作的时候，建模人员要严格按照骨骼名字来作为顶点组名字。
            bone_index=boneNames.index(group_name)#获取骨骼索引。
            vertex_relate_bone_info.bone_index[group_index]=bone_index
            vertex_relate_bone_info.bone_weight[group_index]=group.weight
        else:
            print("Error: group name not in bone names")
    # print(vertex_relate_bone_info)
    vertex_relate_bone_infos.append(vertex_relate_bone_info)

print(*vertex_relate_bone_infos,sep='\n')

blender_project_dir = os.path.dirname(bpy.data.filepath)
if os.path.exists(blender_project_dir+"/export")==False:
    os.mkdir(blender_project_dir+"/export")
    
with open(blender_project_dir+"/export/export.weight", 'wb') as f:
    #写入文件头
    f.write("weight".encode())

    #写入顶点关联骨骼信息(4个骨骼索引、权重)，长度为顶点数。
    for vertex_relate_bone_info in vertex_relate_bone_infos:
        vertex_relate_bone_info.write(f)
    
    f.close()
    
print("----SUCCESS-----------------------------------")