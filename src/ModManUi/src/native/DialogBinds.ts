import { getBinds } from "@webwindowed/web-api";

export interface FileDialogFilterDto {
  name: string;
  filter: string;
}

export interface OpenFileDialogDto {
  filters?: FileDialogFilterDto[];
}

export interface SaveFileDialogDto {
  filters?: FileDialogFilterDto[];
}

type DialogBinds = {
  openFileDialog(options?: OpenFileDialogDto): Promise<string | null>;
  saveFileDialog(options?: SaveFileDialogDto): Promise<string | null>;
  folderSelectDialog(): Promise<string | null>;
};

export const { openFileDialog, saveFileDialog, folderSelectDialog } = getBinds<DialogBinds>();
