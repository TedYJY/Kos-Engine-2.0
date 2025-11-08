#include "Editor.h"
#include "Resources/R_AnimController.h"

namespace ed = ax::NodeEditor;


struct DisplayController {
    std::string name;
    AnimControllerData controller;

    REFLECTABLE(DisplayController, name);
};

DisplayController controllerData;

void gui::ImGuiHandler::DrawAnimatorControllerWindow()
{
    ImGui::Begin("Animator Controller");

    static int nextId = 1;
    static std::unordered_map<int, AnimState> states;
    static std::vector<AnimTransition> transitions;

    // --- Context & Node Editor ---
   // static ed::EditorContext* context = nullptr;
    //if (!context)
    //    context = ed::CreateEditor();
    ed::SetCurrentEditor(m_animControllerContext);

    if (!m_activeController)
    {
        //ImGui::SetCursorPos(ImGui::GetWindowSize() * 0.5f - ImVec2(100, 20));
        //if (ImGui::Button("New Controller", ImVec2(120, 30))) {
        //    m_activeController;
        //    m_activeController->name = "NewController";
        //    m_activeController->path = "Assets/Controllers/NewController.json";
        //    m_activeController->Clear();
        //}

        //ImGui::SetCursorPos(ImGui::GetWindowSize() * 0.5f + ImVec2(-100, 20));
        //if (ImGui::Button("Load Controller", ImVec2(120, 30))) {
        //    // TODO: open file dialog (or hardcoded path for now)
        //    LoadControllerFromFile("Assets/Controllers/Example.json");
        //}

        //ImGui::End();
        //return;
    }

    ed::Begin("AnimationGraph");

    // --- Create Default Nodes ---
    if (states.empty())
    {
        // Entry node
        AnimState entry;
        entry.id = nextId++;
        entry.name = "Entry";
        entry.outputs.push_back({ nextId++, AnimPin::PinKind::Output, "Out" });
        entry.isDefault = true;
        states[entry.id] = entry;
        ed::SetNodePosition(entry.id, ImVec2(50, 150));

        // Any State node
        AnimState any;
        any.id = nextId++;
        any.name = "Any State";
        any.outputs.push_back({ nextId++, AnimPin::PinKind::Output, "Out" });
        any.isDefault = true;
        states[any.id] = any;
        ed::SetNodePosition(any.id, ImVec2(50, 50));

        // Exit node
        AnimState exit;
        exit.id = nextId++;
        exit.name = "Exit";
        exit.inputs.push_back({ nextId++, AnimPin::PinKind::Input, "In" });
        exit.isDefault = true;
        states[exit.id] = exit;
        ed::SetNodePosition(exit.id, ImVec2(50, 100));
    }

    // --- Node creation button ---
    if (ImGui::Button("Add State"))
    {
        AnimState s;
        s.id = nextId++;
        s.name = "NewState";
        s.inputs.push_back({ nextId++, AnimPin::PinKind::Input, "Enter" });
        s.outputs.push_back({ nextId++, AnimPin::PinKind::Output, "Exit" });
        states[s.id] = s;
        ed::SetNodePosition(s.id, ImVec2(100 + 50.0f * states.size(), 100));
    }

    // --- Draw all nodes ---
    for (auto& [id, node] : states)
    {
        ed::BeginNode(node.id);
        ImGui::Text("%s", node.name.c_str());

        for (auto& pin : node.inputs)
        {
            ed::BeginPin(pin.id, ed::PinKind::Input);
            ImGui::Text("%s", pin.name.c_str());
            ed::EndPin();
        }

        for (auto& pin : node.outputs)
        {
            ed::BeginPin(pin.id, ed::PinKind::Output);
            ImGui::Text("%s", pin.name.c_str());
            ed::EndPin();
        }

        ed::EndNode();
    }

    // --- Handle link creation ---
    ed::PinId startPin, endPin;
    if (ed::BeginCreate()) {
        if (ed::QueryNewLink(&startPin, &endPin)) {
            if (ed::AcceptNewItem()) {
                AnimTransition t;
                t.id = nextId++;
                t.fromPinId = startPin.Get();
                t.toPinId = endPin.Get();
                transitions.push_back(t);
            }
        }
    }
    ed::EndCreate();

    // --- Draw links ---
    for (auto& t : transitions)
        ed::Link(t.id, t.fromPinId, t.toPinId);

    // --- Detect node selection ---
    static ed::NodeId selectedNode;
    if (ed::GetSelectedObjectCount() > 0)
    {
        ed::NodeId nodes[1];
        int count = ed::GetSelectedNodes(nodes, 1);
        if (count > 0)
            selectedNode = nodes[0];
    }
    else
    {
        selectedNode = ed::NodeId(0);
    }

    ed::End();
    ed::SetCurrentEditor(nullptr);

    ImGui::End();

    // --- Always-visible property window ---
    ImGui::Begin("Selected Animator Node");

    float windowWidth = ImGui::GetContentRegionAvail().x;
    float buttonWidth = 100.0f; // Width of your button
    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center horizontally
    if (ImGui::Button("Create", ImVec2(buttonWidth, 0))) {
        if (!controllerData.name.empty()) {
            std::string fileName = controllerData.name + ".controller";
            std::string filepath = m_assetManager.GetAssetManagerDirectory() + "/Animation Controller/" + fileName;

            serialization::WriteJsonFile(filepath, &controllerData.controller);
            // std::cout << "TEST CREATION\n";
            LOGGING_POPUP("Animator Controller Successfully Added");
        }
        else {
            LOGGING_WARN("Animator Controller Name cannot be empty");
        }
    }


    if (selectedNode && states.contains(selectedNode.Get()))
    { 
        AnimState& state = states[selectedNode.Get()];

        state.ApplyFunction( DrawComponents{state.Names()} );

        
    }
    else
    {
        ImGui::TextDisabled("No node selected.");
    }



    ImGui::End();
}


void gui::ImGuiHandler::ShutdownAnimatorLayout()
{
    if (m_animControllerContext)
    {
        // DestroyEditor will write settings to the configured SettingsFile.
        ax::NodeEditor::DestroyEditor(m_animControllerContext);
        m_animControllerContext = nullptr;
    }
}