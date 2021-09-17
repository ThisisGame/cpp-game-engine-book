//
// Created by captainchen on 2021/9/13.
//

#include "lua_binding.h"
#include <glm/ext.hpp>
#include "audio/audio.h"
#include "audio/studio/audio_studio.h"
#include "audio/studio/audio_studio_event.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "control/input.h"
#include "renderer/camera.h"
#include "renderer/material.h"
#include "renderer/mesh_filter.h"
#include "renderer/mesh_renderer.h"
#include "renderer/pass.h"
#include "renderer/shader.h"
#include "renderer/technique.h"
#include "renderer/texture2d.h"
#include "utils/application.h"
#include "utils/debug.h"
#include "utils/screen.h"
#include "utils/time.h"


lua_State* LuaBinding::lua_state_;

void LuaBinding::BindLua(lua_State *lua_state) {
    lua_state_=lua_state;

    // depends
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginNamespace("glm")
                .beginClass<glm::vec3>("vec3")
                .addConstructor<void(*)(const float&, const float&, const float&)>()
                .addData("x", &glm::vec3::x)
                .addData("y", &glm::vec3::y)
                .addData("z", &glm::vec3::z)
                .addData("r", &glm::vec3::r)
                .addData("g", &glm::vec3::g)
                .addData("b", &glm::vec3::b)
                .addFunction ("__tostring", std::function <std::string (const glm::vec3*)> ([] (const glm::vec3* vec) {return glm::to_string(*vec);}))
                .endClass();
    }

    // audio
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Audio>("Audio")
                .addStaticFunction("Init",&Audio::Init)
                .addStaticFunction("Update",&Audio::Update)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<AudioStudio>("AudioStudio")
                .addStaticFunction("Init",&AudioStudio::Init)
                .addStaticFunction("Update",&AudioStudio::Update)
                .addStaticFunction("LoadBankFile",&AudioStudio::LoadBankFile)
                .addStaticFunction("CreateEventInstance",&AudioStudio::CreateEventInstance)
                .addStaticFunction("setListenerAttributes",&AudioStudio::setListenerAttributes)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<AudioStudioEvent>("AudioStudioEvent")
                .addConstructor<void (*) ()>()
                .addFunction("event_instance",&AudioStudioEvent::event_instance)
                .addFunction("SetParameterByName",&AudioStudioEvent::SetParameterByName)
                .addFunction("Set3DAttribute",&AudioStudioEvent::Set3DAttribute)
                .addFunction("Start",&AudioStudioEvent::Start)
                .addFunction("Stop",&AudioStudioEvent::Stop)
                .addFunction("Pause",&AudioStudioEvent::Pause)
                .endClass();
    }

    // component
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<GameObject>("GameObject")
                .addConstructor<void (*) (std::string)>()
               .addFunction("name",&GameObject::name)
               .addFunction("set_name",&GameObject::set_name)
               .addFunction("layer",&GameObject::layer)
               .addFunction("set_layer",&GameObject::set_layer)
                .addFunction("__eq", &GameObject::operator==)
                .addFunction("AddComponent", &GameObject::AddComponentFromLua)
                .addFunction("GetComponent",&GameObject::GetComponentFromLua)
                .addFunction("Foreach",&GameObject::ForeachLuaComponent)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Component>("Component")
                .addFunction("Awake",&Component::Awake)
                .addFunction("Update",&Component::Update)
                .addFunction("game_object",&Component::game_object)
                .addFunction("set_game_object",&Component::set_game_object)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .deriveClass<Transform,Component>("Transform")
                .addConstructor<void (*) ()>()
                .addFunction("position", &Transform::position)
                .addFunction("rotation", &Transform::rotation)
                .addFunction("scale", &Transform::scale)
                .addFunction("set_position", &Transform::set_position)
                .addFunction("set_rotation", &Transform::set_rotation)
                .addFunction("set_scale", &Transform::set_scale)
                .endClass();
    }

    // control
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<vec2_ushort>("vec2_ushort")
                .addProperty("x_",&vec2_ushort::x_)
                .addProperty("y_",&vec2_ushort::y_)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Input>("Input")
                .addStaticFunction("RecordKey",&Input::RecordKey)
                .addStaticFunction("GetKey",&Input::GetKey)
                .addStaticFunction("GetKeyDown",&Input::GetKeyDown)
                .addStaticFunction("GetKeyUp",&Input::GetKeyUp)
                .addStaticFunction("Update",&Input::Update)
                .addStaticFunction("GetMouseButton",&Input::GetMouseButton)
                .addStaticFunction("GetMouseButtonDown",&Input::GetMouseButtonDown)
                .addStaticFunction("GetMouseButtonUp",&Input::GetMouseButtonUp)
                .addStaticFunction("mousePosition",&Input::mousePosition)
                .addStaticFunction("set_mousePosition",&Input::set_mousePosition)
                .addStaticFunction("mouse_scroll",&Input::mouse_scroll)
                .addStaticFunction("RecordScroll",&Input::RecordScroll)
                .endClass();
    }

    // renderer
    {
        luabridge::getGlobalNamespace(lua_state)
                .deriveClass<Camera,Component>("Camera")
                .addConstructor<void (*) ()>()
                .addFunction("SetView",&Camera::SetView)
                .addFunction("SetProjection",&Camera::SetProjection)
                .addFunction("view_mat4",&Camera::view_mat4)
                .addFunction("projection_mat4",&Camera::projection_mat4)
                .addFunction("set_clear_color",&Camera::set_clear_color)
                .addFunction("set_clear_flag",&Camera::set_clear_flag)
                .addFunction("Clear",&Camera::Clear)
                .addFunction("depth",&Camera::depth)
                .addFunction("set_depth",&Camera::set_depth)
                .addFunction("culling_mask",&Camera::culling_mask)
                .addFunction("set_culling_mask",&Camera::set_culling_mask)
                .addStaticFunction("Foreach",&Camera::Foreach)
                .addStaticFunction("current_camera",&Camera::current_camera)
                .addStaticFunction("Sort",&Camera::Sort)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Material>("Material")
                .addConstructor<void (*) ()>()
                .addFunction("Parse",&Material::Parse)
//                .addFunction("SetUniformMatrix4fv",&Material::SetUniformMatrix4fv)
                .addFunction("SetUniform1i",&Material::SetUniform1i)
                .addFunction("SetTexture",&Material::SetTexture)
                .addFunction("technique_active",&Material::technique_active)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .deriveClass<MeshFilter,Component>("MeshFilter")
                .addConstructor<void (*) ()>()
                .addFunction("LoadMesh", &MeshFilter::LoadMesh)
                .addFunction("CreateMesh", &MeshFilter::CreateMesh)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .deriveClass<MeshRenderer,Component>("MeshRenderer")
                .addConstructor<void (*) ()>()
                .addFunction("SetMaterial", &MeshRenderer::SetMaterial)
                .addFunction("material", &MeshRenderer::material)
                .addFunction("Render", &MeshRenderer::Render)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Pass>("Pass")
                .addConstructor<void (*) ()>()
                .addFunction("Parse", &Pass::Parse)
                .addFunction("shader", &Pass::shader)
                .addFunction("textures", &Pass::textures)
                .addFunction("SetTexture", &Pass::SetTexture)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Shader>("Shader")
                .addConstructor<void (*) ()>()
                .addFunction("Parse", &Shader::Parse)
                .addFunction("CreateGPUProgram", &Shader::CreateGPUProgram)
                .addFunction("Active", &Shader::Active)
                .addFunction("InActive", &Shader::InActive)
                .addFunction("gl_program_id", &Shader::gl_program_id)
                .addStaticFunction("Find", &Shader::Find)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Technique>("Technique")
                .addConstructor<void (*) ()>()
                .addFunction("Parse", &Technique::Parse)
                .addFunction("pass_vec", &Technique::pass_vec)
                .addFunction("SetTexture", &Technique::SetTexture)
                .endClass();
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Texture2D>("Texture2D")
//                .addConstructor<void (*) ()>()
                .addFunction("mipmap_level", &Texture2D::mipmap_level)
                .addFunction("width", &Texture2D::width)
                .addFunction("height", &Texture2D::height)
                .addFunction("gl_texture_format", &Texture2D::gl_texture_format)
                .addFunction("gl_texture_id", &Texture2D::gl_texture_id)
                .addStaticFunction("LoadFromFile", &Texture2D::LoadFromFile)
                .addStaticFunction("CreateFromTrueTypeFont", &Texture2D::CreateFromTrueTypeFont)
                .endClass();
    }

    // utils
    {
        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Application>("Application")
                .addStaticFunction("set_title",&Application::set_title)
                .addStaticFunction("set_data_path",&Application::set_data_path)
                .addStaticFunction("Init",&Application::Init)
                .addStaticFunction("Run",&Application::Run)
                .addStaticFunction("UpdateScreenSize",&Application::UpdateScreenSize)
                .addStaticFunction("Update",&Application::Update)
                .addStaticFunction("Render",&Application::Render)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Debug>("Debug")
                .addStaticFunction("Init",&Debug::Init)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Screen>("Screen")
                .addStaticFunction("width",&Screen::width)
                .addStaticFunction("height",&Screen::height)
                .addStaticFunction("aspect_ratio",&Screen::aspect_ratio)
                .addStaticFunction("set_width",&Screen::set_width)
                .addStaticFunction("set_height",&Screen::set_height)
                .addStaticFunction("set_width_height",&Screen::set_width_height)
                .endClass();

        luabridge::getGlobalNamespace(lua_state)
                .beginClass<Time>("Time")
                .addStaticFunction("Init",&Time::Init)
                .addStaticFunction("Update",&Time::Update)
                .addStaticFunction("TimeSinceStartup",&Time::TimeSinceStartup)
                .addStaticFunction("delta_time",&Time::delta_time)
                .endClass();
    }




}