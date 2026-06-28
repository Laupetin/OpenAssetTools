export function getModManUrl(path: string) {
  return new URL(path, "modman://localhost").toString();
}
