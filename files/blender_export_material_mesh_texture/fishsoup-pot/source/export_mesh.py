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
object=bpy.context.object
mesh = object.data
uv_layer = mesh.uv_layers.active.data
    
print("-----------EXPORT MESH---------")

# 收集顶点
mEngineVertexes=[]
for v in mesh.vertices:
    engine_vertex=EngineVertex(v.co.x,v.co.z,v.co.y,1,1,1,1,0,0)#构造引擎顶点，注意 y z调换
    mEngineVertexes.append(engine_vertex)

# 收集UV
mEngineVertexIndexes=[]
for poly in mesh.polygons:#遍历多边形
    # print("Polygon index: %d, length: %d" % (poly.index, poly.loop_total))#检查是三角形还是四边形
    if poly.loop_total==4:
        ShowMessageBox("Need Triangle","Polygon Error",  'ERROR')
        break
    # 遍历一个多边形的3个顶点
    for loop_index in range(poly.loop_start, poly.loop_start + poly.loop_total):
        vertex_index=mesh.loops[loop_index].vertex_index #顶点在Blender中的索引
        mEngineVertexIndexes.append(vertex_index)#把index放入顶点索引表

        v=mesh.vertices[vertex_index] #拿顶点索引获取顶点
        uv=uv_layer[loop_index].uv #获取当前顶点的UV坐标
        engine_vertex=mEngineVertexes[vertex_index]#补全顶点UV数据
        engine_vertex.u=uv.x
        engine_vertex.v=uv.y

print(*mEngineVertexes,sep='\n')
print(mEngineVertexIndexes)



blender_project_dir = os.path.dirname(bpy.data.filepath)
if os.path.exists(blender_project_dir+"/export")==False:
    os.mkdir(blender_project_dir+"/export")
    
with open(blender_project_dir+"/export/export.mesh", 'wb') as f:
    #写入文件头
    f.write("mesh".encode())
    f.write(struct.pack('H',len(mEngineVertexes)))
    f.write(struct.pack('H',len(mEngineVertexIndexes)))
    
    #写入顶点数据
    for engine_vertex in mEngineVertexes:
        engine_vertex.write(f)

    #写入索引数据
    kVertexIndexVectorInverse=mEngineVertexIndexes[::-1]
    for engine_vertex_index in kVertexIndexVectorInverse:
        f.write(struct.pack('H',engine_vertex_index))
    
    f.close()
    
print("----SUCCESS-----------------------------------")