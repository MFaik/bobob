#include "program_window.h"

#include "imgui.h"

#include "program_parser.h"
#include "game.h"

ProgramWindow::ProgramWindow(std::string title) : _title(title), _editor() {
    _editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Bobob());
}

bool ProgramWindow::draw(Game &game) {
    ImGuiWindowFlags flag = 0;
    flag |= ImGuiWindowFlags_NoCollapse;
    flag |= ImGuiWindowFlags_MenuBar;
    bool ret = true;
    
    std::string name = "##"+_title;
    if(ImGui::Begin(name.c_str(), &ret, flag)) {
        ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

        if(ImGui::BeginMenuBar()) {
            ImGui::Text("%s", _title.c_str());
            ImGui::Separator();
            if(ImGui::MenuItem("Compile")) {
                auto program = parse_program(_editor.GetText());
                if(program._errors.size()) {
                    TextEditor::ErrorMarkers errors;
                    for(auto error : program._errors) {
                        errors[error.line+1] = error.txt;
                        _editor.SetErrorMarkers(errors);
                    }
                } else {
                    game.setup_program(program);
                }
            }
            ImGui::EndMenuBar();
        }

        _editor.Render("TextEditor");

        ImGui::End();
    }
    return ret;
}

void ProgramWindow::set_text(const std::string& text) {
    _editor.SetText(text);
}

/*
static TextEditor editor;
auto cpos = editor.GetCursorPosition();
ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
if (ImGui::BeginMenuBar())
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Save"))
        {
            auto textToSave = editor.GetText();
            /// save text....
        }
        if (ImGui::MenuItem("Quit", "Alt-F4"))
            break;
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
        bool ro = editor.IsReadOnly();
        if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
            editor.SetReadOnly(ro);
        ImGui::Separator();

        if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
            editor.Undo();
        if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
            editor.Redo();

        ImGui::Separator();

        if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
            editor.Copy();
        if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
            editor.Cut();
        if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
            editor.Delete();
        if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
            editor.Paste();

        ImGui::Separator();

        if (ImGui::MenuItem("Select all", nullptr, nullptr))
            editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
        if (ImGui::MenuItem("Dark palette"))
            editor.SetPalette(TextEditor::GetDarkPalette());
        if (ImGui::MenuItem("Light palette"))
            editor.SetPalette(TextEditor::GetLightPalette());
        if (ImGui::MenuItem("Retro blue palette"))
            editor.SetPalette(TextEditor::GetRetroBluePalette());
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
}
static const char* fileToEdit = "ImGuiColorTextEdit/TextEditor.cpp";
ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
        editor.IsOverwrite() ? "Ovr" : "Ins",
        editor.CanUndo() ? "*" : " ",
        editor.GetLanguageDefinition().mName.c_str(), fileToEdit);

editor.Render("TextEditor");
*/
