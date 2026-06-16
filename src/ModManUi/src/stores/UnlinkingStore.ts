import { ref } from "vue";
import { defineStore } from "pinia";
import { addEventListener } from "@/native/Events.ts";
import { unlinkZone as nativeUnlinkZone } from "@/native/UnlinkingBinds.ts";

export const useUnlinkingStore = defineStore("unlinking", () => {
  const isUnlinking = ref(false);
  const lastPercentage = ref<number>(0);
  const failureMessage = ref<string | null>(null);

  function unlinkZone(zoneName: string) {
    isUnlinking.value = true;
    lastPercentage.value = 0;
    failureMessage.value = null;

    return nativeUnlinkZone(zoneName)
      .catch((e: string) => {
        console.error("Failed to unlink fastfile:", e);
        failureMessage.value = e;
      })
      .finally(() => {
        isUnlinking.value = false;
        lastPercentage.value = 100;
      });
  }

  addEventListener("zoneUnlinkProgress", (dto) => {
    lastPercentage.value = dto.percentage;
  });

  return { isUnlinking, lastPercentage, unlinkZone };
});
