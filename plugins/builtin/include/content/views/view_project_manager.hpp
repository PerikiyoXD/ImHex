#pragma once

#include <hex/ui/view.hpp>

namespace hex::plugin::builtin {
    class ViewProjectManager : public View::Modal {
    public:
        ViewProjectManager();
        ~ViewProjectManager() override = default;


        [[nodiscard]] bool hasViewMenuItemEntry() const override;

        [[nodiscard]] bool shouldDraw() const override { return true; }

        ImVec2 getMinSize() const override { return { -1, -1 }; }
        ImVec2 getMaxSize() const override { return this->getMinSize(); }

        bool hasCloseButton() const override {
            return false;
        }

    private:
        void drawContent() override;
        void drawProjectTree();
        void drawSelectionSettings();
        void drawProjectProviders();
        void drawProjectPatterns();
    };
} // namespace hex::plugin::builtin