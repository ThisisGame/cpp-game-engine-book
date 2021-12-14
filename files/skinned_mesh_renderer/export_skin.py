import bpy

arm = bpy.data.objects['Armature']
# Get the active mesh
obj=bpy.context.object

# 当前选中的物体的所有顶点
obj_verts = obj.data.vertices
# 获取当前Mesh 关联的所有的顶点组名称
obj_group_names = [g.name for g in obj.vertex_groups]

print(obj_group_names)

for bone in arm.pose.bones:
    print(bone.name)
    if bone.name not in obj_group_names:
        continue

    # 获取骨骼的顶点组序号
    gidx = obj.vertex_groups[bone.name].index
    # 获取骨骼的顶点组关联的顶点
    bone_verts = [v for v in obj_verts if gidx in [g.group for g in v.groups]]
    # 遍历顶点
    for v in bone_verts:
        for g in v.groups:
            if g.group == gidx: 
                w = g.weight
                print('Vertex',v.index,'has a weight of',w,'for bone',bone.name)