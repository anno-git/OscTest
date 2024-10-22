#pragma once

#include "ofxGlobalContext.h"
#include "ofxImGui.h"
#include "ofxPubSubOsc.h"
#include <iostream>

class OscTester : public ofxGlobalContext::Context{

public:
    struct Message {
        int ip_list[4] = {127, 0, 0, 1};
        string host = "127.0.0.1";
        int port = 9000;
        int send_receive = 0;
        bool subscribe = false;
        string address = "/";
        string message_str = "/";
        vector<string> selected_types;
        vector<ofPtr<ofxOscArg>> args;
        float sent_time = -1;
    };
    
    vector<ofPtr<Message>> messages;
    string saved_filename = "osc.json";
    vector<string> logs;
    const int LOG_SIZE = 15;
    float received_time = 0;
    bool bundle = true;

    void setup()
    {
    }
    
    void startSubscribe(ofPtr<Message> message)
    {
        ofxSubscribeOsc(message->port, message->address, [&](ofxOscMessage& m){
            int num = m.getNumArgs();
            string received_msg = "received from host: " + m.getRemoteHost() + " on port: " + ofToString(m.getRemotePort()) + " message: " + m.getAddress();
            for(int i = 0; i < num; i++)
            {
                ofxOscArgType type = m.getArgType(i);
                switch(type)
                {
                    case OFXOSC_TYPE_INT32:
                    {
                        received_msg += " " + ofToString(m.getArgAsInt32(i)) + "[int32]";
                        break;
                    }
                    case OFXOSC_TYPE_INT64:
                    {
                        received_msg += " " + ofToString(m.getArgAsInt64(i)) + "[int64]";
                        break;
                    }
                    case OFXOSC_TYPE_FLOAT:
                    {
                        received_msg += " " + ofToString(m.getArgAsFloat(i)) + "[float]";
                        break;
                    }
                    case OFXOSC_TYPE_DOUBLE:
                    {
                        received_msg += " " + ofToString(m.getArgAsDouble(i)) + "[double]";
                        break;
                    }
                    case OFXOSC_TYPE_STRING:
                    {
                        received_msg += " " + ofToString(m.getArgAsString(i)) + "[string]";
                        break;
                    }
                    case OFXOSC_TYPE_SYMBOL:
                    {
                        received_msg += " " + ofToString(m.getArgAsSymbol(i)) + "[symbol]";
                        break;
                    }
                    case OFXOSC_TYPE_CHAR:
                    {
                        received_msg += " " + ofToString(m.getArgAsChar(i)) + "[char]";
                        break;
                    }
                    case OFXOSC_TYPE_TIMETAG:
                    {
                        received_msg += " " + ofToString(m.getArgAsTimetag(i)) + "[timetag]";
                        break;
                    }
                    case OFXOSC_TYPE_MIDI_MESSAGE:
                    {
                        received_msg += " " + ofToString(m.getArgAsMidiMessage(i)) + "[midi message]";
                        break;
                    }
                    case OFXOSC_TYPE_RGBA_COLOR:
                    {
                        received_msg += " " + ofToString(m.getArgAsRgbaColor(i)) + "[RGBA color]";
                        break;
                    }
                    case OFXOSC_TYPE_TRUE:
                    case OFXOSC_TYPE_FALSE:
                    case OFXOSC_TYPE_TRIGGER:
                    {
                        received_msg += " " + ofToString(m.getArgAsBool(i)) + "[bool]";
                        break;
                    }
                    case OFXOSC_TYPE_BLOB:
                    {
                        received_msg += " Object[blob]";
                        break;
                    }
                    default:
                    {
                        received_msg += " type unknown: " + ofToString(m);
                        break;
                    }
                }
            }
            received_msg += " on " + ofToString(ofGetElapsedTimef()) + " ms";
            logs.push_back(received_msg);
            if(logs.size() > LOG_SIZE)
            {
                logs.erase(logs.begin());
            }
            received_time = ofGetElapsedTimef();
        });
    }
    
