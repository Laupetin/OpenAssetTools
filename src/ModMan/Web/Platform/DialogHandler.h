#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace ui
{
    enum class DialogCallbackResultType : std::uint8_t
    {
        SUCCESSFUL,
        CANCELLED,
        FAILED
    };

    class DialogWithCallback
    {
    public:
        using callback_t = std::function<void(DialogCallbackResultType resultType, std::optional<std::string> result)>;

        DialogWithCallback();

        void SetCallback(callback_t callback);

    protected:
        callback_t m_callback;
    };

    class FileDialogFilter
    {
    public:
        FileDialogFilter(std::string name, std::string filter);

        std::string m_name;
        std::string m_filter;
    };

    class FileDialog : public DialogWithCallback
    {
    public:
        FileDialog();

        void AddFilter(std::string name, std::string filter);

    protected:
        std::vector<FileDialogFilter> m_filters;
    };

    class OpenFileDialog : public FileDialog
    {
    public:
        OpenFileDialog();

        void OpenAsync();
    };

    class SaveFileDialog : public FileDialog
    {
    public:
        SaveFileDialog();

        void OpenAsync();
    };

    class FolderSelectDialog : public DialogWithCallback
    {
    public:
        FolderSelectDialog();

        void OpenAsync();
    };
} // namespace ui
