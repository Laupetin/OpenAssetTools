import { computed, ref } from "vue";
import { defineStore } from "pinia";
import type { ZoneAssetsDto } from "@/native/AssetBinds";
import { webviewBinds } from "@/native";

export const useAssetStore = defineStore("asset", () => {
  const zoneName = ref<string | null>(null);
  const assetsOfZone = ref<ZoneAssetsDto | null>(null);

  const isLoading = computed(() => Boolean(assetsOfZone.value));
  const assetCount = computed(() => assetsOfZone.value?.assets.length ?? 0);
  const referenceCount = computed(() => assetsOfZone.value?.references.length ?? 0);

  function loadAssetsForZone(newZoneName: string | null) {
    // Skip if assets are already loaded
    if (newZoneName === zoneName.value) return;

    // Reset current state
    zoneName.value = newZoneName;
    assetsOfZone.value = null;

    // Only load assets when there is a new zone name specified
    if (!newZoneName) return;
    webviewBinds.getAssetsForZone(newZoneName).then((res) => {
      if (zoneName.value === newZoneName) {
        assetsOfZone.value = res;
      }
    });
  }

  return { zoneName, isLoading, assetsOfZone, assetCount, referenceCount, loadAssetsForZone };
});