    void load(std::filesystem::path filePath)
    {
        ofFile f(filePath);
        if(f.exists())
        {
            ofJson json;
            json << f;
            messages.clear();
            bundle = json["bundle"];
            for(int i = 0; i < json["messages"].size(); i++)
            {
                messages.emplace_back(ofPtr<Message>(new Message));
                messages.back()->host = "";
                for(int j = 0; j < 4; j++)
                {
                    messages.back()->ip_list[j] = json["messages"][i]["ip_list"][j];
                    if(j != 0)
                    {
                        messages.back()->host += ".";
                    }
                    messages.back()->host += ofToString(json["messages"][i]["ip_list"][j]);
                }
                messages.back()->port = json["messages"][i]["port"];
                messages.back()->send_receive = json["messages"][i]["send_receive"];
                messages.back()->subscribe = json["messages"][i]["subscribe"];
                messages.back()->address = json["messages"][i]["address"];
                messages.back()->message_str = json["messages"][i]["message_str"];
                vector<string> types = json["messages"][i]["selected_types"];
                messages.back()->selected_types = types;
                for(int j = 0; j < messages.back()->selected_types.size(); j++)
                {
                    string type = messages.back()->selected_types[j];
                    if(type.find(":int32") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgInt32>(new ofxOscArgInt32(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":int64") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgInt64>(new ofxOscArgInt64(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":float") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgFloat>(new ofxOscArgFloat(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":double") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgDouble>(new ofxOscArgDouble(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":string") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgString>(new ofxOscArgString(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":char") != string::npos)
                    {
                        int c = json["messages"][i]["args"][j];
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgChar>(new ofxOscArgChar((char)c)));
                    }
                    else if(type.find(":midi_message") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgMidiMessage>(new ofxOscArgMidiMessage(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":bool") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgBool>(new ofxOscArgBool(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":timetag") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgTimetag>(new ofxOscArgTimetag(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":rgba_color") != string::npos)
                    {
                        messages.back()->args.emplace_back(ofPtr<ofxOscArgRgbaColor>(new ofxOscArgRgbaColor(json["messages"][i]["args"][j])));
                    }
                    else if(type.find(":blob") != string::npos)
                    {
                        ofFileDialogResult r = ofSystemLoadDialog("Select a file for arg[" + ofToString(j) + "]");
                        if(r.bSuccess)
                        {
                            ofFile f(r.getPath());
                            if(f.exists())
                            {
                                ofBuffer buff = ofBufferFromFile(r.getPath(), true);
                                messages.back()->args.emplace_back(ofPtr<ofxOscArgBlob>(new ofxOscArgBlob(buff)));
                            }
                        }
                    }
                }
                if(messages.back()->subscribe)
                {
                    startSubscribe(messages.back());
                }
            }
        }
    }
    
    void save(std::filesystem::path filePath)
    {
        ofJson json;
        json["bundle"] = bundle;
        for(int i = 0; i < messages.size(); i++)
        {
            for(int j = 0; j < 4; j++)
            {
                json["messages"][i]["ip_list"][j] = messages[i]->ip_list[j];
            }
            json["messages"][i]["host"] = messages[i]->host;
            json["messages"][i]["port"] = messages[i]->port;
            json["messages"][i]["send_receive"] = messages[i]->send_receive;
            json["messages"][i]["subscribe"] = messages[i]->subscribe;
            json["messages"][i]["address"] = messages[i]->address;
            json["messages"][i]["message_str"] = messages[i]->message_str;
            json["messages"][i]["selected_types"] = messages[i]->selected_types;
            for(int j = 0; j < messages[i]->args.size();j++)
            {
                ofPtr<ofxOscArg> arg = messages[i]->args[j];
                ofxOscArgType t = arg->getType();
                switch(t)
                {
                    case OFXOSC_TYPE_INT32:
                    {
                        ofxOscArgInt32* a = dynamic_cast<ofxOscArgInt32*>(arg.get());
                        int32_t r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_INT64:
                    {
                        ofxOscArgInt64* a = dynamic_cast<ofxOscArgInt64*>(arg.get());
                        int64_t r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_FLOAT:
                    {
                        ofxOscArgFloat* a = dynamic_cast<ofxOscArgFloat*>(arg.get());
                        float r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_DOUBLE:
                    {
                        ofxOscArgDouble* a = dynamic_cast<ofxOscArgDouble*>(arg.get());
                        double r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_STRING:
                    {
                        ofxOscArgString* a = dynamic_cast<ofxOscArgString*>(arg.get());
                        string r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_CHAR:
                    {
                        ofxOscArgChar* a = dynamic_cast<ofxOscArgChar*>(arg.get());
                        int r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_MIDI_MESSAGE:
                    {
                        ofxOscArgMidiMessage* a = dynamic_cast<ofxOscArgMidiMessage*>(arg.get());
                        uint32_t r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_TIMETAG:
                    {
                        ofxOscArgTimetag* a = dynamic_cast<ofxOscArgTimetag*>(arg.get());
                        uint64_t r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_BLOB:
                    {
                        json["messages"][i]["args"][j] = "blobdata";
                        break;
                    }
                    case OFXOSC_TYPE_TRUE:
                    case OFXOSC_TYPE_FALSE:
                    case OFXOSC_TYPE_TRIGGER:
                    {
                        ofxOscArgBool* a = dynamic_cast<ofxOscArgBool*>(arg.get());
                        bool r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    case OFXOSC_TYPE_RGBA_COLOR:
                    {
                        ofxOscArgRgbaColor* a = dynamic_cast<ofxOscArgRgbaColor*>(arg.get());
                        uint32_t r = a->get();
                        json["messages"][i]["args"][j] = r;
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        ofSaveJson(filePath, json);
    }
    
    void update()
    {
    }
    
    void drawGui()
    {
        ImGui::Begin("osc");
        string name = "filename";
        char* set_str = const_cast<char*>(saved_filename.c_str());
        if(ImGui::InputText(name.c_str(), set_str, 100))
        {
            saved_filename = set_str;
        }
        if(ImGui::Checkbox("bundle", &bundle))
        {
        }
        if(ImGui::Button("save"))
        {
            save(saved_filename);
        }
        ImGui::SameLine();
        if(ImGui::Button("load"))
        {
            load(saved_filename);
        }
        if(ImGui::Button("add message"))
        {
            messages.emplace_back(ofPtr<Message>(new Message));
        }
        int i = 0;
        for(auto it = messages.begin(); it != messages.end();)
        {
            auto message = *it;
            name = "message[" + ofToString(i) + "]";
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            if(ImGui::TreeNode(name.c_str()))
            {
                name = "ip[" + ofToString(i) + "]";
                if(ImGui::DragInt4(name.c_str(), &message->ip_list[0], 1, 0, 255))
                {
                    message->host = ofToString(message->ip_list[0]) + "." + ofToString(message->ip_list[1]) + "." + ofToString(message->ip_list[2]) + "." + ofToString(message->ip_list[3]);
                }
                
                name = "send[" + ofToString(i) + "]";
                ImGui::RadioButton(name.c_str(), &message->send_receive, 0);
                ImGui::SameLine();
                name = "receive[" + ofToString(i) + "]";
                ImGui::RadioButton(name.c_str(), &message->send_receive, 1);
                
                if(!message->send_receive)
                {
                    if(message->subscribe)
                    {
                        message->subscribe = false;
                        ofxUnsubscribeOsc(message->port, message->address);
                    }
                }
//                message->message_str = message->address;
                if(message->subscribe)
                {
#if (IMGUI_VERSION_NUM > 18315)
                    ImGui::BeginDisabled(true);
                }
#else
                    name = "port[" + ofToString(i) + "] " + ofToString(message->port);
                    ImGui::Text(name.c_str());
                    
                    name = "address[" + ofToString(i) + "] " + message->address;
                    ImGui::Text(name.c_str());
                }
                else
                {
                    
#endif
                name = "port[" + ofToString(i) + "]";
                ImGui::DragInt(name.c_str(), &message->port, 1, 0, 65535);
                
                name = "address[" + ofToString(i) + "]";
                char* address_c = const_cast<char*>(message->message_str.c_str());
                if(ImGui::InputText(name.c_str(), address_c, 100))
                {
                    vector <string> tokens;
                         
                    // stringstream class check1
                    stringstream check1(address_c);
                     
                    string intermediate;
                     
                    // Tokenizing w.r.t. space ' '
                    while(getline(check1, intermediate, ' '))
                    {
                        tokens.push_back(intermediate);
                    }
                    if(tokens.size() > 0)
                    {
                        tokens[0].find("/") != string::npos;
                        message->address = tokens[0];
                    }
                    
                    int argsize = message->args.size();
                    int selected_typessize = message->selected_types.size();
                    if(argsize > 0 || selected_typessize > 0)
                    {
                        message->args.clear();
                        message->selected_types.clear();
                    }
                    
                    for(int j = 1; j < tokens.size(); j++)
                    {
                        message->selected_types.push_back("arg_type[" + ofToString(i) + "][" + ofToString(j - 1) + "]");
                        string t = tokens[j];
                        try {
                            float f = stof(t);
                            message->args.emplace_back(ofPtr<ofxOscArgFloat>(new ofxOscArgFloat(f)));
                            message->selected_types.back() += ":float";
                        }
                        catch (...) {
                            try {
                                int num = stoi(t);
                                message->args.emplace_back(ofPtr<ofxOscArgInt32>(new ofxOscArgInt32(num)));
                                message->selected_types.back() += ":int32";
                            }
                            catch (...) {
                                message->args.emplace_back(ofPtr<ofxOscArgString>(new ofxOscArgString(t)));
                                message->selected_types.back() += ":string";
                            }
                        }
                    }
                }

                
#if (IMGUI_VERSION_NUM > 18315)
                if(message->send_receive && !message->subscribe)
                {
                    ImGui::BeginDisabled(true);
                }
#else
                }
#endif
                if(ImGui::Button("add args"))
                {
                    if(!message->subscribe)
                    {
                        message->args.emplace_back(ofPtr<ofxOscArgNone>(new ofxOscArgNone()));
                        message->selected_types.push_back("");
                    }
                }
#if (IMGUI_VERSION_NUM > 18315)
                if(message->send_receive && !message->subscribe)
                {
                    ImGui::EndDisabled();
                }
#endif
                if(!message->send_receive)
                {
                    int j = 0;
                    message->message_str = message->address;
                    for(auto jt = message->args.begin(); jt != message->args.end();)
                    {
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Separator();
                        auto arg = *jt;
                        name = "arg_type[" + ofToString(i) + "][" + ofToString(j) + "]";
                        const string items[] = { name + ":int32", name + ":int64", name + ":float", name + ":double", name + ":string", name + ":char", name + ":midi_message", name + ":bool", name + ":timetag", name + ":blob", name + ":rgba_color"};
                        if (ImGui::BeginCombo(name.c_str(), const_cast<char*>(message->selected_types[j].c_str()))) // The second parameter is the label previewed before opening the combo.
                        {
                            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                            {
                                bool is_selected = (message->selected_types[j].c_str() == items[n]);
                                if (ImGui::Selectable(items[n].c_str(), is_selected))
                                {
                                    ImGui::SetItemDefaultFocus();
                                    message->selected_types[j] = items[n];
                                    switch(n)
                                    {
                                        case 0: // int32
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgInt32>(new ofxOscArgInt32(0)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 1: // int64
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgInt64>(new ofxOscArgInt64(0)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 2: // float
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgFloat>(new ofxOscArgFloat(0)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 3: // double
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgDouble>(new ofxOscArgDouble(0)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 4: // string
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgString>(new ofxOscArgString("")));
                                            arg = *jt;
                                            break;
                                        }
                                        case 5: // char
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            char c = 'a';
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgChar>(new ofxOscArgChar(c)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 6: // midi_message
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgMidiMessage>(new ofxOscArgMidiMessage(0)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 7: // bool
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgBool>(new ofxOscArgBool(0)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 8: // timetag
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgTimetag>(new ofxOscArgTimetag(0)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 9: // blob
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            ofBuffer buffer;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgBlob>(new ofxOscArgBlob(buffer)));
                                            arg = *jt;
                                            break;
                                        }
                                        case 10: // rgba_color
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgRgbaColor>(new ofxOscArgRgbaColor(0)));
                                            arg = *jt;
                                            break;
                                        }
                                        default: // int32
                                        {
                                            jt = message->args.erase(jt);
                                            arg = *jt;
                                            jt = message->args.insert(jt, ofPtr<ofxOscArgInt32>(new ofxOscArgInt32(0)));
                                            arg = *jt;
                                            break;
                                        }
                                    }
                                }
                            }
                            ImGui::EndCombo();
                        }
                        
                        name = "arg_value[" + ofToString(i) + "][" + ofToString(j) + "]";
                        switch(arg->getType())
                        {
                            case OFXOSC_TYPE_INT32:
                            {
                                ofxOscArgInt32* a = dynamic_cast<ofxOscArgInt32*>(arg.get());
                                int32_t r = a->get();
                                message->message_str += " " + ofToString(r);
                                ImGui::DragInt(name.c_str(), &r, 1);
                                {
                                    a->set(r);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_MIDI_MESSAGE:
                            {
                                ofxOscArgMidiMessage* a = dynamic_cast<ofxOscArgMidiMessage*>(arg.get());
                                int32_t r = a->get();
                                message->message_str += " " + ofToString(r);
                                ImGui::DragInt(name.c_str(), &r, 1);
                                {
                                    a->set(r);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_INT64:
                            {
                                ofxOscArgInt64* a = dynamic_cast<ofxOscArgInt64*>(arg.get());
                                int32_t r = a->get();
                                message->message_str += " " + ofToString(r);
                                ImGui::DragInt(name.c_str(), &r, 1);
                                {
                                    a->set(r);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_TIMETAG:
                            {
                                ofxOscArgTimetag* a = dynamic_cast<ofxOscArgTimetag*>(arg.get());
                                int32_t r = a->get();
                                message->message_str += " " + ofToString(r);
                                ImGui::DragInt(name.c_str(), &r, 1);
                                {
                                    a->set(r);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_FLOAT:
                            {
                                ofxOscArgFloat* a = dynamic_cast<ofxOscArgFloat*>(arg.get());
                                float r = a->get();
                                message->message_str += " " + ofToString(r);
                                if(ImGui::DragFloat(name.c_str(), &r, 1))
                                {
                                    a->set(r);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_DOUBLE:
                            {
                                ofxOscArgDouble* a = dynamic_cast<ofxOscArgDouble*>(arg.get());
                                float r = a->get();
                                message->message_str += " " + ofToString(r);
                                if(ImGui::DragFloat(name.c_str(), &r, 1))
                                {
                                    a->set(r);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_STRING:
                            {
                                ofxOscArgString* a = dynamic_cast<ofxOscArgString*>(arg.get());
                                string r = a->get();
                                message->message_str += " " + ofToString(r);
                                char* set_str = const_cast<char*>(r.c_str());
                                if(ImGui::InputText(name.c_str(), set_str, 100))
                                {
                                    a->set(set_str);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_CHAR:
                            {
                                ofxOscArgChar* a = dynamic_cast<ofxOscArgChar*>(arg.get());
                                string r = ofToString(a->get());
                                message->message_str += " " + ofToString(r);
                                char* set_str = const_cast<char*>(r.c_str());
                                if(ImGui::InputText(name.c_str(), set_str, 2))
                                {
                                    char c = set_str[0];
                                    a->set(c);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_TRUE:
                            case OFXOSC_TYPE_FALSE:
                            case OFXOSC_TYPE_TRIGGER:
                            {
                                ofxOscArgBool* a = dynamic_cast<ofxOscArgBool*>(arg.get());
                                bool r = a->get();
                                message->message_str += " " + ofToString(r);
                                if(ImGui::Checkbox(name.c_str(), &r))
                                {
                                    a->set(r);
                                }
                                break;
                            }
                            case OFXOSC_TYPE_BLOB:
                            {
                                ofxOscArgBlob* a = dynamic_cast<ofxOscArgBlob*>(arg.get());
                                message->message_str += " " + ofToString(a->get());
                                if(ImGui::Button(name.c_str()))
                                {
                                    ofFileDialogResult r = ofSystemLoadDialog("Select a file for arg[" + ofToString(j) + "]");
                                    if(r.bSuccess)
                                    {
                                        ofFile f(r.getPath());
                                        if(f.exists())
                                        {
                                            ofBuffer buff = ofBufferFromFile(r.getPath(), true);
                                            a->set(buff.getData(), buff.size());
                                        }
                                    }
                                }
                            }
                                break;
                            case OFXOSC_TYPE_RGBA_COLOR:
                            {
                                float color[] = {0, 0, 0, 1};
                                
                                ofxOscArgRgbaColor* a = dynamic_cast<ofxOscArgRgbaColor*>(arg.get());
                                uint32_t r = a->get();
                                message->message_str += " " + ofToString(r);
                                color[0] = (r >> 24 & 0xFF) / 255.;
                                color[1] = (r >> 16 & 0xFF) / 255.;
                                color[2] = (r >> 8 & 0xFF) / 255.;
                                color[3] = (r & 0xFF) / 255.;
                                if(ImGui::ColorPicker4(name.c_str(), &color[0]))
                                {
                                    r = (int)(color[0] * 255) << 24 | (int)(color[1] * 255) << 16 | (int)(color[2] * 255) << 8 | (int)(color[3] * 255);
                                    a->set(r);
                                }
                                break;
                            }
                            default:
                                
                                break;
                        }
                        name = "remove args[" + ofToString(i) + "][" + ofToString(j) + "]";
                        if(ImGui::Button(name.c_str()))
                        {
                            jt = message->args.erase(jt);
                            int p = 0;
                            for(auto pt = message->selected_types.begin(); pt != message->selected_types.end();)
                            {
                                if(p == j)
                                {
                                    pt = message->selected_types.erase(pt);
                                    break;
                                }
                                pt++;
                                p++;
                            }
                            break;
                        }
                        else
                        {
                            jt++;
                        }
                        j++;
                    }
                }

#if (IMGUI_VERSION_NUM > 18315)
                if(message->subscribe)
                {
                    ImGui::EndDisabled();
                }
#endif

                ImGui::TreePop();
            }
            ImGui::Spacing();
            if(message->send_receive)
            {
                name = "receive osc[" + ofToString(i) + "]";
                if(ImGui::Checkbox(name.c_str(), &message->subscribe))
                {
                    if(message->subscribe)
                    {
                        startSubscribe(message);
                    }
                    else
                    {
                        ofxUnsubscribeOsc(message->port, message->address);
                    }
                }
            }
            else
            {
                name = "send osc[" + ofToString(i) + "]";
                if(ImGui::Button(name.c_str()))
                {
                    string sent_msg = "sent to host: " + ofToString(message->host) + " on port: " + ofToString(message->port) + " message: " + ofToString(message->address);
                    ofxOscMessage m;
                    m.setAddress(message->address);
                    for(auto& arg: message->args)
                    {
                        ofxOscArgType t = arg->getType();
                        switch(t)
                        {
                            case OFXOSC_TYPE_INT32:
                            {
                                int32_t msg = dynamic_cast<ofxOscArgInt32*>(arg.get())->get();
                                m.addInt32Arg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_INT64:
                            {
                                int64_t msg = dynamic_cast<ofxOscArgInt64*>(arg.get())->get();
                                m.addInt64Arg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_FLOAT:
                            {
                                float msg = dynamic_cast<ofxOscArgFloat*>(arg.get())->get();
                                m.addFloatArg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_DOUBLE:
                            {
                                double msg = dynamic_cast<ofxOscArgDouble*>(arg.get())->get();
                                m.addDoubleArg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_TRUE:
                            case OFXOSC_TYPE_FALSE:
                            case OFXOSC_TYPE_TRIGGER:
                            {
                                bool msg = dynamic_cast<ofxOscArgBool*>(arg.get())->get();
                                m.addBoolArg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_STRING:
                            {
                                string msg = dynamic_cast<ofxOscArgString*>(arg.get())->get();
                                m.addStringArg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_CHAR:
                            {
                                char msg = dynamic_cast<ofxOscArgChar*>(arg.get())->get();
                                m.addCharArg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_RGBA_COLOR:
                            {
                                uint32_t msg = dynamic_cast<ofxOscArgRgbaColor*>(arg.get())->get();
                                m.addRgbaColorArg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_TIMETAG:
                            {
                                uint64_t msg = dynamic_cast<ofxOscArgTimetag*>(arg.get())->get();
                                m.addTimetagArg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_MIDI_MESSAGE:
                            {
                                uint32_t msg = dynamic_cast<ofxOscArgMidiMessage*>(arg.get())->get();
                                m.addMidiMessageArg(msg);
                                sent_msg += " " + ofToString(msg);
                                break;
                            }
                            case OFXOSC_TYPE_BLOB:
                            {
                                ofBuffer msg = dynamic_cast<ofxOscArgBlob*>(arg.get())->get();
                                m.addBlobArg(msg);
                                sent_msg += " [blob]";
                                break;
                            }
                            default:
                                ofLogNotice("OscTester") << "unknown arg type.";
                                break;
                        }
                    }
                    ofxOscSender sender;
                    sender.setup(message->host, message->port);
                    
                    sender.sendMessage(m, bundle);
                    message->sent_time = ofGetElapsedTimef();
                    
                    sent_msg += " on " + ofToString(message->sent_time) + " ms";
                    logs.push_back(sent_msg);
                    if(logs.size() > LOG_SIZE)
                    {
                        logs.erase(logs.begin());
                    }
                }
            }
            if(message->subscribe)
            {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                p0.x += 12;
                p0.y += 7;
                ImColor col;
                col.Value.y = 0.3 + 0.7 * max(0.0f, 1.0f + (received_time - ofGetElapsedTimef()) * 4.0f);
                col.Value.w = 1.0;
                ImU32 c = col;
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddCircleFilled(p0, 5, c, 12);
            }
            else
            {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                p0.x += 12;
                p0.y += 7;
                ImColor col;
                col.Value.x = 1.0 * max(0.0f, 1.0f + received_time - ofGetElapsedTimef());
                col.Value.w = 1.0;
                ImU32 c = col;
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddCircleFilled(p0, 5, c, 12);
            }
            ImGui::Dummy(ImVec2(15, 3));
            ImGui::SameLine();
            ImGui::Text(message->message_str.c_str());
            name = "remove message[" + ofToString(i) + "]";
            if(ImGui::Button(name.c_str()))
            {
                it = messages.erase(it);
                break;
            }
            else
            {
                it++;
            }
            i++;
        }
        
        ImGui::End();
    }
};
