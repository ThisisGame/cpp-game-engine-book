//
// Created by captainchen on 2021/9/13.
//

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
#include "renderer/skinned_mesh_renderer.h"
#include "renderer/shader.h"
#include "renderer/texture2d.h"
#include "renderer/animation_clip.h"
#include "renderer/animation.h"
#include "ui/ui_button.h"
#include "ui/ui_camera.h"
#include "ui/ui_image.h"
#include "ui/ui_mask.h"
#include "ui/ui_text.h"
#include "utils/application.h"
#include "utils/debug.h"
#include "utils/screen.h"
#include "utils/time.h"

sol::state LuaBinding::sol_state_;

void LuaBinding::Init(std::string package_path) {
    sol_state_.open_libraries(sol::lib::base,sol::lib::package);
    //设置lua搜索目录
    sol::table package_table=sol_state_["package"];
    std::string path=package_table["path"];
    path.append(package_path);
    package_table["path"]=path;
}

namespace sol2{
    /// 将lua中的table转换为std::vector
    /// \tparam elementType
    /// \param t
    /// \return
    template <typename elementType>
    std::vector<elementType> convert_sequence(sol::table t)
    {
        std::size_t sz = t.size();
        std::vector<elementType> res(sz);
        for (int i = 1; i <= sz; i++) {
            res[i - 1] = t[i];
        }
        return res;
    }
}

