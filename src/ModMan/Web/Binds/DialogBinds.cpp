#include "DialogBinds.h"

#include "Web/Platform/DialogHandler.h"
#include "Web/UiCommunication.h"

#include "Json/JsonExtension.h"

namespace
{
    class FileDialogFilterDto
    {
    public:
        std::string name;
        std::string filter;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(FileDialogFilterDto, name, filter);

    class OpenFileDialogDto
    {
    public:
        std::optional<std::vector<FileDialogFilterDto>> filters;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(OpenFileDialogDto, filters);

    class SaveFileDialogDto
    {
    public:
        std::optional<std::vector<FileDialogFilterDto>> filters;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(SaveFileDialogDto, filters);

    void ReplyWithDialogResult(webview::webview& wv,
                               const std::string& id,
                               const ui::DialogCallbackResultType resultType,
                               const std::optional<std::string>& result)
    {
        if (resultType == ui::DialogCallbackResultType::FAILED)
            ui::PromiseReject(wv, id, result);
        else
            ui::PromiseResolve(wv, id, result);
    }

    void OpenFileDialogBind(webview::webview& wv, const std::string& id, const std::optional<OpenFileDialogDto>& dto)
    {
        ui::OpenFileDialog dialog;
        dialog.SetCallback(
            [&wv, id](const ui::DialogCallbackResultType resultType, const std::optional<std::string>& result)
            {
                ReplyWithDialogResult(wv, id, resultType, result);
            });

        if (dto && dto->filters)
        {
            for (auto& filter : *dto->filters)
            {
                dialog.AddFilter(filter.name, filter.filter);
            }
        }

        dialog.OpenAsync();
    }

    void SaveFileDialogBind(webview::webview& wv, const std::string& id, const std::optional<SaveFileDialogDto>& dto)
    {
        ui::SaveFileDialog dialog;
        dialog.SetCallback(
            [&wv, id](const ui::DialogCallbackResultType resultType, const std::optional<std::string>& result)
            {
                ReplyWithDialogResult(wv, id, resultType, result);
            });

        if (dto && dto->filters)
        {
            for (auto& filter : *dto->filters)
            {
                dialog.AddFilter(filter.name, filter.filter);
            }
        }

        dialog.OpenAsync();
    }

    void FolderSelectDialogBind(webview::webview& wv, const std::string& id)
    {
        ui::FolderSelectDialog dialog;
        dialog.SetCallback(
            [&wv, id](const ui::DialogCallbackResultType resultType, const std::optional<std::string>& result)
            {
                ReplyWithDialogResult(wv, id, resultType, result);
            });

        dialog.OpenAsync();
    }
} // namespace

namespace ui
{
    void RegisterDialogHandlerBinds(webview::webview& wv)
    {
        BindAsync<std::optional<OpenFileDialogDto>>(wv,
                                                    "openFileDialog",
                                                    [&wv](const std::string& id, const std::optional<OpenFileDialogDto>& dto)
                                                    {
                                                        OpenFileDialogBind(wv, id, dto);
                                                    });

        BindAsync<std::optional<SaveFileDialogDto>>(wv,
                                                    "saveFileDialog",
                                                    [&wv](const std::string& id, const std::optional<SaveFileDialogDto>& dto)
                                                    {
                                                        SaveFileDialogBind(wv, id, dto);
                                                    });

        BindAsync(wv,
                  "folderSelectDialog",
                  [&wv](const std::string& id)
                  {
                      FolderSelectDialogBind(wv, id);
                  });
    }
} // namespace ui
