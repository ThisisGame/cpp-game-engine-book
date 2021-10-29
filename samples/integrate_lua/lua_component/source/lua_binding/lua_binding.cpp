//
// Created by captainchen on 2021/9/13.
//

#if USE_LUA_SCRIPT

#include "lua_binding.h"
#include <glm/ext.hpp>
#include <sol/sol.hpp>
#include "audio/audio.h"
#include "audio/studio/audio_studio.h"
#include "audio/studio/audio_studio_event.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "control/input.h"
#include "control/key_code.h"
#include "renderer/camera.h"
#include "renderer/material.h"
#include "renderer/mesh_filter.h"
#include "renderer/mesh_renderer.h"
#include "renderer/shader.h"
#include "renderer/texture2d.h"
#include "utils/application.h"
#include "utils/debug.h"
#include "utils/screen.h"
#include "utils/time.h"

sol::state sol_state;

sol::state& LuaBinding::sol_state(){
    return sol_state;
}

void LuaBinding::Init(std::string package_path) {
    //设置lua搜索目录
    sol::table package_table=sol_state["package"];
    std::string path=package_table["path"];
    path.append(package_path);
    package_table["path"]=path;
}

void LuaBinding::BindLua() {
    // depends
    {
        // glm
        {
            //绑定glm::vec3
            {
                auto glm_ns_table = sol_state["glm"].get_or_create<sol::table>();
                glm_ns_table.new_usertype<glm::vec3>("vec3",sol::call_constructor,sol::constructors<glm::vec3(const float&, const float&, const float&)>(),
                        "x", &glm::vec3::x,
                        "y", &glm::vec3::y,
                        "z", &glm::vec3::z,
                        "r", &glm::vec3::r,
                        "g", &glm::vec3::g,
                        "b", &glm::vec3::b,
                        sol::meta_function::to_string,[] (const glm::vec3* vec) -> std::string {return glm::to_string(*vec);},
                        sol::meta_function::addition,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)+(*vec_b);},
                        sol::meta_function::subtraction,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)-(*vec_b);},
                        sol::meta_function::multiplication,[] (const glm::vec3* vec,const float a) {return (*vec)*a;},
                        sol::meta_function::division,[] (const glm::vec3* vec,const float a) {return (*vec)/a;},
                        sol::meta_function::unary_minus,[] (const glm::vec3* vec) {return (*vec)*-1;},
                        sol::meta_function::equal_to,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)==(*vec_b);}
                );
            }

            //绑定glm::vec4
            {
                auto glm_ns_table = sol_state["glm"].get_or_create<sol::table>();
                glm_ns_table.new_usertype<glm::vec4>("vec4",sol::call_constructor,sol::constructors<glm::vec4(const float&, const float&, const float&, const float&)>(),
                        "x", &glm::vec4::x,
                        "y", &glm::vec4::y,
                        "z", &glm::vec4::z,
                        "w", &glm::vec4::w,
                        "r", &glm::vec4::r,
                        "g", &glm::vec4::g,
                        "b", &glm::vec4::b,
                        "a", &glm::vec4::a,
                        sol::meta_function::to_string,[] (const glm::vec4* vec) {return glm::to_string(*vec);},
                        sol::meta_function::addition,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)+(*vec_b);},
                        sol::meta_function::subtraction,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)-(*vec_b);},
                        sol::meta_function::multiplication,[] (const glm::vec4* vec,const float a) {return (*vec)*a;},
                        sol::meta_function::division,[] (const glm::vec4* vec,const float a) {return (*vec)/a;},
                        sol::meta_function::unary_minus,[] (const glm::vec4* vec) {return (*vec)*-1;},
                        sol::meta_function::equal_to,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)==(*vec_b);}
                );
            }

            //绑定glm::mat4
            {
                auto glm_ns_table = sol_state["glm"].get_or_create<sol::table>();
                glm_ns_table.new_usertype<glm::mat4>("mat4",sol::call_constructor,sol::constructors<glm::mat4(const float&)>(),
                        sol::meta_function::to_string,[] (const glm::mat4* m) {return glm::to_string(*m);},
                        sol::meta_function::addition,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)+(*m_b);},
                        sol::meta_function::subtraction,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)-(*m_b);},
                        sol::meta_function::multiplication,[] (const glm::mat4* m,const glm::vec4* v) {return (*m)*(*v);},
                        sol::meta_function::division,[] (const glm::mat4* m,const float a) {return (*m)/a;},
                        sol::meta_function::unary_minus,[] (const glm::mat4* m) {return (*m)*-1;},
                        sol::meta_function::equal_to,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)==(*m_b);}
                );
            }

            //绑定glm函数
            {
                auto glm_ns_table = sol_state["glm"].get_or_create<sol::table>();
                glm_ns_table.set_function("rotate",sol::overload([] (const glm::mat4* m,const float f,const glm::vec3* v) {return glm::rotate(*m,f,*v);}));
                glm_ns_table.set_function("radians",sol::overload([] (const float f) {return glm::radians(f);}));
                glm_ns_table.set_function("to_string",sol::overload(
                        [] (const glm::mat4* m) {return glm::to_string((*m));},
                        [] (const glm::vec3* v) {return glm::to_string((*v));}
                ));
            }
        }
    }

    // audio
    {
        // FMOD_RESULT
        {
            sol_state.new_enum<FMOD_RESULT,true>("FMOD_RESULT",{
                {"FMOD_OK",FMOD_RESULT::FMOD_OK},
                {"FMOD_ERR_BADCOMMAND",FMOD_RESULT::FMOD_ERR_BADCOMMAND},
                {"FMOD_ERR_CHANNEL_ALLOC",FMOD_RESULT::FMOD_ERR_CHANNEL_ALLOC},
                {"FMOD_ERR_CHANNEL_STOLEN",FMOD_RESULT::FMOD_ERR_CHANNEL_STOLEN},
                {"FMOD_ERR_DMA",FMOD_RESULT::FMOD_ERR_DMA},
                {"FMOD_ERR_DSP_CONNECTION",FMOD_RESULT::FMOD_ERR_DSP_CONNECTION},
                {"FMOD_ERR_DSP_DONTPROCESS",FMOD_RESULT::FMOD_ERR_DSP_DONTPROCESS},
                {"FMOD_ERR_DSP_FORMAT",FMOD_RESULT::FMOD_ERR_DSP_FORMAT},
                {"FMOD_ERR_DSP_INUSE",FMOD_RESULT::FMOD_ERR_DSP_INUSE},
                {"FMOD_ERR_DSP_NOTFOUND",FMOD_RESULT::FMOD_ERR_DSP_NOTFOUND},
                {"FMOD_ERR_DSP_RESERVED",FMOD_RESULT::FMOD_ERR_DSP_RESERVED},
                {"FMOD_ERR_DSP_SILENCE",FMOD_RESULT::FMOD_ERR_DSP_SILENCE},
                {"FMOD_ERR_DSP_TYPE",FMOD_RESULT::FMOD_ERR_DSP_TYPE},
                {"FMOD_ERR_FILE_BAD",FMOD_RESULT::FMOD_ERR_FILE_BAD},
                {"FMOD_ERR_FILE_COULDNOTSEEK",FMOD_RESULT::FMOD_ERR_FILE_COULDNOTSEEK},
                {"FMOD_ERR_FILE_DISKEJECTED",FMOD_RESULT::FMOD_ERR_FILE_DISKEJECTED},
                {"FMOD_ERR_FILE_EOF",FMOD_RESULT::FMOD_ERR_FILE_EOF},
                {"FMOD_ERR_FILE_ENDOFDATA",FMOD_RESULT::FMOD_ERR_FILE_ENDOFDATA},
                {"FMOD_ERR_FILE_NOTFOUND",FMOD_RESULT::FMOD_ERR_FILE_NOTFOUND},
                {"FMOD_ERR_FORMAT",FMOD_RESULT::FMOD_ERR_FORMAT},
                {"FMOD_ERR_HEADER_MISMATCH",FMOD_RESULT::FMOD_ERR_HEADER_MISMATCH},
                {"FMOD_ERR_HTTP",FMOD_RESULT::FMOD_ERR_HTTP},
                {"FMOD_ERR_HTTP_ACCESS",FMOD_RESULT::FMOD_ERR_HTTP_ACCESS},
                {"FMOD_ERR_HTTP_PROXY_AUTH",FMOD_RESULT::FMOD_ERR_HTTP_PROXY_AUTH},
                {"FMOD_ERR_HTTP_SERVER_ERROR",FMOD_RESULT::FMOD_ERR_HTTP_SERVER_ERROR},
                {"FMOD_ERR_HTTP_TIMEOUT",FMOD_RESULT::FMOD_ERR_HTTP_TIMEOUT},
                {"FMOD_ERR_INITIALIZATION",FMOD_RESULT::FMOD_ERR_INITIALIZATION},
                {"FMOD_ERR_INITIALIZED",FMOD_RESULT::FMOD_ERR_INITIALIZED},
                {"FMOD_ERR_INTERNAL",FMOD_RESULT::FMOD_ERR_INTERNAL},
                {"FMOD_ERR_INVALID_FLOAT",FMOD_RESULT::FMOD_ERR_INVALID_FLOAT},
                {"FMOD_ERR_INVALID_HANDLE",FMOD_RESULT::FMOD_ERR_INVALID_HANDLE},
                {"FMOD_ERR_INVALID_PARAM",FMOD_RESULT::FMOD_ERR_INVALID_PARAM},
                {"FMOD_ERR_INVALID_POSITION",FMOD_RESULT::FMOD_ERR_INVALID_POSITION},
                {"FMOD_ERR_INVALID_SPEAKER",FMOD_RESULT::FMOD_ERR_INVALID_SPEAKER},
                {"FMOD_ERR_INVALID_SYNCPOINT",FMOD_RESULT::FMOD_ERR_INVALID_SYNCPOINT},
                {"FMOD_ERR_INVALID_THREAD",FMOD_RESULT::FMOD_ERR_INVALID_THREAD},
                {"FMOD_ERR_INVALID_VECTOR",FMOD_RESULT::FMOD_ERR_INVALID_VECTOR},
                {"FMOD_ERR_MAXAUDIBLE",FMOD_RESULT::FMOD_ERR_MAXAUDIBLE},
                {"FMOD_ERR_MEMORY",FMOD_RESULT::FMOD_ERR_MEMORY},
                {"FMOD_ERR_MEMORY_CANTPOINT",FMOD_RESULT::FMOD_ERR_MEMORY_CANTPOINT},
                {"FMOD_ERR_NEEDS3D",FMOD_RESULT::FMOD_ERR_NEEDS3D},
                {"FMOD_ERR_NEEDSHARDWARE",FMOD_RESULT::FMOD_ERR_NEEDSHARDWARE},
                {"FMOD_ERR_NET_CONNECT",FMOD_RESULT::FMOD_ERR_NET_CONNECT},
                {"FMOD_ERR_NET_SOCKET_ERROR",FMOD_RESULT::FMOD_ERR_NET_SOCKET_ERROR},
                {"FMOD_ERR_NET_URL",FMOD_RESULT::FMOD_ERR_NET_URL},
                {"FMOD_ERR_NET_WOULD_BLOCK",FMOD_RESULT::FMOD_ERR_NET_WOULD_BLOCK},
                {"FMOD_ERR_NOTREADY",FMOD_RESULT::FMOD_ERR_NOTREADY},
                {"FMOD_ERR_OUTPUT_ALLOCATED",FMOD_RESULT::FMOD_ERR_OUTPUT_ALLOCATED},
                {"FMOD_ERR_OUTPUT_CREATEBUFFER",FMOD_RESULT::FMOD_ERR_OUTPUT_CREATEBUFFER},
                {"FMOD_ERR_OUTPUT_DRIVERCALL",FMOD_RESULT::FMOD_ERR_OUTPUT_DRIVERCALL},
                {"FMOD_ERR_OUTPUT_FORMAT",FMOD_RESULT::FMOD_ERR_OUTPUT_FORMAT},
                {"FMOD_ERR_OUTPUT_INIT",FMOD_RESULT::FMOD_ERR_OUTPUT_INIT},
                {"FMOD_ERR_OUTPUT_NODRIVERS",FMOD_RESULT::FMOD_ERR_OUTPUT_NODRIVERS},
                {"FMOD_ERR_PLUGIN",FMOD_RESULT::FMOD_ERR_PLUGIN},
                {"FMOD_ERR_PLUGIN_MISSING",FMOD_RESULT::FMOD_ERR_PLUGIN_MISSING},
                {"FMOD_ERR_PLUGIN_RESOURCE",FMOD_RESULT::FMOD_ERR_PLUGIN_RESOURCE},
                {"FMOD_ERR_PLUGIN_VERSION",FMOD_RESULT::FMOD_ERR_PLUGIN_VERSION},
                {"FMOD_ERR_RECORD",FMOD_RESULT::FMOD_ERR_RECORD},
                {"FMOD_ERR_REVERB_CHANNELGROUP",FMOD_RESULT::FMOD_ERR_REVERB_CHANNELGROUP},
                {"FMOD_ERR_REVERB_INSTANCE",FMOD_RESULT::FMOD_ERR_REVERB_INSTANCE},
                {"FMOD_ERR_SUBSOUNDS",FMOD_RESULT::FMOD_ERR_SUBSOUNDS},
                {"FMOD_ERR_SUBSOUND_ALLOCATED",FMOD_RESULT::FMOD_ERR_SUBSOUND_ALLOCATED},
                {"FMOD_ERR_SUBSOUND_CANTMOVE",FMOD_RESULT::FMOD_ERR_SUBSOUND_CANTMOVE},
                {"FMOD_ERR_TAGNOTFOUND",FMOD_RESULT::FMOD_ERR_TAGNOTFOUND},
                {"FMOD_ERR_TOOMANYCHANNELS",FMOD_RESULT::FMOD_ERR_TOOMANYCHANNELS},
                {"FMOD_ERR_TRUNCATED",FMOD_RESULT::FMOD_ERR_TRUNCATED},
                {"FMOD_ERR_UNIMPLEMENTED",FMOD_RESULT::FMOD_ERR_UNIMPLEMENTED},
                {"FMOD_ERR_UNINITIALIZED",FMOD_RESULT::FMOD_ERR_UNINITIALIZED},
                {"FMOD_ERR_UNSUPPORTED",FMOD_RESULT::FMOD_ERR_UNSUPPORTED},
                {"FMOD_ERR_VERSION",FMOD_RESULT::FMOD_ERR_VERSION},
                {"FMOD_ERR_EVENT_ALREADY_LOADED",FMOD_RESULT::FMOD_ERR_EVENT_ALREADY_LOADED},
                {"FMOD_ERR_EVENT_LIVEUPDATE_BUSY",FMOD_RESULT::FMOD_ERR_EVENT_LIVEUPDATE_BUSY},
                {"FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH",FMOD_RESULT::FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH},
                {"FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT",FMOD_RESULT::FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT},
                {"FMOD_ERR_EVENT_NOTFOUND",FMOD_RESULT::FMOD_ERR_EVENT_NOTFOUND},
                {"FMOD_ERR_STUDIO_UNINITIALIZED",FMOD_RESULT::FMOD_ERR_STUDIO_UNINITIALIZED},
                {"FMOD_ERR_STUDIO_NOT_LOADED",FMOD_RESULT::FMOD_ERR_STUDIO_NOT_LOADED},
                {"FMOD_ERR_INVALID_STRING",FMOD_RESULT::FMOD_ERR_INVALID_STRING},
                {"FMOD_ERR_ALREADY_LOCKED",FMOD_RESULT::FMOD_ERR_ALREADY_LOCKED},
                {"FMOD_ERR_NOT_LOCKED",FMOD_RESULT::FMOD_ERR_NOT_LOCKED},
                {"FMOD_ERR_RECORD_DISCONNECTED",FMOD_RESULT::FMOD_ERR_RECORD_DISCONNECTED},
                {"FMOD_ERR_TOOMANYSAMPLES",FMOD_RESULT::FMOD_ERR_TOOMANYSAMPLES},
                {"FMOD_RESULT_FORCEINT",FMOD_RESULT::FMOD_RESULT_FORCEINT}
            });
        }

        sol_state.new_usertype<Audio>("Audio",
                                      "Init",&Audio::Init,
                                      "Update",&Audio::Update
        );

        sol_state.new_usertype<Audio>("AudioStudio",
                                      "Init",&AudioStudio::Init,
                                      "Update",&AudioStudio::Update,
                                      "LoadBankFile",&AudioStudio::LoadBankFile,
                                      "CreateEventInstance",&AudioStudio::CreateEventInstance,
                                      "SetListenerAttributes",&AudioStudio::SetListenerAttributes
        );

        sol_state.new_usertype<AudioStudioEvent>("AudioStudioEvent",
//                                      "event_instance",&AudioStudioEvent::event_instance,
                                      "SetParameterByName",&AudioStudioEvent::SetParameterByName,
                                      "Set3DAttribute",&AudioStudioEvent::Set3DAttribute,
                                      "Start",&AudioStudioEvent::Start,
                                      "Stop",&AudioStudioEvent::Stop,
                                      "Pause",&AudioStudioEvent::Pause
        );
    }

    // component
    {
        sol_state.new_usertype<GameObject>("GameObject",sol::call_constructor,sol::constructors<GameObject(std::string)>(),
                "name",&GameObject::name,
                "set_name",&GameObject::set_name,
                "layer",&GameObject::layer,
                "set_layer",&GameObject::set_layer,
                "AddComponent", &GameObject::AddComponentFromLua,
                "GetComponent",&GameObject::GetComponentFromLua,
                "Foreach",&GameObject::ForeachLuaComponent
        );

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
                .beginNamespace("KeyAction")
                .addConstant<std::size_t>("KEY_ACTION_UP",KeyAction::KEY_ACTION_UP)
                .addConstant<std::size_t>("KEY_ACTION_DOWN",KeyAction::KEY_ACTION_DOWN)
                .addConstant<std::size_t>("KEY_ACTION_REPEAT",KeyAction::KEY_ACTION_REPEAT)
                .endNamespace();

        // KeyCode
        {
            luabridge::getGlobalNamespace(lua_state)
                    .beginNamespace("KeyCode")
                    .addConstant<std::size_t>("KEY_CODE_UNKNOWN",KeyCode::KEY_CODE_UNKNOWN)
                    .addConstant<std::size_t>("MOUSE_BUTTON_1",KeyCode::MOUSE_BUTTON_1)
                    .addConstant<std::size_t>("MOUSE_BUTTON_2",KeyCode::MOUSE_BUTTON_2)
                    .addConstant<std::size_t>("MOUSE_BUTTON_3",KeyCode::MOUSE_BUTTON_3)
                    .addConstant<std::size_t>("MOUSE_BUTTON_4",KeyCode::MOUSE_BUTTON_4)
                    .addConstant<std::size_t>("MOUSE_BUTTON_5",KeyCode::MOUSE_BUTTON_5)
                    .addConstant<std::size_t>("MOUSE_BUTTON_6",KeyCode::MOUSE_BUTTON_6)
                    .addConstant<std::size_t>("MOUSE_BUTTON_7",KeyCode::MOUSE_BUTTON_7)
                    .addConstant<std::size_t>("MOUSE_BUTTON_8",KeyCode::MOUSE_BUTTON_8)
                    .addConstant<std::size_t>("MOUSE_BUTTON_LAST",KeyCode::MOUSE_BUTTON_LAST)
                    .addConstant<std::size_t>("MOUSE_BUTTON_LEFT",KeyCode::MOUSE_BUTTON_LEFT)
                    .addConstant<std::size_t>("MOUSE_BUTTON_RIGHT",KeyCode::MOUSE_BUTTON_RIGHT)
                    .addConstant<std::size_t>("MOUSE_BUTTON_MIDDLE",KeyCode::MOUSE_BUTTON_MIDDLE)
                    .addConstant<std::size_t>("KEY_CODE_SPACE",KeyCode::KEY_CODE_SPACE)
                    .addConstant<std::size_t>("KEY_CODE_APOSTROPHE",KeyCode::KEY_CODE_APOSTROPHE)
                    .addConstant<std::size_t>("KEY_CODE_COMMA",KeyCode::KEY_CODE_COMMA)
                    .addConstant<std::size_t>("KEY_CODE_MINUS",KeyCode::KEY_CODE_MINUS)
                    .addConstant<std::size_t>("KEY_CODE_PERIOD",KeyCode::KEY_CODE_PERIOD)
                    .addConstant<std::size_t>("KEY_CODE_SLASH",KeyCode::KEY_CODE_SLASH)
                    .addConstant<std::size_t>("KEY_CODE_0",KeyCode::KEY_CODE_0)
                    .addConstant<std::size_t>("KEY_CODE_1",KeyCode::KEY_CODE_1)
                    .addConstant<std::size_t>("KEY_CODE_2",KeyCode::KEY_CODE_2)
                    .addConstant<std::size_t>("KEY_CODE_3",KeyCode::KEY_CODE_3)
                    .addConstant<std::size_t>("KEY_CODE_4",KeyCode::KEY_CODE_4)
                    .addConstant<std::size_t>("KEY_CODE_5",KeyCode::KEY_CODE_5)
                    .addConstant<std::size_t>("KEY_CODE_6",KeyCode::KEY_CODE_6)
                    .addConstant<std::size_t>("KEY_CODE_7",KeyCode::KEY_CODE_7)
                    .addConstant<std::size_t>("KEY_CODE_8",KeyCode::KEY_CODE_8)
                    .addConstant<std::size_t>("KEY_CODE_9",KeyCode::KEY_CODE_9)
                    .addConstant<std::size_t>("KEY_CODE_SEMICOLON",KeyCode::KEY_CODE_SEMICOLON)
                    .addConstant<std::size_t>("KEY_CODE_EQUAL",KeyCode::KEY_CODE_EQUAL)
                    .addConstant<std::size_t>("KEY_CODE_A",KeyCode::KEY_CODE_A)
                    .addConstant<std::size_t>("KEY_CODE_B",KeyCode::KEY_CODE_B)
                    .addConstant<std::size_t>("KEY_CODE_C",KeyCode::KEY_CODE_C)
                    .addConstant<std::size_t>("KEY_CODE_D",KeyCode::KEY_CODE_D)
                    .addConstant<std::size_t>("KEY_CODE_E",KeyCode::KEY_CODE_E)
                    .addConstant<std::size_t>("KEY_CODE_F",KeyCode::KEY_CODE_F)
                    .addConstant<std::size_t>("KEY_CODE_G",KeyCode::KEY_CODE_G)
                    .addConstant<std::size_t>("KEY_CODE_H",KeyCode::KEY_CODE_H)
                    .addConstant<std::size_t>("KEY_CODE_I",KeyCode::KEY_CODE_I)
                    .addConstant<std::size_t>("KEY_CODE_J",KeyCode::KEY_CODE_J)
                    .addConstant<std::size_t>("KEY_CODE_K",KeyCode::KEY_CODE_K)
                    .addConstant<std::size_t>("KEY_CODE_L",KeyCode::KEY_CODE_L)
                    .addConstant<std::size_t>("KEY_CODE_M",KeyCode::KEY_CODE_M)
                    .addConstant<std::size_t>("KEY_CODE_N",KeyCode::KEY_CODE_N)
                    .addConstant<std::size_t>("KEY_CODE_O",KeyCode::KEY_CODE_O)
                    .addConstant<std::size_t>("KEY_CODE_P",KeyCode::KEY_CODE_P)
                    .addConstant<std::size_t>("KEY_CODE_Q",KeyCode::KEY_CODE_Q)
                    .addConstant<std::size_t>("KEY_CODE_R",KeyCode::KEY_CODE_R)
                    .addConstant<std::size_t>("KEY_CODE_S",KeyCode::KEY_CODE_S)
                    .addConstant<std::size_t>("KEY_CODE_T",KeyCode::KEY_CODE_T)
                    .addConstant<std::size_t>("KEY_CODE_U",KeyCode::KEY_CODE_U)
                    .addConstant<std::size_t>("KEY_CODE_V",KeyCode::KEY_CODE_V)
                    .addConstant<std::size_t>("KEY_CODE_W",KeyCode::KEY_CODE_W)
                    .addConstant<std::size_t>("KEY_CODE_X",KeyCode::KEY_CODE_X)
                    .addConstant<std::size_t>("KEY_CODE_Y",KeyCode::KEY_CODE_Y)
                    .addConstant<std::size_t>("KEY_CODE_Z",KeyCode::KEY_CODE_Z)
                    .addConstant<std::size_t>("KEY_CODE_LEFT_BRACKET",KeyCode::KEY_CODE_LEFT_BRACKET)
                    .addConstant<std::size_t>("KEY_CODE_BACKSLASH",KeyCode::KEY_CODE_BACKSLASH)
                    .addConstant<std::size_t>("KEY_CODE_RIGHT_BRACKET",KeyCode::KEY_CODE_RIGHT_BRACKET)
                    .addConstant<std::size_t>("KEY_CODE_GRAVE_ACCENT",KeyCode::KEY_CODE_GRAVE_ACCENT)
                    .addConstant<std::size_t>("KEY_CODE_WORLD_1",KeyCode::KEY_CODE_WORLD_1)
                    .addConstant<std::size_t>("KEY_CODE_WORLD_2",KeyCode::KEY_CODE_WORLD_2)
                    .addConstant<std::size_t>("KEY_CODE_ESCAPE",KeyCode::KEY_CODE_ESCAPE)
                    .addConstant<std::size_t>("KEY_CODE_ENTER",KeyCode::KEY_CODE_ENTER)
                    .addConstant<std::size_t>("KEY_CODE_TAB",KeyCode::KEY_CODE_TAB)
                    .addConstant<std::size_t>("KEY_CODE_BACKSPACE",KeyCode::KEY_CODE_BACKSPACE)
                    .addConstant<std::size_t>("KEY_CODE_INSERT",KeyCode::KEY_CODE_INSERT)
                    .addConstant<std::size_t>("KEY_CODE_DELETE",KeyCode::KEY_CODE_DELETE)
                    .addConstant<std::size_t>("KEY_CODE_RIGHT",KeyCode::KEY_CODE_RIGHT)
                    .addConstant<std::size_t>("KEY_CODE_LEFT",KeyCode::KEY_CODE_LEFT)
                    .addConstant<std::size_t>("KEY_CODE_DOWN",KeyCode::KEY_CODE_DOWN)
                    .addConstant<std::size_t>("KEY_CODE_UP",KeyCode::KEY_CODE_UP)
                    .addConstant<std::size_t>("KEY_CODE_PAGE_UP",KeyCode::KEY_CODE_PAGE_UP)
                    .addConstant<std::size_t>("KEY_CODE_PAGE_DOWN",KeyCode::KEY_CODE_PAGE_DOWN)
                    .addConstant<std::size_t>("KEY_CODE_HOME",KeyCode::KEY_CODE_HOME)
                    .addConstant<std::size_t>("KEY_CODE_END",KeyCode::KEY_CODE_END)
                    .addConstant<std::size_t>("KEY_CODE_CAPS_LOCK",KeyCode::KEY_CODE_CAPS_LOCK)
                    .addConstant<std::size_t>("KEY_CODE_SCROLL_LOCK",KeyCode::KEY_CODE_SCROLL_LOCK)
                    .addConstant<std::size_t>("KEY_CODE_NUM_LOCK",KeyCode::KEY_CODE_NUM_LOCK)
                    .addConstant<std::size_t>("KEY_CODE_PRINT_SCREEN",KeyCode::KEY_CODE_PRINT_SCREEN)
                    .addConstant<std::size_t>("KEY_CODE_PAUSE",KeyCode::KEY_CODE_PAUSE)
                    .addConstant<std::size_t>("KEY_CODE_F1",KeyCode::KEY_CODE_F1)
                    .addConstant<std::size_t>("KEY_CODE_F2",KeyCode::KEY_CODE_F2)
                    .addConstant<std::size_t>("KEY_CODE_F3",KeyCode::KEY_CODE_F3)
                    .addConstant<std::size_t>("KEY_CODE_F4",KeyCode::KEY_CODE_F4)
                    .addConstant<std::size_t>("KEY_CODE_F5",KeyCode::KEY_CODE_F5)
                    .addConstant<std::size_t>("KEY_CODE_F6",KeyCode::KEY_CODE_F6)
                    .addConstant<std::size_t>("KEY_CODE_F7",KeyCode::KEY_CODE_F7)
                    .addConstant<std::size_t>("KEY_CODE_F8",KeyCode::KEY_CODE_F8)
                    .addConstant<std::size_t>("KEY_CODE_F9",KeyCode::KEY_CODE_F9)
                    .addConstant<std::size_t>("KEY_CODE_F10",KeyCode::KEY_CODE_F10)
                    .addConstant<std::size_t>("KEY_CODE_F11",KeyCode::KEY_CODE_F11)
                    .addConstant<std::size_t>("KEY_CODE_F12",KeyCode::KEY_CODE_F12)
                    .addConstant<std::size_t>("KEY_CODE_F13",KeyCode::KEY_CODE_F13)
                    .addConstant<std::size_t>("KEY_CODE_F14",KeyCode::KEY_CODE_F14)
                    .addConstant<std::size_t>("KEY_CODE_F15",KeyCode::KEY_CODE_F15)
                    .addConstant<std::size_t>("KEY_CODE_F16",KeyCode::KEY_CODE_F16)
                    .addConstant<std::size_t>("KEY_CODE_F17",KeyCode::KEY_CODE_F17)
                    .addConstant<std::size_t>("KEY_CODE_F18",KeyCode::KEY_CODE_F18)
                    .addConstant<std::size_t>("KEY_CODE_F19",KeyCode::KEY_CODE_F19)
                    .addConstant<std::size_t>("KEY_CODE_F20",KeyCode::KEY_CODE_F20)
                    .addConstant<std::size_t>("KEY_CODE_F21",KeyCode::KEY_CODE_F21)
                    .addConstant<std::size_t>("KEY_CODE_F22",KeyCode::KEY_CODE_F22)
                    .addConstant<std::size_t>("KEY_CODE_F23",KeyCode::KEY_CODE_F23)
                    .addConstant<std::size_t>("KEY_CODE_F24",KeyCode::KEY_CODE_F24)
                    .addConstant<std::size_t>("KEY_CODE_F25",KeyCode::KEY_CODE_F25)
                    .addConstant<std::size_t>("KEY_CODE_KP_0",KeyCode::KEY_CODE_KP_0)
                    .addConstant<std::size_t>("KEY_CODE_KP_1",KeyCode::KEY_CODE_KP_1)
                    .addConstant<std::size_t>("KEY_CODE_KP_2",KeyCode::KEY_CODE_KP_2)
                    .addConstant<std::size_t>("KEY_CODE_KP_3",KeyCode::KEY_CODE_KP_3)
                    .addConstant<std::size_t>("KEY_CODE_KP_4",KeyCode::KEY_CODE_KP_4)
                    .addConstant<std::size_t>("KEY_CODE_KP_5",KeyCode::KEY_CODE_KP_5)
                    .addConstant<std::size_t>("KEY_CODE_KP_6",KeyCode::KEY_CODE_KP_6)
                    .addConstant<std::size_t>("KEY_CODE_KP_7",KeyCode::KEY_CODE_KP_7)
                    .addConstant<std::size_t>("KEY_CODE_KP_8",KeyCode::KEY_CODE_KP_8)
                    .addConstant<std::size_t>("KEY_CODE_KP_9",KeyCode::KEY_CODE_KP_9)
                    .addConstant<std::size_t>("KEY_CODE_KP_DECIMAL",KeyCode::KEY_CODE_KP_DECIMAL)
                    .addConstant<std::size_t>("KEY_CODE_KP_DIVIDE",KeyCode::KEY_CODE_KP_DIVIDE)
                    .addConstant<std::size_t>("KEY_CODE_KP_MULTIPLY",KeyCode::KEY_CODE_KP_MULTIPLY)
                    .addConstant<std::size_t>("KEY_CODE_KP_SUBTRACT",KeyCode::KEY_CODE_KP_SUBTRACT)
                    .addConstant<std::size_t>("KEY_CODE_KP_ADD",KeyCode::KEY_CODE_KP_ADD)
                    .addConstant<std::size_t>("KEY_CODE_KP_ENTER",KeyCode::KEY_CODE_KP_ENTER)
                    .addConstant<std::size_t>("KEY_CODE_KP_EQUAL",KeyCode::KEY_CODE_KP_EQUAL)
                    .addConstant<std::size_t>("KEY_CODE_LEFT_SHIFT",KeyCode::KEY_CODE_LEFT_SHIFT)
                    .addConstant<std::size_t>("KEY_CODE_LEFT_CONTROL",KeyCode::KEY_CODE_LEFT_CONTROL)
                    .addConstant<std::size_t>("KEY_CODE_LEFT_ALT",KeyCode::KEY_CODE_LEFT_ALT)
                    .addConstant<std::size_t>("KEY_CODE_LEFT_SUPER",KeyCode::KEY_CODE_LEFT_SUPER)
                    .addConstant<std::size_t>("KEY_CODE_RIGHT_SHIFT",KeyCode::KEY_CODE_RIGHT_SHIFT)
                    .addConstant<std::size_t>("KEY_CODE_RIGHT_CONTROL",KeyCode::KEY_CODE_RIGHT_CONTROL)
                    .addConstant<std::size_t>("KEY_CODE_RIGHT_ALT",KeyCode::KEY_CODE_RIGHT_ALT)
                    .addConstant<std::size_t>("KEY_CODE_RIGHT_SUPER",KeyCode::KEY_CODE_RIGHT_SUPER)
                    .addConstant<std::size_t>("KEY_CODE_MENU",KeyCode::KEY_CODE_MENU)
                    .endNamespace();
        }


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

void LuaBinding::RunLuaFile(std::string script_file_path) {
    auto result= sol_state.script_file(script_file_path);
    if(result.valid()==false){
        sol::error err = result;
        DEBUG_LOG_ERROR("---- LOAD LUA ERROR ----\n{}\n------------------------",err.what());
    }
}

sol::protected_function_result LuaBinding::CallLuaFunction(std::string function_name) {
    sol::protected_function main_function=sol_state["main"];
    sol::protected_function_result result=main_function();
    if(result.valid()== false){
        sol::error err = result;
        DEBUG_LOG_ERROR("---- RUN LUA_FUNCTION ERROR ----\n{}\n------------------------",err.what());
    }
    return result;
}
#endif