void LuaBinding::BindLua() {
    //辅助函数
    {
        auto sol2_ns_table = sol_state_["sol2"].get_or_create<sol::table>();
        sol2_ns_table["convert_sequence_float"]=&sol2::convert_sequence<float>;
        sol2_ns_table["convert_sequence_ushort"]=&sol2::convert_sequence<unsigned short>;
        sol2_ns_table["convert_sequence_uchar"]=&sol2::convert_sequence<unsigned char>;
        sol2_ns_table["convert_sequence_int"]=&sol2::convert_sequence<int>;
    }
    //绑定glm::vec3
    {
        auto glm_ns_table = sol_state_["glm"].get_or_create<sol::table>();
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
        auto glm_ns_table = sol_state_["glm"].get_or_create<sol::table>();
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
        auto glm_ns_table = sol_state_["glm"].get_or_create<sol::table>();
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
        auto glm_ns_table = sol_state_["glm"].get_or_create<sol::table>();
        glm_ns_table.set_function("rotate",sol::overload([] (const glm::mat4* m,const float f,const glm::vec3* v) {return glm::rotate(*m,f,*v);}));
        glm_ns_table.set_function("radians",sol::overload([] (const float f) {return glm::radians(f);}));
        glm_ns_table.set_function("to_string",sol::overload(
                [] (const glm::mat4* m) {return glm::to_string((*m));},
                [] (const glm::vec3* v) {return glm::to_string((*v));}
        ));
    }

    // audio
    {
        // FMOD_RESULT
        {
            sol_state_.new_enum<FMOD_RESULT,true>("FMOD_RESULT",{
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

        sol_state_.new_usertype<Audio>("Audio",
                                       "Init",&Audio::Init,
                                       "Update",&Audio::Update
        );

        sol_state_.new_usertype<Audio>("AudioStudio",
                                       "Init",&AudioStudio::Init,
                                       "Update",&AudioStudio::Update,
                                       "LoadBankFile",&AudioStudio::LoadBankFile,
                                       "CreateEventInstance",&AudioStudio::CreateEventInstance,
                                       "SetListenerAttributes",&AudioStudio::SetListenerAttributes
        );

        sol_state_.new_usertype<AudioStudioEvent>("AudioStudioEvent",
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
        sol_state_.new_usertype<GameObject>("GameObject",sol::call_constructor,sol::constructors<GameObject(std::string)>(),
                                            "name",&GameObject::name,
                                            "set_name",&GameObject::set_name,
                                            "layer",&GameObject::layer,
                                            "set_layer",&GameObject::set_layer,
                                            "AddComponent", &GameObject::AddComponentFromLua,
                                            "GetComponent",&GameObject::GetComponentFromLua,
                                            "SetParent",&GameObject::SetParent,
                                            "Foreach",&GameObject::ForeachLuaComponent
        );

        sol_state_.new_usertype<Component>("Component",sol::call_constructor,sol::constructors<Component()>(),
                                           "Awake",&Component::Awake,
                                           "Update",&Component::Update,
                                           "game_object",&Component::game_object,
                                           "set_game_object",&Component::set_game_object
        );

        sol_state_.new_usertype<Transform>("Transform",sol::call_constructor,sol::constructors<Transform()>(),
                                           sol::base_classes,sol::bases<Component>(),
                                           "position", &Transform::position,
                                           "rotation", &Transform::rotation,
                                           "scale", &Transform::scale,
                                           "set_position", &Transform::set_position,
                                           "set_rotation", &Transform::set_rotation,
                                           "set_scale", &Transform::set_scale
        );
    }

    // control
    {
        sol_state_.new_enum<KeyAction,true>("KeyAction",{
                {"KEY_ACTION_UP",KeyAction::KEY_ACTION_UP},
                {"KEY_ACTION_DOWN",KeyAction::KEY_ACTION_DOWN},
                {"KEY_ACTION_REPEAT",KeyAction::KEY_ACTION_REPEAT}
        });

        sol_state_.new_enum<KeyCode,true>("KeyCode",{
                {"KEY_CODE_UNKNOWN",KeyCode::KEY_CODE_UNKNOWN},
                {"MOUSE_BUTTON_1",KeyCode::MOUSE_BUTTON_1},
                {"MOUSE_BUTTON_2",KeyCode::MOUSE_BUTTON_2},
                {"MOUSE_BUTTON_3",KeyCode::MOUSE_BUTTON_3},
                {"MOUSE_BUTTON_4",KeyCode::MOUSE_BUTTON_4},
                {"MOUSE_BUTTON_5",KeyCode::MOUSE_BUTTON_5},
                {"MOUSE_BUTTON_6",KeyCode::MOUSE_BUTTON_6},
                {"MOUSE_BUTTON_7",KeyCode::MOUSE_BUTTON_7},
                {"MOUSE_BUTTON_8",KeyCode::MOUSE_BUTTON_8},
                {"MOUSE_BUTTON_LAST",KeyCode::MOUSE_BUTTON_LAST},
                {"MOUSE_BUTTON_LEFT",KeyCode::MOUSE_BUTTON_LEFT},
                {"MOUSE_BUTTON_RIGHT",KeyCode::MOUSE_BUTTON_RIGHT},
                {"MOUSE_BUTTON_MIDDLE",KeyCode::MOUSE_BUTTON_MIDDLE},
                {"KEY_CODE_SPACE",KeyCode::KEY_CODE_SPACE},
                {"KEY_CODE_APOSTROPHE",KeyCode::KEY_CODE_APOSTROPHE},
                {"KEY_CODE_COMMA",KeyCode::KEY_CODE_COMMA},
                {"KEY_CODE_MINUS",KeyCode::KEY_CODE_MINUS},
                {"KEY_CODE_PERIOD",KeyCode::KEY_CODE_PERIOD},
                {"KEY_CODE_SLASH",KeyCode::KEY_CODE_SLASH},
                {"KEY_CODE_0",KeyCode::KEY_CODE_0},
                {"KEY_CODE_1",KeyCode::KEY_CODE_1},
                {"KEY_CODE_2",KeyCode::KEY_CODE_2},
                {"KEY_CODE_3",KeyCode::KEY_CODE_3},
                {"KEY_CODE_4",KeyCode::KEY_CODE_4},
                {"KEY_CODE_5",KeyCode::KEY_CODE_5},
                {"KEY_CODE_6",KeyCode::KEY_CODE_6},
                {"KEY_CODE_7",KeyCode::KEY_CODE_7},
                {"KEY_CODE_8",KeyCode::KEY_CODE_8},
                {"KEY_CODE_9",KeyCode::KEY_CODE_9},
                {"KEY_CODE_SEMICOLON",KeyCode::KEY_CODE_SEMICOLON},
                {"KEY_CODE_EQUAL",KeyCode::KEY_CODE_EQUAL},
                {"KEY_CODE_A",KeyCode::KEY_CODE_A},
                {"KEY_CODE_B",KeyCode::KEY_CODE_B},
                {"KEY_CODE_C",KeyCode::KEY_CODE_C},
                {"KEY_CODE_D",KeyCode::KEY_CODE_D},
                {"KEY_CODE_E",KeyCode::KEY_CODE_E},
                {"KEY_CODE_F",KeyCode::KEY_CODE_F},
                {"KEY_CODE_G",KeyCode::KEY_CODE_G},
                {"KEY_CODE_H",KeyCode::KEY_CODE_H},
                {"KEY_CODE_I",KeyCode::KEY_CODE_I},
                {"KEY_CODE_J",KeyCode::KEY_CODE_J},
                {"KEY_CODE_K",KeyCode::KEY_CODE_K},
                {"KEY_CODE_L",KeyCode::KEY_CODE_L},
                {"KEY_CODE_M",KeyCode::KEY_CODE_M},
                {"KEY_CODE_N",KeyCode::KEY_CODE_N},
                {"KEY_CODE_O",KeyCode::KEY_CODE_O},
                {"KEY_CODE_P",KeyCode::KEY_CODE_P},
                {"KEY_CODE_Q",KeyCode::KEY_CODE_Q},
                {"KEY_CODE_R",KeyCode::KEY_CODE_R},
                {"KEY_CODE_S",KeyCode::KEY_CODE_S},
                {"KEY_CODE_T",KeyCode::KEY_CODE_T},
                {"KEY_CODE_U",KeyCode::KEY_CODE_U},
                {"KEY_CODE_V",KeyCode::KEY_CODE_V},
                {"KEY_CODE_W",KeyCode::KEY_CODE_W},
                {"KEY_CODE_X",KeyCode::KEY_CODE_X},
                {"KEY_CODE_Y",KeyCode::KEY_CODE_Y},
                {"KEY_CODE_Z",KeyCode::KEY_CODE_Z},
                {"KEY_CODE_LEFT_BRACKET",KeyCode::KEY_CODE_LEFT_BRACKET},
                {"KEY_CODE_BACKSLASH",KeyCode::KEY_CODE_BACKSLASH},
                {"KEY_CODE_RIGHT_BRACKET",KeyCode::KEY_CODE_RIGHT_BRACKET},
                {"KEY_CODE_GRAVE_ACCENT",KeyCode::KEY_CODE_GRAVE_ACCENT},
                {"KEY_CODE_WORLD_1",KeyCode::KEY_CODE_WORLD_1},
                {"KEY_CODE_WORLD_2",KeyCode::KEY_CODE_WORLD_2},
                {"KEY_CODE_ESCAPE",KeyCode::KEY_CODE_ESCAPE},
                {"KEY_CODE_ENTER",KeyCode::KEY_CODE_ENTER},
                {"KEY_CODE_TAB",KeyCode::KEY_CODE_TAB},
                {"KEY_CODE_BACKSPACE",KeyCode::KEY_CODE_BACKSPACE},
                {"KEY_CODE_INSERT",KeyCode::KEY_CODE_INSERT},
                {"KEY_CODE_DELETE",KeyCode::KEY_CODE_DELETE},
                {"KEY_CODE_RIGHT",KeyCode::KEY_CODE_RIGHT},
                {"KEY_CODE_LEFT",KeyCode::KEY_CODE_LEFT},
                {"KEY_CODE_DOWN",KeyCode::KEY_CODE_DOWN},
                {"KEY_CODE_UP",KeyCode::KEY_CODE_UP},
                {"KEY_CODE_PAGE_UP",KeyCode::KEY_CODE_PAGE_UP},
                {"KEY_CODE_PAGE_DOWN",KeyCode::KEY_CODE_PAGE_DOWN},
                {"KEY_CODE_HOME",KeyCode::KEY_CODE_HOME},
                {"KEY_CODE_END",KeyCode::KEY_CODE_END},
                {"KEY_CODE_CAPS_LOCK",KeyCode::KEY_CODE_CAPS_LOCK},
                {"KEY_CODE_SCROLL_LOCK",KeyCode::KEY_CODE_SCROLL_LOCK},
                {"KEY_CODE_NUM_LOCK",KeyCode::KEY_CODE_NUM_LOCK},
                {"KEY_CODE_PRINT_SCREEN",KeyCode::KEY_CODE_PRINT_SCREEN},
                {"KEY_CODE_PAUSE",KeyCode::KEY_CODE_PAUSE},
                {"KEY_CODE_F1",KeyCode::KEY_CODE_F1},
                {"KEY_CODE_F2",KeyCode::KEY_CODE_F2},
                {"KEY_CODE_F3",KeyCode::KEY_CODE_F3},
                {"KEY_CODE_F4",KeyCode::KEY_CODE_F4},
                {"KEY_CODE_F5",KeyCode::KEY_CODE_F5},
                {"KEY_CODE_F6",KeyCode::KEY_CODE_F6},
                {"KEY_CODE_F7",KeyCode::KEY_CODE_F7},
                {"KEY_CODE_F8",KeyCode::KEY_CODE_F8},
                {"KEY_CODE_F9",KeyCode::KEY_CODE_F9},
                {"KEY_CODE_F10",KeyCode::KEY_CODE_F10},
                {"KEY_CODE_F11",KeyCode::KEY_CODE_F11},
                {"KEY_CODE_F12",KeyCode::KEY_CODE_F12},
                {"KEY_CODE_F13",KeyCode::KEY_CODE_F13},
                {"KEY_CODE_F14",KeyCode::KEY_CODE_F14},
                {"KEY_CODE_F15",KeyCode::KEY_CODE_F15},
                {"KEY_CODE_F16",KeyCode::KEY_CODE_F16},
                {"KEY_CODE_F17",KeyCode::KEY_CODE_F17},
                {"KEY_CODE_F18",KeyCode::KEY_CODE_F18},
                {"KEY_CODE_F19",KeyCode::KEY_CODE_F19},
                {"KEY_CODE_F20",KeyCode::KEY_CODE_F20},
                {"KEY_CODE_F21",KeyCode::KEY_CODE_F21},
                {"KEY_CODE_F22",KeyCode::KEY_CODE_F22},
                {"KEY_CODE_F23",KeyCode::KEY_CODE_F23},
                {"KEY_CODE_F24",KeyCode::KEY_CODE_F24},
                {"KEY_CODE_F25",KeyCode::KEY_CODE_F25},
                {"KEY_CODE_KP_0",KeyCode::KEY_CODE_KP_0},
                {"KEY_CODE_KP_1",KeyCode::KEY_CODE_KP_1},
                {"KEY_CODE_KP_2",KeyCode::KEY_CODE_KP_2},
                {"KEY_CODE_KP_3",KeyCode::KEY_CODE_KP_3},
                {"KEY_CODE_KP_4",KeyCode::KEY_CODE_KP_4},
                {"KEY_CODE_KP_5",KeyCode::KEY_CODE_KP_5},
                {"KEY_CODE_KP_6",KeyCode::KEY_CODE_KP_6},
                {"KEY_CODE_KP_7",KeyCode::KEY_CODE_KP_7},
                {"KEY_CODE_KP_8",KeyCode::KEY_CODE_KP_8},
                {"KEY_CODE_KP_9",KeyCode::KEY_CODE_KP_9},
                {"KEY_CODE_KP_DECIMAL",KeyCode::KEY_CODE_KP_DECIMAL},
                {"KEY_CODE_KP_DIVIDE",KeyCode::KEY_CODE_KP_DIVIDE},
                {"KEY_CODE_KP_MULTIPLY",KeyCode::KEY_CODE_KP_MULTIPLY},
                {"KEY_CODE_KP_SUBTRACT",KeyCode::KEY_CODE_KP_SUBTRACT},
                {"KEY_CODE_KP_ADD",KeyCode::KEY_CODE_KP_ADD},
                {"KEY_CODE_KP_ENTER",KeyCode::KEY_CODE_KP_ENTER},
                {"KEY_CODE_KP_EQUAL",KeyCode::KEY_CODE_KP_EQUAL},
                {"KEY_CODE_LEFT_SHIFT",KeyCode::KEY_CODE_LEFT_SHIFT},
                {"KEY_CODE_LEFT_CONTROL",KeyCode::KEY_CODE_LEFT_CONTROL},
                {"KEY_CODE_LEFT_ALT",KeyCode::KEY_CODE_LEFT_ALT},
                {"KEY_CODE_LEFT_SUPER",KeyCode::KEY_CODE_LEFT_SUPER},
                {"KEY_CODE_RIGHT_SHIFT",KeyCode::KEY_CODE_RIGHT_SHIFT},
                {"KEY_CODE_RIGHT_CONTROL",KeyCode::KEY_CODE_RIGHT_CONTROL},
                {"KEY_CODE_RIGHT_ALT",KeyCode::KEY_CODE_RIGHT_ALT},
                {"KEY_CODE_RIGHT_SUPER",KeyCode::KEY_CODE_RIGHT_SUPER},
                {"KEY_CODE_MENU",KeyCode::KEY_CODE_MENU}
        });

        sol_state_.new_enum<KeyAction,true>("KeyAction",{
                {"KEY_ACTION_UP",KeyAction::KEY_ACTION_UP},
                {"KEY_ACTION_DOWN",KeyAction::KEY_ACTION_DOWN},
                {"KEY_ACTION_REPEAT",KeyAction::KEY_ACTION_REPEAT}
        });

        sol_state_.new_usertype<Input>("Input",
                                       "RecordKey",&Input::RecordKey,
                                       "GetKey",&Input::GetKey,
                                       "GetKeyDown",&Input::GetKeyDown,
                                       "GetKeyUp",&Input::GetKeyUp,
                                       "Update",&Input::Update,
                                       "GetMouseButton",&Input::GetMouseButton,
                                       "GetMouseButtonDown",&Input::GetMouseButtonDown,
                                       "GetMouseButtonUp",&Input::GetMouseButtonUp,
                                       "mousePosition",&Input::mousePosition,
                                       "set_mousePosition",&Input::set_mousePosition,
                                       "mouse_scroll",&Input::mouse_scroll,
                                       "RecordScroll",&Input::RecordScroll
        );
    }

    // renderer
    {
        sol_state_.new_usertype<Camera>("Camera",sol::call_constructor,sol::constructors<Camera()>(),
                                        sol::base_classes,sol::bases<Component>(),
                                        "SetView",&Camera::SetView,
                                        "SetPerspective",&Camera::SetPerspective,
                                        "SetOrthographic",&Camera::SetOrthographic,
                                        "view_mat4",&Camera::view_mat4,
                                        "projection_mat4",&Camera::projection_mat4,
                                        "set_clear_color",&Camera::set_clear_color,
                                        "set_clear_flag",&Camera::set_clear_flag,
                                        "SetClearFlagAndClearColorBuffer",&Camera::Clear,
                                        "depth",&Camera::depth,
                                        "set_depth",&Camera::set_depth,
                                        "culling_mask",&Camera::culling_mask,
                                        "set_culling_mask",&Camera::set_culling_mask,
                                        "Foreach",&Camera::Foreach,
                                        "current_camera",&Camera::current_camera,
                                        "Sort",&Camera::Sort
        );

        sol_state_.new_usertype<UICamera>("UICamera",sol::call_constructor,sol::constructors<UICamera()>(),
                                                sol::base_classes,sol::bases<Camera,Component>()
        );

        sol_state_.new_usertype<Material>("Material",sol::call_constructor,sol::constructors<Material()>(),
                                          "Parse",&Material::Parse,
                                          "SetUniform1i",&Material::SetUniform1i,
                                          "SetTexture",&Material::SetTexture
        );

        sol_state_.new_usertype<MeshFilter>("MeshFilter",sol::call_constructor,sol::constructors<MeshFilter()>(),
                                            sol::base_classes,sol::bases<Component>(),
                                            "LoadMesh", &MeshFilter::LoadMesh,
                                            "CreateMesh", [] (MeshFilter* meshFilter,std::vector<float>& vertex_data,std::vector<unsigned short>& vertex_index_data)
                                                                {return meshFilter->CreateMesh(vertex_data,vertex_index_data);},
                                            "GetMeshName",&MeshFilter::GetMeshName,
                                            "set_vertex_relate_bone_infos",&MeshFilter::set_vertex_relate_bone_infos,
                                            "LoadWeight",&MeshFilter::LoadWeight
        );

        sol_state_.new_usertype<MeshRenderer>("MeshRenderer",sol::call_constructor,sol::constructors<MeshRenderer()>(),
                                              sol::base_classes,sol::bases<Component>(),
                                              "SetMaterial", &MeshRenderer::SetMaterial,
                                              "material", &MeshRenderer::material,
                                              "Render", &MeshRenderer::Render
        );


        sol_state_.new_usertype<SkinnedMeshRenderer>("SkinnedMeshRenderer",sol::call_constructor,sol::constructors<SkinnedMeshRenderer()>(),
                                                     sol::base_classes,sol::bases<MeshRenderer,Component>()
        );

        sol_state_.new_usertype<Shader>("Shader",sol::call_constructor,sol::constructors<Shader()>(),
                                        "Parse", &Shader::Parse,
                                        "CreateShaderProgram", &Shader::CreateShaderProgram,
                                        "Active", &Shader::Active,
                                        "InActive", &Shader::InActive,
                                        "shader_program_handle", &Shader::shader_program_handle,
                                        "Find", &Shader::Find
        );

        sol_state_.new_usertype<Texture2D>("Texture2D",
                                           "mipmap_level", &Texture2D::mipmap_level,
                                           "width", &Texture2D::width,
                                           "height", &Texture2D::height,
                                           "gl_texture_format", &Texture2D::gl_texture_format,
                                           "texture_handle", &Texture2D::texture_handle,
                                           "LoadFromFile", &Texture2D::LoadFromFile
        );

        sol_state_.new_usertype<AnimationClip>("AnimationClip",sol::call_constructor,sol::constructors<AnimationClip()>(),
                                            "LoadFromFile", &AnimationClip::LoadFromFile,
                                            "Play", &AnimationClip::Play,
                                            "Stop", &AnimationClip::Stop,
                                            "Update", &AnimationClip::Update
        );

        sol_state_.new_usertype<Animation>("Animation",sol::call_constructor,sol::constructors<Animation()>(),
                                              sol::base_classes,sol::bases<Component>(),
                                              "LoadAnimationClipFromFile", &Animation::LoadAnimationClipFromFile,
                                              "Play", &Animation::Play,
                                              "current_animation_clip", &Animation::current_animation_clip
        );

    }

    // utils
    {
        sol_state_.new_usertype<Application>("Application",
                                             "set_title",&Application::set_title,
                                             "set_data_path",&Application::set_data_path,
                                             "Init",&Application::Init,
                                             "Run",&Application::Run,
                                             "UpdateScreenSize",&Application::UpdateScreenSize,
                                             "Update",&Application::Update,
                                             "Render",&Application::Render
        );

        sol_state_.new_usertype<Debug>("Debug",
                                       "Init",&Debug::Init
        );

        sol_state_.new_usertype<Screen>("Screen",
                                        "width",&Screen::width,
                                        "height",&Screen::height,
                                        "aspect_ratio",&Screen::aspect_ratio,
                                        "set_width",&Screen::set_width,
                                        "set_height",&Screen::set_height,
                                        "set_width_height",&Screen::set_width_height
        );

        sol_state_.new_usertype<Time>("Time",
                                      "Init",&Time::Init,
                                      "Update",&Time::Update,
                                      "TimeSinceStartup",&Time::TimeSinceStartup,
                                      "delta_time",&Time::delta_time
        );
    }
}

void LuaBinding::RunLuaFile(std::string script_file_path) {
    auto result= sol_state_.script_file(script_file_path);
    if(result.valid()==false){
        sol::error err = result;
        DEBUG_LOG_ERROR("\n---- LOAD LUA ERROR ----\n{}\n------------------------",err.what());
    }
}

sol::protected_function_result LuaBinding::CallLuaFunction(std::string function_name) {
    sol::protected_function main_function=sol_state_["main"];
    sol::protected_function_result result=main_function();
    if(result.valid()== false){
        sol::error err = result;
        DEBUG_LOG_ERROR("\n---- RUN LUA_FUNCTION ERROR ----\n{}\n------------------------",err.what());
    }
    return result;
}