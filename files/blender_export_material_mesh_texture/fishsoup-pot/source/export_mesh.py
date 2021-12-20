import bpy
import bmesh
import os
import struct


# 引擎顶点结构(坐标、颜色、纹理坐标)
class EngineVertex:
    x=y=z=r=g=b=a=u=v=0

    def __init__(self,x,y,z,r,g,b,a,u,v):
        self.x=x
        self.y=y
        self.z=z
        self.r=r
        self.g=g
        self.b=b
        self.a=a
        self.u=u
        self.v=v
    
    def __eq__(self,other):
        equal= self.x==other.x and self.y==other.y and self.z==other.z and self.r==other.r and \
            self.g==other.g and self.b==other.b and self.a==other.a and self.u==other.u and self.v==other.v
        return equal

    def __str__(self) -> str:
        return "x:{:.2f},y:{:.2f},z:{:.2f},r:{:.2f},g:{:.2f},b:{:.2f},a:{:.2f},u:{:.2f},v:{:.2f}".format(self.x,self.y,self.z,self.r,self.g,self.b,self.a,self.u,self.v)

    def __repr__(self) -> str:#显示属性 用来打印。
        return str(self)

    def write(self,f):
        f.write(struct.pack('f',self.x))
        f.write(struct.pack('f',self.y))
        f.write(struct.pack('f',self.z))
        f.write(struct.pack('f',self.r))
        f.write(struct.pack('f',self.g))
        f.write(struct.pack('f',self.b))
        f.write(struct.pack('f',self.a))
        f.write(struct.pack('f',self.u))
        f.write(struct.pack('f',self.v))

# 显示信息框
def ShowMessageBox(message = "", title = "Message Box", icon = 'INFO'):
    def draw(self, context):
        self.layout.label(text=message)
    bpy.context.window_manager.popup_menu(draw, title = title, icon = icon)

# 当前选中的Mesh
mesh_selected = bpy.context.object.data
uv_layer = mesh_selected.uv_layers.active.data
    
print("-----------EXPORT MESH---------")

engine_vertexes=[]
engine_vertex_indexes=[]

for poly in mesh_selected.polygons:#遍历多边形
    # print("Polygon index: %d, length: %d" % (poly.index, poly.loop_total))
    if poly.loop_total==4:
        ShowMessageBox("Need Triangle","Polygon Error",  'ERROR')
        break
    
    # 遍历一个多边形的3个顶点，注意这里的loop_index是包含重复顶点的顶点数组的index，是以三角形为单位*3作为顶点总数，并不是实际顶点的index。
    # 例如第一个三角形就是0 1 2，第二个三角形就是 3 4 5。是包含重复顶点的。
    for loop_index in range(poly.loop_start, poly.loop_start + poly.loop_total):
        vertex_index=mesh_selected.loops[loop_index].vertex_index#顶点在Blender中的索引
        v=mesh_selected.vertices[vertex_index]
        uv=uv_layer[loop_index].uv

        engine_vertex=EngineVertex(v.co.x,v.co.z,v.co.y,1,1,1,1,uv.x,uv.y)#构造引擎顶点，注意 y z调换
        
        #判断顶点是否存在
        find_engine_vertex_index=-1
        for engine_vertex_index in range(len(engine_vertexes)):
            if engine_vertexes[engine_vertex_index]==engine_vertex:
                find_engine_vertex_index=engine_vertex_index
                break
        
        if find_engine_vertex_index<0:
            find_engine_vertex_index=len(engine_vertexes)
            engine_vertexes.append(engine_vertex)
            
        engine_vertex_indexes.append(find_engine_vertex_index)#把index放入顶点索引表
        
# print(engine_vertexes)
# print(engine_vertex_indexes)



blender_project_dir = os.path.dirname(bpy.data.filepath)
if os.path.exists(blender_project_dir+"/export")==False:
    os.mkdir(blender_project_dir+"/export")
    
with open(blender_project_dir+"/export/export.mesh", 'wb') as f:
    #写入文件头
    f.write("mesh".encode())
    f.write(struct.pack('H',len(engine_vertexes)))
    f.write(struct.pack('H',len(engine_vertex_indexes)))
    
    #写入顶点数据
    for engine_vertex in engine_vertexes:
        engine_vertex.write(f)

    #写入索引数据
    kVertexIndexVectorInverse=engine_vertex_indexes[::-1]
    for engine_vertex_index in kVertexIndexVectorInverse:
        f.write(struct.pack('H',engine_vertex_index))
    
    f.close()
    
print("----SUCCESS-----------------------------------")