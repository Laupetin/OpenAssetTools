import { ref } from "vue";
import { defineStore } from "pinia";
import { webviewAddEventListener, webviewBinds } from "@/native";

export const useUnlinkingStore = defineStore("unlinking", () => {
  const isUnlinking = ref(false);
  const lastPercentage = ref<number>(0);
  const failureMessage = ref<string | null>(null);

  function unlinkZone(zoneName: string) {
    isUnlinking.value = true;
    lastPercentage.value = 0;
    failureMessage.value = null;
    return webviewBinds
      .unlinkZone(zoneName)
      .catch((e: string) => {
        console.error("Failed to unlink fastfile:", e);
        failureMessage.value = e;
      })
      .finally(() => {
        isUnlinking.value = false;
        lastPercentage.value = 100;
      });
  }

  webviewAddEventListener("zoneUnlinkProgress", (dto) => {
    lastPercentage.value = dto.percentage;
  });

  return { isUnlinking, lastPercentage, unlinkZone };
});
