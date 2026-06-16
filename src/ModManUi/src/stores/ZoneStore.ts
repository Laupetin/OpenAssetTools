import { computed, ref } from "vue";
import { defineStore } from "pinia";
import {
  getZones,
  loadFastFile as nativeLoadFastFile,
  type ZoneDto,
  type ZoneLoadedDto,
} from "@/native/ZoneBinds";
import { addEventListener } from "@/native/Events.ts";

export const useZoneStore = defineStore("zone", () => {
  const loadedZones = ref<ZoneDto[]>([]);
  const zonesCurrentlyBeingLoaded = ref<string[]>([]);
  const lastPercentageByZoneName = ref<Record<string, number>>({});

  const isLoadingZone = computed(() => zonesCurrentlyBeingLoaded.value.length > 0);

  function loadFastFile(fastFilePath: string): Promise<ZoneLoadedDto> {
    const lastDirectorySeparator = fastFilePath.replace(/\\/g, "/").lastIndexOf("/");
    const lastDot = fastFilePath.lastIndexOf(".");
    const expectedZoneName = fastFilePath.substring(
      lastDirectorySeparator >= 0 ? lastDirectorySeparator + 1 : 0,
      lastDot > lastDirectorySeparator ? lastDot : fastFilePath.length,
    );

    zonesCurrentlyBeingLoaded.value.push(expectedZoneName);
    lastPercentageByZoneName.value[expectedZoneName] = 0;

    return nativeLoadFastFile(fastFilePath)
      .catch((e: string) => {
        console.error("Failed to load fastfile:", e);
      })
      .finally(() => {
        zonesCurrentlyBeingLoaded.value.splice(
          zonesCurrentlyBeingLoaded.value.indexOf(expectedZoneName),
          1,
        );
        delete lastPercentageByZoneName.value[expectedZoneName];
      }) as Promise<ZoneLoadedDto>;
  }

  function getPercentageForZoneBeingLoaded(zoneName: string) {
    return lastPercentageByZoneName.value[zoneName] ?? 100;
  }

  function getLoadedZoneByName(zoneName: string) {
    return loadedZones.value.find((zone) => zone.name === zoneName) ?? null;
  }

  // Initially get all loaded zones
  getZones().then((allZones) => {
    loadedZones.value = allZones;
  });

  addEventListener("zoneLoadProgress", (dto) => {
    if (lastPercentageByZoneName.value[dto.zoneName] !== undefined) {
      lastPercentageByZoneName.value[dto.zoneName] = dto.percentage;
    }
  });

  addEventListener("zoneLoaded", (dto) => {
    loadedZones.value.push(dto.zone);
  });

  addEventListener("zoneUnloaded", (dto) => {
    const index = loadedZones.value.findIndex((zone) => zone.name === dto.zoneName);
    if (index >= 0) {
      loadedZones.value.splice(index, 1);
    }
  });

  return {
    loadedZones,
    zonesCurrentlyBeingLoaded,
    isLoadingZone,
    lastPercentageByZoneName,
    loadFastFile,
    getPercentageForZoneBeingLoaded,
    getLoadedZoneByName,
  };
});
