import { ref } from "vue";
import { defineStore } from "pinia";
import { webviewAddEventListener } from "@/native";

export const useZoneStore = defineStore("zone", () => {
  const loadedZones = ref<string[]>([]);

  webviewAddEventListener("zoneLoaded", (dto) => {
    loadedZones.value.push(dto.zoneName);
  });

  webviewAddEventListener("zoneUnloaded", (dto) => {
    const index = loadedZones.value.indexOf(dto.zoneName);
    if (index >= 0) {
      loadedZones.value.splice(index, 1);
    }
  });

  return { loadedZones };
});
