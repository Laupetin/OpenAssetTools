<script setup lang="ts">
import { ref, watch } from "vue";
import { useZoneStore } from "@/stores/ZoneStore";
import ZoneInspectorDetails from "./ZoneInspectorDetails.vue";
import ZoneInspectorZoneList from "./ZoneInspectorZoneList.vue";
import { useAssetStore } from "@/stores/AssetStore";

const assetStore = useAssetStore();
const zoneStore = useZoneStore();

const selectedZone = ref<string | null>(null);

// Make sure we preselect the zone that was last loaded assets for
// if there is one
if (
  assetStore.zoneName &&
  zoneStore.loadedZones.findIndex((zone) => zone.name === assetStore.zoneName) >= 0
) {
  selectedZone.value = assetStore.zoneName;
}

watch(
  () => zoneStore.loadedZones,
  (newValue) => {
    // Reset selection if unloaded
    if (!selectedZone.value) return;
    if (newValue.findIndex((loadedZone) => loadedZone.name === selectedZone.value) >= 0) return;
    selectedZone.value = null;
  },
  { deep: true },
);
</script>

<template>
  <div class="zone-inspector">
    <ZoneInspectorZoneList v-model:selected-zone="selectedZone" />
    <ZoneInspectorDetails :selected-zone="selectedZone" />
  </div>
</template>

<style lang="scss" scoped>
.zone-inspector {
  display: flex;
  flex-direction: row;
  width: 100%;

  & > * {
    width: 50%;
    padding: 1rem 2rem;
  }
}
</style>
