//
// Created by pxd on 16/01/2024.
//

#include "content/views/view_project_manager.hpp"

#include <hex/api/content_registry.hpp>
#include <hex/api/project_file_manager.hpp>


namespace hex::plugin::builtin {

    ViewProjectManager::ViewProjectManager() : Modal("hex.builtin.view.project.manager.name") {
        ContentRegistry::Interface::addSidebarItem(ICON_VS_PROJECT,
                                                   [this] {
                                                       this->drawContent();
                                                   },
                                                   [] {
                                                       return true;
                                                   }
        );
    }

    void ViewProjectManager::drawContent() {
        this->drawProjectTree();
        this->drawSelectionSettings();
    }

    void ViewProjectManager::drawProjectTree() {
        auto projectName = []() -> std::string {
            if (!ProjectFile::hasPath()) {
                return "Unsaved Project";
            }
            return ProjectFile::getPath().filename().string();
        }();
        if (ImGui::CollapsingHeader(projectName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            this->drawProjectProviders();
            this->drawProjectPatterns();
        }
    }

    bool ViewProjectManager::hasViewMenuItemEntry() const {
        return View::hasViewMenuItemEntry();
    }

    void ViewProjectManager::drawSelectionSettings() {
        // ImGui::SeparatorText("Settings");
        // if (ImGui::TreeNode("Setting #1")) {
        //     ImGui::TreePop();
        // }
    }

    void ViewProjectManager::drawProjectProviders() {
        ImGui::SeparatorText("Project Files");

        if (ImGui::BeginTable("provider_manager", 2,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoSavedSettings)) {

            auto getTypeName = [](const std::string &typeName) -> const char * {
                if (typeName == "hex.builtin.provider.mem_file") {
                    return "hex.builtin.provider.mem_file"_lang;
                }
                if (typeName == "hex.builtin.provider.file") {
                    return "hex.builtin.provider.file"_lang;
                }
                if (typeName == "hex.builtin.provider.view") {
                    return "hex.builtin.provider.view"_lang;
                }
                return "hex.builtin.view.information.octet_stream_text"_lang;
            };

            auto drawProvider = [&getTypeName](prv::Provider *provider) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(provider->getName().c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(getTypeName(provider->getTypeName()));
            };

            auto providers = ImHexApi::Provider::getProviders();

            for (auto &provider: providers) {
                drawProvider(provider);
            }

            ImGui::EndTable();
        }
    }

    void ViewProjectManager::drawProjectPatterns() {
        return;

        ImGui::SeparatorText("Patterns");

        if (ImGui::BeginTable("pattern_manager", 2,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoSavedSettings)) {



            auto drawPattern = [](prv::Provider *provider) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(provider->getName().c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(provider->getTypeName().c_str());
            };

            auto providers = []() {
                std::vector<prv::Provider *> providers;
                // for (auto &provider: ImHexApi::Provider::getProviders()) {
                //     if (
                //         providers.push_back(provider);
                //     }
                // }
                return providers;
            }();

            for (auto &provider: providers) {
                drawPattern(provider);
            }

            ImGui::EndTable();
        }
    }
} // namespace hex::plugin::builtin